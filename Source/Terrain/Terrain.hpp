#ifndef TERRAIN_3GP_HPP
#define TERRAIN_3GP_HPP


// STL headers.
#include <vector>


// Personal headers.
#include <Renderer/Mesh.hpp>
#include <Renderer/MeshPool.hpp>


// Forward decalarations.
class HeightMap;


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

        
        //////////////
        // Creation //
        //////////////

        /// <summary> Attempt to build the terrain from a given height map. </summary>
        /// <param name="heightMap"> The height map data to load from. </param>
        /// <param name="upscaledWidth"> How many vertices wide the final terrain should be, leave this at 0 to avoid upscaling the width. </param>
        /// <param name="upscaledDepth"> How many vertices deep the final terrain should be, leave this at 0 to avoid upscaling the depth. </param>
        /// <returns> Whether the terrain was successfully built. </returns>
        void buildFromHeightMap (const HeightMap& heightMap, const unsigned int upscaledWidth = 0, const unsigned int upscaledDepth = 0);


        /////////////
        // Cleanup //
        /////////////

        /// <summary> Delete any allocated memory. </summary>
        void cleanUp();

    private:

        //////////////
        // Creation //
        //////////////

        /// <summary> Generates the vertices of the terrain from a height map. </summary>
        /// <param name="heightMap"> The height map containing necessary data for the terrain generation. </param>
        /// <param name="width"> The desired number of vertices wide to generate. </param>
        /// <param name="depth"> The desired number of vertices deep to generate. </param>
        void generateVertices (const HeightMap& heightMap, const unsigned int width, const unsigned int depth);

        /// <summary> Generates the element array buffer of the terrain from the given dimensions. </summary>
        /// <param name="width"> How many vertices wide the terrain is. </param>
        /// <param name="height"> How many vertices deep the terrain is. </param>
        void generateElements (const unsigned int width, const unsigned int depth);

        /// <summary> Determines a valid divisor to use based on the given height map. </summary>
        /// <param name="heightMap"> The height map containing the dimensions to check against. </param>
        /// <returns> A valid divisor value which won't exceed the height maps dimensions. </returns>
        unsigned int determineDivisor (const HeightMap& heightMap) const;

        ///////////////////
        // Internal data //
        ///////////////////

        MeshPool            m_pool              { };        //!< A pool to store the entire generated terrain inside.
        std::vector<Mesh>   m_patches           { };        //!< A collection of patches which make up the entire terrain.

        unsigned int        m_divisor           { 256 };    //!< The maximum number of vertices wide/deep of each terrain patch.
};

#endif