#ifndef RSTL_SHARED_PTR_H
#define RSTL_SHARED_PTR_H

#pragma once

#include "internal/config.h"
#include "internal/thread_support.h"
#include "allocator.h"
#include "internal/smart_ptr.h"

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
	using default_deleter_type = default_delete<T>;
	using weak_type = weak_ptr<T>;

protected:
	element_type* mpValue;
	ref_count_sp* mpRefCount;

public:
	shared_ptr() noexcept : mpValue(nullptr), mpRefCount(nullptr) { }

	template <typename U>
	explicit shared_ptr(U* pValue, std::enable_if_t<std::is_convertible_v<U*, element_type*>>* = 0) : mpValue(nullptr), mpRefCount(nullptr)
	{
		alloc_internal(pValue, default_allocator_type(), default_delete<U>());
	}

	shared_ptr(std::nullptr_t) noexcept : mpValue(nullptr), mpRefCount(nullptr) { }

	template <typename U, typename Deleter>
	shared_ptr(U* pValue, Deleter deleter, std::enable_if_t<std::is_convertible_v<U*, element_type*>>* = 0) : mpValue(nullptr), mpRefCount(nullptr)
	{
		alloc_internal(pValue, default_allocator_type(), std::move(deleter));
	}

	template<typename Deleter>
	shared_ptr(std::nullptr_t, Deleter deleter) : mpValue(nullptr), mpRefCount(nullptr)
	{
		alloc_internal(nullptr, default_allocator_type(), std::move(deleter));
	}

	template <typename U, typename Deleter, typename Allocator>
	explicit shared_ptr(U* pValue, Deleter deleter, const Allocator& allocator, std::enable_if_t<std::is_convertible_v<U*, element_type*>>* = 0)
	{
		alloc_internal(pValue, std::move(allocator), std::move(deleter));
	}

	template <typename Deleter, typename Allocator>
	shared_ptr(std::nullptr_t, Deleter deleter, Allocator allocator) : mpValue(nullptr), mpRefCount(nullptr)
	{
		alloc_internal(nullptr, std::move(allocator), std::move(deleter));
	}

	shared_ptr(const shared_ptr& sharedPtr) noexcept : mpValue(sharedPtr.mpValue), mpRefCount(sharedPtr.mpRefCount)
	{
		if(mpRefCount)
		{
			mpRefCount->addref();
		}
	}

	template<typename U>
	shared_ptr(const shared_ptr<U>& sharedPtr, std::enable_if_t<std::is_convertible_v<U*, element_type*>>* = 0) noexcept : mpValue(sharedPtr.mpValue), mpRefCount(sharedPtr.mpRefCount)
	{
		if(mpRefCount)
		{
			mpRefCount->addref();
		}
	}

	template <typename U>
	shared_ptr(const shared_ptr<U>&sharedPtr, element_type* pValue) noexcept : mpValue(pValue), mpRefCount(sharedPtr.mpRefCount)
	{
		if(mpRefCount)
		{
			mpRefCount->addref();
		}
	}

	shared_ptr(shared_ptr&& sharedPtr) noexcept : mpValue(sharedPtr.mpValue), mpRefCount(sharedPtr.mpRefCount)
	{
		sharedPtr.mpValue = nullptr;
		sharedPtr.mpRefCount = nullptr;
	}

	template <typename U>
	shared_ptr(shared_ptr<U>&& sharedPtr, std::enable_if_t<std::is_convertible_v<U*, element_type*>>* = 0) noexcept : mpValue(sharedPtr.mpValue), mpRefCount(sharedPtr.mpRefCount)
	{
		sharedPtr.mpValue = nullptr;
		sharedPtr.mpRefCount = nullptr;
	}

	template<typename U, typename Deleter>
	shared_ptr(unique_ptr<U, Deleter>&& uniquePtr, std::enable_if_t<!std::is_array_v<U> && !std::is_lvalue_reference_v<Deleter> && std::is_convertible_v<U*, element_type*>>* = 0) : mpValue(nullptr), mpRefCount(nullptr)
	{
		alloc_internal(uniquePtr.release(), default_allocator_type(), uniquePtr.get_deleter());
	}

	template<typename U, typename Deleter, typename Allocator>
	shared_ptr(unique_ptr<U, Deleter>&& uniquePtr, const Allocator& allocator, std::enable_if_t<!std::is_array_v<U> && !std::is_lvalue_reference_v<Deleter> && std::is_convertible_v<U*, element_type*>>* = 0) : mpValue(nullptr), mpRefCount(nullptr)
	{
		alloc_internal(uniquePtr.release(), allocator, uniquePtr.get_deleter());
	}

	template <typename U>
	explicit shared_ptr(const weak_ptr<U>& weakPtr, std::enable_if_t<std::is_convertible_v<U*, element_type*>>* = 0) : mpValue(weakPtr.mpValue), mpRefCount(weakPtr.mpRefCount ? weakPtr.mpRefCount->lock() : weakPtr.mpRefCount)
	{
		if(!mpRefCount)
		{
			mpValue = nullptr;
			throw std::bad_weak_ptr();
		}
	}

	~shared_ptr()
	{
		if(mpRefCount)
		{
			mpRefCount->release();
		}
	}

	shared_ptr& operator=(const shared_ptr& sharedPtr) noexcept
	{
		if(&sharedPtr != this)
		{
			this_type(sharedPtr).swap(*this);
		}
		return *this;
	}

	template <typename U>
	std::enable_if_t<std::is_convertible_v<U*, element_type*>, this_type&> operator=(const shared_ptr<U>& sharedPtr) noexcept
	{
		if(!equivalent_ownership(sharedPtr))
		{
			this_type(sharedPtr).swap(*this);
		}
		return *this;
	}

	this_type& operator=(shared_ptr&& sharedPtr) noexcept
	{
		if(&sharedPtr != this)
		{
			this_type(std::move(sharedPtr)).swap(*this);
		}
		return *this;
	}

	template <typename U>
	std::enable_if_t<std::is_convertible_v<U*, element_type*>, this_type&> operator=(shared_ptr<U>&& sharedPtr) noexcept
	{
		if(!equivalent_ownership(sharedPtr))
		{
			shared_ptr(std::move(sharedPtr)).swap(*this);
		}
		return *this;
	}

	template<typename U, typename Deleter>
	std::enable_if_t<std::is_array_v<U> && std::is_convertible_v<U*, element_type*>, this_type&> operator=(unique_ptr<U, Deleter>&& uniquePtr)
	{
		this_type(std::move(uniquePtr)).swap(*this);
		return *this;
	}

	void reset() noexcept
	{
		this_type().swap(*this);
	}

	template<typename U>
	std::enable_if_t<std::is_convertible_v<U*, element_type*>, void> reset(U* pValue)
	{
		this_type(pValue).swap(*this);
	}

	template <typename U, typename Deleter>
	std::enable_if_t<std::is_convertible_v<U*, element_type*>, void> reset(U* pValue, Deleter deleter)
	{
		shared_ptr(pValue, deleter).swap(this);
	}

	template <typename U, typename Deleter, typename Allocator>
	std::enable_if_t<std::is_convertible_v<U*, element_type*>, void> reset(U* pValue, Deleter deleter, const Allocator& allocator)
	{
		shared_ptr(pValue, deleter, allocator).swap(*this);
	}

	void swap(this_type& sharedPtr) noexcept
	{
		element_type * const pValue = sharedPtr.mpValue;
		sharedPtr.mpValue = mpValue;
		mpValue = pValue;

		ref_count_sp* const pRefCount = sharedPtr.mpRefCount;
		sharedPtr.mpRefCount = mpRefCount;
		mpRefCount = pRefCount;
	}

	reference_type operator*() const noexcept
	{
		return *mpValue;
	}

	element_type* operator->() const noexcept
	{
		return mpValue;
	}

	element_type* get() const noexcept
	{
		return mpValue;
	}

	int use_count() const noexcept
	{
		return mpRefCount ? mpRefCount->mRefCount : 0;
	}

	bool unique() const noexcept
	{
		return (mpRefCount && (mpRefCount->mRefCount == 1));
	}

	template <typename U>
	bool owner_before(const shared_ptr<U>& sharedPtr) const noexcept
	{
		return (mpRefCount < sharedPtr.mpRefCount);
	}

	template <typename U>
	bool owner_before(const weak_ptr<U>& weakPtr) const noexcept
	{
		return (mpRefCount < weakPtr.mpRefCount);
	}

	template <typename Deleter>
	Deleter* get_deleter() const noexcept
	{
		return mpRefCount ? static_cast<Deleter*>(mpRefCount->get_deleter(typeid(std::remove_cv_t<Deleter>))) : nullptr;
	}

	explicit operator bool() const noexcept
	{
		return (mpValue != nullptr);
	}

	template<typename U>
	bool equivalent_ownership(const shared_ptr<U>& sharedPtr) const
	{
		return (mpRefCount == sharedPtr.mpRefCount);
	}

