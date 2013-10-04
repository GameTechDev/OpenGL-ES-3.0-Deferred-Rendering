uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_World;

attribute vec4 a_Position;

varying vec4 v_Position;

void main(void)
{
    gl_Position = u_Projection * u_View * u_World * a_Position;
    v_Position = gl_Position;
}
