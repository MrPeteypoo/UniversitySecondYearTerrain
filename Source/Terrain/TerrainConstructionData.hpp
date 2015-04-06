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
        ConstructionData (const unsigned int width, const unsigned int depth, const unsigned int divisor);
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
        void recalculate (const unsigned int width, const unsigned int depth, const unsigned int divisor);

        /// <summary> Gets the width of the terrain. </summary>
        /// <returns> How many vertices wide the terrain should be. </returns>
        unsigned int getWidth() const       { return m_width; }

        /// <summary> Gets the depth of the terrain. </summary>
        /// <returns> How many vertices deep the terrain should be. </returns>
        unsigned int getDepth() const       { return m_depth; }

        /// <summary> Gets the total number of vertices for the terrain. </summary>
        /// <returns> How many total vertices the terrain should be. </returns>
        unsigned int getVertexCount() const { return m_vertexCount; }

        /// <summary> Gets the divisor of the terrain. </summary>
        /// <returns> How many vertices wide and deep terrain patches should be. </returns>
        unsigned int getDivisor() const     { return m_divisor; }

        /// <summary> Gets the remainder of vertices on the X axis caused by the given divisor. </summary>
        /// <returns> How wide the last patch on the X axis should be. </returns>
        unsigned int getRemainderX() const  { return m_remainderX; }

        /// <summary> Gets the remainder of vertices on the Z axis caused by the given divisor. </summary>
        /// <returns> How wide the last patch on the Z axis should be. </returns>
        unsigned int getRemainderZ() const  { return m_remainderZ; }

        /// <summary> Gets the mesh width of the terrain </summary>
        /// <returns> How many meshes wide the terrain should be. </returns>
        unsigned int getMeshCountX() const  { return m_meshCountX; }

        /// <summary> Gets the mesh depth of the terrain. </summary>
        /// <returns> How many meshes deep the terrain should be. </returns>
        unsigned int getMeshCountZ() const  { return m_meshCountZ; }

        /// <summary> Gets the total number of meshes of the terrain. </summary>
        /// <returns> How many total meshes there should be. </returns>
        unsigned int getMeshTotal() const   { return m_meshTotal; }

        /// <summary> Checks if the X axis has a remainder. </summary>
        /// <returns> Whether a remainder on the X axis is necessary. </returns>
        bool hasRemainderX() const          { return m_remainderX > 0; }

        /// <summary> Checks if the Z axis has a remainder. </summary>
        /// <returns> Whether a remainder on the Z axis is necessary. </returns>
        bool hasRemainderZ() const          { return m_remainderZ > 0; }

    private:

        ///////////////////
        // Internal data //
        ///////////////////

        unsigned int m_width        { 0 };  //!< How many vertices wide should the terrain be?
        unsigned int m_depth        { 0 };  //!< How many vertices deep should the terrain be?
        unsigned int m_vertexCount  { 0 };  //!< The total number of vertices that make up the terrain.

        unsigned int m_divisor      { 0 };  //!< The width and depth of each terrain partition.
        unsigned int m_remainderX   { 0 };  //!< How wide the last patch on the X axis should be.
        unsigned int m_remainderZ   { 0 };  //!< How deep the last patch on the Z axis should be.

        unsigned int m_meshCountX   { 0 };  //!< How many meshes wide the terrain is.
        unsigned int m_meshCountZ   { 0 };  //!< How many meshes deep the terrain is.
        unsigned int m_meshTotal    { 0 };  //!< How many total meshes will there be when the terrain is completed.

};


#endif // TERRAIN_CONSTRUCTION_DATA_3GP_HPP