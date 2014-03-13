uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_World;

attribute vec4 a_Position;
attribute vec3 a_Normal;
attribute vec3 a_Tangent;
attribute vec3 a_Bitangent;
attribute vec2 a_TexCoord;

varying vec3 v_NormalVS;
varying vec3 v_TangentVS;
varying vec3 v_BitangentVS;
varying vec2 v_TexCoord;


void main(void) {
    mat3 world3 = mat3(u_World);
    mat3 view3 = mat3(u_View);

    vec4 world_pos = u_World * a_Position;
    vec4 view_pos = u_View * world_pos;

    v_NormalVS = view3 * world3 * a_Normal;
    v_TangentVS = view3 * world3 * a_Tangent;
    v_BitangentVS = view3 * world3 * a_Bitangent;
    v_TexCoord = a_TexCoord;

    gl_Position = u_Projection * u_View * u_World * a_Position;
}
