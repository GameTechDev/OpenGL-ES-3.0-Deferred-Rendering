precision highp float;
uniform sampler2D s_Texture;

uniform vec4 u_Color;

varying vec2 v_TexCoord;

void main()
{
    gl_FragColor = vec4(1.0) * texture2D(s_Texture, v_TexCoord);
}