protected:
	template<typename U>
	friend class shared_ptr;

	template<typename U>
	friend class weak_ptr;

	template<typename U>
	friend void allocate_shared_helper(shared_ptr<U>*, ref_count_sp*, U*);

	template <typename U, typename Allocator, typename Deleter>
	void alloc_internal(U pValue, Allocator allocator, Deleter deleter)
	{
		using ref_count_type = ref_count_sp_t<U, Allocator, Deleter>;
		try
		{
			void* const pMemory = CUSTOM_ALLOC(allocator, sizeof(ref_count_type));
			if(!pMemory)
			{
				throw std::bad_alloc();
			}
			mpRefCount = ::new(pMemory) ref_count_type(pValue, std::move(deleter), std::move(allocator));
			mpValue = pValue;
			do_enable_shared_from_this(mpRefCount, pValue, pValue);
		}
		catch(...)
		{
			deleter(pValue);
			throw;
		}
	}
};

template <typename T>
inline typename shared_ptr<T>::element_type* get_pointer(const shared_ptr<T>& sharedPtr) noexcept
{
	return sharedPtr.get();
}

template <typename Deleter, typename T>
Deleter* get_deleter(const shared_ptr<T>& sharedPtr) noexcept
{
	return sharedPtr.template get_deleter<Deleter>();
}

