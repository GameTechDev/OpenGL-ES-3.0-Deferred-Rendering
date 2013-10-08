precision highp float;
uniform sampler2D s_Albedo;
uniform sampler2D s_Normal;

uniform vec3    u_SpecularColor;
uniform float   u_SpecularPower;
uniform float   u_SpecularCoefficient;

varying vec3 v_NormalVS;
varying vec3 v_TangentVS;
varying vec3 v_BitangentVS;
varying vec2 v_TexCoord;
varying vec2 v_Depth;

void main(void) {
    /** Load texture values
     */
    vec3 albedo = texture2D(s_Albedo, v_TexCoord).rgb;
    vec3 normal = normalize(texture2D(s_Normal, v_TexCoord).rgb*2.0 - 1.0);
    vec3 specular_color = u_SpecularCoefficient * u_SpecularColor;
    
    vec3 N = normalize(v_NormalVS);
    vec3 T = normalize(v_TangentVS);
    vec3 B = normalize(v_BitangentVS);

    mat3 TBN = mat3(T, B, N);
    normal = normalize(TBN*normal);
    normal += 1.0;
    normal *= 0.5;

    /** GBuffer format
     *  [0] RGB: Albedo
     *  [1] RGB: VS Normal
     */
    gl_FragData[0] = vec4(albedo, 1.0);
    gl_FragData[1] = vec4(normal, 1.0);
}
