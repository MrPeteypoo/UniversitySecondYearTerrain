#include "TerrainConstructionData.hpp"


// STL headers.
#include <cassert>



//////////////////
// Constructors //
//////////////////

Terrain::ConstructionData::ConstructionData (const unsigned int width, const unsigned int depth, const unsigned int divisor,
                                             const float worldWidth, const float worldDepth)
{
    recalculate (width, depth, divisor, worldWidth, worldDepth);
}


Terrain::ConstructionData::ConstructionData (ConstructionData&& move)
{
    *this = std::move (move);
}


Terrain::ConstructionData& Terrain::ConstructionData::operator= (ConstructionData&& move)
{
    if (this != &move)
    {
        // Copy data across.
        m_width        = move.m_width;
        m_depth        = move.m_depth;
        m_vertexCount  = move.m_vertexCount;

        m_divisor      = move.m_divisor;
        m_meshVertices = move.m_meshVertices;
        m_meshCountX   = move.m_meshCountX;
        m_meshCountZ   = move.m_meshCountZ;
        m_meshTotal    = move.m_meshTotal;

        m_worldWidth   = move.m_worldWidth;
        m_worldDepth   = move.m_worldDepth;
        m_worldArea    = move.m_worldArea;

        // Reset primitives. You don't know how sick I am of writing these. Why can't VS support default move constructors?
        move.m_width        = 0;
        move.m_depth        = 0;
        move.m_vertexCount  = 0;

        move.m_divisor      = 0;
        move.m_meshVertices = 0;
        move.m_meshCountX   = 0;
        move.m_meshCountZ   = 0;
        move.m_meshTotal    = 0;

        m_worldWidth        = 0.f;
        m_worldDepth        = 0.f;
        m_worldArea         = 0.f;
    }

    return *this;
}


//////////////////////
// Public interface //
//////////////////////

void Terrain::ConstructionData::recalculate (const unsigned int width, const unsigned int depth, const unsigned int divisor,
                                             const float worldWidth, const float worldDepth)
{
    // Don't allow stupidity to get the better of us!
    assert (width >= 2 && depth >= 2 && divisor >= 2);
    
    // Calculate the world captain.......
    m_width        = width;
    m_depth        = depth;
    m_vertexCount  = width * depth;
    
    m_divisor      = divisor;
    m_meshVertices = divisor * divisor;
    m_meshCountX   = width / divisor;
    m_meshCountZ   = depth / divisor;
    m_meshTotal    = m_meshCountX * m_meshCountZ;

    m_worldWidth   = std::abs (worldWidth);
    m_worldDepth   = std::abs (worldDepth);
    m_worldArea    = m_worldWidth * m_worldDepth;
}