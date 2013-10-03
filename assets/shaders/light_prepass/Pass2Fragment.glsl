precision highp float;
uniform sampler2D s_GBuffer;
uniform sampler2D s_Depth;

uniform vec2    u_Viewport;

uniform vec3    u_LightColor;
uniform vec3    u_LightPosition;
uniform float   u_LightSize;

varying vec3    v_PositionVS;

void main(void) {
    /** Load texture values
     */
    vec2 tex_coord = gl_FragCoord.xy/u_Viewport;

    vec4 gbuffer_val = texture2D(s_GBuffer, tex_coord);
    vec3 normal = gbuffer_val.rgb;
    float specular_power = gbuffer_val.a;
    float depth = texture2D(s_Depth, tex_coord).r;

    vec3 light_dir = u_LightPosition - v_PositionVS;
    float dist = length(light_dir);
    float size = u_LightSize;
    float attenuation = 1.0 - pow( clamp(dist/size, 0.0, 1.0), 2.0);
    light_dir = normalize(light_dir);

    /* Calculate diffuse lighting */
    float n_dot_l = clamp(dot(light_dir, normal), 0.0, 1.0);
    /* Calculate specular lighting */
    vec3 reflection = reflect(vec3(0.0,0.0,-1.0), normal);
    float r_dot_l = clamp(dot(reflection, -light_dir), 0.0, 1.0);
    /* Calculate final colors */
    vec3 diffuse = u_LightColor * n_dot_l;
    vec3 specular = vec3(1.0) * vec3(min(1.0, pow(r_dot_l, specular_power))) * u_LightColor;

    vec3 final_color = attenuation * (diffuse + specular);

    vec3 ambient = vec3(1.0);
    gl_FragColor = vec4(ambient + (1.0-ambient)*final_color,1.0);
    gl_FragColor = vec4(normal, 1.0);

}
