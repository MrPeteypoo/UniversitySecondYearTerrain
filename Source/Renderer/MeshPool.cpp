#include "MeshPool.hpp"


// STL headers.
#include <stdexcept>
#include <string>
#include <utility>


// Engine headers.
#include <tgl/tgl.h>


// Personal headers.
#include <Renderer/Vertex.hpp>


/////////////////////////////////
// Constructors and destructor //
/////////////////////////////////

MeshPool::MeshPool (MeshPool&& move)
{
    *this = std::move (move);
}


MeshPool& MeshPool::operator= (MeshPool&& move)
{
    // Avoid moving self to self.
    if (this != &move)
    {
        // Delete the old data.
        clear();

        m_vao       = move.m_vao;
        m_vertices  = move.m_vertices;
        m_elements  = move.m_elements;

        // Reset primitives.
        move.m_vao      = 0;
        move.m_vertices = 0;
        move.m_elements = 0;
    }

    return *this;
}


MeshPool::~MeshPool()
{
    // Ensure we don't leak memory.
    clear();
}


/////////////////////
// Data management //
/////////////////////

void MeshPool::generate()
{
    // Ensure we destroy old data.
    clear();

    // Generates the objects, ready for usage by OpenGL.
    glGenVertexArrays (1, &m_vao);
    glGenBuffers (1, &m_vertices);
    glGenBuffers (1, &m_elements);
}


void MeshPool::clear()
{
    // Delete the m_vao and VBOs.
    glDeleteVertexArrays (1, &m_vao);
    glDeleteBuffers (1, &m_vertices);
    glDeleteBuffers (1, &m_elements);
}


void MeshPool::initialiseVAO (const GLuint program)
{
    // Obtain the attribute pointer locations we'll be using to construct the VAO.
    GLint position { glGetAttribLocation (program, "vertex_position") },
          normal   { glGetAttribLocation (program, "vertex_normal") };

    // Initialise the VAO.
    glBindVertexArray (m_vao);

    // Bind the element buffer to the VAO.
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_elements);

    // Enable each attribute pointer.
    glEnableVertexAttribArray (position);
    glEnableVertexAttribArray (normal);

    // Begin creating the vertex attribute pointer from the interleaved buffer.
    glBindBuffer (GL_ARRAY_BUFFER, m_vertices);

    // Set the properties of each attribute pointer.
    glVertexAttribPointer (position, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), TGL_BUFFER_OFFSET (0));
    glVertexAttribPointer (normal,   3, GL_FLOAT, GL_FALSE, sizeof (Vertex), TGL_BUFFER_OFFSET (12));

    // Unbind all buffers.
    glBindVertexArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, 0);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
}


void MeshPool::fillData (const BufferType buffer, const void* const data, const size_t size)
{
    // Simply allocate the data using glBindBuffer.
    const auto bufferData = [=] (const GLuint buffer, const GLenum target)
    {
        glBindBuffer (target, buffer);
        glBufferData (target, size, data, GL_STATIC_DRAW);
        glBindBuffer (target, 0);

        // Ensure we have the allocated memory.
        if (glGetError() == GL_OUT_OF_MEMORY)
        {
            throw std::runtime_error ("MeshPool()::fillData(), unable to allocate " + std::to_string (size) + 
                                      " bytes of data.");
        }
    };

    performBufferOperation (buffer, bufferData);
}


void MeshPool::fillSection (const BufferType buffer, const void* const data, const size_t size, const GLint offset)
{
    // Use glBufferSubData to overwrite some existing memory.
    const auto bufferSubData = [=] (const GLuint buffer, const GLenum target)
    {
        glBindBuffer (target, buffer);
        glBufferSubData (target, offset, size, data);
        glBindBuffer (target, 0);
    };

    performBufferOperation (buffer, bufferSubData);
}


////////////////////
// Implementation //
////////////////////

void MeshPool::performBufferOperation (const BufferType buffer, const BufferOperation& operation)
{
    // Call the desired function with the correct parameters!
    switch (buffer)
    {
        case BufferType::Vertices:
            operation (m_vertices, GL_ARRAY_BUFFER);
            break;

        case BufferType::Elements:
            operation (m_elements, GL_ELEMENT_ARRAY_BUFFER);
            break;

        default:
            throw std::invalid_argument ("MeshPool::performBufferOperation(), given buffer does not exist.");
    }
}