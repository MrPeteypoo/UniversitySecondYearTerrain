#include "HeightMap.hpp"


// STL headers.
#include <cassert>
#include <stdexcept>


// Engine headers.
#include <tygra/FileHelper.hpp>



//////////////////
// Constructors //
//////////////////

HeightMap::HeightMap (const std::string& file, const float maxHeight, const float pixelToWorldScale)
{
    if (!loadFromPNG (file, maxHeight, pixelToWorldScale))
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


bool HeightMap::loadFromPNG (const std::string& file, const float maxHeight, const float pixelToWorldScale)
{
    // Ensure valid parameters!
    assert (pixelToWorldScale != 0.f);

    // Load the height map, we only handle 8-bit per colour (256 possible values).
    const auto heightMap = tygra::imageFromPNG (file);

    if (heightMap.containsData() && heightMap.bytesPerComponent() == 1)
    {
        // Calculate the amount of data we need to create.
        m_width  = heightMap.width();
        m_height = heightMap.height();

        m_data.resize (m_width * m_height);

        // Create the heightmap from the image data.
        auto       image    = (const uint8_t*) heightMap.pixels();

        const auto channels = heightMap.componentsPerPixel();
        const auto maxValue = 255.f * channels;
                                
        for (auto z = 0U; z < m_height; ++z)
        {
            for (auto x = 0U; x < m_width; ++x)
            {
                // Use all colour channels, this gives a potential 1021 steps instead of 256 steps of height.
                auto accumlator = 0U;

                for (auto i = 0U; i < channels; ++i)
                {
                    accumlator += image[i];
                }

                // Put it all together and you get the world position!
                const auto height   = maxHeight * (accumlator / maxValue);

                const auto position = glm::vec3 (x / pixelToWorldScale, height, z / pixelToWorldScale);

                // Update the internal data and increment the pointer.
                m_data[x + z * m_width] = position;
                ++image;
            }
        }

        return true;
    }

    return false;
}