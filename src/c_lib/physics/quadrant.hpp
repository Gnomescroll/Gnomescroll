#pragma once

struct Vec3;
struct Vec3i;

ALWAYS_INLINE bool is_boxed_point(float p);
ALWAYS_INLINE bool is_boxed_point(int p);
ALWAYS_INLINE bool is_boxed_position(const struct Vec3& p);
ALWAYS_INLINE bool is_boxed_position(const struct Vec3i& p);
inline int quadrant_translate_i(int cx, int px);
inline float quadrant_translate_f(float cx, float px);
ALWAYS_INLINE int quadrant_translate(int cx, int px);
ALWAYS_INLINE float quadrant_translate(float cx, float px);
ALWAYS_INLINE float translate_point(float pt);
ALWAYS_INLINE int translate_point(int pt);
ALWAYS_INLINE struct Vec3 translate_position(struct Vec3 pos);
inline struct Vec3 quadrant_translate_position(struct Vec3 pos1, struct Vec3 pos2);
ALWAYS_INLINE struct Vec3i translate_position(struct Vec3i pos);
inline struct Vec3i quadrant_translate_position(struct Vec3i pos1, struct Vec3i pos2);
