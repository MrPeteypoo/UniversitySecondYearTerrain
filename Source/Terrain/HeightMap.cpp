#include "HeightMap.hpp"


// STL headers.
#include <cassert>
#include <stdexcept>


// Engine headers.
#include <tygra/FileHelper.hpp>



//////////////////
// Constructors //
//////////////////

HeightMap::HeightMap (const std::string& file, const glm::vec3& worldScale)
{
    if (!loadFromPNG (file, worldScale))
    {
        throw std::invalid_argument ("HeightMap::HeightMap(), unable to load from the given file location: \"" + file + "\"");
    }
}


HeightMap::HeightMap (HeightMap&& move)
{
    *this = std::move (move);
}


HeightMap& HeightMap::operator= (HeightMap&& move)
{
    if (this != &move)
    {
        m_width  = move.m_width;
        m_height = move.m_height;
        m_data   = std::move (move.m_data);
    }

    return *this;
}


///////////////
// Operators //
///////////////

const glm::vec3& HeightMap::operator[] (const size_t index) const
{
    return getPoint (index);
}


/////////////////////////
// Getters and setters //
/////////////////////////

const glm::vec3& HeightMap::getPoint (const size_t x, const size_t y) const
{
    // Ensure we are accessing valid data.
    assert (x + y * m_width < m_data.size());

    return m_data[x + y * m_width];
}


const glm::vec3& HeightMap::getPoint (const size_t index) const
{
    // Ensure we are accessing valid data.
    assert (index < m_data.size());

    return m_data[index];
}


bool HeightMap::loadFromPNG (const std::string& file, const glm::vec3& worldScale)
{
    // Ensure valid parameters!
    assert (worldScale.x != 0.f && worldScale.y != 0.f && worldScale.z != 0.f);

    // Load the height map, we only handle 8-bit per colour (256 possible values).
    const auto heightMap = tygra::imageFromPNG (file);

    if (heightMap.containsData() && heightMap.bytesPerComponent() == 1)
    {
        // Calculate the amount of data we need to create and update our member variables.
        m_width      = heightMap.width();
        m_height     = heightMap.height();
        m_worldScale = worldScale;

        m_data.reserve (m_width * m_height);

        // The width and height must be divisible by four for the height map to be valid.
        assert (m_width % 4 == 0 && m_height % 4 == 0);

        // Create the heightmap from the image data.
        auto       image    = (const uint8_t*) heightMap.pixels();

        const auto channels  = heightMap.componentsPerPixel();
        const auto maxValues = glm::vec3 (m_width - 1, 255.f * channels, m_height - 1);
                                
        for (auto z = 0U; z < m_height; ++z)
        {
            for (auto x = 0U; x < m_width; ++x)
            {
                // Use all colour channels, this gives a potential 1021 steps instead of 256 steps of height.
                auto accumlator = 0U;

                for (auto i = 0U; i < channels; ++i)
                {
                    // Ideally this would use every channel in the image but unfortunately the ICA would have me place
                    // a silly if statement here so that we only obtain the red channel.
                    if (i == 0)
                    {
                        accumlator += *image;
                        accumlator *= channels;
                    }

                    // Ideal solution here:
                    // accumlator += *image;
                    ++image;
                }

                // Normalise the X, Y and Z values to get scaled co-ordinates.
                const auto coordinates = glm::vec3 (x          / maxValues.x,
                                                    accumlator / maxValues.y,
                                                    z          / maxValues.z);

                // Put it all together and you get the world position!
                const auto position = coordinates * m_worldScale;

                // Update the internal data and increment the pointer.
                m_data.push_back (position);
            }
        }

        return true;
    }

    return false;
}