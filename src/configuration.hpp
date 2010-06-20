#pragma once

// enable overloaded new and delete to count allocated memory
#define OZ_ALLOC_STATISTICS

// enable PoolAlloc allocator; otherwise use a dummy class that calls malloc/free
#define OZ_POOL_ALLOC

// enable GNU math builtins
#define OZ_GNU_MATH

// whether math library has sincosf function
#define OZ_HAVE_SINCOSF

// enable warnings about missing and unused variables in config files
#define OZ_VERBOSE_CONFIG

// enable XML configuration files support
/* #undef OZ_XML_CONFIG */

// platform
/* #undef OZ_MINGW */
/* #undef OZ_MSVC */

#ifdef __GNUG__
# define OZ_SIZE_T __SIZE_TYPE__
#else
# define OZ_SIZE_T unsigned int
#endif

#ifdef OZ_MSVC
# define _CRT_SECURE_NO_WARNINGS 1
# define snprintf sprintf_s
# define chdir _chdir
#endif
