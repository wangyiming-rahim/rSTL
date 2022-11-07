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

#ifndef CUSTOM_FREE
#  define CUSTOM_FREE(allocator, p, size) (allocator).deallocate((void*)(p), (size))
#endif

#ifndef DEFAULT_NAME_PREFIX
#  define DEFAULT_NAME_PREFIX "rstl"
#endif

#ifndef ALLOCATOR_DEFAULT_NAME
#  define ALLOCATOR_DEFAULT_NAME DEFAULT_NAME_PREFIX
#endif


#endif // RSTL_CONFIG_H
