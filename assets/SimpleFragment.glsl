precision mediump float;
uniform sampler2D s_Diffuse;

struct Light
{
    vec3    direction;
    vec3    color;
};

uniform Light   Lights[64];
uniform int     NumLights;

varying vec3 v_Normal;
varying vec2 v_TexCoord;

void main(void) {
    vec2 texcoord = vec2(v_TexCoord.x, 1.0-v_TexCoord.y);
    vec4 albedo = texture2D(s_Diffuse, texcoord);
    vec4 final_color = vec4(0.0);

    for(int ii=0; ii < NumLights; ++ii) {
        float n_dot_l = clamp(dot(normalize(-Lights[ii].direction), normalize(v_Normal)), 0.0, 1.0);
        final_color += albedo * n_dot_l;
    }

    gl_FragColor = final_color;
}
