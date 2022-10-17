#ifndef RSTL_TYPE_TRAITS_H
#define RSTL_TYPE_TRAITS_H

#include "config.h"

#include <limits.h>

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

/*
 * remove_reference
 * */

template <typename T>
struct remove_reference
{
	using type = T;
};

template <typename T>
struct remove_reference<T&>
{
	using type = T;
};

template <typename T>
struct remove_reference<T&&>
{
	using type = T;
};

template <typename T>
using remove_reference_t = typename remove_reference<T>::type;

/*
 * pieceWise_construct_t
 * */

struct piecewise_construct_t
{
	explicit piecewise_construct_t() = default;
};

/*
 * is_nothrow_assignable
 * */

template <typename T, typename U>
struct is_nothrow_assignable : integral_constant<bool, __is_nothrow_assignable(T, U)>
{

};

template <typename T, typename U>
constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<T, U>::value;

/*
 * is_nothrow_copy_assignable
 * */

template <typename T>
struct is_nothrow_copy_assignable : is_nothrow_assignable<add_lvalue_reference_t<T>, add_lvalue_reference_t<add_const_t<T>>>
{

};

template <typename T>
constexpr bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<T>::value;

/*
 * yes_type / no_type
 * */

using yes_type = char;

struct no_type
{
	char padding[8];
};

/*
 * add_rvalue_reference
 * */

template <typename T>
struct add_rvalue_reference
{
	using type = T&&;
};

template <typename T>
struct add_rvalue_reference<T&>
{
	using type = T&;
};

template <>
struct add_rvalue_reference<void>
{
	using type = void;
};

template <>
struct add_rvalue_reference<const void>
{
	using type = const void;
};

template <>
struct add_rvalue_reference<volatile void>
{
	using type = volatile void;
};

template <>
struct add_rvalue_reference<const volatile void>
{
	using type = const volatile void;
};

template <typename T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

/*
 * declval
 * */

template <typename T>
add_rvalue_reference_t<T> declval() noexcept;


/*
 * is_assignable
 * */

template <typename T, typename U>
struct is_assignable_helper
{

	template<typename T1, typename U1>
	static decltype(declval<T1>() = declval<U1>(), yes_type()) is(int);

	template<typename, typename>
	static no_type is(...);

	static const bool value = (sizeof(is<T, U>(0)) == sizeof(yes_type));
};

template<typename T, typename U>
struct is_assignable : integral_constant<bool, is_assignable_helper<T, U>::value>
{

};

template<typename T, typename U>
constexpr bool is_assignable_v = is_assignable<T, U>::value;

/*
 * is_move_assignable
 * */

template <typename T>
struct is_move_assignable : is_assignable<add_lvalue_reference_t<T>, add_rvalue_reference_t<T>>
{

};

template <typename T>
constexpr bool is_move_assignable_v = is_move_assignable<T>::value;

/*
 * is_nothrow_move_assignable
 * */

template <typename T>
struct is_nothrow_move_assignable : is_nothrow_assignable<add_lvalue_reference_t<T>, add_rvalue_reference_t<T>>
{

};

template <typename T>
constexpr bool is_nothrow_move_assignable_v = is_nothrow_move_assignable<T>::value;

/*
 * has_nothrow_constructor
 * */

template <typename T>
struct has_nothrow_constructor : public integral_constant<bool, __has_nothrow_constructor(T)>
{

};

template <typename T>
constexpr bool has_nothrow_constructor_v = has_nothrow_constructor<T>::value;

/*
 * has_nothrow_copy
 * */

template <typename T>
struct has_nothrow_copy : integral_constant<bool, __has_nothrow_copy(T)>
{

};

template <typename T>
constexpr bool has_nothrow_copy_v = has_nothrow_copy<T>::value;

/*
 * is_nothrow_constructible
 * */

template <typename T, typename... Args>
struct is_nothrow_constructible : false_type
{

};

template <typename T>
struct is_nothrow_constructible<T> : integral_constant<bool, has_nothrow_constructor_v<T>>
{

};

template <typename T>
struct is_nothrow_constructible<T, T> : integral_constant<bool, has_nothrow_copy_v<T>>
{

};

template <typename T>
struct is_nothrow_constructible<T, const T&> : public integral_constant<bool, has_nothrow_copy_v<T>>
{

};

template <typename T>
struct is_nothrow_constructible<T, T&> : public integral_constant<bool, has_nothrow_copy_v<T>>
{

};

template <typename T>
struct is_nothrow_constructible<T, T&&> : public integral_constant<bool, has_nothrow_copy_v<T>>
{

};

template <typename T, typename... Args>
constexpr bool is_nothrow_constructible_v = is_nothrow_constructible_v<T, Args...>;


/*
 * is_nothrow_move_constructible
 * */

template <typename T>
struct is_nothrow_move_constructible : is_nothrow_constructible<T, add_rvalue_reference_t<T>>
{

};

template <typename T>
constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;

/*
 * type_and
 * */

template <bool b1, bool b2, bool b3 = true, bool b4 = true, bool b5 = true>
struct type_and;

template <bool b1, bool b2, bool b3, bool b4, bool b5>
struct type_and
{
	static const bool value = false;
};

template <>
struct type_and<true, true, true, true, true>
{
	static const bool value = true;
};

/*
 * is_same
 * */

template <typename T, typename U>
struct is_same : false_type
{

};

template <typename T>
struct is_same<T, T> : true_type
{

};

template <typename T, typename U>
constexpr bool is_same_v = is_same<T, U>::value;

RSTL_NAMESPACE_END

#endif //RSTL_TYPE_TRAITS_H
