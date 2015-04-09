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
#include <Utility/BezierSurface.hpp>
#include <Utility/ElementCreation.hpp>



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

        m_pool          = std::move (move.m_pool);
        m_patches       = std::move (move.m_patches);
        m_meshTemplates = std::move (move.m_meshTemplates);
        
        m_divisor       = move.m_divisor;

        // Reset primitives.
        move.m_divisor = 0;
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

void Terrain::buildFromHeightMap (const HeightMap& heightMap, const NoiseArgs& normal, const NoiseArgs& height, 
                                  const unsigned int upscaledWidth, const unsigned int upscaledDepth)
{
    // Ensure we have a clean set of data to work with.
    cleanUp();
    m_pool.generate();

    // Determine the dimensions of the terrain.
    const auto width   = upscaledWidth == 0 ? heightMap.getWidth()  : upscaledWidth,
               depth   = upscaledDepth == 0 ? heightMap.getHeight() : upscaledDepth,
               divisor = determineDivisor (width, depth);

    assert (width % divisor == 0 && depth % divisor == 0);

    // Create the construction data so we can build the terrain.
    const ConstructionData data { width, depth, divisor, heightMap.getWorldScale().x, heightMap.getWorldScale().z };

    // Ensure the GPU has enough memory to process the terrain.
    allocateGPUMemory (data);

    // We need the elements data to be correct first.
    generateElements (data);

    // Generate the terrain!
    generateVertices (heightMap, data, normal, height);
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
    // We only store the elements it takes to render an entire patch.
    const auto quadCount = data.getMeshVertices();

    // If we have more than one mesh we need four sets of elements data for the four types of patches. Two triangles make a quad.
    const auto triangleCount = data.getMeshTotal() > 1 ? quadCount * 4 * 2 : quadCount * 2;

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
    elements.reserve (data.getMeshVertices() * 3);

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
        m_meshTemplates[(unsigned int) mesh] = { 0, elementOffset, elements.size() };
        
        // Increment the sausage!
        elementOffset += size;
    };

    if (data.getMeshTotal() > 1)
    {
        // We should just use the normal divisor if no remainder exists.
        const auto width      = data.getDivisor(),
                   depth      = data.getDivisor();
    
        // Load the element arrays into the GPU.
        for (auto i = 0U; i < (unsigned int) MeshTemplate::Count; ++i)
        {
            createElements ((MeshTemplate) i, width, depth);
        }
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
    util::triangleAlgorithm (elements, offset, endWidth, endDepth, increment, width);
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
    util::triangleAlgorithm (elements, offset, width, depth, increment, lineIncrement, startMirrored);
}


///////////////////////
// Vertices Creation //
///////////////////////

void Terrain::generateVertices (const HeightMap& heightMap, const ConstructionData& data, const NoiseArgs& normal, const NoiseArgs& height)
{
    // Cache some constants we'll be using.
    const auto divisor       = data.getDivisor();

    const auto meshCountX    = data.getMeshCountX(),
               meshCountZ    = data.getMeshCountZ();
               
    const auto lastMeshX     = meshCountX - 1,
               lastMeshZ     = meshCountZ - 1;

    // We're going to need a vector to store the data of each patch.
    std::vector<Vertex> vertices { };

    // Lets reserve us some memory to speed this process up!
    vertices.reserve (data.getMeshVertices());
    m_patches.reserve (data.getMeshTotal());

    // Keep track of the elements offset.
    GLint firstVertex { 0 };

    // We're going to need two loops to determine the correct mesh and two loops to calculate interpolated co-ordinates.
    for (auto zTile = 0U; zTile < meshCountZ; ++zTile)
    {
        for (auto xTile = 0U; xTile < meshCountX; ++xTile)
        {
            // We need the vertex offsets so can we get the obtain the correct data from the height map.
            const auto xOffset = xTile * divisor,
                       zOffset = zTile * divisor;

            // Cache the values which end the loops so we don't calculate them every time.
            const auto widthEnd = xOffset + divisor,
                       depthEnd = zOffset + divisor;

            for (auto z = zOffset; z < depthEnd; ++z)
            {
                for (auto x = xOffset; x < widthEnd; ++x)
                {
                    // Obtain the necessary UV co-ordinates to create the vertex. 
                    const auto u = (float) x / data.getWidth(),
                               v = (float) z / data.getDepth();

                    addVertex (vertices, heightMap, u, v);
                }
            }

            // Apply some beautiful noise to the terrain.
            applyNoise (vertices, normal, height);

            // Recalculate the normals since we've ruined them with noise.
            calculateNormals (vertices, data);

            // Add the data to the GPU.
            const auto verticesSize = vertices.size() * sizeof (Vertex);

            m_pool.fillSection (BufferType::Vertices, firstVertex * sizeof (Vertex), verticesSize, vertices.data());

            // Check if we're on the last tile on either axis.
            const bool isLastMeshX = xTile == lastMeshX,
                       isLastMeshZ = zTile == lastMeshZ;

            // Now construct the mesh.
            const auto& mesh = m_meshTemplates[templateIndex (isLastMeshX, isLastMeshZ)];

            m_patches.emplace_back (firstVertex, mesh.elementsOffset, mesh.elementCount);

            // Increment everything captain!
            firstVertex += vertices.size();
            vertices.clear();
        }
    }
}


