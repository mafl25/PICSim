#ifndef CLASS_CR
#define CLASS_CR

#include "class.h"
#include "stdlib.h"
#include "stdarg.h"

#define NULL_RETURN(a) if ((a) == NULL) return NULL;

struct Class
{
	size_t size;
	void *(*constructor)(void *self, va_list *app);
	void *(*destructor)(void *self);
	void *(*clone)(const void *self);
	bool (*equal)(const void *self, const void *other);
};

#endif
