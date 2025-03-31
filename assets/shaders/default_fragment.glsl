#version 330 core

out vec4 FragColor;

in vec3 FragPosition;
in vec3 FragNormal;

uniform vec3 ViewVector;

vec3 lightPos = vec3(0.2, 0.3, -1.0);

void main()
{
    vec3 light = FragPosition - lightPos;
    vec3 c_surface = vec3(1.0);
    vec3 c_cool = vec3(0.0, 0.0, 0.55) + 0.25 * c_surface;
    vec3 c_warm = vec3(0.3, 0.3, 0.0) + 0.25 * c_surface;
    vec3 c_highlight = vec3(1.0);

    float t = (dot(FragNormal, light) + 1.0) * 0.5;
    vec3 r = 2.0 * dot(FragNormal, light) * FragNormal - light;

    float s = clamp(100.0 * dot(r, ViewVector) - 97.0, 0.0, 1.0);
    vec3 c_base = mix(c_warm, c_cool , t);
    vec3 c_shaded = mix(c_highlight, c_base, s);
    FragColor = vec4(c_shaded, 1.0);
} 
