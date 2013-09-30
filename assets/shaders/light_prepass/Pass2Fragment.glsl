precision highp float;
uniform sampler2D s_GBuffer;
uniform sampler2D s_Depth;

uniform mat4 u_InverseViewProj;

uniform vec3    u_LightColor;
uniform vec3    u_LightPosition;
uniform float   u_LightSize;

uniform vec3    u_CameraPosition;

varying vec4 v_ScreenPosition;
varying vec2 v_TexCoord;

vec3 pack_normal(vec3 normal)
{
    return (normal + 1.0) * 0.5;
}
vec3 unpack_normal(vec3 normal)
{
    return normal * 2.0 - 1.0;
}

void main(void) {
    /** Load texture values
     */
    vec4 gbuffer_val = texture2D(s_GBuffer, v_TexCoord);
    vec3 normal = normalize(gbuffer_val.rgb*2.0 - 1.0);
    float spec_power = gbuffer_val.a;
    float depth = texture2D(s_Depth, v_TexCoord).r;

    vec4 world_pos = vec4(v_ScreenPosition.xy, depth, 1.0);
    world_pos = u_InverseViewProj * world_pos;
    world_pos /= world_pos.w;

    vec3 dir_to_cam = normalize(u_CameraPosition - world_pos.xyz);


    /** Perform lighting
     */
    vec3 light_color = u_LightColor;
    vec3 light_dir = u_LightPosition - world_pos.xyz;
    float dist = length(light_dir);
    float size = u_LightSize;
    float attenuation = 1.0 - pow( clamp(dist/size, 0.0, 1.0), 2.0);
    light_dir = normalize(light_dir);

    /* Calculate diffuse lighting */
    float n_dot_l = clamp(dot(light_dir, normal), 0.0, 1.0);
    /* Calculate specular lighting */
    vec3 reflection = reflect(dir_to_cam, normal);
    float r_dot_l = clamp(dot(reflection, -light_dir), 0.0, 1.0);
    /* Calculate final colors */
    vec3 diffuse = light_color * n_dot_l;
    float specular = min(1.0, pow(r_dot_l, spec_power));

    gl_FragColor = vec4(diffuse, specular);
    //gl_FragColor = vec4(u_LightColor, 1.0);
    //gl_FragColor = vec4(world_pos.xyz, 1.0);
    //gl_FragColor = vec4(dist/10.0);
}
