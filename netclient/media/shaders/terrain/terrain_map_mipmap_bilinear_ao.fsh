#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

#extension GL_EXT_texture_array : enable

#ifdef GL_EXT_gpu_shader4
    flat varying mat2 lightMatrix;
    flat varying float skyLight;
    flat varying float playerLight;
#else
    varying mat2 lightMatrix;
    varying float skyLight;
    varying float playerLight;
#endif

varying vec3 texCoord;
varying vec3 inColor;

varying float fogFragDepth;

uniform sampler2DArray base_texture;

const float fog_start = 64.0;

const float gamma_factor = 1.0f / 2.2f;
const vec3 gamma_factor3 = vec3(gamma_factor);

void main() 
{
/*
    Can split into 3 bilinear interpolations
*/
    vec2 vx = vec2(1.0f - texCoord.x, texCoord.x);
    vec2 vy = vec2(1.0f - texCoord.y, texCoord.y);

    float tmp = dot(vx, lightMatrix * vy);

    vec3 color = tmp*inColor.rgb;
    color = color*(texture2DArray(base_texture, texCoord.xyz).rgb);      

/*
    if(skyLight > 0.5f)
    {
        color = vec3(1.0, 0.0, 0.0);
    }
*/

    if(fogFragDepth > fog_start)
    {
        float f = gl_Fog.density * fogFragDepth;
        float fogFactor = exp(-(f*f*f*f));
        fogFactor = clamp(fogFactor, 0.0f, 1.0f);
        color = mix(color, gl_Fog.color.xyz, 1.0f-fogFactor); 
    }

    color = color * skyLight;

    color = pow(color, gamma_factor3);
    gl_FragColor.rgb = color;

}



