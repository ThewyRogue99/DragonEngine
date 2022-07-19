#version 450 core

layout(location = 0) out vec4 FragColor;

struct VertexOutput
{
    vec4 Color;
};

layout (location = 0) in VertexOutput Input;

layout (binding = 0) uniform sampler2D u_Textures[32];

void main()
{
    FragColor = Input.Color;
}