template <typename T>
inline void swap(shared_ptr<T>& a, shared_ptr<T>& b) noexcept
{
	a.swap(b);
}

template <typename T, typename U>
inline bool operator==(const shared_ptr<T>& a, const shared_ptr<U>& b) noexcept
{
	return (a.get() == b.get());
}

template <typename T, typename U>
std::strong_ordering operator<=>(const shared_ptr<T>& a, const shared_ptr<U>& b) noexcept
{
	return (a.get() <=> b.get());
}

template <typename T>
inline bool operator==(const shared_ptr<T>& a, std::nullptr_t) noexcept
{
	return !a;
}

template <typename T>
inline std::strong_ordering operator<=>(const shared_ptr<T>& a, std::nullptr_t) noexcept
{
	return a.get() <=> nullptr;
}

template <typename T, typename U>
inline shared_ptr<T> reinterpret_pointer_cast(shared_ptr<U> const& sharedPtr) noexcept
{
	return shared_ptr<T>(sharedPtr, reinterpret_cast<T*>(sharedPtr.get()));
}

template <typename T, typename U>
inline shared_ptr<T> static_pointer_cast(const shared_ptr<U>& sharedPtr) noexcept
{
	return shared_ptr<T>(sharedPtr, static_cast<T*>(sharedPtr.get()));
}

template <typename T, typename U>
inline shared_ptr<T> static_shared_pointer_cast(const shared_ptr<U>& sharedPtr) noexcept
{
	return static_pointer_cast<T, U>(sharedPtr);
}

template <typename T, typename U>
inline shared_ptr<T> const_pointer_cast(const shared_ptr<U>& sharedPtr) noexcept
{
	return shared_ptr<T>(sharedPtr, const_cast<T*>(sharedPtr.get()));
}

template <typename T, typename U>
inline shared_ptr<T> const_shared_pointer_cast(const shared_ptr<U>& sharedPtr) noexcept
{
	return const_pointer_cast<T, U>(sharedPtr);
}

template <typename T, typename U>
inline shared_ptr<T> dynamic_pointer_cast(const shared_ptr<U>& sharedPtr) noexcept
{
	if (T* p = dynamic_cast<T*>(sharedPtr.get()))
	{
		return shared_ptr<T>(sharedPtr, p);
	}
	return shared_ptr<T>();
}

template <typename T, typename U>
inline std::enable_if_t<!std::is_array_v<T> && !std::is_array_v<U>, shared_ptr<T>> dynamic_shared_pointer_cast(const shared_ptr<U>& sharedPtr) noexcept
{
	return dynamic_pointer_cast<T, U>(sharedPtr);
}

