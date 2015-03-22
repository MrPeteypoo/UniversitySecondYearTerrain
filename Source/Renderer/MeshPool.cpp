#include "MeshPool.hpp"


// STL headers.
#include <utility>


///////////////////////
// Move constructors //
///////////////////////

MeshPool::MeshPool (MeshPool&& move)
{
    *this = std::move (move);
}


MeshPool& MeshPool::operator= (MeshPool&& move)
{
    // Avoid moving self to self.
    if (this != &move)
    {
        vao         = move.vao;
        vertices    = move.vertices;
        elements    = move.elements;

        // Reset primitives.
        move.vao        = 0;
        move.vertices   = 0;
        move.elements   = 0;
    }

    return *this;
}