precision highp float;
uniform sampler2D s_Albedo;
uniform sampler2D s_Normal;

uniform vec3   u_LightPositions[64];
uniform vec3   u_LightColors[64];
uniform float  u_LightSizes[64];
uniform int    u_NumLights;

uniform vec3   u_SunDirection;
uniform vec3   u_SunColor;

uniform vec3    u_CameraPosition;

uniform vec3    u_SpecularColor;
uniform float   u_SpecularPower;
uniform float   u_SpecularCoefficient;

varying vec3 v_PositionVS;
varying vec3 v_NormalVS;
varying vec3 v_TangentVS;
varying vec2 v_TexCoord;

varying mat4 v_View;

void main(void) {
    /** Load texture values
     */
    vec3 albedo = texture2D(s_Albedo, v_TexCoord).rgb;
    vec3 normal = normalize(texture2D(s_Normal, v_TexCoord).rgb*2.0 - 1.0);
    vec3 specular_color = u_SpecularColor * u_SpecularCoefficient;

    vec3 dir_to_cam = normalize((mat3(v_View) * u_CameraPosition) - v_PositionVS);

    vec3 N = normalize(v_NormalVS);
    vec3 T = normalize(v_TangentVS - dot(v_TangentVS, N)*N);
    vec3 B = cross(N,T);

    mat3 TBN = mat3(T, B, N);
    normal = normalize(TBN*normal);

    vec3 final_color = vec3(0);
    float ambient_power = 0.1;
    float diffuse_power = 1.0-ambient_power;

    /** Perform lighting
     */
    vec3 light_color = u_SunColor;
    vec3 light_dir = normalize(-u_SunDirection);
    light_dir = mat3(v_View) * light_dir;
    /* Calculate diffuse lighting */
    float n_dot_l = clamp(dot(light_dir, normal), 0.0, 1.0);
    /* Calculate specular lighting */
    vec3 reflection = reflect(dir_to_cam, normal);
    float r_dot_l = clamp(dot(reflection, -light_dir), 0.0, 1.0);
    /* Calculate final colors */
    vec3 diffuse = albedo * light_color * diffuse_power * n_dot_l;
    vec3 specular = specular_color * vec3(min(1.0, pow(r_dot_l, u_SpecularPower))) * light_color;
    vec3 ambient = albedo * light_color * ambient_power;

    final_color += diffuse + ambient + specular;

    for(int ii=0; ii < u_NumLights; ++ii) {
        vec3 light_color = u_LightColors[ii];
        vec3 light_dir = vec3(v_View * vec4(u_LightPositions[ii],1.0)) - v_PositionVS;
        float dist = length(light_dir);
        float size = u_LightSizes[ii];
        float attenuation = 1.0 - pow( clamp(dist/size, 0.0, 1.0), 2.0);
        light_dir = normalize(light_dir);

        /* Calculate diffuse lighting */
        float n_dot_l = clamp(dot(light_dir, normal), 0.0, 1.0);
        /* Calculate specular lighting */
        vec3 reflection = reflect(dir_to_cam, normal);
        float r_dot_l = clamp(dot(reflection, -light_dir), 0.0, 1.0);
        /* Calculate final colors */
        vec3 diffuse = albedo * light_color * n_dot_l;
        vec3 specular = specular_color * vec3(min(1.0, pow(r_dot_l, u_SpecularPower))) * light_color;

        final_color += attenuation * (diffuse + specular);    }

    gl_FragColor = vec4(final_color,1.0);
}
