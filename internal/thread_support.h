#ifndef RSTL_THREAD_SUPPORT_H
#define RSTL_THREAD_SUPPORT_H

#pragma once

#include <cstdint>

#include "config.h"

RSTL_NAMESPACE_BEGIN

namespace Thread_Support_Internal {

	inline int32_t atomic_increment(int32_t* p32) noexcept
	{
		return __sync_add_and_fetch(p32, 1);
	}

	inline int32_t atomic_decrement(int32_t* p32) noexcept
	{
		return __sync_add_and_fetch(p32, -1);
	}

	inline bool atomic_compare_and_swap(int32_t* p32, int32_t newValue, int32_t condition)
	{
		return __sync_bool_compare_and_swap(p32, condition, newValue);
	}

}

RSTL_NAMESPACE_END

#endif //RSTL_THREAD_SUPPORT_H
