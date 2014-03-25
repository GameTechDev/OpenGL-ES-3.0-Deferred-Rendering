precision highp float;
uniform sampler2D s_Normal;

uniform float   u_SpecularPower;

varying vec3 v_NormalVS;
varying vec3 v_TangentVS;
varying vec3 v_BitangentVS;
varying vec2 v_TexCoord;

varying vec2 v_Depth;

vec4 encode (vec3 normal)
{
    float p = sqrt(normal.z*8.0+8.0);
    return vec4(normal.xy/p + 0.5,0,0);
}

void main(void)
{
    /** Load texture values
     */
    vec3 normal = normalize(texture2D(s_Normal, v_TexCoord).rgb*2.0 - 1.0);
    
    vec3 N = normalize(v_NormalVS);
    vec3 T = normalize(v_TangentVS);
    vec3 B = normalize(v_BitangentVS);

    mat3 TBN = mat3(T, B, N);
    normal = normalize(TBN*normal);

    vec4 encoded = encode(normal);
    gl_FragColor = vec4(encoded.rgb, u_SpecularPower);
}
