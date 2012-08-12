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

void main() 
{
    vec2 vx = vec2(1.0f - texCoord.x, texCoord.x);
    vec2 vy = vec2(1.0f - texCoord.y, texCoord.y);

    float tmp = dot(vx, lightMatrix * vy);

    vec3 color = tmp*inColor.rgb;
    color = color*(texture2D(base_texture, texCoord3).rgb);      

    color = pow(color, vec3(1.0f / 2.2f));
    gl_FragColor.rgb = color;

}
