precision highp float;
uniform sampler2D s_GBuffer;
uniform sampler2D s_Depth;

uniform mat4    u_InverseProj;
uniform mat4    u_InverseView;
uniform mat4    u_InverseViewProj;

uniform vec3    u_LightColor;
uniform vec3    u_LightPosition;
uniform float   u_LightSize;

uniform vec3    u_CameraPosition;

varying vec4 v_ScreenPosition;
varying vec2 v_TexCoord;

varying mat4 v_View;

vec3 pack_normal(vec3 normal)
{
    return (normal + 1.0) * 0.5;
}
vec3 unpack_normal(vec3 normal)
{
    return normal * 2.0 - 1.0;
}

float DepthToZPosition(float depth)
{
    vec2 camerarange = vec2(1.0,1000.0);
    return camerarange.x / (camerarange.y - depth * (camerarange.y - camerarange.x)) * camerarange.y;
}

void main(void) {
    /** Load texture values
     */
    vec4 gbuffer_val = texture2D(s_GBuffer, v_TexCoord);
    vec3 normal = normalize(gbuffer_val.rgb);
    float spec_power = gbuffer_val.a;
    float depth = texture2D(s_Depth, v_TexCoord).r;

    //depth = (depth + 1.0) / 2.0;

    float f = 1000.0;
    float n = 1.0;
    //depth = (2.0 * n) / (f + n - depth * (f - n));
//    
//    vec4 world_pos = vec4(gl_FragCoord.xy, depth, 1.0);
//    world_pos = u_InverseProj * world_pos;
//    world_pos /= world_pos.w;
//
//    world_pos = u_InverseView * world_pos;
//
//    vec2 viewport = vec2(640.0,960.0);
//    vec3 ndcspace = vec3(gl_FragCoord.xy/viewport * 2.0 - 1.0, depth);
//    float camspace_z = -1.0 / (ndcspace.z + 1.0);
//    vec4 clipspace = vec4(ndcspace * camspace_z, camspace_z);
//    vec4 viewspace = u_InverseProj * clipspace;
//    vec4 worldspace = u_InverseView * viewspace;

    vec4 world_pos = vec4(gl_FragCoord.xy, depth, 0.0);
    //world_pos = u_InverseProj * world_pos;
    //world_pos /= world_pos.w;
    //world_pos = u_InverseView * world_pos;

    vec3 screencoord;
    vec2 buffersize = vec2(1536.0,2048.0);
    screencoord = vec3(((gl_FragCoord.x/buffersize.x)-0.5) * 2.0,((-gl_FragCoord.y/buffersize.y)+0.5) * 2.0 / (buffersize.x/buffersize.y),DepthToZPosition( depth ));
    screencoord.x *= screencoord.z;
    screencoord.y *= -screencoord.z;

    world_pos.xyz = screencoord;
    
    //world_pos = u_InverseProj * world_pos;
    //world_pos /= world_pos.w;
    //world_pos = u_InverseView * world_pos;
    //world_pos = u_InverseViewProj * world_pos;

    
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

    //gl_FragColor = vec4(normal, 1.0);
    //gl_FragColor = vec4(diffuse, specular);
    //gl_FragColor = vec4(u_LightColor, 1.0);
    gl_FragColor = vec4(world_pos.xyz,1.0);
    //gl_FragColor = vec4(dist/10.0);
    //gl_FragColor = vec4(viewport/1000.0,1.0,1.0);
    //gl_FragColor = vec4(normal,1.0);
}
