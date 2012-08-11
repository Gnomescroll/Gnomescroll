attribute vec3 InTexCoord0;
attribute vec4 InLightMatrix0; //intensity for AO at each corner of quad
 
varying vec3 texCoord;
varying mat2 lightMatrix0;
 
varying vec4 inColor;
varying vec3 vertex;

void main(void) 
{                       
        inColor = gl_Color;
 
        texCoord = InTexCoord0;

        lightMatrix0 = mat2(InLightMatrix0[0], InLightMatrix0[1], InLightMatrix0[2],InLightMatrix0[3]);

        gl_Vertex[2] += sin(gl_Vertex[0])*cos(gl_Vertex[1]) / 10.0f;

        vertex = gl_Vertex.xyz;

        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

        float dist = length(gl_Position);
        gl_Position[0] += (sin((gl_Position[2] + gl_Position[0])/4.0f) / 128.0f) * dist;
        gl_Position[1] += (cos((gl_Position[2] + gl_Position[1])/4.0f) / 128.0f) * dist;
}
