#ifndef RSTL_SHARED_PTR_H
#define RSTL_SHARED_PTR_H

#pragma once

#include "../internal/config.h"
#include "../internal/thread_support.h"
#include "allocator.h"

#include <typeinfo>
#include <exception>

RSTL_NAMESPACE_BEGIN

// Forward declarations
template <typename T, typename Deleter>
class unique_ptr;

template <typename T>
class weak_ptr;

template <typename T>
class enable_shared_from_this;

struct bad_weak_ptr : std::exception
{
	const char* what() const noexcept override
	{
		return "bad weak_ptr";
	}
};

/*
 * This is a utility class used by shared_ptr and weak_ptr.
 * */
struct ref_count_sp
{
	int32_t mRefCount;
	int32_t mWeakRefCount;

public:
	ref_count_sp(int32_t refCount =1, int32_t weakRefCount = 1) noexcept;
	virtual ~ref_count_sp() noexcept {}

	int32_t use_count() const noexcept;
	void addref() noexcept;
	void release() noexcept;
	void weak_addref() noexcept;
	void weak_release() noexcept;
	ref_count_sp* lock() noexcept;

	virtual void free_value() noexcept = 0;
	virtual void free_ref_count_sp() noexcept = 0;

	virtual void* get_deleter(const std::type_info& type) const noexcept = 0;
};

inline ref_count_sp::ref_count_sp(int32_t refCount, int32_t weakRefCount) noexcept : mRefCount(refCount), mWeakRefCount(weakRefCount) {}

inline int32_t ref_count_sp::use_count() const noexcept
{
	return mRefCount;
}

inline void ref_count_sp::addref() noexcept
{
	Thread_Support_Internal::atomic_increment(&mRefCount);
	Thread_Support_Internal::atomic_increment(&mWeakRefCount);
}

inline void ref_count_sp::release() noexcept
{
	if(Thread_Support_Internal::atomic_decrement(&mRefCount) == 0)
	{
		free_value();
	}
	if(Thread_Support_Internal::atomic_decrement(&mWeakRefCount) == 0)
	{
		free_ref_count_sp();
	}
}

inline void ref_count_sp::weak_addref() noexcept
{
	Thread_Support_Internal::atomic_increment(&mWeakRefCount);
}

inline void ref_count_sp::weak_release() noexcept
{
	if(Thread_Support_Internal::atomic_decrement(&mWeakRefCount) == 0)
	{
		free_ref_count_sp();
	}
}

inline ref_count_sp* ref_count_sp::lock() noexcept
{
	for(int32_t refCountTemp = mRefCount; refCountTemp != 0; refCountTemp = mRefCount)
	{
		if(Thread_Support_Internal::atomic_compare_and_swap(&mRefCount, refCountTemp + 1, refCountTemp))
		{
			Thread_Support_Internal::atomic_increment(&mWeakRefCount);
			return this;
		}
	}
	return nullptr;
}

/*
 * This is a version of ref_count_sp which is used to delete the contained pointer.
 * */

template <typename T, typename Allocator, typename Deleter>
class ref_count_sp_t : public ref_count_sp
{
public:
	using this_type = ref_count_sp_t<T, Allocator, Deleter>;
	using value_type = T;
	using allocator_type = Allocator;
	using deleter_type = Deleter;

	value_type mValue; // This expected to be a pointer.
	deleter_type mDeleter;
	allocator_type mAllocator;

	ref_count_sp_t(value_type value, deleter_type deleter, allocator_type allocator) : ref_count_sp(), mValue(value), mDeleter(std::move(deleter)), mAllocator(std::move(allocator)) {}

	void free_value() noexcept
	{
		mDeleter(mValue);
		mValue = nullptr;
	}

	void free_ref_count_sp() noexcept
	{
		allocator_type allocator = mAllocator;
		this->~ref_count_sp_t();
		CUSTOM_FREE(allocator, this, sizeof(*this));
	}

	void* get_deleter(const std::type_info& type) const noexcept
	{
		return (type == typeid(deleter_type)) ? (void*)&mDeleter : nullptr;
	}

};

/*
 * This is a version of ref_count_sp which is used to actually hold an instance of
 * T(instead of a pointer). This is useful to allocate the object and ref count in
 * a single memory allocation.
 * */

template <typename T, typename Allocator>
class ref_count_sp_t_inst : public ref_count_sp
{
public:
	using this_type = ref_count_sp_t_inst<T, Allocator>;
	using value_type = T;
	using allocator_type = Allocator;
	using storage_type = typename std::aligned_storage_t<sizeof(T), std::alignment_of_v<T>>;

	storage_type mMemory;
	allocator_type mAllocator;

	value_type* GetValue()
	{
		return static_cast<value_type*>(static_cast<void*>(&mMemory));
	}

	template<typename... Args>
	ref_count_sp_t_inst(allocator_type allocator, Args&&... args) : ref_count_sp(), mAllocator(std::move(allocator))
	{
		new(&mMemory)value_type(std::forward<Args>(args)...);
	}

	void free_value() noexcept
	{
		GetValue()->~value_type();
	}

	void free_ref_count_sp() noexcept
	{
		allocator_type allocator = mAllocator;
		this->~ref_count_sp_t_inst();
	}

	void* get_deleter(const std::type_info&) const noexcept
	{
		return nullptr;
	}
};

/*
 * If a user calls this function, it sets up mWeakPtr member of
 * the enable_shared_from_this parameter to point to the ref_count_sp
 * object that's passed in. Normally, the user doesn't need to call
 * this function, as the shared_ptr constructor will do it for them.
 * */

template <typename T, typename U>
void do_enable_shared_from_this(const ref_count_sp* pRefCount, const enable_shared_from_this<T>* pEnableSharedFromThis, const U* pValue)
{
	if (pEnableSharedFromThis)
	{
		pEnableSharedFromThis->mWeakPtr.assign(const_cast<U*>(pValue), const_cast<ref_count_sp*>(pRefCount));
	}
}
inline void do_enable_shared_from_this(const ref_count_sp*, ...)
{

}

/*
 * shared_ptr_traits
 * */

template <typename T>
struct shared_ptr_traits
{
	using reference_type = T&;
};

template <>
struct shared_ptr_traits<void>
{
	using reference_type = void;
};

template <>
struct shared_ptr_traits<void const>
{
	using reference_type = void;
};

template <>
struct shared_ptr_traits<void volatile>
{
	using reference_type = void;
};

template <>
struct shared_ptr_traits<void volatile const>
{
	using reference_type = void;
};

/*
 * shared_ptr
 * */

template <typename T>
class shared_ptr
{
public:
	using this_type = shared_ptr<T>;
	using element_type = T;
	using reference_type = typename shared_ptr_traits<T>::reference_type;
	using default_allocator_type = rstl::allocator;
};


RSTL_NAMESPACE_END

#endif //RSTL_SHARED_PTR_H
