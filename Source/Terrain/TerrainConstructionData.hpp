#ifndef TERRAIN_CONSTRUCTION_DATA_3GP_HPP
#define TERRAIN_CONSTRUCTION_DATA_3GP_HPP


// Personal headers.
#include <Terrain/Terrain.hpp>


/// <summary>
/// A structure containing all the information required to generate terrain elements and vertices.
/// </summary>
class Terrain::ConstructionData final
{
    public:

        /////////////////////////////////
        // Constructors and destructor //
        /////////////////////////////////

        /// <summary> Constructs a Terrain::ConstructionData object with the given dimensions. </summary>
        /// <parma name="width"> How many vertices wide should the terrain be? </param>
        /// <param name="depth"> How many vertices deep should the terrain be? </param>
        /// <param name="divisor"> What will the terrain dimensions be subdivided by? </param>
        /// <param name="worldWidth"> How many world units wide is the terrain? </param>
        /// <param name="worldDepth"> How many world units deep is the terrain? </param>
        ConstructionData (const unsigned int width, const unsigned int depth, const unsigned int divisor,
                          const float worldWidth, const float worldDepth);

        ConstructionData (ConstructionData&& move);
        ConstructionData& operator= (ConstructionData&& move);
        
        ConstructionData()                                          = default;
        ConstructionData (const ConstructionData& copy)             = default;
        ConstructionData& operator= (const ConstructionData& copy)  = default;
        ~ConstructionData()                                         = default;


        //////////////////////
        // Public interface //
        //////////////////////

        /// <summary> Causes all stored values to be recalculated from different values. </summary>
        /// <parma name="width"> How many vertices wide should the terrain be? </param>
        /// <param name="depth"> How many vertices deep should the terrain be? </param>
        /// <param name="divisor"> What will the terrain dimensions be subdivided by? </param>
        /// <param name="worldWidth"> How many world units wide is the terrain? </param>
        /// <param name="worldDepth"> How many world units deep is the terrain? </param>
        void recalculate (const unsigned int width, const unsigned int depth, const unsigned int divisor,
                          const float worldWidth, const float worldDepth);

        /// <summary> Gets the width of the terrain. </summary>
        /// <returns> How many vertices wide the terrain should be. </returns>
        unsigned int getWidth() const           { return m_width; }

        /// <summary> Gets the depth of the terrain. </summary>
        /// <returns> How many vertices deep the terrain should be. </returns>
        unsigned int getDepth() const           { return m_depth; }

        /// <summary> Gets the total number of vertices for the terrain. </summary>
        /// <returns> How many total vertices the terrain should be. </returns>
        unsigned int getVertexCount() const     { return m_vertexCount; }

        /// <summary> Gets the divisor of the terrain. </summary>
        /// <returns> How many vertices wide and deep terrain patches should be. </returns>
        unsigned int getDivisor() const         { return m_divisor; }

        /// <summary> Gets the number of vertices per segment. </summary>
        /// <returns> The amount of vertices that make up a segment of terrain. </returns>
        unsigned int getMeshVertices() const    { return m_meshVertices; }

        /// <summary> Gets the mesh width of the terrain </summary>
        /// <returns> How many meshes wide the terrain should be. </returns>
        unsigned int getMeshCountX() const      { return m_meshCountX; }

        /// <summary> Gets the mesh depth of the terrain. </summary>
        /// <returns> How many meshes deep the terrain should be. </returns>
        unsigned int getMeshCountZ() const      { return m_meshCountZ; }

        /// <summary> Gets the total number of meshes of the terrain. </summary>
        /// <returns> How many total meshes there should be. </returns>
        unsigned int getMeshTotal() const       { return m_meshTotal; }

        /// <summary> Gets the width of the terrain in world units. </summary>
        /// <returns> The width of the terrain in world units. </returns>
        float getWorldWidth() const             { return m_worldWidth; }

        /// <summary> Gets the depth of the terrain in world units. </summary>
        /// <returns> The depth of the terrain in world units. </returns>
        float getWorldDepth() const             { return m_worldDepth; }

        /// <summary> Gets the area of the terrain in world units. </summary>
        /// <returns> The width of the terrain in world units. </returns>
        float getWorldArea() const              { return m_worldArea; }

    private:

        ///////////////////
        // Internal data //
        ///////////////////

        unsigned int m_width        { 0 };      //!< How many vertices wide should the terrain be?
        unsigned int m_depth        { 0 };      //!< How many vertices deep should the terrain be?
        unsigned int m_vertexCount  { 0 };      //!< The total number of vertices that make up the terrain.

        unsigned int m_divisor      { 0 };      //!< The width and depth of each terrain partition.
        unsigned int m_meshVertices { 0 };      //!< How many vertices make up a segment of terrain.
        unsigned int m_meshCountX   { 0 };      //!< How many meshes wide the terrain is.
        unsigned int m_meshCountZ   { 0 };      //!< How many meshes deep the terrain is.
        unsigned int m_meshTotal    { 0 };      //!< How many total meshes will there be when the terrain is completed.
        
        float        m_worldWidth   { 0.f };    //!< How many world units wide is the terrain?
        float        m_worldDepth   { 0.f };    //!< How many world units deep is the terrain?
        float        m_worldArea    { 0.f };    //!< The total area of the terrain in world units.
};


#endif // TERRAIN_CONSTRUCTION_DATA_3GP_HPP