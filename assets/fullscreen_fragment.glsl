precision lowp float;
uniform lowp sampler2D s_Diffuse;

varying mediump vec2 v_TexCoord;

void main()
{
    gl_FragColor = texture2D(s_Diffuse, v_TexCoord);
}
