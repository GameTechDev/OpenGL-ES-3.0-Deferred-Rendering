precision mediump float;
uniform sampler2D s_Diffuse;

varying vec2 v_TexCoord;

void main()
{
    gl_FragColor = texture2D(s_Diffuse, v_TexCoord.st);
}
