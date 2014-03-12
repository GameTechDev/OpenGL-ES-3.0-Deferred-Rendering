#version 300 es

precision highp float;
uniform sampler2D s_Albedo;
uniform sampler2D s_Normal;

uniform vec3    u_SpecularColor;
uniform float   u_SpecularPower;
uniform float   u_SpecularCoefficient;

in vec3 v_NormalVS;
in vec3 v_TangentVS;
in vec3 v_BitangentVS;
in vec2 v_TexCoord;

out vec4 FragData[2];

vec4 encode(vec3 normal)
{
    return vec4(normal.xy*0.5+0.5, 0,0);
}

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

    /** GBuffer format
     *  [0] RGB: Albedo
     *  [1] RGB: VS Normal
     *  [2] R: Depth
     */
    FragData[0] = vec4(albedo, 1.0);
    FragData[1] = encode(normal);
}