//template<typename T>
//struct hash<shared_ptr<T>>
//{
//	size_t operator()(const shared_ptr<T>& x) const noexcept
//	{
//		return rstl::hash<T*>()(x.get());
//	}
//};

template<typename T>
void allocate_shared_helper(rstl::shared_ptr<T>& sharedPtr, ref_count_sp* pRefCount, T* pValue)
{
	sharedPtr.mpRefCount = pRefCount;
	sharedPtr.mpValue = pValue;
	do_enable_shared_from_this(pRefCount, pValue, pValue);
}

template <typename T, typename Allocator, typename... Args>
shared_ptr<T> allocate_shared(const Allocator& allocator, Args&&... args)
{
	using ref_count_type = ref_count_sp_t_inst<T, Allocator>;
	shared_ptr<T> ret;
	void* const pMemory = CUSTOM_ALLOC(const_cast<Allocator&>(allocator), sizeof(ref_count_type));
	if (pMemory)
	{
		ref_count_type* pRefCount = ::new(pMemory) ref_count_type(allocator, std::forward<Args>(args)...);
		allocate_shared_helper(ret, pRefCount, pRefCount->GetValue());
	}
	return ret;
}

template <typename T, typename... Args>
shared_ptr<T> make_shared(Args&&... args)
{
	return rstl::allocate_shared<T>(rstl::allocator(), std::forward<Args>(args)...);
}

/*
 *  shared_ptr atomic access
 * */

template <typename T>
inline bool atomic_is_lock_free(const shared_ptr<T>*)
{
	return false;
}

template <typename T>
inline shared_ptr<T> atomic_load(const shared_ptr<T>* pSharedPtr)
{
	Thread_Support_Internal::shared_ptr_auto_mutex autoMutex(pSharedPtr);
	return *pSharedPtr;
}

template <typename T>
inline shared_ptr<T> atomic_load_explicit(const shared_ptr<T>* pSharedPtr, ... /*std::memory_order memoryOrder*/)
{
	return atomic_load(pSharedPtr);
}

template <typename T>
inline void atomic_store(shared_ptr<T>* pSharedPtrA, shared_ptr<T> sharedPtrB)
{
	Thread_Support_Internal::shared_ptr_auto_mutex autoMutex(pSharedPtrA);
	pSharedPtrA->swap(sharedPtrB);
}

template <typename T>
inline void atomic_store_explicit(shared_ptr<T>* pSharedPtrA, shared_ptr<T> sharedPtrB, ... /*std::memory_order memoryOrder*/)
{
	atomic_store(pSharedPtrA, sharedPtrB);
}

template <typename T>
shared_ptr<T> atomic_exchange(shared_ptr<T>* pSharedPtrA, shared_ptr<T> sharedPtrB)
{
	Thread_Support_Internal::shared_ptr_auto_mutex autoMutex(pSharedPtrA);
	pSharedPtrA->swap(sharedPtrB);
	return sharedPtrB;
}

template <typename T>
inline shared_ptr<T> atomic_exchange_explicit(shared_ptr<T>* pSharedPtrA, shared_ptr<T> sharedPtrB, ... /*std::memory_order memoryOrder*/)
{
	return atomic_exchange(pSharedPtrA, sharedPtrB);
}

template <typename T>
bool atomic_compare_exchange_strong(shared_ptr<T>* pSharedPtr, shared_ptr<T>* pSharedPtrCondition, shared_ptr<T> sharedPtrNew)
{
	Thread_Support_Internal::shared_ptr_auto_mutex autoMutex(pSharedPtr);

	if(pSharedPtr->equivalent_ownership(*pSharedPtrCondition))
	{
		*pSharedPtr = sharedPtrNew;
		return true;
	}

	*pSharedPtrCondition = *pSharedPtr;
	return false;
}

template <typename T>
inline bool atomic_compare_exchange_weak(shared_ptr<T>* pSharedPtr, shared_ptr<T>* pSharedPtrCondition, shared_ptr<T> sharedPtrNew)
{
	return atomic_compare_exchange_strong(pSharedPtr, pSharedPtrCondition, sharedPtrNew);
}

