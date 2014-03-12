#version 300 es
precision highp float;
uniform sampler2D s_GBuffer;
uniform sampler2D s_Albedo;

uniform vec2 u_Viewport;

in vec2 v_TexCoord;

out vec4 FragColor;

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
    vec3 light = texture(s_GBuffer,tex_coord).rgb;
    vec3 albedo = texture(s_Albedo, v_TexCoord).rgb;
    FragColor = vec4(light*albedo,1.0);
}
