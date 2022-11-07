#ifndef RSTL_ALLOCATOR_H
#define RSTL_ALLOCATOR_H

#pragma once

#include "../internal/config.h"

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


};

RSTL_NAMESPACE_END


#endif //RSTL_ALLOCATOR_H
