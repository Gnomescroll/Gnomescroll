#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

#extension GL_EXT_texture_array : enable

varying vec3 texCoord;

#ifdef GL_EXT_gpu_shader4
    flat varying vec4 _lightMatrix;
#else
    varying vec4 _lightMatrix;
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

const vec3 fog_color = vec3(0.0, 0.0, 0.0);
const float fog_start = 48.0;
const float fog_depth = 128.0 - fog_start;

void main() 
{

    mat2 lightMatrix = mat2(_lightMatrix[0], _lightMatrix[1], _lightMatrix[2],_lightMatrix[3] );


    //float gamma = 2.2f;

/*
    Can split into 3 bilinear interpolations
*/
    vec2 vx = vec2(1.0f - texCoord.x, texCoord.x);
    vec2 vy = vec2(1.0f - texCoord.y, texCoord.y);

    float tmp = dot(vx, lightMatrix * vy);

    vec3 color = inColor.rgb; 
    
    //color = color*tmp;
    //color = pow(color, vec3(1.0f / 2.2f) );
    //gl_FragColor.rgb = color;

    color = tmp*color;
    color = color*(texture2DArray(base_texture, texCoord.xyz).rgb);      
    //color = color * inColor;



    if(fogFragDepth <= fog_start)
    {
        color = pow(color, vec3(1.0f / 2.2f) );
        gl_FragColor.rgb = color;
    }
    else
    {
        float fogFactor = (fogFragDepth - fog_start) / fog_depth;

        if(fogFactor >= 1.0) discard;
        
        color = mix( color, fog_color, fogFactor);
        color = pow(color, vec3(1.0f / 2.2f) );
        gl_FragColor.rgb = color;
    }

}



