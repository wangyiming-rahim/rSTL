#ifndef RSTL_UTILITY_H
#define RSTL_UTILITY_H

#include "internal/config.h"
#include "internal/piecewise_construct_t.h"
#include "internal/tuple_fwd_decls.h"
#include "internal/integer_sequence.h"

#pragma once

RSTL_NAMESPACE_BEGIN

template <typename T>
inline void swap(T &a, T&b) noexcept(std::is_nothrow_move_assignable_v<T> && std::is_nothrow_move_constructible_v<T>)
{
	T temp(std::move(a));
	a = std::move(b);
	b = std::move(temp);
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

	template <typename U1 = T1, typename U2 = T2, typename = std::enable_if_t<std::is_default_constructible_v<U1> && std::is_default_constructible_v<U2>>>
    constexpr pair() : first(), second() {}

	template <typename U1 = T1, typename U2 = T2, typename = std::enable_if_t<std::is_copy_constructible_v<U1> && std::is_copy_constructible_v<U2>>>
	explicit(!std::is_convertible_v<const U1&, T1> || !std::is_convertible_v<const U2&, T2>) constexpr pair(const T1 &x, const T2 &y) : first(x), second(y) {}

	template <typename U1, typename U2, typename = std::enable_if_t<std::is_constructible_v<T1, U1> && std::is_convertible_v<T2, U2>>>
	explicit(!std::is_convertible_v<U1, T1> || !std::is_convertible_v<U2, T2>) constexpr pair(U1 &&x, U2 &&y) : first(std::forward<U1>(x)), second(std::forward<U2>(y)) {}

	template <typename U1, typename U2, typename = std::enable_if_t<std::is_constructible_v<T1, U1&> && std::is_constructible_v<T2, U2&>>>
	explicit(!std::is_convertible_v<U1&, T1> || !std::is_convertible_v<U2&, T2>) constexpr pair(pair<U1, U2> &p) : first(p.first), second(p.second) {}

	template <typename U1, typename U2, typename = std::enable_if_t<std::is_constructible_v<T1, const U1&> && std::is_constructible_v<T2, const U2&>>>
	explicit(!std::is_convertible_v<const U1&, T1> || !std::is_convertible_v<const U2&, T2>) constexpr pair(const pair<U1, U2> &p) : first(p.first), second(p.second) {}

	template<typename U1, typename U2, typename = std::enable_if_t<std::is_constructible_v<T1, U1> && std::is_constructible_v<T2, U2>>>
	explicit(!std::is_convertible_v<U1, T1> || !std::is_convertible_v<U2, T2>) constexpr pair(pair<U1, U2> &&p) : first(std::forward<U1>(p.first)), second(std::forward<U2>(p.second)) {}

	template <typename... Args1, typename... Args2, typename = std::enable_if_t<std::is_constructible_v<first_type, Args1...> && std::is_constructible_v<second_type, Args2...>>>
	pair(rstl::piecewise_construct_t pwc, tuple<Args1...> first_args, tuple<Args2...> second_args) : pair(pwc, std::move(first_args), std::move(second_args), make_index_sequence<sizeof...(Args1)>(), make_index_sequence<sizeof...(Args2)>()) {}

	constexpr pair(const pair&) = default;

	constexpr pair(pair&&) = default;

	pair& operator=(const pair &p) noexcept(std::is_nothrow_copy_assignable_v<T1> && std::is_nothrow_copy_assignable_v<T2>)
	{
		first = p.first;
		second = p.second;
		return *this;
	}

	template <typename U1, typename U2, typename = std::enable_if_t<std::is_convertible_v<U1, T1> && std::is_convertible_v<U2, T2>>>
	pair& operator=(const pair<U1, U2> &p)
	{
		first = p.first;
		second = p.second;
		return *this;
	}

	pair& operator=(pair&& p) noexcept(std::is_nothrow_move_assignable_v<T1> && std::is_nothrow_move_assignable_v<T2>)
	{
		first = std::forward<T1>(p.first);
		second = std::forward<T2>(p.second);
		return *this;
	}

	template <typename U1, typename U2, typename = std::enable_if_t<std::is_convertible_v<U1, T1> && std::is_convertible_v<U2, T2>>>
	pair& operator=(pair<U1, U2> &&p)
	{
		first = std::forward<U1>(p.first);
		second = std::forward<U2>(p.second);
		return *this;
	}

	void swap(pair &p) noexcept(std::is_nothrow_swappable_v<T1> && std::is_nothrow_swappable_v<T2>)
	{
		using std::swap;
		swap(first, p.first);
		swap(second, p.second);
	}

private:

	template <class... Args1, class... Args2, size_t... I1, size_t... I2>
	pair(piecewise_construct_t, tuple<Args1...> first_args, tuple<Args2...> second_args, index_sequence<I1...>, index_sequence<I2...>) : first(std::forward<Args1>(rstl::get<I1>(first_args))...), second(std::forward<Args2>(rstl::get<I2>(second_args))...) {}

};

