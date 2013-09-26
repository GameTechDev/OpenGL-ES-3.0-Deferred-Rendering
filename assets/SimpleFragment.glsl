precision mediump float;
uniform sampler2D s_Diffuse;

uniform vec4 LightDir;

varying vec3 v_Normal;
varying vec2 v_TexCoord;

void main(void) {
    vec2 texcoord = vec2(v_TexCoord.x, 1.0-v_TexCoord.y);

    float n_dot_l = clamp(dot(normalize(-LightDir.xyz), normalize(v_Normal)), 0.0, 1.0);

    gl_FragColor = texture2D(s_Diffuse, texcoord) * n_dot_l;
}
