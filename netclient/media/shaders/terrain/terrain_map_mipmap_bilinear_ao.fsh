#version 120

#ifdef GL_EXT_gpu_shader4
#extension GL_EXT_gpu_shader4 : enable
#endif

#extension GL_EXT_texture_array : enable

#ifdef GL_EXT_gpu_shader4
    flat varying mat2 lightMatrix;
    //flat varying float skyLight;
    //flat varying float playerLight;
    flat varying vec2 Light;
#else
    varying mat2 lightMatrix;
    //varying float skyLight;
    //varying float playerLight;
    varying vec2 Light;
#endif


varying vec3 texCoord;
varying vec3 inColor;

varying float fogFragDepth;

uniform sampler2D clut_light_texture;
uniform sampler3D clut_texture;
uniform sampler2DArray base_texture;


const float gamma_factor = 1.0f / 2.2f;
const vec3 gamma_factor3 = vec3(gamma_factor);

const vec3 luma_const = vec3(0.299, 0.587, 0.114);


//How to calculate pixel brightness
//Brightness Calculation in Digital Image Processing - KWE
//Y = 0.299 r + 0.587 g + 0.114 b
//V = max (r, g, b)
//B = (r+g+b) /3.0  //works best


void main()
{
    vec4 tex = texture2DArray(base_texture, texCoord.xyz);
    // We can't do the alpha test until the vbo generator can handle
    /// generating vertices for sides that touch transparent sides
    // Otherwise, a texture with any transparency provides a window
    // into the world when placed next to a nontransparent block
    //if(tex.a < 0.4) //alpha test
    //{
        //discard;
    //}

/*
    Can split into 3 bilinear interpolations
*/
    vec2 vx = vec2(1.0f - texCoord.x, texCoord.x);
    vec2 vy = vec2(1.0f - texCoord.y, texCoord.y);
    float tmp = dot(vx, lightMatrix * vy);

    vec3 color = tmp * inColor * tex.rgb * texture2D(clut_light_texture, Light).rgb;

    //color = color * skyLight;
    //float color_power = color.length();

/*
    vec3 cp = (color.r+color.b+color.g) * vec3(0.33, 0.33, 0.33);
    color = color*texture2D(clut_light_texture, Light).rgb;
    const float desat = 0.50;
    color = mix(color, cp, desat);
*/

/*
    const float saturation = 0.50;
    color = color + saturation*color*texture2D(clut_light_texture, Light).rgb;
*/

    color = pow(color, gamma_factor3);
    //color = color*texture2D(clut_light_texture, Light).rgb;
    //color = texture2D(clut_light_texture, Light).rgb;

    const float clut_start = 64;
    const float _clut_depth = 1.0/32.0;

    /*
        Avoid non-uniform flow control in texture sampling
        http://www.opengl.org/wiki/Sampler_(GLSL)
    */
    vec3 color_clut = texture3D(clut_texture, color.rgb).rgb; //clut correction

    if(fogFragDepth > clut_start)
    {
        float f = _clut_depth*(fogFragDepth - clut_start);
        f = clamp(f, 0.0f, 1.0f);
        color.rgb = mix(color, color_clut, f);
    }

    //if(fogFragDepth > gl_Fog.start)
    //{
        //float f = gl_Fog.density * (fogFragDepth - gl_Fog.start);
        //float fogFactor = exp(-(f*f*f*f));
        //fogFactor = clamp(fogFactor, 0.0f, 1.0f);
        //gl_FragColor.a = 1.0f-fogFactor;
    //}

    gl_FragColor.rgb = color;


}



