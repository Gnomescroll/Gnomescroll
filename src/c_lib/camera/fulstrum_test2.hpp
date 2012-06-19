#pragma once

void setup_fulstrum2(float fovy, float aspect, float zfar, Vec3 camera, Vec3 forward, Vec3 right, Vec3 up);

bool point_fulstrum_test_2(float x, float y, float z);
bool point_fulstrum_test_2(struct Vec3 p);