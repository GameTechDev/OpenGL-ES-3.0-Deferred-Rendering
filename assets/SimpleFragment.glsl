precision mediump float;
uniform sampler2D s_Albedo;
uniform sampler2D s_Normal;
uniform sampler2D s_Specular;

uniform vec3    u_LightDirections[64];
uniform vec3    u_LightColors[64];
uniform int     u_NumLights;

varying vec3 v_Normal;
varying vec2 v_TexCoord;

vec3 phong( vec3 light_dir, vec3 light_color,
            vec3 normal, vec3 albedo,
            vec3 dir_to_cam, float attenuation,
            vec3 spec_color, float spec_power, float spec_coefficient)
{
    float n_dot_l = clamp(dot(light_dir, normal), 0.0, 1.0);
    vec3 reflection = reflect(dir_to_cam, normal);
    float r_dot_l = clamp(dot(reflection, -light_dir), 0.0, 1.0);

    vec3 specular = vec3(min(1.0, pow(r_dot_l, spec_power))) * light_color * spec_color * spec_coefficient;
    vec3 diffuse = albedo * light_color * n_dot_l;

    return attenuation * (diffuse + specular);
}

void main(void) {
    vec2 texcoord = vec2(v_TexCoord.x, 1.0-v_TexCoord.y);
    vec4 albedo = texture2D(s_Albedo, texcoord);
    vec4 final_color = vec4(0);
    float ambient_power = 0.05;
    float diffuse_power = 1.0-ambient_power;

    for(int ii=0; ii < u_NumLights; ++ii) {
        float n_dot_l = clamp(dot(normalize(-u_LightDirections[ii]), normalize(v_Normal)), 0.0, 1.0);
        vec4 light_color = vec4(u_LightColors[ii],1);
        vec4 diffuse = albedo * light_color * diffuse_power * n_dot_l;
        vec4 ambient = albedo * light_color * ambient_power;

        final_color += diffuse + ambient;
    }

    gl_FragColor = final_color;
}
