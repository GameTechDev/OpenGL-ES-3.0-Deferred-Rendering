uniform mat4 Projection;
uniform mat4 View;
uniform mat4 World;

attribute vec4 a_Position;
attribute vec3 a_Normal;
attribute vec2 a_TexCoord;

varying vec3 v_Normal;
varying vec2 v_TexCoord;

void main(void) {
    mat3 world3 = mat3(World);
    v_TexCoord = a_TexCoord;
    
    v_Normal = world3 * a_Normal;
    gl_Position = Projection * View * World * a_Position;
}
