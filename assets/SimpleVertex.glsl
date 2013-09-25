uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

attribute vec4 a_Position;
attribute vec3 a_Normal;
attribute vec2 a_TexCoord;

varying vec3 v_Normal;
varying vec2 v_TexCoord;

void main(void) {
    v_Normal = a_Normal;
    v_TexCoord = a_TexCoord;
    gl_Position = Projection * View * Model * a_Position;
}
