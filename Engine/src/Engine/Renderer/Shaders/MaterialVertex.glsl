#version 330 core

layout (location = 0) in vec3 a_Position;
  
uniform mat4 ViewProjection;
uniform mat4 Transform;

void main()
{
    gl_Position = ViewProjection * Transform * vec4(a_Position, 1.0);
}