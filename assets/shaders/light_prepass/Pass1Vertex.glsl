#version 300 es
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_World;

in vec4 a_Position;
in vec3 a_Normal;
in vec3 a_Tangent;
in vec3 a_Bitangent;
in vec2 a_TexCoord;

out vec3 v_NormalVS;
out vec3 v_TangentVS;
out vec3 v_BitangentVS;
out vec2 v_TexCoord;


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
