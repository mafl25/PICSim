#ifndef POINT_CR
#define POINT_CR

#include "point.h"

struct point_vtable{
	void (*draw)(const void *self);
};

struct Point{
	const struct Class *class;
	const struct Class *superClass;
	const struct point_vtable *vtable;
	int x;
	int y;
};

#endif
