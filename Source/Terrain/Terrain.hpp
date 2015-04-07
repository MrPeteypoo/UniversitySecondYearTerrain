#ifndef TERRAIN_3GP_HPP
#define TERRAIN_3GP_HPP


// STL headers.
#include <array>
#include <vector>


// Personal headers.
#include <Renderer/Mesh.hpp>
#include <Renderer/MeshPool.hpp>


// Forward decalarations.
class HeightMap;
struct Vertex;


/// <summary>
/// A terrain class which loads a heightmap into a mesh pool, upscales the data if desired and splits
/// it up into segments which can be used for frustum culling.
/// </summary>
class Terrain final
{
    public:

        /////////////////////////////////
        // Constructors and destructor //
        /////////////////////////////////

        Terrain()                                   = default;

        Terrain (Terrain&& move);
        Terrain& operator= (Terrain&& move);
        ~Terrain();

        Terrain (const Terrain& copy)               = delete;
        Terrain& operator= (const Terrain& copy)    = delete;

        /////////////////////////
        // Getters and setters //
        /////////////////////////

        /// <summary> Gets the maxinum number of vertices wide/deep of each terrain patch. </summary>
        unsigned int getDivisor() const { return m_divisor; }

        /// <summary> Sets the divisor used when building terrain. Note this value will only be used during future build calls. </summary>
        /// <param name="divisor"> The maximum numbers of vertices wide/deep of each terrain patch. </param>
        void setDivisor (const unsigned int divisor);

        
        //////////////////////
        // Public interface //
        //////////////////////

        /// <summary> Attempt to build the terrain from a given height map. </summary>
        /// <param name="heightMap"> The height map data to load from. </param>
        /// <param name="upscaledWidth"> How many vertices wide the final terrain should be, leave this at 0 to avoid upscaling the width. </param>
        /// <param name="upscaledDepth"> How many vertices deep the final terrain should be, leave this at 0 to avoid upscaling the depth. </param>
        /// <returns> Whether the terrain was successfully built. </returns>
        void buildFromHeightMap (const HeightMap& heightMap, const unsigned int upscaledWidth = 0, const unsigned int upscaledDepth = 0);

        /// <summary> Delete any allocated memory. </summary>
        void cleanUp();

        /// <summary> Prepares the terrain for renderering by initialising it with an OpenGL program. </summary>
        /// <param name="program"> The program to use when preparing the terrain. </param>
        void prepareForRender (const GLuint program);

        /// <summary> Draw the terrain bro! </summary>
        void draw();

    private:

        //////////////////////////
        // Member classes/enums //
        //////////////////////////

        class ConstructionData;

        /// <summary>
        /// Contains the index for use in the m_meshTemplates array to obtain the correct element count and offset data.
        /// </summary>
        enum class MeshTemplate : unsigned int
        {
            Central,        //!< Normal terrain patches.
            TopRow,         //!< Patches on the top-most row.
            RightColumn,    //!< Patches on the right-most column.
            TopRightCorner, //!< The top-right corner patch.
            Count           //!< How many templates exist. NEVER MOVE THIS! IT MUST ALWAYS BE LAST!
        };

        /// <summary>
        /// Indicates how the stitching function should process the data given.
        /// </summary>
        enum class StitchingMode : int
        {
            XAxis,  //!< Stitch the X axis together.
            ZAxis,  //!< Stitch the Z axis together.
            Corner  //!< Fill in the missing corner.
        };

        
        //////////////
        // Creation //
        //////////////

        /// <summary> Determines a valid divisor to use based on the given dimensions. </summary>
        /// <param name="width"> The desired width of the terrain. </param>
        /// <param name="depth"> The desired depth of the terrain. </param>
        /// <returns> A valid divisor value which won't exceed the terrain dimensions. </returns>
        unsigned int determineDivisor (const unsigned int width, const unsigned int depth) const;

        /// <summary> Allocates enough memory in the GPU for the upscaled terrain according to the data given. </summary>
        /// <param name="data"> Contains the data required to allocate enough memory for the terrain. </param>
        void allocateGPUMemory (const ConstructionData& data);


        //////////////////////
        // Element creation //
        //////////////////////
        
        /// <summary> Generates the elements data required for the terrain and loads it into the GPU. </summary>
        /// <param name="data"> The data required to create the correct element data. </param>
        void generateElements (const ConstructionData& data);

        /// <summary> Calculates the element array required to render an entire patch of terrain. </summary>
        /// <param name="elements"> The vector to add the elements to. </param>
        /// <param name="width"> How many vertices wide the patch is. </param>
        /// <param name="height"> How many vertices deep the patch is. </param>
        void addElements (std::vector<unsigned int>& elements, const unsigned int width, const unsigned int depth);

        /// <summary> Add supplementary stitching to the list of elements to ensure the terrain joins correctly. </summary>
        /// <param name="elements"> The vector to add the elements to. </param>
        /// <param name="data"> The data required for the stitching to calculate required values for. </param>
        /// <param name="length"> How much long the stitching should be. </param>
        /// <param name="mode"> Determines what type of stitching will be performed. </param>
        void addStitching (std::vector<unsigned int>& elements, const ConstructionData& data, const unsigned int length, const StitchingMode mode);
        

        ///////////////////////
        // Vertices creation //
        ///////////////////////

        /// <summary> Generates the vertices of the terrain from a height map. </summary>
        /// <param name="heightMap"> The height map containing necessary data for the terrain generation. </param>
        /// <param name="data"> The data required to construct the terrain to specific dimensions. </param>
        void generateVertices (const HeightMap& heightMap, const ConstructionData& data);

        
        /// <summary> Adds a calculated vertex to the given vector from the U and V values passed. </summary>
        /// <param name="vector"> The vector to contain the new Vertex. </param>
        /// <param name="heightMap"> The height map containing desired vertex data. </param>
        /// <param name="u"> A 0 to 1 co-ordinate on the X axis where the vertex should be. </param>
        /// <param name="v"> A 0 to 1 co-ordinate on the Z axis where the vertex should be. </param>
        void addVertex (std::vector<Vertex>& vector, const HeightMap& heightMap, const float u, const float v);

        /// <summary> Obtains the index of the template to use for a mesh with the given properties. </summary>
        /// <param name="isLastMeshX"> Is the mesh the last mesh on the X axis? </param>
        /// <param name="isLastMeshZ"> Is the mesh the last mesh on the Z axis? </param>
        /// <returns> An index value. </returns>
        size_t templateIndex (const bool isLastMeshX, const bool isLastMeshZ) const;

        ///////////////////
        // Internal data //
        ///////////////////

        /// <summary>
        /// Contains the element count and offset required by the four unique terain patch types. The types are central,
        /// top, right and top right. Each requires unique element data to stitch the terrain together.
        /// </summary>
        using MeshTemplates = std::array<Mesh, (size_t) MeshTemplate::Count>;


        MeshPool            m_pool          { };        //!< A pool to store the entire generated terrain inside.
        std::vector<Mesh>   m_patches       { };        //!< A collection of patches which make up the entire terrain.
        MeshTemplates       m_meshTemplates { };        //!< Each Mesh has the element offset and count required for the four patch types.

        unsigned int        m_divisor       { 256 };    //!< The maximum number of vertices wide/deep of each terrain patch.
};

#endif