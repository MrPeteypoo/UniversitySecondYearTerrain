#include "MyView.hpp"


// STL headers.
#include <algorithm>
#include <iostream>
#include <vector>


// Engine headers.
#include <tygra/FileHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Terrain/HeightMap.hpp>


// Constants.
const int kVertexPosition { 0 },
          kVertexNormal   { 1 };


/////////////////////////////////
// Constructors and destructor //
/////////////////////////////////

MyView::MyView (MyView&& move)
{
    *this = std::move (move);
}


MyView& MyView::operator= (MyView&& move)
{
    if (this != &move)
    {
        // TODO: Write the move constructor.
    }

    return *this;
}


////////////////////
// Initialisation //
////////////////////

void MyView::windowViewWillStart (std::shared_ptr<tygra::Window> window)
{
    // Let the framework do it's business, it isn't relevant to us.
    frameworkLoading();

    // Generate the terrain.
    terrainLoading();
}


void MyView::frameworkLoading()
{
    GLint compile_status = 0;
    GLint link_status = 0;    
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    std::string vertex_shader_string = tygra::stringFromFile("terrain_vs.glsl");
    const char *vertex_shader_code = vertex_shader_string.c_str();
    glShaderSource(vertex_shader, 1,
                   (const GLchar **) &vertex_shader_code, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length]= "";
        glGetShaderInfoLog(vertex_shader, string_length, NULL, log);
        std::cerr << log << std::endl;
    } 
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragment_shader_string = tygra::stringFromFile("terrain_fs.glsl");
    const char *fragment_shader_code = fragment_shader_string.c_str();
    glShaderSource(fragment_shader, 1,
                   (const GLchar **) &fragment_shader_code, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length]= "";
        glGetShaderInfoLog(fragment_shader, string_length, NULL, log);
        std::cerr << log << std::endl;
    }
    m_terrainShader = glCreateProgram();
    glAttachShader(m_terrainShader, vertex_shader);
    glDeleteShader(vertex_shader);
    glAttachShader(m_terrainShader, fragment_shader);
    glDeleteShader(fragment_shader);
    glLinkProgram(m_terrainShader);
    glGetProgramiv(m_terrainShader, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length]= "";
        glGetProgramInfoLog(m_terrainShader, string_length, NULL, log);
        std::cerr << log << std::endl;
    }
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    vertex_shader_string = tygra::stringFromFile("shapes_vs.glsl");
    vertex_shader_code = vertex_shader_string.c_str();
    glShaderSource(vertex_shader, 1,
        (const GLchar **)&vertex_shader_code, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length] = "";
        glGetShaderInfoLog(vertex_shader, string_length, NULL, log);
        std::cerr << log << std::endl;
    }
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    fragment_shader_string = tygra::stringFromFile("shapes_fs.glsl");
    fragment_shader_code = fragment_shader_string.c_str();
    glShaderSource(fragment_shader, 1,
        (const GLchar **)&fragment_shader_code, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length] = "";
        glGetShaderInfoLog(fragment_shader, string_length, NULL, log);
        std::cerr << log << std::endl;
    }
    m_shapesShader = glCreateProgram();
    glAttachShader(m_shapesShader, vertex_shader);
    glDeleteShader(vertex_shader);
    glAttachShader(m_shapesShader, fragment_shader);
    glDeleteShader(fragment_shader);
    glLinkProgram(m_shapesShader);
    glGetProgramiv(m_shapesShader, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        const int string_length = 1024;
        GLchar log[string_length] = "";
        glGetProgramInfoLog(m_terrainShader, string_length, NULL, log);
        std::cerr << log << std::endl;
    }
    glGenVertexArrays(1, &m_cubeVAO);
    glBindVertexArray(m_cubeVAO);
    glGenBuffers(1, &m_cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);
    float cube_vertices[] = {
        -0.5f, 0.f, -0.5f,   0.5f, 0.f, -0.5f,   0.5f, 0.f,  0.5f,
        -0.5f, 0.f, -0.5f,   0.5f, 0.f,  0.5f,  -0.5f, 0.f,  0.5f,
        -0.5f, 0.f,  0.5f,   0.5f, 0.f,  0.5f,   0.5f, 1.f,  0.5f,
        -0.5f, 0.f,  0.5f,   0.5f, 1.f,  0.5f,  -0.5f, 1.f,  0.5f,
         0.5f, 0.f,  0.5f,   0.5f, 0.f, -0.5f,   0.5f, 1.f, -0.5f,
         0.5f, 0.f,  0.5f,   0.5f, 1.f, -0.5f,   0.5f, 1.f,  0.5f,
         0.5f, 0.f, -0.5f,  -0.5f, 0.f, -0.5f,  -0.5f, 1.f, -0.5f,
         0.5f, 0.f, -0.5f,  -0.5f, 1.f, -0.5f,   0.5f, 1.f, -0.5f,
        -0.5f, 0.f, -0.5f,  -0.5f, 0.f,  0.5f,  -0.5f, 1.f,  0.5f,
        -0.5f, 0.f, -0.5f,  -0.5f, 1.f,  0.5f,  -0.5f, 1.f, -0.5f,
        -0.5f, 1.f,  0.5f,   0.5f, 1.f,  0.5f,   0.5f, 1.f, -0.5f,
        -0.5f, 1.f,  0.5f,   0.5f, 1.f, -0.5f,  -0.5f, 1.f, -0.5f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices),
                 cube_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(kVertexPosition);
    glVertexAttribPointer(kVertexPosition, 3, GL_FLOAT, GL_FALSE,
        sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void MyView::terrainLoading()
{
    // Obtain the terrain properties from the scene information and generate the terrain.
    const auto& file   = m_scene->getTerrainHeightMapName();
    
    const auto  width  = m_scene->getTerrainSizeX(),
                height = m_scene->getTerrainSizeY(),    
                depth  = -m_scene->getTerrainSizeZ();

    const auto  shrink = 8U;

    // Construct the terrain scalar to get the correct visual output.
    const auto scale = glm::vec3 (width, height, depth);

    // Load the height map with the desired values.
    const HeightMap heightMap { file, scale / (float) shrink };

    // Build the terrain and get it ready for rendering.
    m_terrain.setDivisor (256);
    m_terrain.buildFromHeightMap (heightMap, 8192U / shrink, 8192U / shrink);
    m_terrain.prepareForRender (m_terrainShader);
}


//////////////
// Clean up //
//////////////

void MyView::windowViewDidStop (std::shared_ptr<tygra::Window> window)
{
    glDeleteProgram (m_terrainShader);
    glDeleteProgram (m_shapesShader);

    glDeleteBuffers (1, &m_cubeVBO);
    glDeleteVertexArrays (1, &m_cubeVAO);

    m_terrain.cleanUp();
}


///////////////
// Rendering //
///////////////

void MyView::windowViewDidReset (std::shared_ptr<tygra::Window> window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void MyView::windowViewRender (std::shared_ptr<tygra::Window> window)
{
    // Framework crap, not allowed to touch.
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    const float aspect_ratio = viewport[2] / (float)viewport[3];
    const auto& camera = m_scene->getCamera();
    glm::mat4 projection_xform = glm::perspective(camera.getVerticalFieldOfViewInDegrees(),
                                                  aspect_ratio,
                                                  camera.getNearPlaneDistance(),
                                                  camera.getFarPlaneDistance());
    glm::vec3 camera_pos = camera.getPosition();
    glm::vec3 camera_at = camera.getPosition() + camera.getDirection();
    glm::vec3 world_up{ 0, 1, 0 };
    glm::mat4 view_xform = glm::lookAt(camera_pos, camera_at, world_up);


    // Personal crap, I can touch this as much as I want *wink* *wink*.
    glClearColor(0.f, 0.f, 0.25f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, m_shadeNormals ? GL_FILL : GL_LINE);

    glUseProgram(m_terrainShader);

    GLuint shading_id = glGetUniformLocation(m_terrainShader, "use_normal");
    glUniform1i(shading_id, m_shadeNormals);

    glm::mat4 world_xform = glm::mat4(1);
    glm::mat4 view_world_xform = view_xform * world_xform;

    GLuint projection_xform_id = glGetUniformLocation(m_terrainShader,
                                                      "projection_xform");
    glUniformMatrix4fv(projection_xform_id, 1, GL_FALSE,
                       glm::value_ptr(projection_xform));

    GLuint view_world_xform_id = glGetUniformLocation(m_terrainShader,
                                                      "view_world_xform");
    glUniformMatrix4fv(view_world_xform_id, 1, GL_FALSE,
                       glm::value_ptr(view_world_xform));

    m_terrain.draw();
    //glBindVertexArray(m_terrainMesh.vao);
    //glDrawElements(GL_TRIANGLES, m_terrainMesh.element_count, GL_UNSIGNED_INT, 0);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glUseProgram(m_shapesShader);

    projection_xform_id = glGetUniformLocation(m_shapesShader,
                                               "projection_xform");
    glUniformMatrix4fv(projection_xform_id, 1, GL_FALSE,
                       glm::value_ptr(projection_xform));

    glBindVertexArray(m_cubeVAO);

    for (const auto& pos : m_scene->getAllShapePositions())
    {
        world_xform = glm::translate(glm::mat4(1), glm::vec3(pos.x, 64, -pos.y));
        view_world_xform = view_xform * world_xform;

        view_world_xform_id = glGetUniformLocation(m_shapesShader,
                                                   "view_world_xform");
        glUniformMatrix4fv(view_world_xform_id, 1, GL_FALSE,
                           glm::value_ptr(view_world_xform));

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}