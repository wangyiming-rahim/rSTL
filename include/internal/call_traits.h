#ifndef RSTL_CALL_TRAITS_H
#define RSTL_CALL_TRAITS_H

#include "config.h"
#include <type_traits>

#pragma once

RSTL_NAMESPACE_BEGIN

template <typename T, bool small_>
struct ct_imp2
{
	using param_type = const T&;
};

template <typename T>
struct ct_imp2<T, true>
{
	using param_type = const T;
};

template <typename T, bool isp, bool b1>
struct ct_imp
{
	using param_type = const T&;
};

template <typename T, bool isp>
struct ct_imp<T, isp, true>
{
	using param_type = typename ct_imp2<T, sizeof(T) <= sizeof(void*)>::param_type;
};

template <typename T, bool b1>
struct ct_imp<T, true, b1>
{
	using param_type = T const;
};

template <typename T>
struct call_traits
{
public:
	using value_type = T;
	using reference = T&;
	using const_reference = const T&;
	using param_type = typename ct_imp<T, std::is_pointer_v<T>, std::is_arithmetic_v<T>>::param_type;
};

template <typename T>
struct call_traits<T&>
{
	using value_type = T&;
	using reference = T&;
	using const_reference = const T&;
	using param_type = T&;
};

template <typename T, size_t N>
struct call_traits<T [N]>
{
private:
	using array_type = T[N];
//	typedef T array_type[N];
public:
	using value_type = const T*;
	using reference = array_type&;
	using const_reference = const array_type&;
	using param_type = const T* const;
};

template <typename T, size_t N>
struct call_traits<const T [N]>
{
private:
	using array_type = const T[N];
//	typedef const T array_type[N];
public:
	using value_type = const T*;
	using reference = array_type&;
	using const_reference = const array_type&;
	using param_type = const T* const;
};


RSTL_NAMESPACE_END

#endif //RSTL_CALL_TRAITS_H
