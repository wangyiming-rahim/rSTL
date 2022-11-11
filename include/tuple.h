#ifndef RSTL_TUPLE_H
#define RSTL_TUPLE_H

#pragma once

#include "internal/config.h"
#include "internal/tuple_fwd_decls.h"
#include "internal/integer_sequence.h"
#include "../include/utility.h"

#include <cstddef>
#include <type_traits>

RSTL_NAMESPACE_BEGIN

// TupleTypes helper
template <typename... Ts>
struct TupleTypes
{

};

// tuple_size helper
template <typename T>
class tuple_size
{

};

template <typename T>
class tuple_size<const T> : public tuple_size<T>
{

};

template <typename T>
class tuple_size<volatile T> : public tuple_size<T>
{

};

template <typename T>
class tuple_size<const volatile T> : public tuple_size<T>
{

};
// tuple_size helper end

template <typename... Ts>
class tuple_size<TupleTypes<Ts...>> : public std::integral_constant<size_t, sizeof...(Ts)>
{

};

template <typename... Ts>
class tuple_size<tuple<Ts...>> : public std::integral_constant<size_t, sizeof...(Ts)>
{

};

template <typename T>
constexpr size_t tuple_size_v = tuple_size<T>::value;

// declaration of TupleImpl
namespace Tuple_Internal {
	template<typename TupleIndices, typename... Ts>
	struct TupleImpl;
}

template <typename Indices, typename... Ts>
class tuple_size<Tuple_Internal::TupleImpl<Indices, Ts...>> : public std::integral_constant<size_t, sizeof...(Ts)>
{

};

template <size_t I, typename T>
class tuple_element
{

};

template <size_t I>
class tuple_element<I, TupleTypes<>>
{
public:
	static_assert(I != I, "tuple_element index out of range");
};

template <typename H, typename... Ts>
class tuple_element<0, TupleTypes<H, Ts...>>
{
public:
	using type = H;
};

template <size_t I, typename H, typename... Ts>
class tuple_element<I, TupleTypes<H, Ts...>>
{
public:
	using type = tuple_element_t<I - 1, TupleTypes<Ts...>>;
};

template <size_t I, typename... Ts>
class tuple_element<I, tuple<Ts...>>
{
public:
	using type = tuple_element_t<I, TupleTypes<Ts...>>;
};

template <size_t I, typename...Ts>
class tuple_element<I, const tuple<Ts...>>
{
public:
	using type = typename std::add_const_t<tuple_element_t<I, TupleTypes<Ts...>>>;
};

template <size_t I, typename... Ts>
class tuple_element<I, volatile tuple<Ts...>>
{
public:
	using type = typename std::add_volatile_t<tuple_element_t<I, TupleTypes<Ts...>>>;
};

template <size_t I, typename... Ts>
class tuple_element<I, const volatile tuple<Ts...>>
{
public:
	using type = typename std::add_cv_t<tuple_element_t<I, TupleTypes<Ts...>>>;
};

template <size_t I, typename Indices, typename... Ts>
class tuple_element<I, Tuple_Internal::TupleImpl<Indices, Ts...>> : public tuple_element<I, tuple<Ts...>>
{

};

template <size_t I, typename Indices, typename... Ts>
class tuple_element<I, const Tuple_Internal::TupleImpl<Indices, Ts...>> : public tuple_element<I, const tuple<Ts...>>
{

};


template <size_t I, typename Indices, typename... Ts>
class tuple_element<I, volatile Tuple_Internal::TupleImpl<Indices, Ts...>> : public tuple_element<I, volatile tuple<Ts...>>
{

};

template <size_t I, typename Indices, typename... Ts>
class tuple_element<I, const volatile Tuple_Internal::TupleImpl<Indices, Ts...>> : public tuple_element< I, const volatile tuple<Ts...>>
{

};

template <typename T, typename Tuple>
struct tuple_index
{

};

