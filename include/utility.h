/* ----------------------------------------------------------------------------
 * 在阅读C++标准库(第二版)的同时进行编码.
 * Support:
 *      pragma once
 *      noexcept
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 * ----------------------------------------------------------------------------*/


#ifndef RSTL_UTILITY_H
#define RSTL_UTILITY_H

#include "config.h"
#include "type_traits.h"

#pragma once

RSTL_NAMESPACE_BEGIN

template <typename T>
inline void swap(T &x, T &y) noexcept {
	T temp = std::move(x);
	x = std::move(y);
	y = std::move(temp);
}

template <typename T1, typename T2>
struct pair
{
	using first_type = T1;
	using second_type = T2;

	T1 first;
	T2 second;

	template <typename U1 = T1, typename U2 = T2, typename = enable_if_t<is_default_constructible_v<U1> && is_default_constructible_v<U2>>>
    explicit(!is_convertible_v<U1, T1> || !is_convertible_v<U2, T2>) constexpr pair() : first(), second() {}

	template <typename U1 = T1, typename U2 = T2, typename = enable_if_t<is_copy_constructible_v<U1> && is_copy_constructible_v<U2>>>
	explicit(!is_convertible_v<U1, T1> || !is_convertible_v<U2, T2>) constexpr pair(const T1 &x, const T2 &y) : first(x), second(y) {}


};

template <typename T1, typename T2>
std::ostream& operator<<(std::ostream &stream, const pair<T1, T2> &p)
{
	return stream << "[" << p.first << "," << p.second << "]";
}

RSTL_NAMESPACE_END

#endif //RSTL_UTILITY_H
