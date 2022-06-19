#version 450 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int ID;

struct VertexOutput
{
    vec4 Color;
    vec2 TexCoord;
    float TilingFactor;
};

layout (location = 0) in VertexOutput Input;
layout (location = 3) in flat float v_TexIndex;
layout (location = 4) in flat int v_EntityID;

layout (binding = 0) uniform sampler2D u_Textures[32];

void main()
{
    FragColor = texture(u_Textures[int(v_TexIndex)], Input.TexCoord * Input.TilingFactor) * Input.Color;

    ID = v_EntityID;
} 