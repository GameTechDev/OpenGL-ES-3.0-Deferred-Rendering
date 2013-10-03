uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_World;

attribute vec4 a_Position;

varying vec3 v_PositionVS;

void main(void) {
    vec4 screen_position = u_View * u_World * a_Position;

    v_PositionVS = vec3(screen_position);

    gl_Position = u_Projection * screen_position;
}