template <typename T>
struct tuple_index<T, TupleTypes<>>
{
	using DuplicateTypeCheck = void;
	tuple_index() = delete;
	static const size_t index = 0;
};

template <typename T, typename... TsRest>
struct tuple_index<T, TupleTypes<T, TsRest...>>
{
	using DuplicateTypeCheck = int;
	static_assert(std::is_void_v<typename tuple_index<T, TupleTypes<TsRest...>>::DuplicateTypeCheck>, "duplicate type T in tuple_vector::get<T>(); unique types must be provided in declaration, or only use get<size_t>()");
	static const size_t index = 0;
};

template <typename T, typename TsHead, typename... TsRest>
struct tuple_index<T, TupleTypes<TsHead, TsRest...>>
{
	using DuplicateTypeCheck =  typename tuple_index<T, TupleTypes<TsRest...>>::DuplicateTypeCheck;
	static const size_t index = tuple_index<T, TupleTypes<TsRest...>>::index + 1;
};

template <typename T, typename Indices, typename... Ts>
struct tuple_index<T, Tuple_Internal::TupleImpl<Indices, Ts...>> : public tuple_index<T, TupleTypes<Ts...>> // Find type T
{

};

// Provides a container to expand variadic packs
template <typename... Ts>
void swallow(Ts&&...)
{

}

namespace Tuple_Internal {

	// TupleLeaf
	template<size_t I, typename ValueType, bool IsEmpty = std::is_empty_v<ValueType>>
	class TupleLeaf;

	template<size_t I, typename ValueType, bool IsEmpty>
	inline void swap(TupleLeaf<I, ValueType, IsEmpty> &a, TupleLeaf<I, ValueType, IsEmpty> &b) {
		std::swap(a.getInternal(), b.getInternal());
	}

	template<size_t I, typename ValueType, bool IsEmpty>
	class TupleLeaf {
	public:
		TupleLeaf() : mValue() {}

		TupleLeaf(const TupleLeaf &) = default;

		TupleLeaf &operator=(const TupleLeaf &) = delete;

		explicit TupleLeaf(ValueType &&v) : mValue(std::forward<ValueType>(v)) {}

		template<typename T, typename = std::enable_if_t<std::is_constructible_v<ValueType, T &&>>>
		explicit TupleLeaf(T &&t) : mValue(std::forward<T>(t)) {}

		template<typename T>
		explicit TupleLeaf(const TupleLeaf<I, T> &t) : mValue(t.getInternal()) {}

		template<typename T>
		TupleLeaf &operator=(T &&t) {
			mValue = std::forward<T>(t);
			return *this;
		}

		int swap(TupleLeaf &t) {
			rstl::Tuple_Internal::swap(*this, t);
			return 0;
		}

		ValueType& getInternal()
		{
			return mValue;
		}

		const ValueType& getInternal() const
		{
			return mValue;
		}

	private:
		ValueType mValue;
	};

	// TupleLeaf: partial specialization for when we can use the Empty Base Class Optimization
	template <size_t I, typename ValueType>
	class TupleLeaf<I, ValueType, true> : private ValueType
	{
	public:
		TupleLeaf(const TupleLeaf&) = default;

		template <typename T, typename = std::enable_if_t<std::is_constructible_v<ValueType, T&&>>>
		explicit TupleLeaf(T &&t) : ValueType(std::forward<T>(t)) {}

		template <typename T>
		explicit TupleLeaf(const TupleLeaf<I, T> &t) : ValueType((t.getInternal())) {}

		template <typename T>
		TupleLeaf& operator=(T &&t)
		{
			ValueType::operator=(std::forward<T>(t));
			return *this;
		}

		int swap(TupleLeaf &t)
		{
			rstl::Tuple_Internal::swap(*this, t);
			return 0;
		}

		ValueType& getInternal()
		{
			return static_cast<ValueType&>(*this);
		}

		const ValueType& getInternal() const
		{
			return static_cast<const ValueType&>(*this);
		}

	private:
		TupleLeaf& operator=(const TupleLeaf&) = delete;
	};

