uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_World;

attribute vec4 a_Position;

varying vec3    v_PositionVS;

void main(void)
{
    vec4 view_position = u_View * u_World * a_Position;
    v_PositionVS = vec3(view_position);
    gl_Position = u_Projection * view_position;
}
