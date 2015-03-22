#include "Mesh.hpp"


// STL headers.
#include <utility>


///////////////////////
// Move constructors //
///////////////////////

Mesh::Mesh (Mesh&& move)
{
    *this = std::move (move);
}


Mesh& Mesh::operator= (Mesh&& move)
{
    // Avoid moving self to self.
    if (this != &move)
    {
        firstVertex     = move.firstVertex;
        elementsOffset  = move.elementsOffset;
        elementCount    = move.elementCount;

        // Reset primitives.
        move.firstVertex    = 0;
        move.elementsOffset = 0;
        move.elementCount   = 0;
    }

    return *this;
}