	// MakeTupleTypes
	template <typename TupleTypes, typename Tuple, size_t Start, size_t End>
    struct MakeTupleTypesImpl;

	template <typename... Types, typename Tuple, size_t Start, size_t End>
	struct  MakeTupleTypesImpl<TupleTypes<Types...>, Tuple, Start, End>
	{
		using TupleType = std::remove_reference_t<Tuple>;
		using type = typename MakeTupleTypesImpl<TupleTypes<Types..., typename std::conditional<std::is_lvalue_reference_v<Tuple>, tuple_element_t<Start, TupleType> &, tuple_element_t<Start, TupleType>>::type>, Tuple, Start + 1, End>::type;
	};

	template <typename... Types, typename Tuple, size_t End>
	struct MakeTupleTypesImpl<TupleTypes<Types...>, Tuple, End, End>
	{
		using type = TupleTypes<Types...>;
	};

	template <typename Tuple>
	using MakeTupleTypes_t = typename MakeTupleTypesImpl<TupleTypes<>, Tuple, 0, tuple_size<std::remove_reference_t<Tuple>>::value>::type;

	// TupleImpl
	template <size_t I, typename Indices, typename... Ts>
	tuple_element_t<I, TupleImpl<Indices, Ts...>>& get(TupleImpl<Indices, Ts...>& t);

	template <size_t I, typename Indices, typename... Ts>
	const_tuple_element_t<I, TupleImpl<Indices, Ts...>>& get(const TupleImpl<Indices, Ts...>& t);

	template <size_t I, typename Indices, typename... Ts>
	tuple_element_t<I, TupleImpl<Indices, Ts...>>&& get(TupleImpl<Indices, Ts...>&& t);

	template <typename T, typename Indices, typename... Ts>
	T& get(TupleImpl<Indices, Ts...>& t);

	template <typename T, typename Indices, typename... Ts>
	const T& get(const TupleImpl<Indices, Ts...>& t);

	template <typename T, typename Indices, typename... Ts>
	T&& get(TupleImpl<Indices, Ts...>&& t);

	/*
	 * template<typename T0, typename T1, ..., typename Tn>
	 * class Tuple : TupleLeaf<0, T0>, TupleLeaf<1, T1>, ..., TupleLeaf<n, Tn> {
	 * ...
	 * };
	 * */

	template <size_t... Indices, typename... Ts>
	struct TupleImpl<integer_sequence<size_t, Indices...>, Ts...> : public TupleLeaf<Indices, Ts>...
	{
		constexpr TupleImpl() = default;

		template<typename... Us, typename... ValueTypes>
		explicit TupleImpl(integer_sequence<size_t, Indices...>, TupleTypes<Us...>, ValueTypes&&... values) : TupleLeaf<Indices, Ts>(std::forward<ValueTypes>(values))... {}

		template <typename OtherTuple>
		TupleImpl(OtherTuple &&t) : TupleLeaf<Indices, Ts>(std::forward<tuple_element_t<Indices, MakeTupleTypes_t<OtherTuple>>>(get<Indices>(t)))... {}

		template <typename OtherTuple>
		TupleImpl& operator=(OtherTuple &&t)
		{
			swallow(TupleLeaf<Indices, Ts>::operator=(std::forward<tuple_element_t<Indices, MakeTupleTypes_t<OtherTuple>>>(get<Indices>(t)))...);
			return *this;
		}

		TupleImpl& operator=(const TupleImpl& t)
		{
			swallow(TupleLeaf<Indices, Ts>::operator=(static_cast<const TupleLeaf<Indices, Ts>&>(t).getInternal())...);
			return *this;
		}

		void swap(TupleImpl &t)
		{
			swallow(TupleLeaf<Indices, Ts>::swap(static_cast<TupleLeaf<Indices, Ts>&>(t))...);
		}
	};

