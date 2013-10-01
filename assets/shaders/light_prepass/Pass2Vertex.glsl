uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_World;

attribute vec4 a_Position;

varying vec4 v_ScreenPosition;
varying vec2 v_TexCoord;

varying mat4 v_View;

void main(void) {
    vec4 screen_position = u_Projection * u_View * u_World * a_Position;

    v_ScreenPosition = screen_position;
    v_TexCoord = (screen_position.xy/screen_position.w)*0.5 + 0.5;

    gl_Position = screen_position;

    
    v_View = u_View;
}
