#include "Terrain.hpp"


// STL headers.
#include <stdexcept>


// Engine headers.
#include <tgl/tgl.h>
#include <tygra/FileHelper.hpp>


// Personal headers.
#include <Renderer/Vertex.hpp>
#include <Terrain/HeightMap.hpp>
#include <Terrain/TerrainConstructionData.hpp>



/////////////////////////////////
// Constructors and destructor //
/////////////////////////////////

Terrain::Terrain (Terrain&& move)
{
    *this = std::move (move);
}


Terrain& Terrain::operator= (Terrain&& move)
{
    if (this != &move)
    {
        // Ensure we don't have valid data!
        cleanUp();

        m_pool    = std::move (move.m_pool);
        m_patches = std::move (move.m_patches);
    }

    return *this;
}


Terrain::~Terrain()
{
    // Ensure we don't keep allocated memory in the GPU.
    cleanUp();
}


/////////////
// Setters //
/////////////

void Terrain::setDivisor (const unsigned int divisor)
{
    // Don't allow silly values.
    assert (divisor > 1);

    m_divisor = divisor;
}


//////////////////////
// Public interface //
//////////////////////

void Terrain::buildFromHeightMap (const HeightMap& heightMap, const unsigned int upscaledWidth, const unsigned int upscaledDepth)
{
    // Ensure we have a clean set of data to work with.
    cleanUp();
    m_pool.generate();

    // Determine the dimensions of the terrain.
    const auto width = upscaledWidth == 0 ? heightMap.getWidth()  : upscaledWidth,
               depth = upscaledDepth == 0 ? heightMap.getHeight() : upscaledDepth;

    // Create the construction data so we can build the terrain.
    const ConstructionData data { width, depth, determineDivisor (width, depth) };

    // Ensure the GPU has enough memory to process the terrain.
    allocateGPUMemory (data);

    // We need the elements data to be correct first.
    generateElements (data);

    // Generate the terrain!
    generateVertices (heightMap, data);
}


void Terrain::cleanUp()
{
    // Put that memory away!
    m_pool.clear();
    m_patches.clear();
}


void Terrain::prepareForRender (const GLuint program)
{
    // Pass on our best regards to the MeshPool.
    m_pool.initialiseVAO (program);
}


void Terrain::draw()
{
    glBindVertexArray (m_pool.getVAO());

    for (const auto& mesh : m_patches)
    {
        glDrawElementsBaseVertex (GL_TRIANGLES, mesh.elementCount, GL_UNSIGNED_INT, (GLuint*) mesh.elementsOffset, mesh.firstVertex);
    }

    glBindVertexArray (0);
}


//////////////
// Creation //
//////////////

unsigned int Terrain::determineDivisor (const unsigned int width, const unsigned int depth) const
{
    // If the width or depth exceeds the divisor we must use that value.
    if (m_divisor > width && width <= depth)
    {
        return width;
    }

    if (m_divisor > depth && depth < width)
    {
        return depth;
    }

    return m_divisor;
}


void Terrain::allocateGPUMemory (const ConstructionData& data)
{
    // We have four potential sets of element data, we must calculate the size of each to determine the correct memory.
    const auto patchCount = data.getDivisor() * data.getDivisor(),
               remXCount  = data.hasRemainderX() ? data.getRemainderX() * data.getDivisor() : patchCount,
               remZCount  = data.hasRemainderZ() ? data.getRemainderZ() * data.getDivisor() : patchCount;

    auto remXZCount = patchCount;

    if (data.hasRemainderX())
    {
        remXZCount = data.hasRemainderZ() ? data.getRemainderX() * data.getRemainderZ() : remXCount;
    }

    else if (data.hasRemainderZ())
    {
        remXZCount = data.getRemainderZ() * data.getDivisor();
    }

    // The total number of uniquely drawn quads is the sum all of sets, multiple by two to convert quads to triangles.
    const auto triangleCount = (patchCount + remXCount + remZCount + remXZCount) * 2;

    // Three elements are required to draw a single triangle.
    const auto elementCount = triangleCount * 3;

    // Now we can convert these values to bytes and allocate memory. Elements are stored using an unsigned int.
    const auto verticesSize = data.getVertexCount() * sizeof (Vertex),
               elementsSize = elementCount * sizeof (unsigned int);

    // Finally allocate the memory. The calculated element size will always reserve more memory than necessary.
    m_pool.fillData (BufferType::Vertices, verticesSize, nullptr);
    m_pool.fillData (BufferType::Elements, elementsSize, nullptr);
}


//////////////////////
// Element creation //
//////////////////////

