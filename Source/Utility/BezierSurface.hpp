#ifndef UTILITY_BEZIER_SURFACE_3GP_HPP
#define UTILITY_BEZIER_SURFACE_3GP_HPP


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
    class BezierSurface final
    {
        public:

            /// <summary> Determines how the surface will be generated.
            enum class BezierAlgorithm : unsigned int
            {
                Quadratic = 3,  //!< 3x3 grid is required for quadratic Bezier.
                Cubic     = 4   //!< 4x4 grid is required for cubic Bezier.
            };


            /// <summary> Calculates the vertex of a point on a Bezier surface at the given U and V values. </summary>
            /// <param name="controlPoints"> A vector of 16 control points which make up the curve grid. </param>
            /// <param name="u"> The 0 - 1 U co-ordinate representing a parametric point along the X axis. </param>
            /// <param name="v"> The 0 - 1 V co-ordinate representing a parametric point along the Z axis. </param>
            /// <param name="mode"> The desired Bezier algorithm to use when generating the surface. </param>
            /// <returns> The computed vertex. </returns>
            static Vertex calculatePoint (const std::vector<glm::vec3>& controlPoints, const float u, const float v, const BezierAlgorithm mode);
    };
}


#endif // UTILITY_BEZIER_SURFACE_3GP_HPP