template <typename T1, typename T2>
constexpr inline bool operator==(const pair<T1, T2> &a, const pair<T1, T2> &b)
{
	return ((a.first == b.first) && (a.second == b.second));
}

template <typename T1, typename T2>
constexpr inline bool operator!=(const pair<T1, T2> &a, const pair<T1, T2> &b)
{
	return !(a == b);
}

template <typename T1, typename T2>
constexpr inline bool operator<(const pair<T1, T2> &a, const pair<T1, T2> &b)
{
	return ((a.first < b.first) || (a.first == b.first && a.second < b.second));
}

template <typename T1, typename T2>
constexpr inline bool operator>(const pair<T1, T2> &a, const pair<T1, T2> &b)
{
	return b < a;
}

template <typename T1, typename T2>
constexpr inline bool operator>=(const pair<T1, T2> &a, const pair<T1, T2> &b)
{
	return !(a < b);
}

template <typename T1, typename T2>
constexpr inline bool operator<=(const pair<T1, T2> &a, const pair<T1, T2> &b)
{
	return !(b < a);
}

template <typename T1, typename T2>
constexpr inline pair<typename std::reference_wrapper<std::decay_t<T1>>::type, typename std::reference_wrapper<std::decay_t<T2>>::type> make_pair(T1 &&a, T2 &&b)
{
	using first_type = typename std::reference_wrapper<std::decay_t<T1>>::type;
	using second_type = typename std::reference_wrapper<std::decay_t<T2>>::type;
	return pair<first_type, second_type>(std::forward<T1>(a), std::forward<T2>(b));
}

template <typename T1, typename T2>
class tuple_size<pair<T1, T2>> : public std::integral_constant<size_t, 2> {};

template <typename T1, typename T2>
class tuple_size<const pair<T1, T2>> : public std::integral_constant<size_t, 2> {};

template <typename T1, typename T2>
class tuple_element<0, pair<T1, T2>>
{
public:
	typedef T1 type;
};

template <typename T1, typename T2>
class tuple_element<1, pair<T1, T2>>
{
public:
	typedef T2 type;
};

template <typename T1, typename T2>
class tuple_element<0, const pair<T1, T2>>
{
public:
	typedef const T1 type;
};

template <typename T1, typename T2>
class tuple_element<1, const pair<T1, T2>>
{
public:
	typedef const T2 type;
};

template <size_t I>
struct GetPair;

template <>
struct GetPair<0>
{
	template <typename T1, typename T2>
	static constexpr T1& getInternal(pair<T1, T2>& p)
	{
		return p.first;
	}

	template <typename T1, typename T2>
	static constexpr const T1& getInternal(const pair<T1, T2>& p)
	{
		return p.first;
	}

	template <typename T1, typename T2>
	static constexpr T1&& getInternal(pair<T1, T2>&& p)
	{
		return std::forward<T1>(p.first);
	}
};

template <>
struct GetPair<1>
{
	template <typename T1, typename T2>
	static constexpr T2& getInternal(pair<T1, T2>& p)
	{
		return p.second;
	}

	template <typename T1, typename T2>
	static constexpr const T2& getInternal(const pair<T1, T2>& p)
	{
		return p.second;
	}

	template <typename T1, typename T2>
	static constexpr T2&& getInternal(pair<T1, T2>&& p)
	{
		return std::forward<T2>(p.second);
	}
};

template <size_t I, typename T1, typename T2>
tuple_element_t<I, pair<T1, T2>>& get(pair<T1, T2>& p)
{
	return GetPair<I>::getInternal(p);
}

template <size_t I, typename T1, typename T2>
const tuple_element_t<I, pair<T1, T2>>& get(const pair<T1, T2>& p)
{
	return GetPair<I>::getInternal(p);
}

template <size_t I, typename T1, typename T2>
tuple_element_t<I, pair<T1, T2>>&& get(pair<T1, T2>&& p)
{
	return GetPair<I>::getInternal(std::move(p));
}

template <typename T1, typename T2>
std::ostream& operator<<(std::ostream &stream, const pair<T1, T2> &p)
{
	return stream << "[" << p.first << "," << p.second << "]";
}

RSTL_NAMESPACE_END

#endif // RSTL_UTILITY_H
