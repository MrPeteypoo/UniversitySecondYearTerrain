#version 330

uniform mat4 view_world_xform;
uniform mat4 projection_xform;

layout(location=0)
in vec3 vertex_position;

layout(location=1)
in vec3 vertex_normal;

out vec3 varying_position;

void main(void)
{
    vec4 view_position = view_world_xform * vec4(vertex_position, 1.0);
    varying_position = view_position.xyz;
    gl_Position = projection_xform * view_position;
}
