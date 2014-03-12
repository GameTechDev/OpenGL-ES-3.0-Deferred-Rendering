#version 300 es
precision highp float;
uniform sampler2D s_Albedo;
uniform sampler2D s_Normal;

uniform vec3    u_LightPositions[64];
uniform vec3    u_LightColors[64];
uniform float   u_LightSizes[64];
uniform int     u_NumLights;

uniform vec3    u_SpecularColor;
uniform float   u_SpecularPower;
uniform float   u_SpecularCoefficient;

in vec3 v_PositionVS;
in vec3 v_NormalVS;
in vec3 v_TangentVS;
in vec3 v_BitangentVS;
in vec2 v_TexCoord;

out vec4 FragColor;

void main(void) {
    /** Load texture values
     */
    vec3 albedo = texture(s_Albedo, v_TexCoord).rgb;
    vec3 normal = normalize(texture(s_Normal, v_TexCoord).rgb*2.0 - 1.0);
    vec3 specular_color = u_SpecularCoefficient * u_SpecularColor;

    vec3 N = normalize(v_NormalVS);
    vec3 T = normalize(v_TangentVS);
    vec3 B = normalize(v_BitangentVS);

    mat3 TBN = mat3(T, B, N);
    normal = normalize(TBN*normal);

    vec3 final_color = vec3(0);
    for(int ii=0; ii < u_NumLights; ++ii) {
        vec3 light_color = u_LightColors[ii];
        vec3 light_dir = u_LightPositions[ii] - v_PositionVS;
        float dist = length(light_dir);
        float size = u_LightSizes[ii];
        float attenuation = 1.0 - pow( clamp(dist/size, 0.0, 1.0), 2.0);
        light_dir = normalize(light_dir);

        /* Calculate diffuse lighting */
        float n_dot_l = clamp(dot(light_dir, normal), 0.0, 1.0);
        /* Calculate specular lighting */
        vec3 reflection = reflect(vec3(0.0,0.0,-1.0), normal);
        float r_dot_l = clamp(dot(reflection, -light_dir), 0.0, 1.0);
        /* Calculate final colors */
        vec3 diffuse = albedo * light_color * n_dot_l;
        vec3 specular = specular_color * vec3(min(1.0, pow(r_dot_l, u_SpecularPower))) * light_color;

        final_color += attenuation * (diffuse + specular);
    }
    FragColor = vec4(final_color,1.0);
}
