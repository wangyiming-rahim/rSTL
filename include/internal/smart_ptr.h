#ifndef RSTL_SMART_PTR_H
#define RSTL_SMART_PTR_H

#include "config.h"

#include <type_traits>

#pragma once

RSTL_NAMESPACE_BEGIN

namespace SmartPTR_Internal{

	template <typename T, typename Deleter>
	class unique_pointer_type
	{
		template <typename U>
		static typename U::pointer test(typename U::pointer);

		template <typename U>
		static T* test(...);

	public:
		using type = decltype(test<std::remove_reference_t<Deleter>>(0));
	};

	template <typename P1, typename P2, bool = std::is_same_v<std::remove_cv_t<typename std::pointer_traits<P1>::element_type>, std::remove_cv_t<typename std::pointer_traits<P2>::element_type>>>
	struct is_array_cv_convertible_impl : public std::is_convertible<P1, P2>
	{

	};

	template <typename P1, typename P2>
	struct is_array_cv_convertible_impl<P1, P2, false> : public std::false_type
	{

	};

	template <typename P1, typename P2, bool = std::is_scalar_v<P1> && !std::is_pointer_v<P1>>
	struct is_array_cv_convertible : public is_array_cv_convertible_impl<P1, P2>
	{

	};

	template <typename P1, typename P2>
	struct is_array_cv_convertible<P1, P2, true> : public std::false_type
	{

	};

	template <typename Base, typename Derived>
	struct is_derived : public std::integral_constant<bool, std::is_base_of_v<Base, Derived> && !std::is_same_v<std::remove_cv_t<Base>, std::remove_cv_t<Derived>>>
	{

	};

	template <typename T, typename T_pointer, typename U, typename U_pointer>
	struct is_safe_array_conversion : public std::integral_constant<bool, std::is_convertible_v<U_pointer, T_pointer> && std::is_array_v<U> && (!std::is_pointer_v<U_pointer> || !std::is_pointer_v<T_pointer> || !is_derived<T, std::remove_extent_t<U>>::value)>
	{

	};

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
