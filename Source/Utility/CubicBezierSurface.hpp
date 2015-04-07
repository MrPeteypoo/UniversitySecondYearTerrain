#ifndef UTILITY_CUBIC_BEZIER_SURFACE_3GP_HPP
#define UTILITY_CUBIC_BEZIER_SURFACE_3GP_HPP


// STL headers.
#include <vector>


// Engine headers.
#include <glm/gtc/type_ptr.hpp>


// Personal headers.
#include <Renderer/Vertex.hpp>


namespace util
{
    /// <summary>
    /// A static helper class which can be used in the construction of Bezier surfaces with GLM.
    /// </summary>
    class CubicBezierSurface final
    {
        public:

            /// <summary> Calculates the vertex of a point on a Bezier surface at the given U and V values. </summary>
            /// <param name="controlPoints"> A vector of 16 control points which make up the curve grid. </param>
            /// <param name="u"> The 0 - 1 U co-ordinate representing a parametric point along the X axis. </param>
            /// <param name="v"> The 0 - 1 V co-ordinate representing a parametric point along the Z axis. </param>
            /// <returns> The computed vertex. </returns>
            static Vertex calculatePoint (const std::vector<glm::vec3>& controlPoints, const float u, const float v);
    };
}


#endif // UTILITY_CUBIC_BEZIER_SURFACE_3GP_HPP