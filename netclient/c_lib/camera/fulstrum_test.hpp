#pragma once

void setup_test_fulstrum(int camera);
void setup_fulstrum();

bool sphere_fulstrum_test(float x, float y, float  z, float r) __attribute((always_inline));

void draw_fulstrum();
