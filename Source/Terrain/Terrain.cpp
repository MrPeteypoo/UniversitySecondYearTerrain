#include "Terrain.hpp"


// Engine headers.
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
    assert (divisor != 0);

    m_divisor = divisor;
}


//////////////
// Creation //
//////////////

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
    generateElements (width, depth);


    //const auto heightMap = tygra::imageFromPNG (file);
    //tygra::Image height_image = tygra::imageFromPNG(m_scene->getTerrainHeightMapName());

	// below is an example of reading the red component of pixel(x,y) as a byte [0,255]
	//uint8_t height = *(uint8_t*)height_image(x, y);

    // below is indicative code for initialising a terrain VAO

    //const int vertex_count = mesh.GetVertexCount();
    //const int element_count = mesh.GetElementCount();
    //const auto& elements = mesh.GetElementArray();
    //const auto& positions = mesh.GetPositionArray();

    //glGenBuffers(1, &m_terrainMesh.element_vbo);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_terrainMesh.element_vbo);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER,
    //    elements.size() * sizeof(unsigned int),
    //    elements.data(), GL_STATIC_DRAW);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //m_terrainMesh.element_count = elements.size();

    //glGenBuffers(1, &m_terrainMesh.position_vbo);
    //glBindBuffer(GL_ARRAY_BUFFER, m_terrainMesh.position_vbo);
    //glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3),
    //             positions.data(), GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    //glGenVertexArrays(1, &m_terrainMesh.vao);
    //glBindVertexArray(m_terrainMesh.vao);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_terrainMesh.element_vbo);
    //glBindBuffer(GL_ARRAY_BUFFER, m_terrainMesh.position_vbo);
    //glEnableVertexAttribArray(kVertexPosition);
    //glVertexAttribPointer(kVertexPosition, 3, GL_FLOAT, GL_FALSE,
    //                      sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);
}


void Terrain::generateVertices (const HeightMap& heightMap, const unsigned int width, const unsigned int depth)
{
    // Allocate enough data in the GPU for the terrain.
    const auto verticesSize = width * depth * sizeof (Vertex);
    
    m_pool.fillData (BufferType::Vertices, nullptr, verticesSize);

    // Create the vertices.
    const auto divisor = determineDivisor (heightMap);
    
    std::vector<Vertex> vertices (divisor);
}


/////////////
// Cleanup //
/////////////

void Terrain::cleanUp()
{
    m_pool.clear();
    m_patches.clear();
}