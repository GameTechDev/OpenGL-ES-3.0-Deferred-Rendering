attribute vec4 a_Position;
attribute vec2 a_TexCoord;

varying vec2 v_TexCoord;

void main()
{
    v_TexCoord = a_TexCoord;
    
    gl_Position = a_Position;
}