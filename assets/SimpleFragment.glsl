precision mediump float;
uniform sampler2D s_Albedo;
uniform sampler2D s_Normal;
uniform sampler2D s_Specular;

uniform vec3    u_LightDirections[64];
uniform vec3    u_LightColors[64];
uniform int     u_NumLights;

uniform vec3    u_CameraPosition;

uniform vec3    u_SpecularColor;
uniform float   u_SpecularPower;
uniform float   u_SpecularCoefficient;

varying vec3 v_WorldPos;
varying vec3 v_Normal;
varying vec3 v_TangentWorldSpace;
varying vec2 v_TexCoord;

void main(void) {
    vec2 texcoord = vec2(v_TexCoord.x, 1.0-v_TexCoord.y);

    /** Load texture values
     */
    vec3 albedo = texture2D(s_Albedo, texcoord).rgb;
    vec3 normal = normalize(texture2D(s_Normal, texcoord).rgb*2.0 - 1.0);
    vec3 specular_color = (texture2D(s_Normal, texcoord).rgb + u_SpecularColor) * u_SpecularCoefficient;

    vec3 dir_to_cam = normalize(u_CameraPosition - v_WorldPos);

    vec3 N = normalize(v_Normal);
    vec3 T = normalize(v_TangentWorldSpace - dot(v_TangentWorldSpace, N)*N);
    vec3 B = cross(N,T);

    mat3 TBN = mat3(T, B, N);
    normal = normalize(TBN*normal);

    vec4 final_color = vec4(0);
    float ambient_power = 0.15;
    float diffuse_power = 1.0-ambient_power;

    /** Perform lighting
     */
    for(int ii=0; ii < u_NumLights; ++ii) {
        vec3 light_color = u_LightColors[ii];
        vec3 light_dir = normalize(-u_LightDirections[ii]);
        /* Calculate diffuse lighting */
        float n_dot_l = clamp(dot(light_dir, normal), 0.0, 1.0);
        /* Calculate specular lighting */
        vec3 reflection = reflect(dir_to_cam, normal);
        float r_dot_l = clamp(dot(reflection, -light_dir), 0.0, 1.0);
        /* Calculate final colors */
        vec3 diffuse = albedo * light_color * diffuse_power * n_dot_l;
        vec3 specular = specular_color * vec3(min(1.0, pow(r_dot_l, u_SpecularPower))) * light_color;
        vec3 ambient = albedo * light_color * ambient_power;

        final_color += vec4(diffuse + ambient + specular,1.0);
    }

    gl_FragColor = final_color;
}
