#ifndef RSTL_UTILITY_H
#define RSTL_UTILITY_H

#include "config.h"
#include "type_traits.h"
#include "move.h"
#include "iterator.h"

#pragma once

RSTL_NAMESPACE_BEGIN

template <typename T>
inline void swap(T &a, T&b) noexcept(is_nothrow_move_assignable_v<T> && is_nothrow_move_constructible_v<T>)
{
	T temp(move(a));
	a = move(b);
	b = move(temp);
}

/*
 * is_swappable
 * */

template <typename>
struct is_swappable : false_type
{

};

template <typename T>
constexpr bool is_swappable_v = is_swappable<T>::value;

/*
 * is_nothrow_swappable
 * */

template <typename T>
struct is_nothrow_swappable_helper_noexcept_wrapper
{
	const static bool value = noexcept(swap(declval<T&>(), declval<T&>()));
};

template <typename T, bool>
struct is_nothrow_swappable_helper : integral_constant<bool, is_nothrow_swappable_helper_noexcept_wrapper<T>::value>
{

};

template <typename T>
struct is_nothrow_swappable_helper<T, false> : false_type
{

};

template <typename T>
struct is_nothrwo_swappable : public is_nothrow_swappable_helper<T, is_swappable<T>::value>
{

};

template <typename T>
constexpr bool is_nothrow_swappable_v = is_nothrwo_swappable<T>::value;

/*
 * iter_swap
 * */

template <bool bTypeAreEqual>
struct iter_swap_impl
{
	template <typename ForwardIterator1, typename ForwardIterator2>
	static void iter_swap(ForwardIterator1 a, ForwardIterator2 b)
	{
		using value_type_a = typename iterator_traits<ForwardIterator1>::value_type;
		value_type_a temp(move(*a));
		*a = move(*b);
		*b = move(temp);
	}
};

template <>
struct iter_swap_impl<true>
{
	template <typename ForwardIterator1, typename ForwardIterator2>
	static void iter_swap(ForwardIterator1 a, ForwardIterator2 b)
	{
		swap(*a, *b);
	}
};

template <typename ForwardIterator1, typename ForwardIterator2>
inline void iter_swap(ForwardIterator1 a, ForwardIterator2 b)
{
	using value_type_a = typename iterator_traits<ForwardIterator1>::value_type;
	using value_type_b = typename iterator_traits<ForwardIterator2>::value_type;
	using reference_a = typename iterator_traits<ForwardIterator1>::reference;
	using reference_b = typename iterator_traits<ForwardIterator2>::reference;
    iter_swap_impl<type_and<is_same_v<value_type_a, value_type_b>, is_same_v<value_type_a&, reference_a>, is_same_v<value_type_b&, reference_b>>::value>::iter_swap(a, b);
}


/*
 * pair
 * */

template <typename T1, typename T2>
struct pair
{
	using first_type = T1;
	using second_type = T2;

	T1 first;
	T2 second;

	template <typename U1 = T1, typename U2 = T2, typename = enable_if_t<is_default_constructible_v<U1> && is_default_constructible_v<U2>>>
    constexpr pair() : first(), second() {}

	template <typename U1 = T1, typename U2 = T2, typename = enable_if_t<is_copy_constructible_v<U1> && is_copy_constructible_v<U2>>>
	explicit(!is_convertible_v<const U1&, T1> || !is_convertible_v<const U2&, T2>) constexpr pair(const T1 &x, const T2 &y) : first(x), second(y) {}

	template <typename U1, typename U2, typename = enable_if_t<is_constructible_v<T1, U1> && is_convertible_v<T2, U2>>>
	explicit(!is_convertible_v<U1, T1> || !is_convertible_v<U2, T2>) constexpr pair(U1 &&x, U2 &&y) : first(forward<U1>(x)), second(forward<U2>(y)) {}

	template <typename U1, typename U2, typename = enable_if_t<is_constructible_v<T1, U1&> && is_constructible_v<T2, U2&>>>
	explicit(!is_convertible_v<U1&, T1> || !is_convertible_v<U2&, T2>) constexpr pair(pair<U1, U2> &p) : first(p.first), second(p.second) {}

	template <typename U1, typename U2, typename = enable_if_t<is_constructible_v<T1, const U1&> && is_constructible_v<T2, const U2&>>>
	explicit(!is_convertible_v<const U1&, T1> || !is_convertible_v<const U2&, T2>) constexpr pair(const pair<U1, U2> &p) : first(p.first), second(p.second) {}

	template<typename U1, typename U2, typename = enable_if_t<is_constructible_v<T1, U1> && is_constructible_v<T2, U2>>>
	explicit(!is_convertible_v<U1, T1> || !is_convertible_v<U2, T2>) constexpr pair(pair<U1, U2> &&p) : first(forward<U1>(p.first)), second(forward<U2>(p.second)) {}


	//template <typename... Args1, typename... Args2, typename = enable_if_t<is_constructible_v<first_type, Args1&&...> && is_constructible_v<second_type, Args2&&...>>>
	//pair(piecewise_construct_t pwc, )

	constexpr pair(const pair&) = default;

	constexpr pair(pair&&) = default;

	pair& operator=(const pair &p) noexcept(is_nothrow_copy_assignable_v<T1> && is_nothrow_copy_assignable_v<T2>)
	{
		first = p.first;
		second = p.second;
		return *this;
	}

	template <typename U1, typename U2, typename = enable_if_t<is_convertible_v<U1, T1> && is_convertible_v<U2, T2>>>
	pair& operator=(const pair<U1, U2> &p)
	{
		first = p.first;
		second = p.second;
		return *this;
	}

	pair& operator=(pair&& p) noexcept(is_nothrow_move_assignable_v<T1> && is_nothrow_move_assignable_v<T2>)
	{
		first = forward<T1>(p.first);
		second = forward<T2>(p.second);
		return *this;
	}

	template <typename U1, typename U2, typename = enable_if_t<is_convertible_v<U1, T1> && is_convertible_v<U2, T2>>>
	pair& operator=(pair<U1, U2> &&p)
	{
		first = forward<U1>(p.first);
		second = forward<U2>(p.second);
		return *this;
	}

	void swap(pair &p) noexcept(is_nothrow_swappable_v<T1> && is_nothrow_swappable_v<T2>)
	{
		iter_swap(&first, &p.first);
		iter_swap(&second, &p.second);
	}

};



template <typename T1, typename T2>
std::ostream& operator<<(std::ostream &stream, const pair<T1, T2> &p)
{
	return stream << "[" << p.first << "," << p.second << "]";
}

RSTL_NAMESPACE_END

#endif // RSTL_UTILITY_H
