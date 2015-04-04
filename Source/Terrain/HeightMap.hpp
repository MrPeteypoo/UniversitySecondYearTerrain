#ifndef HEIGHT_MAP_3GP_HPP
#define HEIGHT_MAP_3GP_HPP


// STL headers.
#include <vector>


// Engine headers.
#include <glm/gtc/type_ptr.hpp>


/// <summary>
/// A basic class which loads a height map from a file and stores the control points for a 3D terrain.
/// </summary>
class HeightMap final
{
    public:

        /////////////////////////////////
        // Constructors and destructor //
        /////////////////////////////////

        /// <summary> 
        /// Constructs a HeightMap object from an image file. 
        /// </summary>
        /// <param name="file"> The file location of the image to load. </param>
        /// <param name="maxHeight"> The scalar value to be used when reading height values. 1 == maxHeight, 0.5 == maxHeight / 2. </param>
        /// <param name="pixelToUnitScale"> How many pixels make up a single world unit? This effects how large it appears in game. </param>
        HeightMap (const std::string& file, const float maxHeight, const float pixelToWorldScale);

        HeightMap (HeightMap&& move);
        HeightMap& operator= (HeightMap&& move);

        HeightMap (const HeightMap& copy)               = default;
        HeightMap& operator= (const HeightMap& copy)    = default;
        ~HeightMap()                                    = default;


        ///////////////
        // Operators //
        ///////////////

        /// <summary> Short-hand for HeightMap::getPoint (const size_t). </summary>
        const glm::vec3& operator[] (const size_t index) const;


        /////////////////////////
        // Getters and setters //
        /////////////////////////

        /// <summary> Gets the width of the height map data. </summary>
        unsigned int getWidth() const   { return m_width; }

        /// <summary> Gets the height of the height map data. </summary>
        unsigned int getHeight() const  { return m_height; }

        /// <summary> Gets the point at the given co-ordinates. </summary>
        /// <param name="x"> The X co-ordinate of the height map. </param>
        /// <param name="y"> The Y co-orindate of the height map. </param>
        /// <returns> The desired point. </returns>
        const glm::vec3& getPoint (const size_t x, const size_t y) const;

        /// <summary> Gets the point at the desired index of the height map. </summary>
        /// <param name="index"> The index of the point to obtain. </param>
        /// <returns> The desired point. </returns>
        const glm::vec3& getPoint (const size_t index) const;

        /// <summary> Attempts to load the height map from a new file. If the file is invalid no data will be lost. </summary>
        /// <param name="file"> The file location to load from. </param>
        /// <param name="maxHeight"> The scalar value to be used when reading height values. 1 == maxHeight, 0.5 == maxHeight / 2. </param>
        /// <param name="pixelToUnitScale"> How many pixels make up a single world unit? This effects how large it appears in game. </param>
        /// <returns> Whether the load was successful or not. </returns>
        bool loadFromPNG (const std::string& file, const float maxHeight, const float pixelToWorldScale);

    private:

        ///////////////////
        // Internal data //
        ///////////////////

        unsigned int            m_width     { 0 };  //!< The width of the height map image.
        unsigned int            m_height    { 0 };  //!< The height of the height map image.

        std::vector<glm::vec3>  m_data      { };    //!< The 3D co-ordinates of the desired terrain.
};

#endif // HEIGHT_MAP_3GP_HPP