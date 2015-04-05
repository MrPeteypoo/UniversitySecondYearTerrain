#ifndef TERRAIN_3GP_HPP
#define TERRAIN_3GP_HPP


// STL headers.
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

        /// <summary> Prepares the terrain for renderering by initialising it with an OpenGL program. </summary>
        /// <param name="program"> The program to use when preparing the terrain. </param>
        void prepareForRender (const GLuint program);

        /// <summary> Delete any allocated memory. </summary>
        void cleanUp();

        /// <summary> Draw the terrain bro! </summary>
        void draw();

    private:

        //////////////
        // Creation //
        //////////////

        /// <summary> Generates the vertices of the terrain from a height map. </summary>
        /// <param name="heightMap"> The height map containing necessary data for the terrain generation. </param>
        /// <param name="width"> The desired number of vertices wide to generate. </param>
        /// <param name="depth"> The desired number of vertices deep to generate. </param>
        void generateVertices (const HeightMap& heightMap, const unsigned int width, const unsigned int depth);

        /// <summary> Determines a valid divisor to use based on the given dimensions. </summary>
        /// <param name="width"> The desired width of the terrain. </param>
        /// <param name="height"> The desired height of the terrain. </param>
        /// <returns> A valid divisor value which won't exceed the terrain dimensions. </returns>
        unsigned int determineDivisor (const unsigned int width, const unsigned int depth) const;

        /// <summary> Allocates enough memory in the given vectors to begin constructing each terrain patch. </summary>
        /// <param name="vertices"> The Vertex vector to reserve capacity for. </param>
        /// <param name="elements"> The elements vector to reserve capacity for. </param>
        /// <param name="divisor"> The divisor value to use when reserving capacity. </param>
        void allocateLocalMemory (std::vector<Vertex>& vertices, std::vector<unsigned int>& elements, const unsigned int divisor);

        /// <summary> Allocates enough memory in the GPU for the upscaled terrain according to the dimensions given. </summary>
        /// <param name="width"> How many vertices wide is the terrain? </param>
        /// <param name="depth"> How many vertices deep is the terrain? </param>
        void allocateGPUMemory (const unsigned int width, const unsigned int depth);

        /// <summary> Adds a calculated vertex to the given vector from the U and V values passed. </summary>
        /// <param name="vector"> The vector to contain the new Vertex. </param>
        /// <param name="heightMap"> The height map containing desired vertex data. </param>
        /// <param name="u"> A 0 to 1 co-ordinate on the X axis where the vertex should be. </param>
        /// <param name="v"> A 0 to 1 co-ordinate on the Z axis where the vertex should be. </param>
        void addVertex (std::vector<Vertex>& vector, const HeightMap& heightMap, const float u, const float v);

        /// <summary> Uses an algorithm to calculate the element index of a particular patch which are added to the given vector. </summary>
        /// <param name="elements"> The vector to add the elements to. </param>
        /// <param name="xOffset"> The initial elements to start at, this provides an offset to all values generated. </param>
        /// <param name="width"> How many vertices wide the patch is. </param>
        /// <param name="height"> How many vertices deep the patch is. </param>
        /// <param name="startMirror"> Indicates whether the algorithm should start with mirror mode turned on or off. </param>
        void addElements (std::vector<unsigned int>& elements, const unsigned int initial, const unsigned int width, const unsigned int depth);

        ///////////////////
        // Internal data //
        ///////////////////

        MeshPool            m_pool              { };        //!< A pool to store the entire generated terrain inside.
        std::vector<Mesh>   m_patches           { };        //!< A collection of patches which make up the entire terrain.

        unsigned int        m_divisor           { 256 };    //!< The maximum number of vertices wide/deep of each terrain patch.
};

#endif