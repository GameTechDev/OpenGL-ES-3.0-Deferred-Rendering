#version 300 es
uniform mat4 u_ViewProjection;
uniform mat4 u_World;

in vec4 a_Position;
in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
    v_TexCoord = a_TexCoord;
    gl_Position = u_ViewProjection * u_World * a_Position;
}
