#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

uniform sampler2D base_texture;

varying vec2 texCoord;
//varying float diffuse;

#define diffuse_p .30f //diffuse lighting weight
#define ambient_p .70f //ambient lighting weight

varying vec3 normal1;
const vec3 light_dir = normalize(vec3(1.00, 1.00, 2.00));

void main() 
{
    //float gamma = 2.2f;
    vec4 color = texture2D(base_texture, texCoord.xy);
    //vec4 color = vec4(1.0,1.0,1.0,1.0);

    float diffuse1 =  abs(dot(gl_NormalMatrix*normal1, light_dir ));
    color = ((diffuse_p*diffuse1)*color) + ambient_p*color;
    
    gl_FragColor = color;
    //gl_FragColor.rgb = pow(color, vec3(1.0f / 2.2f) );

}