void Terrain::generateElements (const ConstructionData& data)
{
    // Firstly we need a vector to store the glorious data inside of. 
    std::vector<unsigned int> elements { };
    elements.reserve (data.getDivisor() * data.getDivisor());

    // Keep track of the element offset for the meshes.
    GLuint elementOffset { 0 };

    // We have four types of elements to generate, one has stitching on two sides, one has stitching on the top, one
    // has stitching on the right and one has no stitching.
    const auto createElements = [&] (const MeshTemplate mesh, const unsigned int width, const unsigned int depth)
    {
        // Start with clean data.
        elements.clear();

        // Generate the elements as normal.
        addElements (elements, width, depth);
        
        // Stitch the X.
        if (mesh == MeshTemplate::Central || mesh == MeshTemplate::TopRow)
        {
            addStitching (elements, data, depth, StitchingMode::XAxis);
        }

        // Stitch the Z.
        if (mesh == MeshTemplate::Central || mesh == MeshTemplate::RightColumn)
        {
            addStitching (elements, data, width, StitchingMode::ZAxis);
        }

        // Clean up after ourselves.
        if (mesh == MeshTemplate::Central)
        {
            // The length is ignored.
            addStitching (elements, data, 1, StitchingMode::Corner);
        }

        // Load the data into the GPU.
        const auto size = elements.size() * sizeof (unsigned int);
        
        m_pool.fillSection (BufferType::Elements, elementOffset, size, elements.data());

        // Update the mesh with the correct values.
        m_meshTemplates[(size_t) mesh] = Mesh (0, elementOffset, elements.size());
        
        // Increment the sausage!
        elementOffset += size;
    };

    if (data.getMeshTotal() > 1)
    {
        // We should just use the normal divisor if no remainder exists.
        const auto width      = data.getDivisor(),
                   depth      = data.getDivisor(),
                   remainderX = data.hasRemainderX() ? data.getRemainderX() : data.getDivisor(),
                   remainderZ = data.hasRemainderZ() ? data.getRemainderZ() : data.getDivisor();
    
        // Load the element arrays into the GPU.
        createElements (MeshTemplate::Central,        width, depth);
        createElements (MeshTemplate::TopRow,         width, remainderZ);
        createElements (MeshTemplate::RightColumn,    remainderX, depth);
        createElements (MeshTemplate::TopRightCorner, remainderX, remainderZ);
    }

    // If we aren't segmenting then just load the top right corner template.
    else
    {
        createElements (MeshTemplate::TopRightCorner, data.getWidth(), data.getDepth());
    }
}


void Terrain::addElements (std::vector<unsigned int>& elements, const unsigned int width, const unsigned int depth)
{
    // Increment normally to create the pattern.
    const auto offset    = 0U,
               increment = 1U;

    // Prevent overflow by reducing the width and depth by one.
    const auto endWidth = width - 1,
               endDepth = depth - 1;

    // Run the triangle algorithm to add the elements.
    triangleAlgorithm (elements, offset, endWidth, endDepth, increment, width);
}


void Terrain::addStitching (std::vector<unsigned int>& elements, const ConstructionData& data, const unsigned int length, const StitchingMode mode)
{
    // We need to determine the correct values to create the desired stitching.
    auto offset        = 0U,
         width         = 1U, 
         depth         = 1U,
         increment     = 0U,
         lineIncrement = 0U;

    bool startMirrored = !(data.getDivisor() % 2);

    // Cache commonly computed calculations.
    const auto segmentWidth    = data.getDivisor(),
               segmentTotal    = segmentWidth * segmentWidth,
               segmentMinusRow = segmentTotal - segmentWidth;

    switch (mode)
    {
        case StitchingMode::XAxis:

            // The offset needs to be in array notation form.
            offset = segmentWidth - 1;

            // We will have loads of boundary issues if we try to do the top-right corner.
            depth = length - 1;
        
            // We need to move an entire segment but add an extra one so that we obtain the adjacent element.
            increment     = segmentMinusRow + 1;
            lineIncrement = segmentWidth;
            break;

        case StitchingMode::ZAxis:
            
            // Start at the top row of a segment on the first element of the X axis.
            offset = segmentMinusRow;

            // We will have loads of boundary issues if we try to do the top-right corner.
            width = length - 1;

            // We just need to move to the right once.
            increment = 1;

            // Move up an entire segment, back one and up a row to get the first row of the segment above.
            lineIncrement = segmentTotal * (data.getMeshCountX() - 1) + segmentWidth;
            break;

        case StitchingMode::Corner:

            // We need the last element of the segment.
            offset = segmentTotal - 1;

            // Followed by the first element on the last row of the next segment.
            increment = segmentMinusRow + 1;

            // Same as the ZAxis increment.
            lineIncrement = segmentTotal * (data.getMeshCountX() - 1) + segmentWidth;

            // Make it blend in!
            startMirrored = !startMirrored;
            break;

        default:
            throw std::logic_error ("Terrain::AddStitching(), this should never be thrown.");
    }

    // Finally run the algorithm to generate elements.
    triangleAlgorithm (elements, offset, width, depth, increment, lineIncrement, startMirrored);
}


