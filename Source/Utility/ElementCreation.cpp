#include "ElementCreation.hpp"


namespace util
{
    void triangleAlgorithm (std::vector<unsigned int>& elements, 
                            const unsigned int offset, const unsigned int width, const unsigned int depth, 
                            const unsigned int increment, const unsigned int lineIncrement, 
                            const bool startMirrored)
    {
        /// We create an alternating square pattern for efficiency. It should look as shown below.
        /// 
        /// |/|\|/|
        /// |\|/|\|
        /// |/|\|/|
        /// 

        // Use mirrors to maintain the alternating pattern when patches are made up with even or odd number of vertices.
        bool mirrorX = startMirrored, 
             mirrorZ = startMirrored;

        for (auto z = 0U; z < depth; ++z)
        {
            for (auto x = 0U; x < width; ++x)
            {
                // Calculate the current vertex.
                const auto vertex = offset + x + z * lineIncrement;

                lowerTriangle (elements, vertex, increment, lineIncrement, mirrorX);
                upperTriangle (elements, vertex, increment, lineIncrement, mirrorX);
            
                mirrorX = !mirrorX;
            }

            // Here's the magic which maintains the pattern with an even/odd width.
            mirrorX = mirrorZ = !mirrorZ;
        }
    }


    void lowerTriangle (std::vector<unsigned int>& elements, 
                        const unsigned int current, const unsigned int increment, const unsigned int width, 
                        const bool mirror)
    {
        const auto triangleEnd = mirror ? current + width : current + width + increment;

        elements.push_back (current);
        elements.push_back (current + increment);
        elements.push_back (triangleEnd);
    }


    void upperTriangle (std::vector<unsigned int>& elements, 
                        const unsigned int current, const unsigned int increment, const unsigned int width, 
                        const bool mirror)
    {
        const auto triangleEnd = mirror ? current + increment : current;

        elements.push_back (current + width + increment);
        elements.push_back (current + width);
        elements.push_back (triangleEnd);
    }
}