#ifndef RSTL_UNIQUE_PTR_H
#define RSTL_UNIQUE_PTR_H

#include "internal/config.h"
#include "internal/smart_ptr.h"
#include "internal/compressed_pair.h"

#pragma once

RSTL_NAMESPACE_BEGIN

template <typename T, typename Deleter = rstl::default_delete<T>>
class unique_ptr {
	static_assert(!std::is_rvalue_reference_v<Deleter>, "The supplied Deleter cannot be a r-value reference.");

public:
	using deleter_type = Deleter;
	using element_type = T;
	using this_type = unique_ptr<element_type, deleter_type>;
	using pointer = typename SmartPTR_Internal::unique_pointer_type<element_type, deleter_type>::type;

public:
	constexpr unique_ptr() noexcept: mPair(pointer()) {
		static_assert(!std::is_pointer_v<deleter_type>,
		              "unique_ptr deleter default-constructed with null pointer. Use a different constructor or change your deleter to a class.");
	}

	constexpr unique_ptr(pointer pValue) noexcept: mPair(pValue) {
		static_assert(!std::is_pointer_v<deleter_type>,
		              "unique_ptr deleter default-constructed with null pointer. Use a different constructor or change your deleter to a class.");
	}

	unique_ptr(pointer pValue,
	           std::conditional_t<std::is_lvalue_reference_v<deleter_type>, deleter_type, std::add_lvalue_reference_t<const deleter_type>> deleter) noexcept
			: mPair(pValue, deleter) {}

	unique_ptr(pointer pValue, std::remove_reference_t<deleter_type> &&deleter) noexcept: mPair(pValue,
	                                                                                            std::move(deleter)) {
		static_assert(!std::is_lvalue_reference_v<deleter_type>,
		              "deleter_type reference refers to an rvalue deleter. The reference will probably become invalid before used. Change the deleter_type to not be a reference or construct with permanent deleter.");
	}

	unique_ptr(this_type &&x) noexcept: mPair(x.release(), std::forward<deleter_type>(x.get_deleter())) {}

	template<typename U, typename E>
	unique_ptr(unique_ptr<U, E> &&u, typename std::enable_if_t<
			!std::is_array_v<U> && std::is_convertible_v<typename unique_ptr<U, E>::pointer, pointer> &&
			std::is_convertible_v<E, deleter_type> &&
			(std::is_same_v<deleter_type, E> || !std::is_lvalue_reference_v<deleter_type>)>::type * = 0) noexcept
			: mPair(u.release(), std::forward<E>(u.get_deleter())) {}

	this_type &operator=(this_type &&x) noexcept {
		reset(x.release());
		mPair.second() = std::move(std::forward<deleter_type>(x.get_deleter()));
		return *this;
	}

	template<typename U, typename E>
	typename std::enable_if_t<
			!std::is_array_v<U> && std::is_convertible_v<typename unique_ptr<U, E>::pointer, pointer> &&
			std::is_assignable_v<deleter_type &, E &&>, this_type &> operator=(unique_ptr<U, E> &&u) noexcept {
		reset(u.release());
		mPair.second() = std::move(std::forward<E>(u.get_deleter()));
		return *this;
	}

	this_type &operator=(std::nullptr_t) noexcept {
		reset();
		return *this;
	}

	~unique_ptr() noexcept {
		reset();
	}

	void reset(pointer pValue = pointer()) noexcept {
		if (pValue != mPair.first()) {
			if (auto first = exchange(mPair.first(), pValue)) {
				get_deleter()(first);
			}
		}
	}

	pointer release() noexcept {
		pointer const pTemp = mPair.first();
		mPair.first() = pointer();
		return pTemp;
	}

	pointer detach() noexcept {
		return release();
	}

	void swap(this_type &x) noexcept {
		mPair.swap(x.mPair);
	}

	typename std::add_lvalue_reference_t<T> operator*() const {
		return *mPair.first();
	}

	pointer operator->() const noexcept {
		return mPair.first();
	}

	pointer get() const noexcept {
		return mPair.first();
	}

	deleter_type &get_deleter() noexcept {
		return mPair.second();
	}

	const deleter_type &get_deleter() const noexcept {
		return mPair.second();
	}

	explicit operator bool() const noexcept
	{
		return (mPair.first() != pointer());
	}

	unique_ptr(const this_type&) = delete;
	unique_ptr& operator=(const this_type&) = delete;
	unique_ptr& operator=(pointer pValue) = delete;

protected:
	rstl::compressed_pair<pointer, deleter_type> mPair;
};

