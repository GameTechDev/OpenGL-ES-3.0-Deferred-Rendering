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
varying vec3 v_BitangentVS;
varying vec2 v_TexCoord;

void main(void) {
    mat3 world3 = mat3(u_World);
    mat3 view3 = mat3(u_View);

    vec4 world_pos = u_World * a_Position;
    vec4 view_pos = u_View * world_pos;

    v_PositionVS = vec3(world_pos);
    v_TexCoord = a_TexCoord;
    v_NormalVS = world3 * a_Normal;
    v_TangentVS = world3 * a_Tangent;
    v_BitangentVS = world3 * a_Bitangent;

    gl_Position = u_Projection * view_pos;
}
