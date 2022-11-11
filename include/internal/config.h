/*------------------------------------------------------------------------------------
 * RSTL_VERSION
 * RSTL_NAMESPACE_BEGIN
 * RSTL_NAMESPACE_END
 *------------------------------------------------------------------------------------*/
#ifndef RSTL_CONFIG_H
#define RSTL_CONFIG_H

#ifndef RSTL_VERSION
#   define RSTL_VERSION "1.0.0"
#   define RSTL_VERSION_N 100
#endif

#define RSTL_NAMESPACE_BEGIN namespace rstl{
#define RSTL_NAMESPACE_END }

#ifndef CUSTOM_ALLOC
#  define CUSTOM_ALLOC(allocator, n) (allocator).allocate(n);
#endif

#ifndef CUSTOM_FREE
#  define CUSTOM_FREE(allocator, p, size) (allocator).deallocate((void*)(p), (size))
#endif

#ifndef ALLOC_ALIGNED
#  define ALLOC_ALIGNED(allocator,  n, alignment, offset) (allocator).allocate((n), (alignment), (offset))
#endif

#ifndef DEFAULT_NAME_PREFIX
#  define DEFAULT_NAME_PREFIX "rstl"
#endif

#ifndef ALLOCATOR_DEFAULT_NAME
#  define ALLOCATOR_DEFAULT_NAME DEFAULT_NAME_PREFIX
#endif

#ifndef ALLOCATOR_MIN_ALIGNMENT
#  define ALLOCATOR_MIN_ALIGNMENT 8
#endif

#ifndef PLATFORM_PTR_SIZE
#  define PLATFORM_PTR_SIZE 8
#endif

#ifndef UNUSED
#  define UNUSED(x) (void)(x)
#endif


#endif // RSTL_CONFIG_H
