#version 120

varying float diffuse_light;
varying mat2 AOMatrix;
varying vec2 AOInterpolate;

#define gamma 1.0f/2.2f
#define diffuse_p .30f //diffuse lighting weight
#define ambient_p .50f //ambient lighting weight

void main() 
{
    vec2 vx = vec2(1.0f - AOInterpolate.x, AOInterpolate.x);
    vec2 vy = vec2(1.0f - AOInterpolate.y, AOInterpolate.y);
    float ao = dot(vx, AOMatrix * vy);

    //vec3 color = ((diffuse_light*diffuse_p) + (ao*ambient_p)) * gl_Color.rgb;
    vec3 color = ((diffuse_light*diffuse_p) + (ambient_p)) * gl_Color.rgb;
    color = pow(color, vec3(gamma));  // gamma correction
    gl_FragColor.rgb = color;
    //gl_FragColor.rgb = vec3(ao);

    //uncomment to see the diffuse
    //gl_FragColor.rgb = vec3(diffuse_light, diffuse_light, diffuse_light);

    //uncomment to see total lighting
    //gl_FragColor.rgb = vec3(diffuse_light*diffuse_p+ambient_p, diffuse_light*diffuse_p+ambient_p, diffuse_light*diffuse_p+ambient_p);
}
