#version 300 es
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_World;

in vec4 a_Position;
in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main(void)
{
    v_TexCoord = a_TexCoord;
    gl_Position = u_Projection * u_View * u_World * a_Position;
}
