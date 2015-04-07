#version 330

uniform uint use_normal = 0;

in vec3 varying_position;
in vec3 varying_normal;
in vec3 varying_colour;

layout(location=0)
out vec4 fragment_colour;

void main(void)
{
    vec3 colour = vec3(0, 0.4, 0.8);
    switch (use_normal)
    {
        case 1:
        {
            float lit = dot(normalize(varying_normal), normalize(-varying_position));
            colour = vec3(pow(lit, 0.5));
            break;
        }

        case 2:
            colour = varying_colour;
            break;

        default:
            break;
    }

    fragment_colour = vec4(colour, 1.0);
}
