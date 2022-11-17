#ifndef RSTL_COMPRESSED_PAIR_H
#define RSTL_COMPRESSED_PAIR_H

#include "config.h"
#include "call_traits.h"

#include <type_traits>

#pragma once

RSTL_NAMESPACE_BEGIN

template <typename T1, typename T2>
class compressed_pair;

template <typename T1, typename T2, bool isSame, bool firstEmpty, bool secondEmpty>
struct compressed_pair_switch;

template <typename T1, typename T2>
struct compressed_pair_switch<T1, T2, false, false, false>
{
	static const int value = 0;
};

template <typename T1, typename T2>
struct compressed_pair_switch<T1, T2, false, true, false>
{
	static const int value = 1;
};

template <typename T1, typename T2>
struct compressed_pair_switch<T1, T2, false, false, true>
{
	static const int value = 2;
};

template <typename T1, typename T2>
struct compressed_pair_switch<T1, T2, false, true, true>
{
	static const int value = 3;
};

template <typename T1, typename T2>
struct compressed_pair_switch<T1, T2, true, true, true>
{
	static const int value = 4;
};

template <typename T1, typename T2>
struct compressed_pair_switch<T1, T2, true, false, false>
{
	static const int value = 5;
};

template <typename T1, typename T2, int version>
class compressed_pair_imp;

template <typename T>
inline void cp_swap(T& t1, T& t2)
{
	T temp = t1;
	t1 = t2;
	t2 = temp;
}

template <typename T1, typename T2>
class compressed_pair_imp<T1, T2, 0>
{
public:
	using first_type = T1;
	using second_type = T2;
	using first_param_type = typename call_traits<first_type>::param_type;
	using second_param_type = typename call_traits<second_type>::param_type;
	using first_reference = typename call_traits<first_type>::reference;
	using second_reference = typename call_traits<second_type>::reference;
	using first_const_reference = typename call_traits<first_type>::const_reference;
	using second_const_reference = typename call_traits<second_type>::const_reference;

	compressed_pair_imp() {}

	compressed_pair_imp(first_param_type x, second_param_type y) : mFirst(x), mSecond(y) {}

	compressed_pair_imp(first_param_type x) : mFirst(x) {}

	compressed_pair_imp(second_param_type y) : mSecond(y) {}

	first_reference first()
	{
		return mFirst;
	}

	first_const_reference first() const
	{
		return mFirst;
	}

	second_reference second()
	{
		return mSecond;
	}

	second_const_reference second() const
	{
		return mSecond;
	}

	void swap(compressed_pair<T1, T2>& y)
	{
		cp_swap(mFirst, y.first());
		cp_swap(mSecond, y.second());
	}

private:
	first_type mFirst;
	second_type mSecond;
};

template <typename T1, typename T2>
class compressed_pair_imp<T1, T2, 1> : private T1
{
public:
	using first_type = T1;
	using second_type = T2;
	using first_param_type = typename call_traits<first_type>::param_type;
	using second_param_type = typename call_traits<second_type>::param_type;
	using first_reference = typename call_traits<first_type>::reference;
	using second_reference = typename call_traits<second_type>::reference;
	using first_const_reference = typename call_traits<first_type>::const_reference;
	using second_const_reference = typename call_traits<second_type>::const_reference;

	compressed_pair_imp() {}

	compressed_pair_imp(first_param_type x, second_param_type y) : first_type(x), mSecond(y) {}

	compressed_pair_imp(first_param_type x) : first_type(x) {}

	compressed_pair_imp(second_param_type y) : mSecond(y) {}

	first_reference first()
	{
		return *this;
	}

	first_const_reference first() const
	{
		return *this;
	}

	second_reference second()
	{
		return mSecond;
	}

	second_const_reference second() const
	{
		return mSecond;
	}

	void swap(compressed_pair<T1, T2>& y)
	{
		cp_swap(mSecond, y.second());
	}

private:
	second_type mSecond;
};

