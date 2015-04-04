#include "Vertex.hpp"


// STL headers.
#include <utility>



//////////////////
// Constructors //
//////////////////

Vertex::Vertex (Vertex&& move)
{
    *this = std::move (move);
}


Vertex& Vertex::operator= (Vertex&& move)
{
    // Avoid moving self to self.
    if (this != &move)
    {
        position        = std::move (move.position);
        normal          = std::move (move.normal);
    }

    return *this;
}

#pragma endregion