	template <size_t I, typename Indices, typename... Ts>
	inline tuple_element_t<I, TupleImpl<Indices, Ts...>>& get(TupleImpl<Indices, Ts...>& t)
	{
		using Type = tuple_element_t<I, TupleImpl<Indices, Ts...>>;
		return static_cast<TupleLeaf<I, Type>&>(t).getInternal();
	}

	template <size_t I, typename Indices, typename... Ts>
	inline const_tuple_element_t<I, TupleImpl<Indices, Ts...>>& get(const TupleImpl<Indices, Ts...>& t)
	{
		using Type = tuple_element_t<I, TupleImpl<Indices, Ts...>>;
		return static_cast<const TupleLeaf<I, Type>&>(t).getInternal();
	}

	template <size_t I, typename Indices, typename... Ts>
	inline tuple_element<I, TupleImpl<Indices, Ts...>>&& get(TupleImpl<Indices, Ts...>&& t)
	{
		using Type = tuple_element_t<I, TupleImpl<Indices, Ts...>>;
		return static_cast<Type&&>(static_cast<TupleLeaf<I, Type>&>(t).getInternal());
	}

	template <typename T, typename Indices, typename... Ts>
	inline T& get(TupleImpl<Indices, Ts...> &t)
	{
		using Index = tuple_index<T, TupleImpl<Indices, Ts...>>;
		return static_cast<TupleLeaf<Index::index, T>&>(t).getInternal();
	}

	template <typename T, typename Indices, typename... Ts>
	inline const T& get(const TupleImpl<Indices, Ts...> &t)
	{
		using Index = tuple_index<T, TupleImpl<Indices, Ts...>>;
		return static_cast<const TupleLeaf<Index::index, T>&>(t).getInternal();
	}

	template <typename T, typename Indices, typename... Ts>
	inline T&& get(TupleImpl<Indices, Ts...> &&t)
	{
		using Index = tuple_index<T, TupleImpl<Indices, Ts...>>;
		return static_cast<T&&>(static_cast<TupleLeaf<Index::index, T>&>(t).getInternal());
	}

	/*
	 * TupleLike
	 * type-trait that determines if a type is a tuple or pair.
	 * */

	template <typename T>
	struct TupleLike : public std::false_type
	{

	};

	template <typename T>
	struct TupleLike<const T> : public TupleLike<T>
	{

	};

	template <typename T>
	struct TupleLike<volatile T> : public TupleLike<T>
	{

	};

	template <typename T>
	struct TupleLike<const volatile T> : public TupleLike<T>
	{

	};

	template <typename... Ts>
	struct TupleLike<tuple<Ts...>> : public std::true_type
	{

	};

	template <typename First, typename Second>
	struct TupleLike<pair<First, Second>> : public std::true_type
	{

	};

	/*
	 * TupleConvertible
	 * */

	template <bool IsSameSize, typename From, typename to>
	struct TupleConvertibleImpl : public std::false_type
	{

	};

	template <typename... FromTypes, typename... ToTypes>
	struct TupleConvertibleImpl<true, TupleTypes<FromTypes...>, TupleTypes<ToTypes...>> : public std::integral_constant<bool, std::conjunction_v<std::is_convertible<FromTypes, ToTypes>...>>
	{

	};

	template <typename From, typename To, bool = TupleLike<typename std::remove_reference_t<From>>::value, bool = TupleLike<typename std::remove_reference_t<To>>::value>
	struct TupleConvertible : public std::false_type
	{

	};

	template <typename From, typename To>
	struct TupleConvertible<From, To, true, true> : public TupleConvertibleImpl<tuple_size_v<std::remove_reference_t<From>> == tuple_size_v<std::remove_reference_t<To>>, MakeTupleTypes_t<From>, MakeTupleTypes_t<To>>
	{

	};

	/*
	 * TupleAssignable
	 * */

	template <bool IsSameSize, typename Target, typename From>
	struct TupleAssignableImpl : public std::false_type
	{

	};

