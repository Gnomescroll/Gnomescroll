#pragma once

void setup_test_fulstrum(int camera);

void setup_fulstrum(float fovy, float aspect, float zfar, Vec3 camera, Vec3 forward, Vec3 right, Vec3 up);

inline bool sphere_fulstrum_test(float x, float y, float  z, float r) __attribute((always_inline));
inline bool point_fulstrum_test(float x, float y, float  z) __attribute((always_inline));

//distance check with camera
inline bool point_fulstrum_test2(float x, float y, float  z) __attribute((always_inline));
inline bool point_fulstrum_test_map(float x, float y, float  z) __attribute((always_inline));

inline bool xy_circle_fulstrum_test(float x, float y, float r) __attribute((always_inline));
inline bool xy_point_fulstrum_test(float x, float y) __attribute((always_inline));

void draw_fulstrum();
