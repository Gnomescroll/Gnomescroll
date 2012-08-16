#ifndef __EPSILON_3D__MATRIX_H__
#define __EPSILON_3D__MATRIX_H__

#include "vector3.h"

class e3dMatrix4x4
{
public:
  /**
   * Stores 4 components from this 4x4 matrix...
   *
   *   Used for things like Row (...) and Column (...), making sure that
   *     they return an object instead of a float*.
  **/
  class Real4 {
  public:
    inline operator const float* (void) const { return m; }

    union {
      float m [4];
      struct {
        float x, y, z, w;
      };
    };
  };

  inline  e3dMatrix4x4 (void) { Identity (); }
  inline ~e3dMatrix4x4 (void) { }

  //explicit e3dMatrix4x4 (const e3dMatrix3x3& matrix };

  // Copy constructor
  inline e3dMatrix4x4 (const e3dMatrix4x4& m) {
    *this = m;
  }

  inline e3dMatrix4x4 (float mat [16]) {
    for (int i = 0; i < 16; i++) {
      m [i] = mat [i];
    }
  }

  e3dMatrix4x4& operator= (const e3dMatrix4x4& m) {
    // Handle possible self-assignment
    if (&m == this)
      return *this;

    xx = m.xx,  xy = m.xy,  xz = m.xz,  xw = m.xw;
    yx = m.yx,  yy = m.yy,  yz = m.yz,  yw = m.yw;
    zx = m.zx,  zy = m.zy,  zz = m.zz,  zw = m.zw;
    wx = m.wx,  wy = m.wy,  wz = m.wz,  ww = m.ww;

    return *this;
  }

  inline float& operator[] (unsigned int i)       { return m [i]; }
  inline float  operator[] (unsigned int i) const { return m [i]; }

  inline void LookAt (const e3dVector3& vEye,
                      const e3dVector3& vLookAt,
                      const e3dVector3& vUp)
  {
    e3dVector3 vFwd (vLookAt - vEye);
    vFwd.Normalize ();

    e3dVector3 vNewUp (vUp % vFwd);
    vNewUp.Normalize ();

    e3dVector3 vRight (vFwd % vNewUp);

    xx =  vNewUp.x;
    yx =  vNewUp.y;
    zx =  vNewUp.z;
    wx = -(vNewUp * vEye);

    xy =  vRight.x;
    yy =  vRight.y;
    zy =  vRight.z;
    wy = -(vRight * vEye);

    xz =  vFwd.x;
    yz =  vFwd.y;
    zz =  vFwd.z;
    wz = -(vFwd * vEye);

    xw = yw = zw = 0.0f;
    ww = 1.0f;
  }

  inline e3dMatrix4x4& Rotation (e3dVector3& vAxis, float fAngle)
  {
    const float c = cosf (fAngle);
    const float s = sinf (fAngle);
    const float t = 1.0f - c;

    e3dVector3 nAxis = vAxis;
    nAxis.Normalize ();

    // intermediate values
    float tx  =  t*nAxis.x; float ty  =  t*nAxis.y; float tz  =  t*nAxis.z;
    float sx  =  s*nAxis.x; float sy  =  s*nAxis.y; float sz  =  s*nAxis.z;
    float txy = tx*nAxis.y; float tyz = tx*nAxis.z; float txz = tx*nAxis.z;

    // set matrix
    m[0] = tx*nAxis.x + c;
    m[4] = txy - sz; 
    m[8] = txz + sy;
    m[12] = 0.0f;

    m[1] = txy + sz;
    m[5] = ty*nAxis.y + c;
    m[9] = tyz - sx;
    m[13] = 0.0f;

    m[2] = txz - sy;
    m[6] = tyz + sx;
    m[10] = tz*nAxis.z + c;
    m[14] = 0.0f;

    m[3] = 0.0f;
    m[7] = 0.0f;
    m[11] = 0.0f;
    m[15] = 1.0f;

    return *this;
  }

