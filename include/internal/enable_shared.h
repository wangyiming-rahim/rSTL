#ifndef RSTL_ENABLE_SHARED_H
#define RSTL_ENABLE_SHARED_H

#include "config.h"

#pragma once

RSTL_NAMESPACE_BEGIN

template <typename T>
class enable_shared_from_this
{
public:
	shared_ptr<T> shared_from_this()
	{
		return shared_ptr<T>(mWeakPtr);
	}

	shared_ptr<const T> shared_from_this() const
	{
		return shared_ptr<const T>(mWeakPtr);
	}

	weak_ptr<T> weak_from_this()
	{
		return mWeakPtr;
	}

	weak_ptr<const T> weak_from_this() const
	{
		return mWeakPtr;
	}
public:
	mutable weak_ptr<T> mWeakPtr;

protected:
	template <typename U>
	friend class shared_ptr;

	constexpr enable_shared_from_this() noexcept { }

	enable_shared_from_this(const enable_shared_from_this&) noexcept { }

	enable_shared_from_this& operator=(const enable_shared_from_this&) noexcept
	{
		return *this;
	}

	~enable_shared_from_this() { }
};

RSTL_NAMESPACE_END

#endif //RSTL_ENABLE_SHARED_H
