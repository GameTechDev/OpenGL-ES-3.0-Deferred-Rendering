precision highp float;
uniform sampler2D s_Normal;

uniform float   u_SpecularPower;

varying vec3 v_Normal;
varying vec3 v_TangentWorldSpace;
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
    vec3 normal = normalize(texture2D(s_Normal, v_TexCoord).rgb*2.0 - 1.0);

    /* Calculate normal */
    vec3 N = normalize(v_Normal);
    vec3 T = normalize(v_TangentWorldSpace - dot(v_TangentWorldSpace, N)*N);
    vec3 B = cross(N,T);

    mat3 TBN = mat3(T, B, N);
    normal = normalize(TBN*normal);

    gl_FragColor = vec4(pack_normal(normal),u_SpecularPower);
}