	template <typename... TargetTypes, typename... FromTypes>
	struct TupleAssignableImpl<true, TupleTypes<TargetTypes...>, TupleTypes<FromTypes...>> : public std::bool_constant<std::conjunction_v<std::is_assignable<TargetTypes, FromTypes>...>>
	{

	};

	template <typename Target, typename From, bool = TupleLike<std::remove_reference_t<Target>>::value, bool = TupleLike<std::remove_reference_t<From>>::value>
	struct TupleAssignable : public std::false_type
	{

	};

	template <typename Target, typename From>
	struct TupleAssignable<Target, From, true, true> : public TupleAssignableImpl<tuple_size_v<std::remove_reference_t<Target>> == tuple_size_v<std::remove_reference_t<From>>, MakeTupleTypes_t<Target>, MakeTupleTypes_t<From>>
	{

	};

	/*
	 * TupleImplicitlyConvertible and TupleExplicitlyConvertible
	 * */

	template <bool IsSameSize, typename TargetType, typename... FromTypes>
	struct TupleImplicitlyConvertibleImpl : public std::false_type
	{

	};

	template <typename... TargetTypes, typename... FromTypes>
	struct TupleImplicitlyConvertibleImpl<true, TupleTypes<TargetTypes...>, FromTypes...> : public std::conjunction<std::is_constructible<TargetTypes, FromTypes>..., std::is_convertible<FromTypes, TargetTypes>...>
	{

	};

	template <typename TargetTupleType, typename... FromTypes>
	struct TupleImplicitlyConvertible : public TupleImplicitlyConvertibleImpl<tuple_size_v<TargetTupleType> == sizeof...(FromTypes), MakeTupleTypes_t<TargetTupleType>, FromTypes...>::type
	{

	};

	template <typename TargetTupleType, typename... FromTypes>
	using TupleImplicitlyConvertible_t = std::enable_if_t<TupleImplicitlyConvertible<TargetTupleType, FromTypes...>::value, bool>;

	template<bool IsSameSize, typename TargetType, typename... FromTypes>
	struct TupleExplicitlyConvertibleImpl : public std::false_type
	{

	};

	template <typename... TargetTypes, typename... FromTypes>
	struct TupleExplicitlyConvertibleImpl<true, TupleTypes<TargetTypes...>, FromTypes...> : public std::conjunction<std::is_constructible<TargetTypes, FromTypes>..., std::negation<std::conjunction<std::is_convertible<FromTypes, TargetTypes>...>>>
	{

	};

	template <typename TargetTupleType, typename... FromTypes>
	struct TupleExplicitlyConvertible : public TupleExplicitlyConvertibleImpl<tuple_size_v<TargetTupleType> == sizeof...(FromTypes), MakeTupleTypes_t<TargetTupleType>, FromTypes...>::type
	{

	};

	template <typename TargetTupleType, typename... FromTypes>
	using TupleExplicitlyConvertible_t = std::enable_if_t<TupleExplicitlyConvertible<TargetTupleType, FromTypes...>::value, bool>;


	/*
	 * TupleEqual
	 * */

	template <size_t I>
	struct TupleEqual
	{
		template <typename Tuple1, typename Tuple2>
		bool operator()(const Tuple1 &t1, const Tuple2 &t2)
		{
			static_assert(tuple_size_v<Tuple1> == tuple_size_v<Tuple2>, "comparing tuples of different sizes.");
			return TupleEqual<I - 1>()(t1, t2) && get<I - 1>(t1) == get<I - 1>(t2);
		}
	};

	template <>
	struct TupleEqual<0>
	{
		template <typename Tuple1, typename Tuple2>
		bool operator()(const Tuple1 &t1, const Tuple2 &t2)
		{
			return true;
		}
	};

	/*
	 * TupleLess
	 * */
	template <size_t I>
	struct TupleLess
	{
		template <typename Tuple1, typename Tuple2>
		bool operator()(const Tuple1 &t1, const Tuple2 &t2)
		{
			static_assert(tuple_size_v<Tuple1> == tuple_size_v<Tuple2>, "comparing tuples of different sizes.");
			return TupleLess<I - 1>()(t1, t2) || (!TupleLess<I-1>(t2, t1) && get<I - 1>(t1) < get<I - 1>(t2));
		}
	};

