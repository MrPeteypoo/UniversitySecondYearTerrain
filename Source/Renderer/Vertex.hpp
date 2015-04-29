#ifndef VERTEX_3GP_HPP
#define VERTEX_3GP_HPP


// Engine headers.
#include <glm/gtc/type_ptr.hpp>


/// <summary> 
/// An object with a position, and a normal vector to represent a single vertex.
/// </summary>
struct Vertex final
{
    //////////
    // Data //
    //////////

    glm::vec3   position        { 0 },  //!< The position vector of the vertex.
                normal          { 0 };  //!< The normal vector for the vertex.

    
    /////////////////////////////////
    // Constructors and destructor //
    /////////////////////////////////

    /// <summary> Construct a Vertex with the given values. </summary>
    /// <param name="position"> The initial position value. </param>
    /// <param name="normal"> The initial normal value. </param>
    Vertex (const glm::vec3& position, const glm::vec3& normal)  
        : position (position), normal (normal) { }

    Vertex()                                = default;
    Vertex (const Vertex& copy)             = default;
    Vertex& operator= (const Vertex& copy)  = default;
    
    Vertex (Vertex&& move);
    Vertex& operator= (Vertex&& move);

    ~Vertex()                               = default;
};

#endif // VERTEX_3GP_HPP