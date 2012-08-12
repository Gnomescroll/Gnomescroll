#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

#extension GL_EXT_texture_array : enable

varying vec3 texCoord;

#ifdef GL_EXT_gpu_shader4
    flat varying mat2 lightMatrix;
#else
    varying mat2 lightMatrix;
#endif

varying vec3 inColor;
 
uniform sampler2DArray base_texture;

varying float fogFragDepth;

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

    float f = gl_Fog.density * fogFragDepth;
    float fogFactor = exp(-(f*f*f*f));
    fogFactor = clamp(fogFactor, 0.0f, 1.0f);
    color = mix(color, gl_Fog.color.xyz, 1.0f-fogFactor); 

    color = pow(color, vec3(1.0f / 2.2f));
    gl_FragColor.rgb = color;

}



