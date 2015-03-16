#version 330

uniform bool use_normal = false;

in vec3 varying_position;
in vec3 varying_normal;

layout(location=0)
out vec4 fragment_colour;

void main(void)
{
    vec3 colour = vec3(0, 0.4, 0.8);
    if (use_normal)
    {
        float lit = dot(normalize(varying_normal), normalize(-varying_position));
        colour = vec3(pow(lit, 0.5));
    }
    fragment_colour = vec4(colour, 1.0);
}