template <typename T> // Returns true if pSharedPtr was equivalent to *pSharedPtrCondition.
inline bool atomic_compare_exchange_strong_explicit(shared_ptr<T>* pSharedPtr, shared_ptr<T>* pSharedPtrCondition, shared_ptr<T> sharedPtrNew, ... /*memory_order memoryOrderSuccess, memory_order memoryOrderFailure*/)
{
	return atomic_compare_exchange_strong(pSharedPtr, pSharedPtrCondition, sharedPtrNew);
}

template <typename T>
inline bool atomic_compare_exchange_weak_explicit(shared_ptr<T>* pSharedPtr, shared_ptr<T>* pSharedPtrCondition, shared_ptr<T> sharedPtrNew, ... /*memory_order memoryOrderSuccess, memory_order memoryOrderFailure*/)
{
	return atomic_compare_exchange_weak(pSharedPtr, pSharedPtrCondition, sharedPtrNew);
}

/*
 * weak_ptr
 * */

template <typename T>
class weak_ptr
{
public:
	using this_type = weak_ptr<T>;
	using element_type = T;

public:
	weak_ptr() noexcept : mpValue(nullptr), mpRefCount(nullptr) { }

	weak_ptr(const this_type& weakPtr) noexcept : mpValue(weakPtr.mpValue), mpRefCount(weakPtr.mpRefCount)
	{
		if(mpRefCount)
		{
			mpRefCount->weak_addref();
		}
	}

	weak_ptr(this_type&& weakPtr) noexcept : mpValue(weakPtr.mpValue), mpRefCount(weakPtr.mpRefCount)
	{
		weakPtr.mpValue = nullptr;
		weakPtr.mpRefCount = nullptr;
	}

	template <typename U>
	weak_ptr(const weak_ptr<U>& weakPtr, std::enable_if_t<std::is_convertible_v<U*, element_type*>>* = 0) noexcept : mpValue(weakPtr.mpValue), mpRefCount(weakPtr.mpRefCount)
	{
		if(mpRefCount)
		{
			mpRefCount->weak_addref();
		}
	}

	template <typename U>
	weak_ptr(weak_ptr<U>&& weakPtr,  std::enable_if_t<std::is_convertible_v<U*, element_type*>>* = 0) noexcept : mpValue(weakPtr.mpValue), mpRefCount(weakPtr.mpRefCount)
	{
		weakPtr.mpValue = nullptr;
		weakPtr.mpRefCount = nullptr;
	}

	template <typename U>
	weak_ptr(const shared_ptr<U>& sharedPtr, std::enable_if_t<std::is_convertible_v<U*, element_type*>>* = 0) noexcept : mpValue(sharedPtr.mpValue), mpRefCount(sharedPtr.mpRefCount)
	{
		if(mpRefCount)
		{
			mpRefCount->weak_addref();
		}
	}

	~weak_ptr()
	{
		if(mpRefCount)
		{
			mpRefCount->weak_release();
		}
	}

	this_type& operator=(const this_type& weakPtr) noexcept
	{
		assign(weakPtr);
		return *this;
	}

	this_type& operator=(this_type&& weakPtr) noexcept
	{
		weak_ptr(std::move(weakPtr)).swap(*this);
		return *this;
	}

	template<typename U>
	std::enable_if_t<std::is_convertible_v<U*, element_type*>, this_type&> operator=(const weak_ptr<U>& weakPtr) noexcept
	{
		assign(weakPtr);
		return *this;
	}

	template<typename U>
	std::enable_if_t<std::is_convertible_v<U*, element_type*>, this_type&> operator=(weak_ptr<U>&& weakPtr) noexcept
	{
		weak_ptr(std::move(weakPtr)).swap(*this);
		return *this;
	}

	template<typename U>
	std::enable_if_t<std::is_convertible_v<U*, element_type*>, this_type&> operator=(const shared_ptr<U>& sharedPtr) noexcept
	{
		if(mpRefCount != sharedPtr.mpRefCount)
		{
			if(mpRefCount)
			{
				mpRefCount->weak_release();
			}
			mpValue = sharedPtr.mpValue;
			mpRefCount = sharedPtr.mpRefCount;
			if(mpRefCount)
			{
				mpRefCount->weak_addref();
			}
		}
		return *this;
	}

