#version 450 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int EntityID;

struct VertexOutput
{
	vec3 LocalPosition;
	vec4 Color;
	float Thickness;
	float Fade;
};

layout (location = 0) in VertexOutput Input;
layout (location = 4) in flat int v_EntityID;

void main()
{
    float distance = 1.0 - length(Input.LocalPosition);
    float circle = smoothstep(0.0, Input.Fade, distance);
    circle *= smoothstep(Input.Thickness + Input.Fade, Input.Thickness, distance);

	if (circle == 0.0)
		discard;

    FragColor = Input.Color;
	FragColor.a *= circle;

	EntityID = v_EntityID;
}