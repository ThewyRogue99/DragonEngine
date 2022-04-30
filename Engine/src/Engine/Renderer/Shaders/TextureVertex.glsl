#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;
  
uniform mat4 ViewProjection;
uniform mat4 Transform;

out vec4 vertexColor;
out vec2 v_TexCoord;

void main()
{
    gl_Position = ViewProjection * Transform * vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
}