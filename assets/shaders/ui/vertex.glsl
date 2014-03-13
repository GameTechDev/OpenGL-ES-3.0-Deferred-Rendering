uniform mat4 u_ViewProjection;
uniform mat4 u_World;

attribute vec4 a_Position;
attribute vec2 a_TexCoord;

varying vec2 v_TexCoord;

void main()
{
    v_TexCoord = a_TexCoord;
    gl_Position = u_ViewProjection * u_World * a_Position;
}
