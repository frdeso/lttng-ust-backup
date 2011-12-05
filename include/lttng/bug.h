#ifndef _LTTNG_BUG_H
#define _LTTNG_BUG_H

/*
 * lttng/bug.h
 *
 * (C) Copyright 2010-2011 - Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY EXPRESSED
 * OR IMPLIED.  ANY USE IS AT YOUR OWN RISK.
 *
 * Permission is hereby granted to use or copy this program
 * for any purpose,  provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 */

#include <urcu/compiler.h>
#include <stdio.h>
#include <stdlib.h>

#define LTTNG_BUG_ON(condition)						\
	do {								\
		if (caa_unlikely(condition)) {				\
			fprintf(stderr,					\
				"LTTng BUG in file %s, line %d.\n",	\
				__FILE__, __LINE__);			\
			exit(EXIT_FAILURE);				\
		}							\
	} while (0)

#define LTTNG_BUILD_BUG_ON(condition)					\
	((void) sizeof(char[-!!(condition)]))

/**
 * LTTNG_BUILD_RUNTIME_BUG_ON - check condition at build (if constant) or runtime
 * @condition: the condition which should be false.
 *
 * If the condition is a constant and true, the compiler will generate a build
 * error. If the condition is not constant, a BUG will be triggered at runtime
 * if the condition is ever true. If the condition is constant and false, no
 * code is emitted.
 */
#define LTTNG_BUILD_RUNTIME_BUG_ON(condition)			\
	do {							\
		if (__builtin_constant_p(condition))		\
			LTTNG_BUILD_BUG_ON(condition);		\
		else						\
			LTTNG_BUG_ON(condition);		\
	} while (0)

#endif