void Terrain::addVertex (std::vector<Vertex>& vector, const HeightMap& heightMap, const float u, const float v)
{
    // We need a vector of 16 control points, four by four.
    const auto bezierWidth     = 4U,
               bezierHeight    = 4U,
               bezierWidthInc  = 3U,
               bezierHeightInc = 3U,
               gridSize        = bezierWidth * bezierHeight;

    static std::vector<glm::vec3> controlPoints { 0 };
    controlPoints.reserve (gridSize);

    // Ensure we don't go beyond the maximum array values.
    const auto maxX = heightMap.getWidth() - 1,
               maxY = heightMap.getHeight() - 1;

    // Calculate where we are in the height map.
    const auto smallX = u * maxX,
               smallY = v * maxY;

    // Determine the base control point.
    const auto unsignedX = (unsigned int) smallX,
               unsignedY = (unsigned int) smallY,
               baseX     = unsignedX - unsignedX % bezierWidthInc,
               baseY     = unsignedY - unsignedY % bezierHeightInc;
               
    auto       basePoint = baseX + baseY * heightMap.getWidth();

    // Check if we need to change the control points stored.
    if (controlPoints.empty() || controlPoints.front() != heightMap[basePoint])
    {
        controlPoints.clear();
        
        const auto newLine = heightMap.getWidth() - bezierWidth;

        for (auto j = 0U; j < bezierHeight; ++j)
        {
            for (auto i = 0U; i < bezierWidth; ++i)
            {
                controlPoints.push_back (heightMap[basePoint++]);
            }

            basePoint += newLine;
        }
    }

    // Create the local co-ordinates used by the bezier surface algorithm.
    const auto localU = (smallX - baseX) / bezierWidthInc,
               localV = (smallY - baseY) / bezierHeightInc;

    vector.push_back (util::BezierSurface::calculatePoint (controlPoints, localU, localV, util::BezierSurface::BezierAlgorithm::Cubic));
}


void Terrain::applyNoise (std::vector<Vertex>& vertices, const NoiseArgs& normal, const NoiseArgs& height)
{
    // Check if we need to bother performing noise at all.
    const bool applyNormalDisplacement = normal.samples > 0,
               applyHeightDisplacement = height.samples > 0;
    
    if (applyNormalDisplacement || applyHeightDisplacement)
    {    
        for (auto& vertex : vertices)
        {
            // Cache the position captain!
            auto& position = vertex.position;

            if (applyNormalDisplacement)
            {
                // Calculate some beautiful normal displacement.
                const auto normalDisplacement = util::NoiseGenerator<float>::brownianMotion (position, normal);
                
                // Move the vertex along its normal vector.
                position += vertex.normal * normalDisplacement;
            }

            if (applyHeightDisplacement)
            {
                // Calculate some beautiful normal displacement.
                const auto heightDisplacement = util::NoiseGenerator<float>::brownianMotion (position, height);
                
                // Move the vertex along its normal vector.
                position += glm::vec3 (0, heightDisplacement, 0);
            }
        }
    }
}


