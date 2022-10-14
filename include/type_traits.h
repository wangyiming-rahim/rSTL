/* ----------------------------------------------------------------------------
 * Support:
 *
 *
 * Implementation:
 * integral_constant
 * true_type
 * false_type
 * bool_constant
 * enable_if
 * disable_if
 * add_lvalue_reference
 * is_constructible
 * is_default_constructible
 * is_convertible
 * is_const
 * is_reference
 * is_function
 * add_const
 *
 *
 * ----------------------------------------------------------------------------*/
#ifndef RSTL_TYPE_TRAITS_H
#define RSTL_TYPE_TRAITS_H

#include "config.h"

#pragma once

RSTL_NAMESPACE_BEGIN

/*
 * integral_constant
 * */

template <typename T, T v>
struct integral_constant
{
	static constexpr T value = v;
	using value_type = T;
	using type = integral_constant<T, v>;

	constexpr operator value_type() const noexcept
	{
		return value;
	}

	constexpr value_type operator()() const noexcept
	{
		return value;
	}
};

/*
 * true_type / false_type
 * */

using true_type = integral_constant<bool, true>;
using false_type = integral_constant<bool, false>;


/*
 * bool_constant
 * */

template <bool F>
using bool_constant = integral_constant<bool, F>;


/*
 * enable_if
 * disable_if
 * */

template <bool F, typename T = void>
struct enable_if
{

};

template <typename T>
struct enable_if<true, T>
{
	using type = T;
};

template <bool F, typename T = void>
using enable_if_t = typename enable_if<F, T>::type;

template <bool F, typename T = void>
struct disable_if
{

};

template <typename T>
struct disable_if<false, T>
{
	using type = T;
};

template <bool F, typename T = void>
using disable_if_t = typename disable_if<F, T>::type;

/*
 * is_constructible
 * */

template <typename T, typename... Args>
struct is_constructible : public bool_constant<__is_constructible(T, Args...)>
{

};

template <typename T, typename... Args>
constexpr bool is_constructible_v = is_constructible<T, Args...>::value;

/*
 * is_default_constructible
 * */

template <typename T>
struct is_default_constructible : public is_constructible<T>
{

};

template <typename T>
constexpr bool is_default_constructible_v = is_default_constructible<T>::value;

/*
 * is_convertible
 * */

template <typename From, typename To>
struct is_convertible : public integral_constant<bool, __is_convertible_to(From, To)>
{

};

template <typename From, typename To>
constexpr bool is_convertible_v = is_convertible<From, To>::value;

/*
 * add_lvalue_reference
 * */

template <typename T>
struct add_lvalue_reference
{
	using type = T&;
};

template <typename T>
struct add_lvalue_reference<T&>
{
	using type = T&;
};

template <>
struct add_lvalue_reference<void>
{
	using type = void;
};

template <>
struct add_lvalue_reference<const void>
{
	using type = const void;
};

template <>
struct add_lvalue_reference<volatile void>
{
	using type = volatile void;
};

template <>
struct add_lvalue_reference<const volatile void>
{
	using type = const volatile void;
};

template <typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

/*
 * is_const
 * */

template <typename T>
struct is_const_value : public false_type
{

};

template <typename T>
struct is_const_value<const T*> : public true_type
{

};

template <typename T>
struct is_const_value<const volatile T*> : public true_type
{

};

template <typename T>
struct is_const : public is_const_value<T*>
{

};

template <typename T>
struct is_const<T&> : public false_type
{

};

template <typename T>
constexpr bool is_const_v = is_const<T>::value;

/*
 * is_reference
 * */

template <typename T>
struct is_reference : public false_type
{

};

template <typename T>
struct is_reference<T&> : public true_type
{

};

template <typename T>
struct is_reference<T&&> : public true_type
{

};

template <typename T>
constexpr bool is_reference_v = is_reference<T>::value;

/*
 * is_function
 * */

template <typename>
struct is_function : public false_type
{

};

template <typename ReturnValue, typename... ArgPack>
struct is_function<ReturnValue (ArgPack...)> : public true_type
{

};
template <typename ReturnValue, typename... Argpack>
struct is_function<ReturnValue (Argpack..., ...)> : public true_type
{

};

template <typename T>
constexpr bool is_function_v = is_function<T>::value;

/*
 * add_const
 * */

template <typename T, bool = is_const_v<T> || is_reference_v<T> || is_function_v<T>>
struct add_const_helper
{
	using type = T;
};

template <typename T>
struct add_const_helper<T, false>
{
	using type = const T;
};

template <typename T>
struct add_const
{
	using type = typename add_const_helper<T>::type;
};

template <typename T>
using add_const_t = typename add_const<T>::type;

/*
 * is_copy_constructible
 * */

template <typename T>
struct is_copy_constructible : public is_constructible<T, add_lvalue_reference_t<add_const_t<T>>>
{

};

template <typename T>
constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;



RSTL_NAMESPACE_END

#endif //RSTL_TYPE_TRAITS_H