template <typename T1, typename T2>
class compressed_pair_imp<T1, T2, 2> : private T2
{
public:
	using first_type = T1;
	using second_type = T2;
	using first_param_type = typename call_traits<first_type>::param_type;
	using second_param_type = typename call_traits<second_type>::param_type;
	using first_reference = typename call_traits<first_type>::reference;
	using second_reference = typename call_traits<second_type>::reference;
	using first_const_reference = typename call_traits<first_type>::const_reference;
	using second_const_reference = typename call_traits<second_type>::const_reference;

	compressed_pair_imp() {}

	compressed_pair_imp(first_param_type x, second_param_type y) : second_type(y), mFirst(x) {}

	compressed_pair_imp(first_param_type x) : mFirst(x) {}

	compressed_pair_imp(second_param_type y) : second_type(y) {}

	first_reference first()
	{
		return mFirst;
	}

	first_const_reference first() const
	{
		return mFirst;
	}

	second_reference second()
	{
		return *this;
	}

	second_const_reference second() const
	{
		return *this;
	}

	void swap(compressed_pair<T1, T2>& y)
	{
		cp_swap(mFirst, y.first());
	}

private:
	first_type mFirst;
};

template <typename T1, typename T2>
class compressed_pair_imp<T1, T2, 3> : private T1, private T2
{
public:
	using first_type = T1;
	using second_type = T2;
	using first_param_type = typename call_traits<first_type>::param_type;
	using second_param_type = typename call_traits<second_type>::param_type;
	using first_reference = typename call_traits<first_type>::reference;
	using second_reference = typename call_traits<second_type>::reference;
	using first_const_reference = typename call_traits<first_type>::const_reference;
	using second_const_reference = typename call_traits<second_type>::const_reference;

	compressed_pair_imp() {}

	compressed_pair_imp(first_param_type x, second_param_type y) : first_type(x), second_type(y) {}

	compressed_pair_imp(first_param_type x) : first_type(x) {}

	compressed_pair_imp(second_param_type y) : second_type(y) {}

	first_reference first()
	{
		return *this;
	}

	first_const_reference first() const
	{
		return *this;
	}

	second_reference second()
	{
		return *this;
	}

	second_const_reference second() const
	{
		return *this;
	}

	void swap(compressed_pair<T1, T2>&) {}

};

template <typename T1, typename T2>
class compressed_pair_imp<T1, T2, 4> : private T1
{
public:
	using first_type = T1;
	using second_type = T2;
	using first_param_type = typename call_traits<first_type>::param_type;
	using second_param_type = typename call_traits<second_type>::param_type;
	using first_reference = typename call_traits<first_type>::reference;
	using second_reference = typename call_traits<second_type>::reference;
	using first_const_reference = typename call_traits<first_type>::const_reference;
	using second_const_reference = typename call_traits<second_type>::const_reference;

	compressed_pair_imp() {}

	compressed_pair_imp(first_param_type x, second_param_type) : first_type(x) {}

	compressed_pair_imp(first_param_type x) : first_type(x) {}

	first_reference first()
	{
		return *this;
	}

	first_const_reference first() const
	{
		return *this;
	}

	second_reference second()
	{
		return *this;
	}

	second_const_reference second() const
	{
		return *this;
	}

	void swap(compressed_pair<T1, T2>&) {}

};

template <typename T1, typename T2>
class compressed_pair_imp<T1, T2, 5>
{
public:
	using first_type = T1;
	using second_type = T2;
	using first_param_type = typename call_traits<first_type>::param_type;
	using second_param_type = typename call_traits<second_type>::param_type;
	using first_reference = typename call_traits<first_type>::reference;
	using second_reference = typename call_traits<second_type>::reference;
	using first_const_reference = typename call_traits<first_type>::const_reference;
	using second_const_reference = typename call_traits<second_type>::const_reference;

