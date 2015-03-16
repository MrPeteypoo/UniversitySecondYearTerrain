#version 330

in vec3 varying_position;

layout(location=0)
out vec4 fragment_colour;

void main(void)
{
    vec3 P = varying_position;
    vec3 N = normalize(cross(dFdx(P), dFdy(P)));
    fragment_colour = vec4(0.5 + 0.5 * N, 1.0);
}