void Terrain::triangleAlgorithm (std::vector<unsigned int>& elements, const unsigned int offset, const unsigned int width, const unsigned int depth, 
                                                                      const unsigned int increment, const unsigned int lineIncrement, const bool startMirrored)
{
    /// We create an alternating square pattern for efficiency. It should look as shown below.
    /// 
    /// |/|\|/|
    /// |\|/|\|
    /// |/|\|/|
    /// 

    // Use mirrors to maintain the alternating pattern when patches are made up with even or odd number of vertices.
    bool mirrorX = startMirrored, 
         mirrorZ = startMirrored;

    for (auto z = 0U; z < depth; ++z)
    {
        for (auto x = 0U; x < width; ++x)
        {
            // Calculate the current vertex.
            const auto vertex = offset + x + z * lineIncrement;

            lowerTriangle (elements, vertex, increment, lineIncrement, mirrorX);
            upperTriangle (elements, vertex, increment, lineIncrement, mirrorX);
            
            mirrorX = !mirrorX;
        }

        // Here's the magic which maintains the pattern with an even/odd width.
        mirrorX = mirrorZ = !mirrorZ;
    }
}


void Terrain::lowerTriangle (std::vector<unsigned int>& elements, const unsigned int current, const unsigned int increment, const unsigned int width, const bool mirror)
{
    const auto triangleEnd = mirror ? current + width : current + width + increment;

    elements.push_back (current);
    elements.push_back (current + increment);
    elements.push_back (triangleEnd);
}


void Terrain::upperTriangle (std::vector<unsigned int>& elements, const unsigned int current, const unsigned int increment, const unsigned int width, const bool mirror)
{
    const auto triangleEnd = mirror ? current + increment : current;

    elements.push_back (current + width + increment);
    elements.push_back (current + width);
    elements.push_back (triangleEnd);
}


///////////////////////
// Vertices Creation //
///////////////////////

void Terrain::generateVertices (const HeightMap& heightMap, const ConstructionData& data)
{
    // Cache some constants we'll be using.
    const auto divisor       = data.getDivisor();

    const auto hasRemainderX = data.hasRemainderX(),
               hasRemainderZ = data.hasRemainderZ();

    const auto remainderX    = data.getRemainderX(),
               remainderZ    = data.getRemainderZ();

    const auto meshCountX    = data.getMeshCountX(),
               meshCountZ    = data.getMeshCountZ(),               
               meshTotal     = meshCountX * meshCountZ;
               
    const auto lastMeshX     = meshCountX - 1,
               lastMeshZ     = meshCountZ - 1;

    // We're going to need a vector to store the data of each patch.
    std::vector<Vertex> vertices { };

    // Lets reserve us some memory to speed this process up!
    vertices.reserve (divisor * divisor);
    m_patches.reserve (meshTotal);

    // Keep track of the elements offset.
    GLint firstVertex { 0 };

    // We're going to need three loops, one is updates the Mesh vector and the other two create the vertices.
    for (auto mesh = 0U; mesh < meshTotal; ++mesh)
    {
        // We need the vertex offsets so can we get the obtain the correct data from the height map.
        const auto xTile   = mesh % meshCountX,
                   zTile   = mesh / meshCountZ,
                   xOffset = xTile * divisor,
                   zOffset = zTile * divisor;

        // Check if we're on the last tile on either axis.
        const auto isLastMeshX = xTile == lastMeshX,
                   isLastMeshZ = zTile == lastMeshZ;

        // Make sure we don't cause access violation errors by checking if we're using an oddly sized heightmap providing remainders.
        const auto patchWidth = hasRemainderX && isLastMeshX ? remainderX : divisor,
                   patchDepth = hasRemainderZ && isLastMeshZ ? remainderZ : divisor;

        // Cache the values which end the loops so we don't calculate them every time.
        const auto widthEnd = xOffset + patchWidth,
                   depthEnd = zOffset + patchDepth;

        for (auto z = zOffset; z < depthEnd; ++z)
        {
            for (auto x = xOffset; x < widthEnd; ++x)
            {
                /*// Obtain the necessary UV co-ordinates to create the vertex. 
                const auto u = (float) x / width,
                            v = (float) z / depth;

                addVertex (vertices, heightMap, u, v);*/
                vertices.emplace_back (heightMap.getPoint (x, z), glm::vec3 (0, 1, 0));
            }
        }

        // Add the data to the GPU.
        const auto verticesSize = vertices.size() * sizeof (Vertex);

        m_pool.fillSection (BufferType::Vertices, firstVertex * sizeof (Vertex), verticesSize, vertices.data());

        // Now construct the mesh.
        const auto mesh = m_meshTemplates[templateIndex (isLastMeshX, isLastMeshZ)];

        m_patches.emplace_back (firstVertex, mesh.elementsOffset, mesh.elementCount);

        // Increment everything captain!
        firstVertex += vertices.size();
        vertices.clear();
    }
}


size_t Terrain::templateIndex (const bool isLastMeshX, const bool isLastMeshZ) const
{
    // Y + Y = TopRightCorner.
    // Y + N = RightColumn,
    // N + Y = TopRow,
    // N + N = Central,

    if (isLastMeshX)
    {
        return isLastMeshZ ? (size_t) MeshTemplate::TopRightCorner : (size_t) MeshTemplate::RightColumn;
    }

    return isLastMeshZ ? (size_t) MeshTemplate::TopRow : (size_t) MeshTemplate::Central;
}