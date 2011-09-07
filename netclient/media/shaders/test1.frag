uniform vec4 Color;

varying vec2 gEndpoints[2];
varying vec2 gPosition;

uniform float Radius;
uniform mat4 Projection;

float line_distance(vec2 pt, vec2 a, vec2 b)
{
    float dist = distance(a,b);
    vec2 v = normalize(b-a);
    float t = dot(v,pt-a);
    vec2 spinePoint;
    if (t > dist) spinePoint = b;
    else if (t > 0.0) spinePoint = a + t*v;
    else spinePoint = a;
    return distance(pt,spinePoint);
}

void main()
{
    vec2 x1 = gEndpoints[0];
    vec2 x2 = gEndpoints[1];
    float d = line_distance(gPosition,x1,x2);
    d = 1.0 - 12.0 * d;
    gl_FragColor = vec4(vec3(d), 1.0);
}
