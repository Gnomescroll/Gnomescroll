/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

void setup_test_fulstrum(int camera);

void setup_fulstrum(float fovy, float aspect, float zfar, Vec3 camera, Vec3 forward, Vec3 right, Vec3 up);

// does the camera translate for the caller
ALWAYS_INLINE bool sphere_fulstrum_test_translate(float x, float y, float z, float r);
ALWAYS_INLINE bool point_fulstrum_test_translate(float x, float y, float z);

// use for coordinates that are already translated into camera space
ALWAYS_INLINE bool sphere_fulstrum_test(float x, float y, float z, float r);
ALWAYS_INLINE bool sphere_fulstrum_test(const Vec3& p, float r)
{
    return sphere_fulstrum_test(p.x, p.y, p.z, r);
}

ALWAYS_INLINE bool point_fulstrum_test(float x, float y, float z);
ALWAYS_INLINE bool point_fulstrum_test(const Vec3& p)
{
    return point_fulstrum_test(p.x, p.y, p.z);
}

//distance check with camera
ALWAYS_INLINE bool point_fulstrum_test_no_view_distance(float x, float y, float z);

inline bool point_fulstrum_test_debug(float x, float y, float z);

ALWAYS_INLINE bool point_fulstrum_test_map(float x, float y, float z);

ALWAYS_INLINE bool xy_circle_fulstrum_test(float x, float y, float r);
ALWAYS_INLINE bool xy_point_fulstrum_test(float x, float y);

void draw_fulstrum();
