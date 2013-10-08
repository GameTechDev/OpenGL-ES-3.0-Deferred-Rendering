precision highp float;
uniform sampler2D s_GBuffer[3];
uniform sampler2D s_Depth;

uniform mat4    u_InvProj;

uniform vec2    u_Viewport;

uniform vec3    u_LightColor;
uniform vec3    u_LightPosition;
uniform float   u_LightSize;

/** GBuffer format
 *  [0] RGB: Albedo
 *  [1] RGB: VS Normal
 *  [2] R: Depth
 */
void main(void)
{
    /** Load texture values
     */
    vec2 tex_coord = gl_FragCoord.xy/u_Viewport; // map to [0..1]

    vec3 albedo = texture2D(s_GBuffer[0], tex_coord).rgb;
    vec3 normal = texture2D(s_GBuffer[1], tex_coord).rgb * 2.0 - 1.0;
    float depth = texture2D(s_GBuffer[2], tex_coord).r;

    float hardware_depth = texture2D(s_Depth, tex_coord).r;

    /* Calculate the pixel's position in view space */
    vec4 view_pos = vec4(tex_coord*2.0-1.0, hardware_depth*2.0 - 1.0, 1.0);
    view_pos = u_InvProj * view_pos;
    view_pos /= view_pos.w;

    vec3 light_dir = u_LightPosition - view_pos.xyz;
    float dist = length(light_dir);
    float size = u_LightSize;
    float attenuation = 1.0 - pow( clamp(dist/size, 0.0, 1.0), 2.0);
    light_dir = normalize(light_dir);

    /* Calculate diffuse lighting */
    float n_dot_l = clamp(dot(light_dir, normal), 0.0, 1.0);
    vec3 diffuse = u_LightColor * n_dot_l;

    vec3 final_lighting = attenuation * (diffuse);

    gl_FragColor = vec4(final_lighting * albedo,1.0);
}
