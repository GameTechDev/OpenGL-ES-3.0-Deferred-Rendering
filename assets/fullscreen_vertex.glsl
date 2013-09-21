attribute vec4 a_position;
attribute vec2 a_texcoord;

varying vec2 v_texcoord;  

void main()
{
    v_texcoord = a_texcoord;
    
    gl_Position = a_position;
}