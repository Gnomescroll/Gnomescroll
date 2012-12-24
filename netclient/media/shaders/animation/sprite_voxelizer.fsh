#version 120

varying float diffuse_light;

#define gamma 1.0f/2.2f
#define diffuse_p 1.0f //diffuse lighting weight
#define ambient_p .50f //ambient lighting weight

void main() 
{
    vec3 color = ((diffuse_light*diffuse_p) + (ambient_p)) * gl_Color.rgb;
    color = pow(color, vec3(gamma));  // gamma correction
    
    //uncomment to see the diffuse
    //color = vec3(diffuse_light, diffuse_light, diffuse_light);

    //uncomment to see total lighting
    //color = vec3(diffuse_light*diffuse_p+ambient_p, diffuse_light*diffuse_p+ambient_p, diffuse_light*diffuse_p+ambient_p);

    gl_FragColor.rgb = color;
}
