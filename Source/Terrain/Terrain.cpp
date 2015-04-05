#include "Terrain.hpp"


// Engine headers.
#include <tgl/tgl.h>
#include <tygra/FileHelper.hpp>


// Personal headers.
#include <Renderer/Vertex.hpp>
#include <Terrain/HeightMap.hpp>



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

    // Generate the terrain!
    generateVertices (heightMap, width, depth);
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
}


void Terrain::cleanUp()
{
    // Put that memory away!
    m_pool.clear();
    m_patches.clear();
}


//////////////
// Creation //
//////////////

void Terrain::generateVertices (const HeightMap& heightMap, const unsigned int width, const unsigned int depth)
{
    // Don't allow something stupid like width || depth <= 1.
    assert (width > 1 && depth > 1);

    // Obtain the correct divisor so we can segment the terrain properly.
    const auto divisor = determineDivisor (width, depth);

    // We need to know the dimensions of the terrain in tiles. Take into account remainders as additional tiles.
    const auto widthRemainder = width % divisor,
               depthRemainder = depth % divisor,
               
               tileWidth = widthRemainder > 0 ? width / divisor + 1 : width / divisor,
               tileDepth = depthRemainder > 0 ? depth / divisor + 1 : depth / divisor,
               meshTotal = tileWidth * tileDepth;

    m_patches.reserve (meshTotal);

    // We're going to need some vectors to store the data of each patch.
    std::vector<Vertex>       vertices { };
    std::vector<unsigned int> elements { };

    // Allocate enough memory to construct the terrain.
    allocateLocalMemory (vertices, elements, divisor);
    allocateGPUMemory (width, depth);

    // Keep track of the elements offset.
    GLint  firstVertex    { 0 };
    GLuint elementsOffset { 0 };

    // We're going to need three loops, one is updates the Mesh vector and the other two create the vertices.
    for (auto zTile = 0U; zTile < tileDepth; ++zTile)
    {
        for (auto xTile = 0U; xTile < tileWidth; ++xTile)
        {
            // We need the vertex offsets so can we get the obtain the correct data from the height map.
            const auto xOffset = xTile * divisor,
                       zOffset = zTile * divisor;

            // Make sure we don't cause access violation errors by checking if we're using an oddly sized heightmap providing remainders.
            const auto patchWidth = widthRemainder > 0 && xTile == (tileWidth - 1) ? widthRemainder : divisor,
                       patchDepth = depthRemainder > 0 && zTile == (tileDepth - 1) ? depthRemainder : divisor;

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

            // Before we create the patch we need the elements array.
            addElements (elements, (unsigned int) firstVertex, patchWidth, patchDepth);

            // Add the data to the GPU.
            m_pool.fillSection (BufferType::Vertices, vertices.data(), vertices.size() * sizeof (Vertex), firstVertex * sizeof (Vertex));
            m_pool.fillSection (BufferType::Elements, elements.data(), elements.size() * sizeof (unsigned int), elementsOffset);

            // Now construct the mesh.
            m_patches.emplace_back (firstVertex, elementsOffset, elements.size());

            // Increment everything captain!
            firstVertex    += vertices.size();
            elementsOffset += elements.size() * sizeof (unsigned int);

            vertices.clear();
            elements.clear();
        }
    }
}


unsigned int Terrain::determineDivisor (const unsigned int width, const unsigned int depth) const
{
    // If the width or depth exceeds the divisor we must use that value.
    if (m_divisor > width && width < depth)
    {
        return width;
    }

    if (m_divisor > depth && depth < width)
    {
        return depth;
    }

    return m_divisor;
}


void Terrain::allocateLocalMemory (std::vector<Vertex>& vertices, std::vector<unsigned int>& elements, const unsigned int divisor)
{
    // Reserve enough memory to create the vertices. See Terrain::allocateGPUMemory() for the elementCount calculation.
    const auto vertexCount  = divisor * divisor;
    const auto elementCount = (divisor - 1) * (divisor - 1) * 2 * 3;

    vertices.reserve (vertexCount);
    elements.reserve (elementCount);
}


void Terrain::allocateGPUMemory (const unsigned int width, const unsigned int depth)
{
    // Vertex count is simple. 
    const auto vertexCount   = width * depth;

    // We calculate the quad count by subtracting one from the width and depth then obtaining the area. Two triangles make a quad.
    const auto triangleCount = (width - 1) * (depth - 1) * 2;

    // Three elements are required to draw a single triangle.
    const auto elementCount  = triangleCount * 3;

    // Now we can convert these values to bytes and allocate memory. Elements are stored using an unsigned int.
    const auto verticesSize  = vertexCount * sizeof (Vertex),
               elementsSize  = elementCount * sizeof (unsigned int);

    // Finally allocate the memory.
    m_pool.fillData (BufferType::Vertices, nullptr, verticesSize);
    m_pool.fillData (BufferType::Elements, nullptr, elementsSize);
}


void Terrain::addElements (std::vector<unsigned int>& elements, const unsigned int initial, const unsigned int width, const unsigned int depth)
{
    /// We create an alternating square pattern for efficiency. It should look as shown below.
    /// 
    /// |/|\|/|
    /// |\|/|\|
    /// |/|\|/|
    ///     

    // Create a function which creates the lower /| && \| triangle.
    const auto lowerTriangle = [&elements] (const unsigned int current, const unsigned int width, const bool mirror)
    {
        const auto triangleEnd = mirror ? current + width : current + width + 1;

        elements.push_back (current);
        elements.push_back (current + 1);        
        elements.push_back (triangleEnd);
    };

    // Create a function which creates the upper |/ && |\ triangle.
    const auto upperTriangle = [&elements] (const unsigned int current, const unsigned int width, const bool mirror)
    {
        const auto triangleEnd = mirror ? current + 1 : current;

        elements.push_back (current + width + 1);
        elements.push_back (current + width);
        elements.push_back (triangleEnd);
    };

    // Use a boolean flag to determine if we should mirror the triangles or not.
    bool mirror = false;

    // Prevent overflow by reducing the width and depth by one.
    const auto endWidth = width - 1,
               endDepth = depth - 1;

    // Create a quad at a time.
    for (auto z = 0U; z < endDepth; ++z)
    {
        for (auto x = 0U; x < endWidth; ++x)
        {
            // Calculate the current vertex.
            const auto vertex = initial + (x + z * width);
            
            lowerTriangle (vertex, width, mirror);
            upperTriangle (vertex, width, mirror);
        
            mirror = !mirror;
        }
    }
}