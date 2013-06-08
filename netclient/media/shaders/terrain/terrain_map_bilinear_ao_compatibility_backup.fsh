#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

varying mat2 lightMatrix;

varying vec2 texCoord;
varying vec2 texCoord2;

varying vec3 inColor;

varying float skyLight;
varying float playerLight;

uniform sampler2D base_texture;

const float gamma_factor = 1.0f / 2.2f;
const vec3 gamma_factor3 = vec3(gamma_factor);

void main()
{
    vec2 vx = vec2(1.0f - texCoord2.x, texCoord2.x);
    vec2 vy = vec2(1.0f - texCoord2.y, texCoord2.y);

    float tmp = skyLight*dot(vx, lightMatrix * vy);

    vec3 color = tmp*inColor.rgb;
    color = color*(texture2D(base_texture, texCoord).rgb);

    color = pow(color, gamma_factor3);
    gl_FragColor.rgb = color;
}