	template <>
	struct TupleLess<0>
	{
		template <typename Tuple1, typename Tuple2>
		bool operator()(const Tuple1 &t1, const Tuple2 &t2)
		{
			return false;
		}
	};

	/*
	 * MakeTupleReturnImpl
	 * */

	template <typename T>
	struct MakeTupleReturnImpl
	{
		using type = T;
	};

	template <typename T>
	struct MakeTupleReturnImpl<std::reference_wrapper<T>>
	{
		using type = T&;
	};

	template <typename T>
	using MakeTupleReturn_t = typename MakeTupleReturnImpl<std::decay_t<T>>::type;

	/*
	 * TupleCat
	 * */

	template <typename Tuple1, typename Is1, typename Tuple2, typename Is2>
	struct TupleCat2Impl;

	template <typename... T1s, size_t... I1s, typename... T2s, size_t... I2s>
	struct TupleCat2Impl<tuple<T1s...>, index_sequence<I1s...>, tuple<T2s...>, index_sequence<I2s...>>
	{
		using ResultType = tuple<T1s..., T2s...>;

		template <typename Tuple1, typename Tuple2>
		static inline ResultType DoCat2(Tuple1 &&t1, Tuple2 &&t2)
		{
			return ResultType(get<I1s>(std::forward<Tuple1>(t1))..., get<I2s>(std::forward<Tuple2>(t2))...);
		}
	};

	template <typename Tuple1, typename Tuple2>
	struct TupleCat2;

	template <typename... T1s, typename... T2s>
	struct TupleCat2<tuple<T1s...>, tuple<T2s...>>
	{
		using Is1 = make_index_sequence<sizeof...(T1s)>;
		using Is2 = make_index_sequence<sizeof...(T2s)>;
		using TCI = TupleCat2Impl<tuple<T1s...>, Is1, tuple<T2s...>, Is2>;
		using ResultType = typename TCI::ResultType;

		template <typename Tuple1, typename Tuple2>
		static inline ResultType DoCat2(Tuple1 &&t1, Tuple2 &&t2)
		{
			return TCI::DoCat2(std::forward<Tuple1>(t1), std::forward<Tuple2>(t2));
		}
	};

	template <typename... Tuples>
	struct TupleCat;

	template <typename Tuple1, typename Tuple2, typename... TuplesRest>
	struct TupleCat<Tuple1, Tuple2, TuplesRest...>
	{
		using FirstResultType = typename TupleCat2<Tuple1, Tuple2>::ResultType;
		using ResultType = typename TupleCat<FirstResultType, TuplesRest...>::ResultType;

		template <typename TupleArg1, typename TupleArg2, typename... TupleArgsRest>
		static inline ResultType DoCat(TupleArg1 &&t1, TupleArg2 &&t2, TupleArgsRest &&...ts)
		{
			return TupleCat<FirstResultType, TuplesRest...>::DoCat(TupleCat2<TupleArg1, TupleArg2>::DoCat2(std::forward<TupleArg1>(t1), std::forward<TupleArg2>(t2)), std::forward<TupleArgsRest>(ts)...);
		}
	};

	template <typename Tuple1, typename Tuple2>
	struct TupleCat<Tuple1, Tuple2>
	{
		using TC2 = TupleCat2<Tuple1, std::remove_reference_t<Tuple2>>;
		using ResultType = typename TC2::ResultType;

		template <typename TupleArg1, typename TupleArg2>
		static inline ResultType DoCat(TupleArg1&& t1, TupleArg2&& t2)
		{
			return TC2::DoCat2(std::forward<TupleArg1>(t1), std::forward<TupleArg2>(t2));
		}
	};

} // namespace Tuple_Internal end


