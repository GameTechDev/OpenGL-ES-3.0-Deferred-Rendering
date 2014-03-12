#version 300 es
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_World;

in vec4 a_Position;

void main(void)
{
    gl_Position = u_Projection * u_View * u_World * a_Position;
}