	shared_ptr<T> lock() const noexcept
	{
		shared_ptr<T> temp;
		temp.mpRefCount = mpRefCount ? mpRefCount->lock() : mpRefCount;
		if(temp.mpRefCount)
		{
			temp.mpValue = mpValue;
		}
		return temp;
	}

	int use_count() const noexcept
	{
		return mpRefCount ? mpRefCount->mRefCount : 0;
	}

	bool expired() const noexcept
	{
		return (!mpRefCount || (mpRefCount->mRefCount == 0));
	}

	void reset()
	{
		if(mpRefCount)
		{
			mpRefCount->weak_release();
		}
		mpValue    = nullptr;
		mpRefCount = nullptr;
	}

	void swap(this_type& weakPtr)
	{
		T* const pValue = weakPtr.mpValue;
		weakPtr.mpValue = mpValue;
		mpValue         = pValue;

		ref_count_sp* const pRefCount = weakPtr.mpRefCount;
		weakPtr.mpRefCount = mpRefCount;
		mpRefCount         = pRefCount;
	}

	template <typename U>
	void assign(const weak_ptr<U>& weakPtr, std::enable_if_t<std::is_convertible_v<U*, element_type*>>* = 0) noexcept
	{
		if(mpRefCount != weakPtr.mpRefCount)
		{
			if(mpRefCount)
			{
				mpRefCount->weak_release();
			}
			mpValue = weakPtr.mpValue;
			mpRefCount = weakPtr.mpRefCount;
			if(mpRefCount)
			{
				mpRefCount->weak_addref();
			}
		}
	}

	template <typename U>
	bool owner_before(const weak_ptr<U>& weakPtr) const noexcept
	{
		return (mpRefCount < weakPtr.mpRefCount);
	}

	template <typename U>
	bool owner_before(const shared_ptr<U>& sharedPtr) const noexcept
	{
		return (mpRefCount < sharedPtr.mpRefCount);
	}

	template <typename U>
	bool less_than(const weak_ptr<U>& weakPtr) const noexcept
	{
		return (mpRefCount < weakPtr.mpRefCount);
	}

	void assign(element_type* pValue, ref_count_sp*pRefCount)
	{
		mpValue = pValue;
		if(pRefCount != mpRefCount)
		{
			if(mpRefCount)
			{
				mpRefCount->weak_release();
			}
			mpRefCount = pRefCount;
			if(mpRefCount)
			{
				mpRefCount->weak_addref();
			}
		}
	}

protected:
	element_type* mpValue;
	ref_count_sp* mpRefCount;

	template <typename U>
	friend class shared_ptr;

	template <typename U>
	friend class weak_ptr;

};

template <typename T, typename U>
inline bool operator<(const weak_ptr<T>& weakPtr1, const weak_ptr<U>& weakPtr2)
{
	return weakPtr1.owner_before(weakPtr2);
}

template <typename T>
void swap(weak_ptr<T>& weakPtr1, weak_ptr<T>& weakPtr2)
{
	weakPtr1.swap(weakPtr2);
}

template <typename T>
struct owner_less;

template <typename T>
struct owner_less<shared_ptr<T>> : std::binary_function<shared_ptr<T>, shared_ptr<T>, bool>
{
	using result_type = bool;

	bool operator()(shared_ptr<T> const& a, shared_ptr<T> const& b) const
	{
		return a.owner_before(b);
	}

	bool operator()(shared_ptr<T> const& a, weak_ptr<T> const& b) const
	{
		return a.owner_before(b);
	}

	bool operator()(weak_ptr<T> const& a, shared_ptr<T> const& b) const
	{
		return a.owner_before(b);
	}
};

template <typename T>
struct owner_less<weak_ptr<T>> : public std::binary_function<weak_ptr<T>, weak_ptr<T>, bool>
{
	typedef bool result_type;

	bool operator()(weak_ptr<T> const& a, weak_ptr<T> const& b) const
	{
		return a.owner_before(b);
	}

	bool operator()(weak_ptr<T> const& a, shared_ptr<T> const& b) const
	{
		return a.owner_before(b);
	}

	bool operator()(shared_ptr<T> const& a, weak_ptr<T> const& b) const
	{
		return a.owner_before(b);
	}
};

RSTL_NAMESPACE_END

#endif //RSTL_SHARED_PTR_H
