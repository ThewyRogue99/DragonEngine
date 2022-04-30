#version 330 core
out vec4 FragColor;
  
in vec2 v_TexCoord;

void main()
{
    FragColor = vec4(v_TexCoord, 0.f, 0.f);
} 