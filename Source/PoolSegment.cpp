#include "PoolSegment.hpp"


// STL headers.
#include <utility>


///////////////////////
// Move constructors //
///////////////////////

PoolSegment::PoolSegment (PoolSegment&& move)
{
    *this = std::move (move);
}


PoolSegment& PoolSegment::operator= (PoolSegment&& move)
{
    if (this != &move)
    {
        mesh = std::move (move.mesh);
        area = std::move (move.area);
    }

    return *this;
}