template <typename... Ts>
class tuple;

template <typename T, typename... Ts>
class tuple<T, Ts...>
{
public:
	constexpr tuple() = default;

	template <typename T2 = T, Tuple_Internal::TupleImplicitlyConvertible_t<tuple, const T2&, const Ts&...> = 0>
	constexpr tuple(const T &t, const Ts&... ts) : mImpl(make_index_sequence<sizeof...(Ts) + 1>{}, Tuple_Internal::MakeTupleTypes_t<tuple>{}, t, ts...) {} // use constructor of TupleImpl

	template <typename T2 = T, Tuple_Internal::TupleExplicitlyConvertible_t<tuple, const T2&, const Ts&...> = 0>
	explicit constexpr tuple(const T &t, const Ts&... ts) : mImpl(make_index_sequence<sizeof...(Ts) + 1>{}, Tuple_Internal::MakeTupleTypes_t<tuple>{}, t, ts...) {}

	template <typename U, typename... Us, Tuple_Internal::TupleImplicitlyConvertible_t<tuple, U, Us...> = 0>
	constexpr tuple(U &&u, Us&&... us) : mImpl(make_index_sequence<sizeof...(Us) + 1>{}, Tuple_Internal::MakeTupleTypes_t<tuple>{}, std::forward<U>(u), std::forward<Us>(us)...) {}

	template <typename U, typename... Us, Tuple_Internal::TupleExplicitlyConvertible_t<tuple, U, Us...> = 0>
	explicit tuple(U &&u, Us&&... us) : mImpl(make_index_sequence<sizeof...(Us) + 1>{}, Tuple_Internal::MakeTupleTypes_t<tuple>{}, std::forward<U>(u), std::forward<Us>(us)...) {}

	template <typename OtherTuple, typename std::enable_if_t<Tuple_Internal::TupleConvertible<OtherTuple, tuple>::value, bool> = false>
	tuple(OtherTuple &&t) : mImpl(std::forward<OtherTuple>(t)) {}

	template <typename OtherTuple, typename std::enable_if_t<Tuple_Internal::TupleConvertible<OtherTuple, tuple>::value, bool> = false>
	tuple& operator=(OtherTuple &&t)
	{
		mImpl.operator=(std::forward<OtherTuple>(t));
		return *this;
	}

	void swap(tuple &t)
	{
		mImpl.swap(t.mImpl);
	}

private:
	using Impl = Tuple_Internal::TupleImpl<make_index_sequence<sizeof...(Ts) + 1>, T, Ts...>;
	Impl mImpl;

	template <size_t I, typename... Ts_>
	friend tuple_element_t<I, tuple<Ts_...>>& get(tuple<Ts_...>& t);

	template <size_t I, typename... Ts_>
	friend const_tuple_element_t<I, tuple<Ts_...>>& get(const tuple<Ts_...>& t);

	template <size_t I, typename... Ts_>
	friend tuple_element_t<I, tuple<Ts_...>>&& get(tuple<Ts_...>&& t);

	template <typename T_, typename... ts_>
	friend T_& get(tuple<ts_...>& t);

	template <typename T_, typename... ts_>
	friend const T_& get(const tuple<ts_...>& t);

	template <typename T_, typename... ts_>
	friend T_&& get(tuple<ts_...>&& t);
};

template <>
class tuple<>
{
public:
	void swap(tuple&) {}
};

template <size_t I, typename... Ts>
inline tuple_element_t<I, tuple<Ts...>>& get(tuple<Ts...>& t)
{
	return get<I>(t.mImpl);
}

template <size_t I, typename... Ts>
inline const_tuple_element_t<I, tuple<Ts...>>& get(const tuple<Ts...>& t)
{
	return get<I>(t.mImpl);
}

template <size_t I, typename... Ts>
inline tuple_element_t<I, tuple<Ts...>>&& get(tuple<Ts...>&& t)
{
	return get<I>(std::move(t.mImpl));
}

