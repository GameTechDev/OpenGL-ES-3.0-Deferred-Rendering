uniform mat4 Projection;
uniform mat4 ModelView;

attribute vec4 a_Position;
attribute vec4 a_Color;

varying vec4 v_Color;

void main(void) {
    v_Color = a_Color;
    gl_Position = Projection * ModelView * a_Position;
}
