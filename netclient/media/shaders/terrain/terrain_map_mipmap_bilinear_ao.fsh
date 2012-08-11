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
//varying float fogFragZ;

//const vec3 fog_color = vec3(0.0, 0.0, 0.0);
//const float fog_start = 96.0;
//const float fog_depth = 32.0;
//const vec3 fog_z_color = vec3(40.0, 0.0, 20.0);
//const vec3 fog_color = vec3(10.0);
//const float fog_z_start = 16.0f;
//const float fog_z_depth = 128.0f;
//const float fog_z_density = 0.35f;
//const float z_depth_max = 128.0f;

//const float LOG2 = 1.442695;

uniform vec3 InFogColor;
uniform float InFogStart;
uniform float InFogDepth;

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

    if (fogFragDepth <= InFogStart)
    {
        color = pow(color, vec3(1.0f / 2.2f));
        gl_FragColor.rgb = color;
    }
    else
    {
        float fogFactor = (fogFragDepth - InFogStart) / InFogDepth;

        if (fogFactor >= 1.0) discard;
        
        color = mix(color, InFogColor, fogFactor);
        color = pow(color, vec3(1.0f / 2.2f));
        gl_FragColor.rgb = color;
    }
}



