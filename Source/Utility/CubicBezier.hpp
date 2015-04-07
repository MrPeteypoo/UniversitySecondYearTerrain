#ifndef UTILITY_CUBIC_BEZIER_3GP_HPP
#define UTILITY_CUBIC_BEZIER_3GP_HPP


#include <cmath>


namespace util
{
    /// <summary>
    /// A static class containing cubic Bezier curve calculation functionality. This is completely header based so hopefully
    /// the compiler inlines everything providing some nice speed benefits.
    /// </summary>
    class CubicBezier final
    {
        public:

            /// <summary>
            /// An enumeration to represent which derivative is desired from a curve point on the path.
            /// </summary>
            enum class Derivative : int
            {
                None    = 0,    //!< Provides a position.
                First   = 1     //!< Provides the tangent.
            };

            /// <summary> Obtain the Bernstein polynominal for the given index and delta value. </summary>
            /// <param name="index"> An index from 0 - 3, this indicates the polynominal desired. </param>
            /// <param name="delta"> The delta value from 0 - 1 which acts as an interpolation factor. </param>
            /// <param name="derivative"> Which polynominal derivative would you like? This effects how it can be used. </param>
            /// <returns> The calculated polynominal. </returns>
            template <typename T> inline static T bernstein (const unsigned int index, const T delta, const Derivative derivative = Derivative::None);

        private:

            /// <summary> Obtains the Bernstein polynominal used in calculating positions on Bezier curves. </summary>
            template <typename T> inline static T bernsteinPosition (const unsigned int index, const T delta);

            /// <summary> Obtains the Bernstein polynominal used in calculating tangents on Bezier curves. </summary>
            template <typename T> inline static T bernsteinTangent (const unsigned int index, const T delta);
    };


    template <typename T>
    T CubicBezier::bernstein (const unsigned int index, const T delta, const Derivative derivative)
    {
        // Forward the data to the correct function.
        switch (derivative)
        {
            case Derivative::First:
                return bernsteinTangent (index, delta);

            default:
                return bernsteinPosition (index, delta);
        }
    }

    
    template <typename T>
    T CubicBezier::bernsteinPosition (const unsigned int index, const T delta)
    {
        // B0,3 == (1-u)^3,
        // B1,3 == 3u(1-u)^2,
        // B2,3 == 3u^2 * (1-u),
        // B3,3 == u^3.
        const auto one = (T) 1, two = (T) 2, three = (T) 3;

        switch (index)
        {
            case 0:
                return std::pow (one - delta, three);

            case 1:
                return three * delta * std::pow (one - delta, two);

            case 2:
                return three * std::pow (delta, two) * (one - delta);

            case 3:
                return std::pow (delta, three);

            default:
                return (T) 0;
        }
    }


    template <typename T>
    T CubicBezier::bernsteinTangent (const unsigned int index, const T delta)
    {
        // B'0,3 == -3(1-u)^2,
        // B'1,3 == 3(1-u)^2 - 6u(1-u),
        // B'2,3 == 6u(1-u) - 3u^2,
        // B'3,3 == 3u^2.
        const auto one = (T) 1, two = (T) 2, three = (T) 3, six = (T) 6;

        switch (index)
        {
            case 0:
                return -three * std::pow (one - delta, two);

            case 1:
                return three * std::pow (one - delta, two) - six * delta * (one - delta);

            case 2:
                return six * delta * (one - delta) - three * std::pow (delta, two);

            case 3:
                return three * std::pow (delta, two);

            default:
                return (T) 0;
        }        
    }
}


#endif // UTILITY_CUBIC_BEZIER_3GP_HPP