#include "CubicBezierSurface.hpp"


// Personal headers.
#include <Utility/CubicBezier.hpp>



namespace util
{
    Vertex CubicBezierSurface::calculatePoint (const std::vector<glm::vec3>& controlPoints, const float u, const float v)
    {
        // We must have a four by four grid of control points.
        assert (controlPoints.size() == 16);

        // The dimensions of the grid are four by four.
        const auto width  = 4,
                   height = 4;

        // We need to accumlate the position and two partial differentiated tangent vectors to calculate the final vertex.
        glm::vec3 position { 0 },
                  partialU { 0 },
                  partialV { 0 };

        // We need to loop through the control points calculating Bernstein polynominals.
        for (auto j = 0U; j < height; ++j)
        {
            for (auto i = 0U; i < width; ++i)
            {
                // Obtain the control point we're currently using.
                const auto& point = controlPoints[i + j * width];

                // Calculate the Bernstein polynominals required.
                const auto  bernsteinPositionI = CubicBezier::bernstein (i, u),
                            bernsteinPositionJ = CubicBezier::bernstein (j, v),
                            bernsteinTangentI  = CubicBezier::bernstein (i, u, CubicBezier::Derivative::First),
                            bernsteinTangentJ  = CubicBezier::bernstein (j, v, CubicBezier::Derivative::First);
                                    
                // The formula for position is: += (Pij * Bi(u) * Bj(v)).
                position += point * bernsteinPositionI * bernsteinPositionJ;
           
                // The partial derivative of u is: += (Pij * B'i(u) * Bj(v)).
                partialU += point * bernsteinTangentI * bernsteinPositionJ;

                // The partial derivative of v is: += (Pij * Bi(u) * B'j(v)).
                partialV += point * bernsteinPositionI * bernsteinTangentJ;
            }
        }

        // The position is fine, we need to calculate the tangent. The tangent is the cross product of both partial derivatives.
        return { position, glm::normalize (glm::cross (partialU, partialV)) };
    }
}