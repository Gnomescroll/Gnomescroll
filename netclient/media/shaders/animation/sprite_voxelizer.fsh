#version 120

varying float diffuse_light;
varying vec3 base_color;

#define diffuse_p .30f //diffuse lighting weight
#define ambient_p .70f //ambient lighting weight

void main() 
{
    vec3 color = (diffuse_light*diffuse_p+ambient_p) * base_color;
    color = pow(color, vec3(1.0f / 2.2f));  // gamma correction  TOO BRIGHT
    gl_FragColor.rgb = color;

    gl_FragColor.rgb = vec3(diffuse_light, diffuse_light, diffuse_light);
}
