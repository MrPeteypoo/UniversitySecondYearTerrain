#ifndef MY_VIEW_GL_HPP
#define MY_VIEW_GL_HPP


// Engine headers.
#include <SceneModel/Context.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>


// Personal headers.
#include <Terrain/Terrain.hpp>


/// <summary>
/// The renderer used to show terrain generation and frustrum culling functionality.
/// </summary>
class MyView final : public tygra::WindowViewDelegate
{
    public:
        
        // Constants.
        const unsigned int shadingModesAvailable = 3U;


        /////////////////////////////////
        // Constructors and destructor //
        /////////////////////////////////

        MyView()                                = default;
        ~MyView()                               = default;

        MyView (MyView&& move);
        MyView& operator= (MyView&& move);

        MyView (const MyView& copy)             = delete;
        MyView& operator= (const MyView& copy)  = delete;


        /////////////////////////
        // Getters and setters //
        /////////////////////////

        /// <summary> Set the SceneModel::Context used when drawing the scene. </summary>
        /// <param name="scene"> The scene data to use. </param>
        void setScene (const std::shared_ptr<const SceneModel::Context>& scene) { m_scene = scene; }

        /// <summary> 
        /// Toggle the shading model used by the application, this will determine whether to shade the
        /// terrain using the normal vectors.
        /// </summary>
        void toggleShading() { m_shadeNormals = ++m_shadeNormals % shadingModesAvailable; }

    private:

        ////////////////////
        // Initialisation //
        ////////////////////

        /// <summary> Generates smooth terrain based on the information contained in a heightmap. </summary>
        void windowViewWillStart (std::shared_ptr<tygra::Window> window) override final;

        /// <summary> This contains all of the framework-specific loading code. </summary>
        void frameworkLoading();

        /// <summary> Attempt to load the terrain from a height map. </summary>
        void terrainLoading();


        //////////////
        // Clean up //
        //////////////

        /// <summary> Cleans up the dynamic CPU and GPU memory in use by the view. </summary>
        void windowViewDidStop (std::shared_ptr<tygra::Window> window) override final;


        ///////////////
        // Rendering //
        ///////////////

        /// <summary> Updates the viewport with the given values. </summary>
        void windowViewDidReset (std::shared_ptr<tygra::Window> window, int width, int height) override final;

        /// <summary> Renders the terrain and cube objects using culling optimisation techniques. </summary>
        void windowViewRender (std::shared_ptr<tygra::Window> window) override final;


        ///////////////////
        // Internal data //
        ///////////////////

        GLuint                                      m_terrainShader { 0 };          //!< The shader program used for drawing the terrain.
        GLuint                                      m_shapesShader  { 0 };          //!< The shader program used for drawing the static cubes.
        
        Terrain                                     m_terrain       { };            //!< The mesh containing information required to draw the terrain.

	    GLuint                                      m_cubeVAO       { 0 };          //!< The ID of the VAO containing information relating to the drawing of a cube.
	    GLuint                                      m_cubeVBO       { 0 };          //!< The ID of the VBO containing the vertices of a cube.

        unsigned int                                m_shadeNormals  { 0 };          //!< Determines whether the terrain should be shaded in white or in pastel with its normal vector.
        
        std::shared_ptr<const SceneModel::Context>  m_scene         { nullptr };    //!< A poiner to the context used for camera information when rendering the scene.

};

#endif