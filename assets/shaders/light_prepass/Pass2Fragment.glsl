precision highp float;
uniform sampler2D s_GBuffer;
uniform sampler2D s_Depth;

varying vec4 v_ScreenPosition;
varying vec2 v_TexCoord;

void main(void) {
    /** Load texture values
     */
    vec4 gbuffer_val = texture2D(s_GBuffer, v_TexCoord);
    float depth = texture2D(s_Depth, v_TexCoord).r;

    gl_FragColor = vec4(1.0);
}
