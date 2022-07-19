#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
  
layout(std140, binding = 0) uniform Camera
{
    mat4 ViewProjection;
};

struct VertexOutput
{
    vec4 Color;
};

layout (location = 0) out VertexOutput Output;

void main()
{
    Output.Color = a_Color;

    gl_Position = ViewProjection * vec4(a_Position, 1.0);
}