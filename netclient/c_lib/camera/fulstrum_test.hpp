#pragma once

void setup_test_fulstrum(int camera);

//void setup_fulstrum(float fovy, float aspect, float zfar);
struct Vector; //fordward declaration
void setup_fulstrum(float fovy, float aspect, float zfar, Vector camera, Vector* forward, Vector *right, Vector* up);

static inline bool sphere_fulstrum_test(float x, float y, float  z, float r) __attribute((always_inline));

void draw_fulstrum();
