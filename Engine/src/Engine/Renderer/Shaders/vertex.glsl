#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;
  
out vec4 vertexColor;

void main()
{
    gl_Position = vec4(a_Position, 1.0);
    vertexColor = vec4(a_Color, 1.0);
}