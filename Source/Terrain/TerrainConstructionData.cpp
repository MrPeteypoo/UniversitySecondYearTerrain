#include "TerrainConstructionData.hpp"


// STL headers.
#include <cassert>



//////////////////
// Constructors //
//////////////////

Terrain::ConstructionData::ConstructionData (const unsigned int width, const unsigned int depth, const unsigned int divisor)
{
    recalculate (width, depth, divisor);
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
        m_width       = move.m_width;
        m_depth       = move.m_depth;
        m_vertexCount = move.m_vertexCount;

        m_divisor     = move.m_divisor;
        m_remainderX  = move.m_remainderX;
        m_remainderZ  = move.m_remainderZ;

        m_meshCountX  = move.m_meshCountX;
        m_meshCountZ  = move.m_meshCountZ;
        m_meshTotal   = move.m_meshTotal;

        // Reset primitives. You don't know how sick I am of writing these. Why can't VS support default move constructors?
        move.m_width       = 0;
        move.m_depth       = 0;
        move.m_vertexCount = 0;

        move.m_divisor     = 0;
        move.m_remainderX  = 0;
        move.m_remainderZ  = 0;

        move.m_meshCountX  = 0;
        move.m_meshCountZ  = 0;
        move.m_meshTotal   = 0;
    }

    return *this;
}


//////////////////////
// Public interface //
//////////////////////

void Terrain::ConstructionData::recalculate (const unsigned int width, const unsigned int depth, const unsigned int divisor)
{
    // Don't allow stupidity to get the better of us!
    assert (width >= 2 && depth >= 2 && divisor >= 2);
    
    // Calculate the world captain.......
    m_width       = width;
    m_depth       = depth;
    m_vertexCount = width * depth;
    
    m_divisor     = divisor;
    m_remainderX  = width % divisor;
    m_remainderZ  = depth % divisor;

    m_meshCountX  = width / divisor;
    m_meshCountZ  = depth / divisor;

    if (hasRemainderX())
    {
        ++m_meshCountX;
    }

    if (hasRemainderZ())
    {
        ++m_meshCountZ;
    }

    m_meshTotal = m_meshCountX * m_meshCountZ;
}