#ifndef UTILITY_NOISE_GENERATOR_3GP_HPP
#define UTILITY_NOISE_GENERATOR_3GP_HPP


// STL headers.
#include <cmath>


namespace util
{
    /// <summary>
    /// A utility class containing fractional brownian motion and improved perlin noise functionality as indicated 
    /// at: http://mrl.nyu.edu/~perlin/noise/
    /// </summary>
    template <typename T> class NoiseGenerator final 
    {
        public:

            /// <summary>
            /// Creates a smoother, more natural noise according to the values given.
            /// </summary>
            /// <param name="position"> A position vector containing an X, Y and Z co-ordinate. </param>
            /// <param name="samples"> Also known as octaves, the number of layers which make up the final noise value. </param>
            /// <param name="frequency"> The higher the frequency the smaller details become. </param>
            /// <param name="gain"> Effects how tall or short things can become when noise is applied. </param>
            /// <param name="lacunarity"> Scales the frequency every sample, commonly 2. </param>
            /// <returns> A detailed noise value ready to be applied. </returns>
            template <typename U> static T brownianMotion (const U& position, const unsigned int samples, 
                                                           const T frequency, const T gain, const T lacunarity);
            
            /// <summary>
            /// Calculates a noise value according to the initial values given.
            /// </summary>
            /// <param name="initialX"> An X value to use in the calculation. </param>
            /// <param name="initialY"> An Y value to use in the calculation. </param>
            /// <param name="initialZ"> An Z value to use in the calculation. </param>
            /// <returns> A noise scalar from -1 to 1. If all parameters are integers then it will return 0. </returns>
            static T perlinNoise (const T initialX, const T initialY, const T initialZ);

            /// <summary> Calculates a noise value according to the initial values given. </summary>
            /// <param name="position"> A vector containing X, Y and Z co-ordinates. </param>
            /// <returns> A noise scalar from -1 to 1. </returns>
            template <typename U> inline static T perlinNoise (const U& position);

        private:

            static T fade (const T t);
            static T grad (const int hash, const T x, const T y, const T z);
            
            inline static T lerp (const T t, const T a, const T b);

            static const int p[512];    //!< The permutations array which causes the pseudo-random values.
    };


    template <typename T>
    template <typename U>
    T NoiseGenerator<T>::brownianMotion (const U& position, const unsigned int samples, 
                                         const T frequency, const T gain, const T lacunartiy)
    {
        // Set the amplitude to the gain which will increase every sample.
        T amp  = gain,
          freq = frequency;
        
        // We need to keep track of the result.
        T result = (T) 0;

        for (auto i = 0U; i < samples; ++i)
        {
            // Create the multi-layered effect by contorting the noise values.
            result += amp * perlinNoise (position * freq);

            // Increase the amplitude and frequency for the next octave.
            amp  *= gain;
            freq *= lacunartiy;
        }

        return result;
    }


    template <typename T>
    T NoiseGenerator<T>::perlinNoise (const T initialX, const T initialY, const T initialZ) 
    {
        const T floorX = std::floor (initialX),
                floorY = std::floor (initialY),
                floorZ = std::floor (initialZ);

        const int X = (int) floorX & 255,                           // FIND UNIT CUBE THAT
                  Y = (int) floorY & 255,                           // CONTAINS POINT.
                  Z = (int) floorZ & 255;

        const T x = initialX - floorX,                              // FIND RELATIVE X,Y,Z
                y = initialY - floorY,                              // OF POINT IN CUBE.
                z = initialZ - floorZ,
                u = fade(x),                                        // COMPUTE FADE CURVES
                v = fade(y),                                        // FOR EACH OF X,Y,Z.
                w = fade(z);
        
        const int A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z,         // HASH COORDINATES OF
                  B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;         // THE 8 CUBE CORNERS,

        return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),  // AND ADD
                                       grad(p[BA  ], x-1, y  , z   )), // BLENDED
                               lerp(u, grad(p[AB  ], x  , y-1, z   ),  // RESULTS
                                       grad(p[BB  ], x-1, y-1, z   ))),// FROM  8
                       lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),  // CORNERS
                                       grad(p[BA+1], x-1, y  , z-1 )), // OF CUBE
                               lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                                       grad(p[BB+1], x-1, y-1, z-1 ))));
    }

    
    template <typename T>
    template <typename U>
    T NoiseGenerator<T>::perlinNoise (const U& position)
    {
        return perlinNoise (position.x, position.y, position.z);   
    }


    template <typename T>
    T NoiseGenerator<T>::fade (const T t)
    {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }


    template <typename T>
    T NoiseGenerator<T>::lerp (const T t, const T a, const T b)
    {
        return a + t * (b - a);
    }

    
    template <typename T>
    T NoiseGenerator<T>::grad (const int hash, const T x, const T y, const T z) 
    {
        const int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
        const T u = h<8 ? x : y,                      // INTO 12 GRADIENT DIRECTIONS.
                v = h<4 ? y : h==12||h==14 ? x : z;
        return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
    }


    template <typename T>
    const int NoiseGenerator<T>::p[512] =
    {
        // Look at the beautiful permutation values!
        151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
        8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
        35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
        134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
        55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,
        169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
        250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
        189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
        172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,
        228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,
        107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
        
        151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
        8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
        35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
        134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
        55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,
        169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
        250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
        189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
        172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,
        228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,
        107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
    };
}

#endif // UTILITY_NOISE_GENERATOR_3GP_HPP