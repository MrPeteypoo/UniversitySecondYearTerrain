#ifndef TERRAIN_3GP_HPP
#define TERRAIN_3GP_HPP


// STL headers.
#include <vector>


// Engine headers.
#include <Renderer/MeshPool.hpp>
#include <Renderer/PoolSegment.hpp>


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

        Terrain (Terrain&& move);
        Terrain& operator= (Terrain&& move);

        Terrain()                                   = default;
        Terrain (const Terrain& copy)               = default;
        Terrain& operator= (const Terrain& copy)    = default;
        ~Terrain()                                  = default;

    private:

        ///////////////////
        // Internal data //
        ///////////////////

        MeshPool                    m_pool      { };    //!< A pool to store the entire generated terrain inside.
        std::vector<PoolSegment>    m_patches   { };    //!< A collection of patches which make up the entire terrain.
};

#endif