precision mediump float;
uniform sampler2D s_Diffuse;

varying vec3 v_Normal;
varying vec3 v_TexCoord;

void main(void) {
    gl_FragColor = texture2D(s_Diffuse, v_TexCoord.st);
}
