precision mediump float;
uniform sampler2D s_Diffuse;

uniform vec3    LightDirections[64];
uniform vec3    LightColors[64];
uniform int     NumLights;

varying vec3 v_Normal;
varying vec2 v_TexCoord;

void main(void) {
    vec2 texcoord = vec2(v_TexCoord.x, 1.0-v_TexCoord.y);
    vec4 albedo = texture2D(s_Diffuse, texcoord);
    vec4 final_color = vec4(0.0);

    for(int ii=0; ii < NumLights; ++ii) {
        float n_dot_l = clamp(dot(normalize(-LightDirections[ii]), normalize(v_Normal)), 0.0, 1.0);
        final_color += albedo * n_dot_l * vec4(LightColors[ii],1);
    }

    gl_FragColor = final_color;
}