	compressed_pair_imp() {}

	compressed_pair_imp(first_param_type x, second_param_type y) : mFirst(x), mSecond(y) {}

	compressed_pair_imp(first_param_type x) : mFirst(x), mSecond(x) {}

	first_reference first()
	{
		return mFirst;
	}

	first_const_reference first() const
	{
		return mFirst;
	}

	second_reference second()
	{
		return mSecond;
	}

	second_const_reference second() const
	{
		return mSecond;
	}

	void swap(compressed_pair<T1, T2>& y)
	{
		cp_swap(mFirst, y.first());
		cp_swap(mSecond, y.second());
	}

private:
	first_type mFirst;
	second_type mSecond;
};

template <typename T1, typename T2>
class compressed_pair : private compressed_pair_imp<T1, T2, compressed_pair_switch<T1, T2, std::is_same_v<std::remove_cv_t<T1>, std::remove_cv_t<T2>>, std::is_empty_v<T1>, std::is_empty_v<T2>>::value>
{
private:
	using base = compressed_pair_imp<T1, T2, compressed_pair_switch<T1, T2, std::is_same_v<std::remove_cv_t<T1>, std::remove_cv_t<T2>>, std::is_empty_v<T1>, std::is_empty_v<T2>>::value>;

public:
	using first_type = T1;
	using second_type = T2;
	using first_param_type = typename call_traits<first_type>::param_type;
	using second_param_type = typename call_traits<second_type>::param_type;
	using first_reference = typename call_traits<first_type>::reference;
	using second_reference = typename call_traits<second_type>::reference;
	using first_const_reference = typename call_traits<first_type>::const_reference;
	using second_const_reference = typename call_traits<second_type>::const_reference;

	compressed_pair() : base() {}

	compressed_pair(first_param_type x, second_param_type y) : base(x, y) {}

	explicit compressed_pair(first_param_type x) : base(x) {}

	explicit compressed_pair(second_param_type y) : base(y) {}

	first_reference first()
	{
		return base::first();
	}

	first_const_reference first() const
	{
		return base::first();
	}

	second_reference second()
	{
		return base::second();
	}

	second_const_reference second() const
	{
		return base::second();
	}

	void swap(compressed_pair& y)
	{
		base::swap(y);
	}
};

template <typename T>
class compressed_pair<T, T> : private compressed_pair_imp<T, T, compressed_pair_switch<T, T, std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<T>>, std::is_empty_v<T>, std::is_empty_v<T>>::value>
{
private:
	using base = compressed_pair_imp<T, T, compressed_pair_switch<T, T, std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<T>>, std::is_empty_v<T>, std::is_empty_v<T>>::value>;

public:
	using first_type = T;
	using second_type = T;
	using first_param_type = typename call_traits<first_type>::param_type;
	using second_param_type = typename call_traits<second_type>::param_type;
	using first_reference = typename call_traits<first_type>::reference;
	using second_reference = typename call_traits<second_type>::reference;
	using first_const_reference = typename call_traits<first_type>::const_reference;
	using second_const_reference = typename call_traits<second_type>::const_reference;

	compressed_pair() : base() {}

	compressed_pair(first_param_type x, second_param_type y) : base(x, y) {}

	explicit compressed_pair(first_param_type x) : base(x) {}

	first_reference first()
	{
		return base::first();
	}

	first_const_reference first() const
	{
		return base::first();
	}

	second_reference second()
	{
		return base::second();
	}

	second_const_reference second() const
	{
		return base::second();
	}

	void swap(compressed_pair& y)
	{
		base::swap(y);
	}
};

template <typename T1, typename T2>
inline void swap(compressed_pair<T1, T2>& x, compressed_pair<T1, T2>& y)
{
	x.swap(y);
}

RSTL_NAMESPACE_END

#endif //RSTL_COMPRESSED_PAIR_H
