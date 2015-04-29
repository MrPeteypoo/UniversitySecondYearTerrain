#ifndef UTILITY_QUADRATIC_BEZIER_3GP_HPP
#define UTILITY_QUADRATIC_BEZIER_3GP_HPP


#include <cmath>


namespace util
{
    /// <summary>
    /// A static class containing quadratic Bezier curve calculation functionality. This is completely header based so hopefully
    /// the compiler inlines everything providing some nice speed benefits.
    /// </summary>
    class QuadraticBezier final
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
    T QuadraticBezier::bernstein (const unsigned int index, const T delta, const Derivative derivative)
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
    T QuadraticBezier::bernsteinPosition (const unsigned int index, const T delta)
    {
        // B0,2 == (1-u)^2,
        // B1,2 == 2u * (1-u),
        // B2,2 == u^2,
        const auto one = (T) 1, two = (T) 2;

        switch (index)
        {
            case 0:
                return std::pow (one - delta, two);

            case 1:
                return two * delta * (one - delta);

            case 2:
                return std::pow (delta, two);

            default:
                return (T) 0;
        }
    }


    template <typename T>
    T QuadraticBezier::bernsteinTangent (const unsigned int index, const T delta)
    {
        // B'0,2 == 2(1-u),
        // B'1,2 == 2-4u,
        // B'2,2 == 2u.
        const auto one = (T) 1, two = (T) 2, three = (T) 3, four = (T) 4;

        switch (index)
        {
            case 0:
                return two * (one - delta);

            case 1:
                return two - four * delta;

            case 2:
                return two * delta;

            default:
                return (T) 0;
        }        
    }
}


#endif // UTILITY_QUADRATIC_BEZIER_3GP_HPP