
#version 330 core
layout (location = 0) in vec3 Vertex; // the position variable has attribute position 0
layout (location = 1) in vec3 Normal; // the position variable has attribute position 0
layout (location = 2) in vec2 TexCoord; // the position variable has attribute position 0
  
void main()
{
    gl_Position = vec4(Vertex, 1.0); // see how we directly give a vec3 to vec4's constructor
}