  inline e3dMatrix4x4& RotationX (const float fAngle) {
    Identity ();

    xrot (fAngle);

    return *this;
  }

  inline e3dMatrix4x4& RotationY (const float fAngle) {
    Identity ();

    yrot (fAngle);
    
    return *this;
  }
  
  inline e3dMatrix4x4& RotationZ (const float fAngle) {
    Identity ();

    zrot (fAngle);

    return *this;
  }

  void xrot (const float fAngle) {
    const float c = cosf (fAngle);
    const float s = sinf (fAngle);

    yy =  c;
    yz =  s;
    zy = -s;
    zz =  c;

    ww = 1.0f;
  }

  void yrot (const float fAngle) {
    const float c = cosf (fAngle);
    const float s = sinf (fAngle);

    xx =  c;
    xz = -s;
    zx =  s;  
    zz =  c;

    ww = 1.0f;
  }
  
  void zrot (const float fAngle) {
    const float c = cosf (fAngle);
    const float s = sinf (fAngle);

    xx =  c;   
    xy =  s;
    yx = -s;  
    yy =  c;

    ww = 1.0f;
  }

  inline e3dVector3 transform (const e3dVector3& v) const;

  /// Multiply two matrices
  inline e3dMatrix4x4 operator* (const e3dMatrix4x4& s) const;


  /* Always Copy the Array Returned by Row / Column If
       The Value Must Persist... */

  /// Returns a 4-element floating-point array containing the
  ///   requested row (valid row idx. = 0-3)
  inline const Real4 Row (int row_num) const {
    /* Assert (row_num >= 0 && row_num < 4); */
    Real4 ret;

    const int row_base = (row_num * 4);

    ret.m [0] = m [row_base + 0];
    ret.m [1] = m [row_base + 1];
    ret.m [2] = m [row_base + 2];
    ret.m [3] = m [row_base + 3];

    return ret;
  }

  /// Returns a 4-element floating-point array containing the
  ///   requested column (valid col idx. = 0-3)
  inline const Real4 Column (int col_num) const {
    /* Assert (col_num >= 0 && col_num < 4); */
    Real4 ret;

    ret.m [0] = m [col_num +  0];
    ret.m [1] = m [col_num +  4];
    ret.m [2] = m [col_num +  8];
    ret.m [3] = m [col_num + 12];

    return ret;
  }


  inline void Identity (void)
  {
          m12 = m13 = m14 =
    m21 =       m23 = m24 =
    m31 = m32 =       m34 =
    m41 = m42 = m43       = 0.0f;
    m11 = m22 = m33 = m44 = 1.0f;
  }

  inline void Ortho ( float left,
                      float right,
                      float bottom,
                      float top,
                      float zNear,
                      float zFar )
  { 
    float r_l = right - left;
    float t_b = top   - bottom;
    float f_n = zFar  - zNear;
    float tx  = - (right + left)   / (right - left);
    float ty  = - (top   + bottom) / (top   - bottom);
    float tz  = - (zFar  + zNear)  / (zFar  - zNear);
 
    m [ 0] = 2.0f / r_l;
    m [ 4] = 0.0f;
    m [ 8] = 0.0f;
    m [12] = tx;
 
    m [ 1] = 0.0f;
    m [ 5] = 2.0f / t_b;
    m [ 9] = 0.0f;
    m [13] = ty;
 
    m [ 2] = 0.0f;
    m [ 6] = 0.0f;
    m [10] = 2.0f / f_n;
    m [14] = tz;
 
    m [ 3] = 0.0f;
    m [ 7] = 0.0f;
    m [11] = 0.0f;
    m [15] = 1.0f;
  }

