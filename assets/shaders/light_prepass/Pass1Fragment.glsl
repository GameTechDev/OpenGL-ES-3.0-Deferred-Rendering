#version 300 es
precision highp float;
uniform sampler2D s_Normal;

uniform float   u_SpecularPower;

in vec3 v_NormalVS;
in vec3 v_TangentVS;
in vec3 v_BitangentVS;
in vec2 v_TexCoord;

in vec2 v_Depth;

out vec4 FragColor;

void main(void)
{
    /** Load texture values
     */
    vec3 normal = normalize(texture(s_Normal, v_TexCoord).rgb*2.0 - 1.0);
    
    vec3 N = normalize(v_NormalVS);
    vec3 T = normalize(v_TangentVS);
    vec3 B = normalize(v_BitangentVS);

    mat3 TBN = mat3(T, B, N);
    normal = normalize(TBN*normal);

    FragColor = vec4((normal + 1.0) * 0.5, u_SpecularPower);
}
