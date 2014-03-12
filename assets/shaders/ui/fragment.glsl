#version 300 es
precision highp float;
uniform sampler2D s_Texture;

uniform vec4 u_Color;

in vec2 v_TexCoord;

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0) * texture(s_Texture, v_TexCoord);
}
