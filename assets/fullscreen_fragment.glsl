#version 300 es
precision highp float;
uniform sampler2D s_Texture;

in vec2 v_TexCoord;

void main()
{
    gl_FragColor = texture2D(s_Texture, v_TexCoord);
    //gl_FragColor = vec4(v_TexCoord,1.0,1.0);
}
