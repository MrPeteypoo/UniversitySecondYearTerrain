#ifndef MESH_POOL_GL_3GP_HPP
#define MESH_POOL_GL_3GP_HPP


// OpenGL aliases.
using GLint  = int;
using GLuint = unsigned int;


/// <summary> 
/// A basic structure containing the data required to store a pool of meshes inside the GPU. 
/// Typically used in combination with MeshGL.
/// </summary>
struct MeshPool final
{
    ///////////////////
    // Internal data //
    ///////////////////

    GLuint  vao         { 0 };  //!< The vertex array object to bind when drawing from the pool.
    GLuint  vertices    { 0 };  //!< A buffer containing the attributes of each vertex for every mesh stored.
    GLuint  elements    { 0 };  //!< An elements index buffer for every mesh.

    
    /////////////////////////////////
    // Constructors and destructor //
    /////////////////////////////////

    /// <summary> Construct a mesh pool with the given values.. </summary>
    /// <param name="firstVertex"> The vertex array object to bind when drawing from the pool. </param>
    /// <param name="elementsOffset"> A buffer containing the attributes of each vertex for every mesh stored. </param>
    /// <param name="elementCount"> An elements index buffer for every mesh. </param>
    MeshPool (const GLuint vao, const GLint vertices, const GLuint elements)
        : vao (vao), vertices (vertices), elements (elements) {}

    MeshPool (MeshPool&& move);
    MeshPool& operator= (MeshPool&& move);
    
    MeshPool()                                  = default;
    MeshPool (const MeshPool& copy)             = default;
    MeshPool& operator= (const MeshPool& copy)  = default;
    ~MeshPool()                                 = default;
};

#endif