template <typename T, typename Deleter>
class unique_ptr<T[], Deleter>
{
public:
	using deleter_type = Deleter;
	using element_type = T;
	using this_type = unique_ptr<element_type[], deleter_type>;
	using pointer = typename SmartPTR_Internal::unique_pointer_type<element_type, deleter_type>::type;

public:
	constexpr unique_ptr() noexcept : mPair(pointer())
	{
		static_assert(!std::is_pointer_v<deleter_type>, "unique_ptr deleter default-constructed with null pointer. Use a different constructor or change your deleter to a class.");
	}

	constexpr unique_ptr(std::nullptr_t) noexcept : mPair(pointer())
	{
		static_assert(!std::is_pointer_v<deleter_type>, "unique_ptr deleter default-constructed with null pointer. Use a different constructor or change your deleter to a class.");
	}

	template <typename P, typename = std::enable_if_t<SmartPTR_Internal::is_array_cv_convertible<P, pointer>::value>>
	explicit unique_ptr(P pArray) noexcept : mPair(pArray)
	{
		static_assert(!std::is_pointer_v<deleter_type>, "unique_ptr deleter default-constructed with null pointer. Use a different constructor or change your deleter to a class.");
	};

	template<typename P>
	unique_ptr(P pArray, typename std::conditional<std::is_lvalue_reference_v<deleter_type>, deleter_type, std::add_lvalue_reference_t<const deleter_type>>::type deleter, typename std::enable_if_t<SmartPTR_Internal::is_array_cv_convertible<P, pointer>::value>::type* = 0) noexcept : mPair(pArray, deleter) {}

	template <typename P>
	unique_ptr(P pArray, std::remove_reference_t<deleter_type> && deleter, std::enable_if_t<SmartPTR_Internal::is_array_cv_convertible<P, pointer>::value>* = 0) noexcept : mPair(pArray, std::move(deleter))
	{
		static_assert(!std::is_lvalue_reference_v<deleter_type>, "deleter_type reference refers to an rvalue deleter. The reference will probably become invalid before used. Change the deleter_type to not be a reference or construct with permanent deleter.");
	}

	unique_ptr(this_type&& x) noexcept : mPair(x.release(), std::forward<deleter_type>(x.get_deleter())) {}

	template <typename U, typename E>
	unique_ptr(unique_ptr<U, E>&& u, std::enable_if_t<SmartPTR_Internal::is_safe_array_conversion<T, pointer, U, typename unique_ptr<U, E>::pointer>::value && std::is_convertible_v<E, deleter_type> && (!std::is_lvalue_reference_v<deleter_type> || std::is_same_v<E, deleter_type>)>* = 0) noexcept : mPair(u.release(), std::forward<E>(u.get_deleter())) {}

	this_type& operator=(this_type&& x) noexcept
	{
		reset(x.release());
		mPair.second() = std::move(std::forward<deleter_type>(x.get_deleter()));
		return *this;
	}

	this_type& operator=(std::nullptr_t) noexcept
	{
		reset();
		return *this;
	}

	~unique_ptr() noexcept
	{
		reset();
	}

	void reset(pointer pArray = pointer()) noexcept
	{
		if(pArray != mPair.first())
		{
			if(auto first = std::exchange(mPair.first(), pArray))
			{
				get_deleter()(first);
			}
		}
	}

	pointer release() noexcept
	{
		pointer const pTemp = mPair.first();
		mPair.first() = pointer();
		return pTemp;
	}

	pointer detach() noexcept
	{
		return release();
	}

	void swap(this_type& x) noexcept
	{
		mPair.swap(x.mPair);
	}

	typename std::add_lvalue_reference_t<T> operator[](ptrdiff_t i) const
	{
		return mPair.first()[i];
	}

	pointer get() const noexcept
	{
		return mPair.first();
	}

	deleter_type& get_deleter() noexcept
	{
		return mPair.second();
	}

	const deleter_type& get_deleter() const noexcept
	{
		return mPair.second();
	}

	explicit operator bool() const noexcept
	{
		return (mPair.first() != pointer());
	}

	unique_ptr(const this_type&) = delete;
	unique_ptr& operator=(const this_type&) = delete;
	unique_ptr& operator=(pointer pArray) = delete;

protected:
	rstl::compressed_pair<pointer, deleter_type> mPair;
};

namespace UniquePTR_Internal {

	template <typename T>
	struct unique_type
	{
		using unique_type_single = unique_ptr<T>;
	};

	template <typename T>
	struct unique_type<T[]>
	{
		using unique_tpye_unbounded_array =  unique_ptr<T[]>;
	};

