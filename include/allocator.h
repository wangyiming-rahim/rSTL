#ifndef RSTL_ALLOCATOR_H
#define RSTL_ALLOCATOR_H

#pragma once

#include "internal/config.h"

#include <cstddef>


RSTL_NAMESPACE_BEGIN

enum alloc_flags
{
	MEN_TEMP = 0,
	MEN_PERM = 1
};

class allocator
{
public:
	explicit allocator(const char* pName = ALLOCATOR_DEFAULT_NAME);
	allocator(const allocator& x);
	allocator(const allocator& x, const char* pName);

	allocator& operator=(const allocator& x);

	void* allocate(size_t n, int flags = 0);
	void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0);
	void deallocate(void *p, size_t n);

	const char* get_name() const;
	void set_name(const char* pName);

	// Debug name
	const char* mpName;
};

bool operator==(const allocator& a, const allocator& b);

/*
 * Defines an allocator which does nothing. it returns null from allocate calls.
 * */

class dummy_allocator {
public:
	explicit dummy_allocator(const char * = NULL) {}

	dummy_allocator(const dummy_allocator &) {}

	dummy_allocator(const dummy_allocator &, const char *) {}

	dummy_allocator &operator=(const dummy_allocator &) {
		return *this;
	}

	void *allocate(size_t n, int = 0)
	{
		return NULL;
	}

	void* allocate(size_t, size_t, size_t, int = 0)
	{
		return NULL;
	}

	void deallocate(void *, size_t) { }

	const char* get_name() const
	{
		return "";
	}

	void set_name(const char*) { }
};

inline bool operator==(const dummy_allocator&, const dummy_allocator&)
{
	return true;
}

allocator* GetDefaultAllocator();
allocator* SetDefaultAllocator(allocator* pAllocator);

template <typename Allocator>
Allocator* get_default_allocator(const Allocator*);

rstl::allocator* get_default_allocator(const rstl::allocator*);

/* Implements a default allocfreemethod which uses the default global allocator.
 * This version supports only default alignment.
 */

void* default_allocfreemethod(size_t n, void* pBuffer, void* /* pContext */);

/* This is a memory allocation dispatching function.
 * Make aligned and unaligned specializations.
 * Note that to do this we will need to use a class with a static
 * function instead of a standalone function like below.
 */

template <typename Allocator>
void* allocate_memory(Allocator& a, size_t n, size_t alignment, size_t alignmentOffset);

RSTL_NAMESPACE_END

#include <new>

RSTL_NAMESPACE_BEGIN

inline allocator::allocator(const char *pName)
{
	mpName = pName;
}

inline allocator::allocator(const allocator& alloc)
{
	mpName = alloc.mpName;
}

inline allocator::allocator(const allocator& alloc, const char* pName)
{
	mpName = pName;
}

inline allocator& allocator::operator=(const allocator &alloc)
{
	mpName = alloc.mpName;
	return *this;
}

inline const char* allocator::get_name() const
{
	return mpName;
}

inline void allocator::set_name(const char* pName)
{
	mpName = pName;
}

inline void* allocator::allocate(size_t n, int flags)
{
	return allocate(n, 16, 0, flags);
}

inline void* allocator::allocate(size_t n, size_t alignment, size_t offset, int flags)
{
	UNUSED(offset);
	UNUSED(flags);

	size_t adjustedAlignment = (alignment > PLATFORM_PTR_SIZE) ? alignment : PLATFORM_PTR_SIZE;

	void* p = new char[n + adjustedAlignment + PLATFORM_PTR_SIZE];
	void* pPlusPointerSize = (void*)((uintptr_t)p + PLATFORM_PTR_SIZE);
	void* pAligned = (void*)(((uintptr_t)pPlusPointerSize + adjustedAlignment - 1) & ~(adjustedAlignment - 1));

	void** pStoredPtr = (void**)pAligned - 1;
	*(pStoredPtr) = p;

	return pAligned;
}

inline void allocator::deallocate(void *p, size_t)
{
	if(p != nullptr)
	{
		void* pOriginalAllocation = *((void**)p - 1);
		delete [](char*)pOriginalAllocation;
	}
}

inline bool operator==(const allocator&, const allocator&)
{
	return true;
}

inline bool operator!=(const allocator&, const allocator&)
{
	return false;
}

template <typename Allocator>
inline Allocator* get_default_allocator(const Allocator*)
{
	return NULL;
}

inline rstl::allocator* get_default_allocator(const rstl::allocator*)
{
	return rstl::GetDefaultAllocator();
}

inline void* default_allocfreemethod(size_t n, void* pBuffer, void* /*pContext*/)
{
	rstl::allocator* const pAllocator = rstl::GetDefaultAllocator();

	if(pBuffer)
	{
		CUSTOM_FREE(*pAllocator, pBuffer, n);
		return NULL;
	}
	else
	{
		return CUSTOM_ALLOC(*pAllocator, n);
	}
}

/* This is a memory allocation dispatching function.
 * Make aligned and unaligned specializations.
 * Note that to do this we will need to use a class with a static
 * function instead of a standalone function like below.
 */

template <typename Allocator>
inline void* allocate_memory(Allocator& a, size_t n, size_t alignment, size_t alignmentOffset)
{
	void* result;
	if(alignment <= ALLOCATOR_MIN_ALIGNMENT)
	{
		result = CUSTOM_ALLOC(a, n);
	}
	else
	{
		result = ALLOC_ALIGNED(a, n, alignment, alignmentOffset);
	}
	return result;
}

RSTL_NAMESPACE_END


#endif // RSTL_ALLOCATOR_H
