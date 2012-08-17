#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

#ifdef GL_EXT_gpu_shader4
    flat varying mat2 lightMatrix;
#else
    varying mat2 lightMatrix;
#endif

varying vec2 texCoord;
varying vec2 texCoord3;
 
varying vec3 inColor;
 
uniform sampler2D base_texture;

varying float fogFragDepth;

varying float skyLight;
varying float playerLight;

const float gamma_factor = 1.0f / 2.2f;
const vec3 gamma_factor3 = vec3(gamma_factor);

void main() 
{
    vec2 vx = vec2(1.0f - texCoord.x, texCoord.x);
    vec2 vy = vec2(1.0f - texCoord.y, texCoord.y);

    float tmp = skyLight*dot(vx, lightMatrix * vy);

    vec3 color = tmp*inColor.rgb;
    color = color*(texture2D(base_texture, texCoord3).rgb);      

    float f = gl_Fog.density * fogFragDepth;
    f = f*f; 
    float fogFactor = exp(-(f*f));
    fogFactor = clamp(fogFactor, 0.0f, 1.0f);
    color = mix(color, gl_Fog.color.xyz, 1.0f-fogFactor); 

    color = pow(color, gamma_factor3);
    gl_FragColor.rgb = color;

}