void Terrain::calculateNormals (std::vector<Vertex>& vertices, const ConstructionData& data)
{
    /// This function makes use of the algorithm discussed here: http://www.emeyex.com/site/tuts/VertexNormals.pdf
    const auto divisor = (int) data.getDivisor();

    // The max linked triangles to any vertex is 8.
    const auto maxTriangles = 8,
               linkedCount  = maxTriangles * 2;

    // We're going to need a vector which stores the indices of the linked vertices. 
    std::vector<int> linked { };
    linked.reserve (linkedCount);

    for (auto z = 0; z < divisor; ++z)
    {
        for (auto x = 0; z < divisor; ++z)
        {
            // Start afresh.
            linked.clear();
            
            // We need to know the properties of the current vertex.
            const auto  index    = x + z * divisor;
            auto&       vertex   = vertices[index];
            const auto& position = vertex.position;

            // Fill the vector with linked elements.
            determineLinkedVertices (linked, x, z, divisor);

            // We'll need to accumlate cross product of triangles which use the current vertex.
            auto result = glm::vec3 (0);

            for (auto i = 0U; i < linked.size(); i += 2U)
            {
                // Get the distance between the current vertex and the linked vertices.
                const auto aToB = vertices[linked[i]].position - position,
                           aToC = vertices[linked[i + 1]].position - position;

                // The cross product of the two will provide the basis of calculating a weighted normal.
                const auto cross = glm::cross (aToB, aToC);

                // The magnitude provides a means of calculating the area of the triangle and normalising the vector.
                const auto magnitude = glm::length (cross);
                
                // Area == 1/2 * magnitude, multiply this by the normal of the cross product to get a weighted normal.
                result += (magnitude / 2) * 
                          (cross / magnitude);
            }

            // Normalise it to get a smoothed normal.
            vertex.normal = glm::normalize (result);
        }
    }

    assert (linked.capacity() == linkedCount);
}


void Terrain::determineLinkedVertices (std::vector<int>& linked, const int x, const int z, const int divisor)
{
    // Cache some constants we'll be using.
    const int xLeft = x - 1, xRight = x + 1,
              zDown = z - 1, zUp    = z + 1;
    
    // Neither the X or Y can be negative or larger than divisor-1 (array notation).
    const auto isValid = [divisor] (const int x, const int z)
    {
        return x >= 0 && z >= 0 && x < divisor && z < divisor;
    };

    // Only add the two elements to the vector if they're both valid.
    const auto addToVector = [&, divisor] (const int x1, const int z1, const int x2, const int z2)
    {
        if (isValid (x1, z1) && isValid (x2, z2))
        {
            linked.push_back (x1 + z1 * divisor);
            linked.push_back (x2 + z2 * divisor);
        }
    };

    // This will add the vertices required for triangles with eight potential links.
    const auto eightLinks = [=, &addToVector] (const int x, const int z, const int divisor)
    {
        // \|/
        // -V-  V = Current vertex, this is the shape of the patch to calculate.
        // /|\

        addToVector (xLeft,  zDown, x,      zDown);
        addToVector (x,      zDown, xRight, zDown);
        addToVector (xRight, zDown, xRight, z);
        addToVector (xRight, z,     xRight, zUp);
        addToVector (xRight, zUp,   x,      zUp);
        addToVector (x,      zUp,   xLeft,  zUp);
        addToVector (xLeft,  zUp,   xLeft,  z);
        addToVector (xLeft,  z,     xLeft,  zDown);
    };

    // This will add the vertices required for triangles with four potential linkes.
    const auto fourLinks = [=, &addToVector] (const int x, const int z, const int divisor)
    {
        // /|\
        // -V-  V = Current vertex, this is the shape of the patch to calculate.
        // \|/
        
        addToVector (xLeft,  z,     x,      zDown);
        addToVector (x,      zDown, xRight, z);
        addToVector (xRight, z,     x,      zUp);
        addToVector (x,      zUp,   xLeft,  z);
    };

    // Maybe four lambdas is over the top but none seem worth putting into some kind of utility.
    switch ((x + z) % 2)
    {
        case 0:
            eightLinks (x, z, divisor);
            break;

        case 1:
            fourLinks (x, z, divisor);
            break;
    }
}


size_t Terrain::templateIndex (const bool isLastMeshX, const bool isLastMeshZ) const
{
    // Y + Y = TopRightCorner,
    // Y + N = RightColumn,
    // N + Y = TopRow,
    // N + N = Central.
    if (isLastMeshX)
    {
        return isLastMeshZ ? (size_t) MeshTemplate::TopRightCorner : (size_t) MeshTemplate::RightColumn;
    }

    return isLastMeshZ ? (size_t) MeshTemplate::TopRow : (size_t) MeshTemplate::Central;
}