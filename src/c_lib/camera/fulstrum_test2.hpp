#pragma once

void setup_fulstrum2(float fovy, float aspect, float znear, float zfar, 
    Vec3 camera, Vec3 forward, Vec3 right, Vec3 up);

bool point_fulstrum_test_2(float x, float y, float z);
bool point_fulstrum_test_2(struct Vec3 p);

float top_z_projection(float x, float y);
float bottom_z_projection(float x, float y);

void chunk_top_z_projection(float x, float y, float* bottom, float *top);