	template <typename T, size_t N>
	struct unique_type<T[N]>
	{
		using unique_type_bounded_array = void;
	};
}

template <typename T, typename... Args>
inline typename UniquePTR_Internal::unique_type<T>::unique_type_single make_unique(Args&&... args)
{
	return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
inline typename UniquePTR_Internal::unique_type<T>::unique_type_unbounded_array make_unique(size_t n)
{
	using TBase = std::remove_extent_t<T>;
	return unique<T>(new TBase[n]);
}

template <typename T, typename... Args>
typename UniquePTR_Internal::unique_type<T>::unique_type_bounded_array make_unique(Args&&...) = delete;

template <typename T, typename D>
inline void swap(unique_ptr<T, D>& a, unique_ptr<T, D>& b) noexcept
{
	a.swap(b);
}

template <typename T1, typename D1, typename T2, typename D2>
inline bool operator==(const unique_ptr<T1, D1>& a, const unique_ptr<T2, D2>& b)
{
	return (a.get() == b.get());
}

template <typename T1, typename D1, typename T2, typename D2>
inline bool operator!=(const unique_ptr<T1, D1>& a, const unique_ptr<T2, D2>& b)
{
	return !(a.get() == b.get());
}

template <typename T1, typename D1, typename T2, typename D2>
inline bool operator<(const unique_ptr<T1, D1>& a, const unique_ptr<T2, D2>& b)
{
	using P1 = typename rstl::unique_ptr<T1, D1>::pointer ;
	using P2 = typename rstl::unique_ptr<T1, D2>::pointer ;
	using PCommon = std::common_type_t<P1, P2>;
	PCommon pT1 = a.get();
	PCommon pT2 = b.get();
	return less<PCommon>()(pT1, pT2);
}

template <typename T1, typename D1, typename T2, typename D2>
inline bool operator>(const unique_ptr<T1, D1>& a, const unique_ptr<T2, D2>& b)
{
	return (b < a);
}

template <typename T1, typename D1, typename T2, typename D2>
inline bool operator<=(const unique_ptr<T1, D1>& a, const unique_ptr<T2, D2>& b)
{
	return !(b < a);
}

template <typename T1, typename D1, typename T2, typename D2>
inline bool operator>=(const unique_ptr<T1, D1>& a, const unique_ptr<T2, D2>& b)
{
	return !(a < b);
}


template <typename T, typename D>
inline bool operator==(const unique_ptr<T, D>& a, std::nullptr_t) noexcept
{
	return !a;
}

template <typename T, typename D>
inline bool operator==(std::nullptr_t, const unique_ptr<T, D>& a) noexcept
{
	return !a;
}

template <typename T, typename D>
inline bool operator!=(const unique_ptr<T, D>& a, std::nullptr_t) noexcept
{
	return static_cast<bool>(a);
}

template <typename T, typename D>
inline bool operator!=(std::nullptr_t, const unique_ptr<T, D>& a) noexcept
{
	return static_cast<bool>(a);
}

template <typename T, typename D>
inline bool operator<(const unique_ptr<T, D>& a, std::nullptr_t)
{
	typedef typename unique_ptr<T, D>::pointer pointer;
	return less<pointer>()(a.get(), nullptr);
}

template <typename T, typename D>
inline bool operator<(std::nullptr_t, const unique_ptr<T, D>& b)
{
	typedef typename unique_ptr<T, D>::pointer pointer;
	pointer pT = b.get();
	return less<pointer>()(nullptr, pT);
}

template <typename T, typename D>
inline bool operator>(const unique_ptr<T, D>& a, std::nullptr_t)
{
	return (nullptr < a);
}

template <typename T, typename D>
inline bool operator>(std::nullptr_t, const unique_ptr<T, D>& b)
{
	return (b < nullptr);
}

template <typename T, typename D>
inline bool operator<=(const unique_ptr<T, D>& a, std::nullptr_t)
{
	return !(nullptr < a);
}

template <typename T, typename D>
inline bool operator<=(std::nullptr_t, const unique_ptr<T, D>& b)
{
	return !(b < nullptr);
}

template <typename T, typename D>
inline bool operator>=(const unique_ptr<T, D>& a, std::nullptr_t)
{
	return !(a < nullptr);
}

template <typename T, typename D>
inline bool operator>=(std::nullptr_t, const unique_ptr<T, D>& b)
{
	return !(nullptr < b);
}

RSTL_NAMESPACE_END

#endif //RSTL_UNIQUE_PTR_H
