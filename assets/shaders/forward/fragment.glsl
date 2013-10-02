precision highp float;
uniform sampler2D s_Albedo;
uniform sampler2D s_Normal;

varying vec3 v_PositionVS;
varying vec3 v_NormalVS;
varying vec3 v_TangentVS;
varying vec2 v_TexCoord;


void main(void) {
    /** Load texture values
     */
    vec3 albedo = texture2D(s_Albedo, v_TexCoord).rgb;
    vec3 normal = normalize(texture2D(s_Normal, v_TexCoord).rgb*2.0 - 1.0);

    gl_FragColor = vec4(albedo,1.0);
}