  inline void Frustum ( float left,
                        float right,
                        float bottom,
                        float top,
                        float zNear,
                        float zFar )
  {
    float temp, temp2, temp3, temp4;

    temp  = 2.0f  * zNear;
    temp2 = right - left;
    temp3 = top   - bottom;
    temp4 = zFar  - zNear;

    m [ 0] = temp / temp2;
    m [ 4] = 0.0f;
    m [ 8] = (right + left) / temp2;
    m [12] = 0.0f;

    m [ 1] =  0.0f;
    m [ 5] = temp / temp3;
    m [ 9] = (top + bottom) / temp3;
    m [13] =  0.0f;

    m [ 2] =  0.0f;
    m [ 6] =  0.0f;
    m [10] = (-zFar - zNear) / temp4;
    m [14] = (-temp * zFar)  / temp4;

    m [ 3] =  0.0f;
    m [ 7] =  0.0f;
    m [11] = -1.0f;
    m [15] =  0.0f;
  }

  inline void Perspective (float fovy, float aspect, float zNear, float zFar)
  { 
    float ymax, xmax;
    ymax   = zNear * tanf (fovy * 3.14159265358979323846f / 360.0f);
    //ymin = -ymax;
    //xmin = -ymax * aspect;
    xmax   = ymax * aspect;
    Frustum (-xmax, xmax, -ymax, ymax, zNear, zFar);
  }


  union {
    float m [16];
    struct {
      float xx, xy, xz, xw;
      float yx, yy, yz, yw;
      float zx, zy, zz, zw;
      float wx, wy, wz, ww;
    };
    struct {
      float m11, m12, m13, m14;
      float m21, m22, m23, m24;
      float m31, m32, m33, m34;
      float m41, m42, m43, m44;
    };
  };
};

inline
e3dMatrix4x4
e3dMatrix4x4::operator* (const e3dMatrix4x4& s) const
{
  e3dMatrix4x4 r;

  r.xx = (xx * s.xx) + (yx * s.xy) + (zx * s.xz) + (wx * s.xw);
  r.xy = (xy * s.xx) + (yy * s.xy) + (zy * s.xz) + (wy * s.xw);
  r.xz = (xz * s.xx) + (yz * s.xy) + (zz * s.xz) + (wz * s.xw);
  r.xw = (xw * s.xx) + (yw * s.xy) + (zw * s.xz) + (ww * s.xw);
  r.yx = (xx * s.yx) + (yx * s.yy) + (zx * s.yz) + (wx * s.yw);
  r.yy = (xy * s.yx) + (yy * s.yy) + (zy * s.yz) + (wy * s.yw);
  r.yz = (xz * s.yx) + (yz * s.yy) + (zz * s.yz) + (wz * s.yw);
  r.yw = (xw * s.yx) + (yw * s.yy) + (zw * s.yz) + (ww * s.yw);
  r.zx = (xx * s.zx) + (yx * s.zy) + (zx * s.zz) + (wx * s.zw);
  r.zy = (xy * s.zx) + (yy * s.zy) + (zy * s.zz) + (wy * s.zw);
  r.zz = (xz * s.zx) + (yz * s.zy) + (zz * s.zz) + (wz * s.zw);
  r.zw = (xw * s.zx) + (yw * s.zy) + (zw * s.zz) + (ww * s.zw);
  r.wx = (xx * s.wx) + (yx * s.wy) + (zx * s.wz) + (wx * s.ww);
  r.wy = (xy * s.wx) + (yy * s.wy) + (zy * s.wz) + (wy * s.ww);
  r.wz = (xz * s.wx) + (yz * s.wy) + (zz * s.wz) + (wz * s.ww);
  r.ww = (xw * s.wx) + (yw * s.wy) + (zw * s.wz) + (ww * s.ww);

  return r;
}

inline
e3dVector3
e3dMatrix4x4::transform (const e3dVector3& v) const
{
  float w = v.x * wx +
            v.y * wy +
            v.z * wz +
            ww;

  return e3dVector3 (
    ((v.x * xx) + (v.y * xy) + (v.z * xz) + xw) / w,
    ((v.x * yx) + (v.y * yy) + (v.z * yz) + yw) / w,
    ((v.x * zx) + (v.y * zy) + (v.z * zz) + zw) / w
  );
}

#endif /* __EPSILON_3D__MATRIX_H__ */
