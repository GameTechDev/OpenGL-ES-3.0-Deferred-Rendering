#version 300 es
in vec4 a_Position;
in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
    v_TexCoord = a_TexCoord;
    gl_Position = a_Position;
}
