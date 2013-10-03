precision highp float;
uniform sampler2D s_GBuffer;
uniform sampler2D s_Depth;

uniform mat4    u_InvProj;

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
    vec3 normal = gbuffer_val.rgb * 2.0 - 1.0;
    float specular_power = gbuffer_val.a;
    float depth = texture2D(s_Depth, tex_coord).r;

    //depth = DepthToZPosition(depth);
    //vec4 view_pos = vec4(tex_coord.x*2.0 - 1.0, (tex_coord.y*2.0 - 1.0), depth, 1.0);
    //view_pos = u_InvProj * view_pos;
    //view_pos /= view_pos.w;

    float near_plane = 1.0;
    float far_plane = 100.0;
    vec3 view_ray = vec3(v_PositionVS.xy/v_PositionVS.z, 1.0);
    float ProjA = far_plane / (far_plane - near_plane);
    float ProjB = (-far_plane * near_plane) / (far_plane - near_plane);
    float linear_depth = ProjB / (depth - ProjA);
    vec3 view_pos = view_ray * linear_depth;

    vec3 light_dir = u_LightPosition - view_pos.rgb;
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

    vec3 final_color = (diffuse);

    gl_FragColor = vec4(dist/10.0);
    //gl_FragColor = vec4(view_pos.z/10.0);
    //gl_FragColor = vec4(v_PositionVS/10.0,1.0);
}
