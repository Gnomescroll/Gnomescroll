#pragma once
#pragma GCC diagnostic ignored "-Wunused-function"

#include <math.h>

#include <c_lib/physics/vec3.hpp>

struct Vec4 {
    union
    {
        float f[4];
        struct 
        {
            float x,y,z,w;
        };
        struct 
        {
            Vec3 v3;    
        };
    };
} __attribute__((aligned (16)));

/*
static struct Vec4 vec4_init(float x, float y, float z) __attribute((always_inline)); 

struct Vec4 vec4_init(float x, float y, float z)
{
    struct Vec4 v;
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = 0;
    return v;
}
*/

static struct Vec4 vec4_init(float x, float y, float z, float w) __attribute((always_inline)); 

struct Vec4 vec4_init(float x, float y, float z, float w)
{
    struct Vec4 v;
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w;
    return v;
}


/*


void mmul_vec4
(const float * a, const float * b, float * r)
{
  for (int i=0; i<16; i+=4) {
    vec4 rl = vec4(a) * vec4(b[i]);
    for (int j=1; j<4; j++)
      rl += vec4(&a[j*4]) * vec4(b[i+j]);
    rl >> &r[i];
  }
}

Vec4 is actually a wrapper on __m128 SSE operations, so mmul_vec4 runs as fast as mmul_sse

struct vec4
{
  __m128 xmm;

  vec4 (__m128 v) : xmm (v) {}

  vec4 (float v) { xmm = _mm_set1_ps(v); }

  vec4 (float x, float y, float z, float w)
  { xmm = _mm_set_ps(w,z,y,x); }

  vec4 (const float *v) { xmm = _mm_load_ps(v); }

  vec4 operator* (const vec4 &v) const
  { return vec4(_mm_mul_ps(xmm, v.xmm)); }

  vec4 operator+ (const vec4 &v) const
  { return vec4(_mm_add_ps(xmm, v.xmm)); }

  vec4 operator- (const vec4 &v) const
  { return vec4(_mm_sub_ps(xmm, v.xmm)); }

  vec4 operator/ (const vec4 &v) const
  { return vec4(_mm_div_ps(xmm, v.xmm)); }

  void operator*= (const vec4 &v)
  { xmm = _mm_mul_ps(xmm, v.xmm); }

  void operator+= (const vec4 &v)
  { xmm = _mm_add_ps(xmm, v.xmm); }

  void operator-= (const vec4 &v)
  { xmm = _mm_sub_ps(xmm, v.xmm); }

  void operator/= (const vec4 &v)
  { xmm = _mm_div_ps(xmm, v.xmm); }

  void operator>> (float *v)
  { _mm_store_ps(v, xmm); }

};


*/