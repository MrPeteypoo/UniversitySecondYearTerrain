#pragma once


#include <SceneModel/Context.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>



/// <summary>
/// The renderer used to show terrain generation and frustrum culling functionality.
/// </summary>
class MyView final : public tygra::WindowViewDelegate
{
    public:

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
        void toggleShading() { m_shadeNormals = !m_shadeNormals; }

    private:

        ////////////////////
        // Initialisation //
        ////////////////////

        /// <summary> Generates smooth terrain based on the information contained in a heightmap. </summary>
        void windowViewWillStart (std::shared_ptr<tygra::Window> window) override final;

        /// <summary> This contains all of the framework-specific loading code. </summary>
        void frameworkLoading();


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

        struct MeshGL
        {
            GLuint position_vbo{ 0 };
            GLuint element_vbo{ 0 };
            GLuint vao{ 0 };
            int element_count{ 0 };
        };

        GLuint                                      m_terrainShader { 0 };          //!< The shader program used for drawing the terrain.
        GLuint                                      m_shapesShader  { 0 };          //!< The shader program used for drawing the static cubes.
        
        MeshGL                                      m_terrainMesh   { };            //!< The mesh containing information required to draw the terrain.

	    GLuint                                      m_cubeVAO       { 0 };          //!< The ID of the VAO containing information relating to the drawing of a cube.
	    GLuint                                      m_cubeVBO       { 0 };          //!< The ID of the VBO containing the vertices of a cube.

        bool                                        m_shadeNormals  { false };      //!< Determines whether the terrain should be shaded using its normal vector.
        
        std::shared_ptr<const SceneModel::Context>  m_scene         { nullptr };    //!< A poiner to the context used for camera information when rendering the scene.

};
