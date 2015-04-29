#ifndef UTILITY_ELEMENT_CREATION_3GP_HPP
#define UTILITY_ELEMENT_CREATION_3GP_HPP


// STL headers.
#include <vector>


/// <summary>
/// A namespace containing utility functions and constants applicable to many use cases.
/// </summary>
namespace util
{
    /// <summary> Creates a triangular pattern out of the given dimensions and incrementation values which is added to the given vector. </summary>
    /// <param name="elements"> The elements vector to add to. </param>
    /// <param name="offset"> The starting offset of every element. </param>
    /// <param name="width"> How many elements wide the pattern should be. </param>
    /// <param name="depth"> How many elements deep the pattern should be. </param>
    /// <param name="increment"> How much to increment by to obtain an element to the right. </param>
    /// <param name="lineIncrement"> How much to increment by to reach the element directly above. </param>
    /// <param name="startMirrored"> Whether the triangle pattern should start mirrored or not. </param>
    void triangleAlgorithm (std::vector<unsigned int>& elements, 
                            const unsigned int offset, const unsigned int width, const unsigned int depth, 
                            const unsigned int increment, const unsigned int lineIncrement,
                            const bool startMirrored = false);

    /// <summary> Adds the elements to the given vector which create a /| or \| triangle depending on the mirror flag. </summary>
    /// <param name="elements"> The vector to modify. </param>
    /// <param name="current"> The starting element number. </param>
    /// <param name="increment"> How much to increment by to reach the adjacent element. </param>
    /// <param name="width"> How much to apply when attempting to move up a row of elements. </param>
    /// <param name="mirror"> Whether the /| triangle should be mirror to become a \| triangle. </param>
    void lowerTriangle (std::vector<unsigned int>& elements, 
                        const unsigned int current, const unsigned int increment, const unsigned int width, 
                        const bool mirror);

    /// <summary> Adds the elements to the given vector which create a |/ or |\ triangle depending on the mirror flag. </summary>
    /// <param name="elements"> The vector to modify. </param>
    /// <param name="current"> The starting element number. </param>
    /// <param name="increment"> How much to increment by to reach the adjacent element. </param>
    /// <param name="width"> How much apply when attempting to move up a row of elements. </param>
    /// <param name="mirror"> Whether the |/ triangle should be mirror to become a |\ triangle. </param>
    void upperTriangle (std::vector<unsigned int>& elements, 
                        const unsigned int current, const unsigned int increment, const unsigned int width, 
                        const bool mirror);
    
}


#endif // UTILITY_ELEMENT_CREATION_3GP_HPP