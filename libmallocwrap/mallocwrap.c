#define _GNU_SOURCE
#include <dlfcn.h>
#include <sys/types.h>
#include <stdio.h>

#include "marker.h"

//INTERCEPT_PROTOTYPE(void, malloc, size_t size)
//INTERCEPT_TRACE("size %d", size)
//INTERCEPT_CALL_ARGS(size)
//INTERCEPT()
//
//#define INTERCEPT_FUNC(type, name, args...) 						\
//__I_FUNC_TYPE(type)									\
//__I_FUNC_NAME(name)									\
//__I_FUNC_ARGS(args)
//
//#define INTERCEPT_TRACE(fmt, args...)							\
//#define __I_TRACE_FMT fmt								\
//#define __I_TRACE_ARGS args
//
//#define INTERCEPT_CALL_ARGS(args...)							\
//#define __I_CALL_ARGS args
//
//#define INTERCEPT()									\
//__I_FUNC_TYPE __I_FUNC_NAME(__I_FUNC_ARGS)						\
//{											\
//	static __I_FUNC_TYPE (*plibc_ ## __I_FUNC_NAME)(args) = NULL;			\
//											\
//	if(plibc_ ## __I_FUNC_NAME == NULL) {						\
//		plibc_ ## __I_FUNC_NAME = dlsym(RTLD_NEXT, "malloc");			\
//		if(plibc_ ## __I_FUNC_NAME == NULL) {					\
//			fprintf(stderr, "mallocwrap: unable to find malloc\n");		\
//			return NULL;							\
//		}									\
//	}										\
//											\
//	trace_mark(ust, __I_FUNC_NAME, __I_TRACE_FMT, __I_TRACE_ARGS);			\
//											\
//	return plibc_ ## __I_FUNC_NAME (__I_CALL_ARGS);					\
//}											

void *malloc(size_t size)
{
	static void *(*plibc_malloc)(size_t size) = NULL;

	void *retval;

	if(plibc_malloc == NULL) {
		plibc_malloc = dlsym(RTLD_NEXT, "malloc");
		if(plibc_malloc == NULL) {
			fprintf(stderr, "mallocwrap: unable to find malloc\n");
			return NULL;
		}
	}

	retval = plibc_malloc(size);

	trace_mark(ust, malloc, "size %d ptr %p", (int)size, retval);

	return retval;
}

void free(void *ptr)
{
	static void *(*plibc_free)(void *ptr) = NULL;

	if(plibc_free == NULL) {
		plibc_free = dlsym(RTLD_NEXT, "free");
		if(plibc_free == NULL) {
			fprintf(stderr, "mallocwrap: unable to find free\n");
			return NULL;
		}
	}

	trace_mark(ust, free, "%p", ptr);

	return plibc_free(ptr);
}

MARKER_LIB