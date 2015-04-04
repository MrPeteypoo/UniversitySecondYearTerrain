#ifndef MESH_GL_3GP_HPP
#define MESH_GL_3GP_HPP


// OpenGL aliases.
using GLint  = int;
using GLuint = unsigned int;


/// <summary> 
/// Contains the data required to draw the a mesh from a pool of meshes.
/// </summary>
struct Mesh final
{
    //////////
    // Data //
    //////////

    GLint   firstVertex     { 0 };  //!< The index of a VBO where the vertices for the mesh begin.
    GLuint  elementsOffset  { 0 };  //!< An offset in bytes used to draw the mesh in the scene.
    GLuint  elementCount    { 0 };  //!< Indicates how many elements in the elements buffer define the mesh.

    
    /////////////////////////////////
    // Constructors and destructor //
    /////////////////////////////////

    /// <summary> Construct a Mesh with the given values. </summary>
    /// <param name="firstVertex"> The index of a VBO where the vertices for the mesh begin. </param>
    /// <param name="elementsOffset"> An offset in bytes used to draw the mesh in the scene. </param>
    /// <param name="elementCount"> Indicates how many elements in the elements buffer define the mesh. </param>
    Mesh (const GLint firstVertex, const GLint elementsOffset, const GLuint elementCount)
        : firstVertex (firstVertex), elementsOffset (elementsOffset), elementCount (elementCount) {}

    Mesh (Mesh&& move);
    Mesh& operator= (Mesh&& move);
    
    Mesh()                              = default;
    Mesh (const Mesh& copy)             = default;
    Mesh& operator= (const Mesh& copy)  = default;
    ~Mesh()                             = default;
};

#endif