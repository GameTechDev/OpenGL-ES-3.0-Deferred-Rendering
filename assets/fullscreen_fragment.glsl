precision mediump float;
uniform sampler2D s_diffuse;

varying vec2 v_texcoord;

void main()
{
    gl_FragColor = texture2D( s_diffuse, v_texcoord.st );
}
