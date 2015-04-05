#ifndef MESH_POOL_GL_3GP_HPP
#define MESH_POOL_GL_3GP_HPP


// STL headers.
#include <functional>


// OpenGL aliases.
using GLenum    = unsigned int;
using GLint     = int;
using GLuint    = unsigned int;


/// <summary>
/// Specifies the type of buffer when allocating data.
/// </summary>
enum class BufferType : int
{
    Vertices = 0, //!< Specifies the vertices VBO.
    Elements = 1  //!< Specifies the elements VBO.
};


/// <summary> 
/// A basic structure containing the data required to store a pool of meshes inside the GPU. Remember to call MeshPool::generate()
/// to initialise the MeshPool. Typically used in combination with MeshGL.
/// </summary>
class MeshPool final
{
    public:

        /////////////////////////////////
        // Constructors and destructor //
        /////////////////////////////////

        MeshPool()                                  = default;

        MeshPool (MeshPool&& move);
        MeshPool& operator= (MeshPool&& move);
        ~MeshPool();
    
        MeshPool (const MeshPool& copy)             = delete;
        MeshPool& operator= (const MeshPool& copy)  = delete;


        /////////////
        // Getters //
        /////////////

        /// <summary> Gets the ID of the VAO used by this MeshPool. </summary>
        const GLuint& getVAO() const            { return m_vao; }

        /// <summary> Gets the ID of the VBO used to store vertices data for all meshes contained. </summary>
        const GLuint& getVerticesVBO() const    { return m_vertices; }

        /// <summary> Gets the ID of the VBO used to store element data for all meshes contained. </summary>
        const GLuint& getElementsVBO() const    { return m_elements; }


        /////////////////////
        // Data management //
        /////////////////////

        /// <summary> Causes all objects to be generated, ready for use. This needs to be called before use. </summary>
        void generate();

        /// <summary> Deletes the buffers and VAO associated with the MeshPool </summary>
        void clear();

        /// <summary> Initialises the VAO ready for renderering. </summary>
        /// <param name="program"> The program to obtain attribute location from. </param>
        void initialiseVAO (const GLuint program);

        /// <summary> Fills the desired buffer with the given data. This will completely wipe the previous contents. </summary>
        /// <param name="buffer"> The buffer to fill with data. </param>
        /// <param name="size"> The amount of data to allocate in bytes. </param>
        /// <param name="data"> A pointer to the data to fill the buffer with. This can be a nullptr. </param>
        void fillData (const BufferType buffer, const size_t size, const void* const data);

        /// <summary> Fills a section of the desired buffer with the given data. This will not allocate memory. </summary>
        /// <param name="buffer"> The buffer to modify. </param>
        /// <param name="offset"> The starting offset of the buffer to copy data to. </param>
        /// <param name="size"> How many bytes to copy from the data. </param>
        /// <param name="data"> A pointer to the data to fill the section with. This cannot be a nullptr. </param>
        void fillSection (const BufferType buffer, const GLint offset, const size_t size, const void* const data);

    private:

        ////////////////////
        // Implementation //
        ////////////////////

        // Aliases.
        using BufferOperation = std::function<void (GLuint&, const GLenum)>;


        /// <summary> Performs an operation on the desired buffer. </summary>
        /// <param name="buffer"> The type of buffer. </param>
        /// <param name="operation"> A function to be called which will be given the correct buffer ID and GLenum. </param>
        void performBufferOperation (const BufferType buffer, const BufferOperation& operation);


        ///////////////////
        // Internal data //
        ///////////////////

        GLuint  m_vao         { 0 };    //!< The vertex array object to bind when drawing from the pool.
        GLuint  m_vertices    { 0 };    //!< A buffer containing the attributes of each vertex for every mesh stored.
        GLuint  m_elements    { 0 };    //!< An elements index buffer for every mesh.
};

#endif