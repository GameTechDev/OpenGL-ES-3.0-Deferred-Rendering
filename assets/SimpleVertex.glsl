uniform mat4 Projection;
uniform mat4 ModelView;

attribute vec4 a_Position;
attribute vec3 a_Normal;
attribute vec3 a_TexCoord;

varying vec3 v_Normal;
varying vec3 v_TexCoord;

void main(void) {
    v_Normal = a_Normal;
    v_TexCoord = a_TexCoord;
    gl_Position = Projection * ModelView * a_Position;
}
