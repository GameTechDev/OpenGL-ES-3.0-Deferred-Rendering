#version 300 es
precision highp float;
uniform sampler2D s_Texture;

in vec2 v_TexCoord;

out vec4 FragColor;

void main()
{
    FragColor = texture(s_Texture, v_TexCoord);
}
