#ifndef POOL_SEGMENT_3GP_HPP
#define POOL_SEGMENT_3GP_HPP


// Engine headers.
#include <Renderer/Mesh.hpp>
#include <Utility/Rectangle.hpp>


/// <summary>
/// A segment of a MeshPoolGL which simply contains the data required to draw a certain section of the pool
/// and an axis-aligned rectangle which can be used for frustum culling.
/// </summary>
struct PoolSegment final
{        
    //////////
    // Data //
    //////////
    
    Mesh            mesh    { };    //!< The information required to draw this particular segment of a mesh.
    Rectangle<int>  area    { };    //!< The axis-aligned area of the mesh, used for culling detection.


    /////////////////////////////////
    // Constructors and destructor //
    /////////////////////////////////

    /// <summary> Contains a pool segment with the given values. </summary>
    /// <param name="mesh"> The information required to draw this particular segment of a mesh. </param>
    /// <param name="area"> The axis-aligned area of the mesh, used for culling detection. </param>
    PoolSegment (const Mesh& mesh, const Rectangle<int>& area)
        : mesh (mesh), area (area) { }

    PoolSegment (PoolSegment&& move);
    PoolSegment& operator= (PoolSegment&& move);

    PoolSegment()                                       = default;
    PoolSegment (const PoolSegment& copy)               = default;
    PoolSegment& operator= (const PoolSegment& copy)    = default;
    ~PoolSegment()                                      = default;
};

#endif