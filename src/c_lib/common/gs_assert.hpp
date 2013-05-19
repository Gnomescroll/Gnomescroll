#pragma once

#include <common/macros.hpp>

void _GS_ASSERT_TEARDOWN();
int _GS_ASSERT_INTERNAL(const char* FILE, const char* FUNC, size_t LINE, size_t LIMIT);
ALWAYS_INLINE int _GS_ASSERT_INTERNAL(const char* FILE, const char* FUNC, int LINE);

#define GS_ASSERT(CONDITION) if (unlikely(!(CONDITION))) { _GS_ASSERT_INTERNAL(__FILE__, __FUNCTION__, __LINE__); }
#define GS_ASSERT_LIMIT(CONDITION, LIMIT) if (unlikely(!(CONDITION))) { _GS_ASSERT_INTERNAL(__FILE__, __FUNCTION__, __LINE__, (LIMIT)); }
#define GS_ASSERT_ABORT(CONDITION) if (unlikely(!(CONDITION))) { _GS_ASSERT_INTERNAL(__FILE__, __FUNCTION__, __LINE__); GS_ABORT(); }
#define IF_ASSERT(CONDITION) if (unlikely((CONDITION)) && _GS_ASSERT_INTERNAL(__FILE__, __FUNCTION__, __LINE__))

#if defined (__MSVC__)
# define __func__ __FUNCTION__
#endif