template <typename T, typename... Ts>
inline T& get(tuple<Ts...>& t)
{
	return get<T>(t.mImpl);
}

template <typename T, typename... Ts>
inline const T& get(const tuple<Ts...>& t)
{
	return get<T>(t.mImpl);
}

template <typename T, typename... Ts>
inline T&& get(tuple<Ts...> &&t)
{
	return get<T>(std::move(t.mImpl));
}

template <typename... Ts>
inline void swap(tuple<Ts...> &a, tuple<Ts...> &b)
{
	a.swap(b);
}

/*
 * tuple operators
 * */

template <typename... T1s, typename... T2s>
inline bool operator==(const tuple<T1s...> &t1, const tuple<T2s...> &t2)
{
	return Tuple_Internal::TupleEqual<sizeof...(T1s)>()(t1, t2);
}

template <typename... T1s, typename... T2s>
inline bool operator<(const tuple<T1s...> &t1, const tuple<T2s...> &t2)
{
	return Tuple_Internal::TupleLess<sizeof...(T1s)>()(t1, t2);
}

template <typename... T1s, typename... T2s>
inline bool operator!=(const tuple<T1s...>& t1, const tuple<T2s...>& t2)
{
	return !(t1 == t2);
}

template <typename... T1s, typename... T2s>
inline bool operator>(const tuple<T1s...>& t1, const tuple<T2s...>& t2)
{
	return t2 < t1;
}

template <typename... T1s, typename... T2s>
inline bool operator<=(const tuple<T1s...>& t1, const tuple<T2s...>& t2)
{
	return !(t2 < t1);
}

template <typename... T1s, typename... T2s>
inline bool operator>=(const tuple<T1s...>& t1, const tuple<T2s...>& t2)
{
	return !(t1 < t2);
}

/*
 * tuple_cat
 * */

template <typename... Tuples>
inline typename Tuple_Internal::TupleCat<Tuples...>::ResultType tuple_cat(Tuples&&... ts)
{
	return Tuple_Internal::TupleCat<Tuples...>::DoCat(std::forward<Tuples>(ts)...);
}

/*
 * make_tuple
 * */

template <typename... Ts>
inline constexpr tuple<Tuple_Internal::MakeTupleReturn_t<Ts>...> make_tuple(Ts&&... values)
{
	return tuple<Tuple_Internal::MakeTupleReturn_t<Ts>...>(std::forward<Ts>(values)...);
}

/*
 * forward_as_tuple
 * */

template <typename... Ts>
inline constexpr tuple<Ts&&...> forward_as_tuple(Ts&& ...ts) noexcept
{
	return tuple<Ts&&...>(std::forward<Ts&&>(ts)...);
}

/*
 * ignore
 * */

namespace Tuple_Internal {
	struct ignore_t
	{
		ignore_t() = default;

		template <typename T>
		const ignore_t& operator=(const T&) const
		{
			return *this;
		}
	};
}

static const Tuple_Internal::ignore_t ignore;

// tie

template <typename... Ts>
inline constexpr tuple<Ts&&...> tie(Ts&& ...ts) noexcept
{
	return tuple<Ts&&...>(ts...);
}

/*
 * print_tuple
 * */

template <size_t start, size_t end, typename... Args>
struct print_tuple
{
	static void print(std::ostream &stream, const tuple<Args...> &t)
	{
		stream << get<start>(t) << (start + 1 == end ? "" : ",");
		print_tuple<start + 1, end, Args...>::print(stream, t);
	}
};

template <size_t end, typename... Args>
struct print_tuple<end, end, Args...>
{
	static void print(std::ostream &stream, const tuple<Args...> &t)
	{

	}
};

template <typename... Args>
std::ostream& operator<<(std::ostream& stream, const tuple<Args...> &t)
{
	stream << "[";
	print_tuple<0, sizeof...(Args), Args...>::print(stream, t);
	return stream << "]";
}

RSTL_NAMESPACE_END

#endif //RSTL_TUPLE_H
