uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_World;

attribute vec4 a_Position;
attribute vec2 a_TexCoord;

varying vec2 v_TexCoord;

void main(void)
{
    v_TexCoord = a_TexCoord;
    gl_Position = u_Projection * u_View * u_World * a_Position;
}
