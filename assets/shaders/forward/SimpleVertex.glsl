uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_World;

attribute vec4 a_Position;
attribute vec3 a_Normal;
attribute vec3 a_Tangent;
attribute vec3 a_Bitangent;
attribute vec2 a_TexCoord;

varying vec3 v_PositionVS;
varying vec3 v_NormalVS;
varying vec3 v_TangentVS;
varying vec2 v_TexCoord;

varying mat4 v_View;

void main(void) {
    mat3 world3 = mat3(u_World);
    mat3 view3 = mat3(u_View);

    vec4 world_pos = u_World * a_Position;
    vec4 view_pos = u_View * world_pos;

    v_PositionVS = vec3(view_pos);
    v_TexCoord = a_TexCoord;
    v_NormalVS = view3 * world3 * a_Normal;
    v_TangentVS = view3 * world3 * a_Tangent;

    gl_Position = u_Projection * view_pos;

    v_View = u_View;
}
