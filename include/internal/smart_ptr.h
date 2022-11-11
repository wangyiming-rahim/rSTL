#ifndef RSTL_SMART_PTR_H
#define RSTL_SMART_PTR_H

#include "config.h"

#include <type_traits>

#pragma once

RSTL_NAMESPACE_BEGIN

namespace SmartPTR_Internal{



}

template <typename T>
struct default_delete
{
	constexpr default_delete() noexcept = default;

	template<typename U>
	default_delete(const default_delete<U>&, std::enable_if_t<std::is_convertible_v<U*, T*>>* = 0) { }

	void operator()(T* p) const noexcept
	{
		delete p;
	}
};

template <typename T>
struct default_delete<T[]>
{
	constexpr default_delete() noexcept = default;

	template<typename U>
	default_delete(const default_delete<U[]>&, std::enable_if_t<std::is_convertible_v<U*, T*>>* = 0) { }

	void operator()(T* p) const noexcept
	{
		delete[] p;
	}
};

RSTL_NAMESPACE_END

#endif //RSTL_SMART_PTR_H
