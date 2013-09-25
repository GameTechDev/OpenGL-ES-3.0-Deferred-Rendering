precision mediump float;
uniform sampler2D s_Diffuse;

varying vec3 v_Normal;
varying vec2 v_TexCoord;

void main(void) {
    vec2 texcoord = vec2(v_TexCoord.x, 1.0-v_TexCoord.y);
    gl_FragColor = texture2D(s_Diffuse, texcoord);
}
