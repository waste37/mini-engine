
#version 330 core
layout (location = 0) in vec3 Position; // the position variable has attribute position 0
layout (location = 1) in vec3 Normal; // the position variable has attribute position 0
layout (location = 2) in vec2 TexCoord; // the position variable has attribute position 0

uniform mat4 ProjectionTransform;
uniform mat4 ViewTransform;
uniform mat4 ModelTransform;

out vec3 FragPosition;
out vec3 FragNormal;
  
void main()
{
    gl_Position = ProjectionTransform * ViewTransform * ModelTransform * vec4(Position, 1.0); // see how we directly give a vec3 to vec4's constructor
    FragPosition = vec3(ModelTransform * vec4(Position, 1.0));
    FragNormal = Normal;
}
