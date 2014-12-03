#include "circle.h"
#include "point.cr"
#include "class.cr"
#include <assert.h>
#include <stdio.h>

//Method prototypes
static void *circle_constructor(void *_self, va_list *pa);
static void *circle_destructor(void *_self);
static void *circle_clone(const void *_self);

static void circle_draw(const void *self);

//Class description

struct circle_vtable
{
	struct point_vtable base;
};

struct Circle
{
	struct Point base;
	int rad;
};

static const struct circle_vtable vtable = 
{
	{circle_draw}
};

static const struct Class _Circle =
{
	sizeof(struct Circle),
	circle_constructor,
	circle_destructor,
	circle_clone
};

const void *Circle = &_Circle;

//Class methods

static void *circle_constructor(void *_self, va_list *pa)
{
	struct Circle *self = _self;

	self->base.superClass = Point;
	self->base.vtable = (struct point_vtable *)&vtable;
	self->base.x = va_arg(*pa, int);
	self->base.y = va_arg(*pa, int);
	self->rad = va_arg(*pa, int);

	return self;
}

static void *circle_destructor(void *_self)
{
	return _self;
}

static void *circle_clone(const void *_self)
{
	const struct Circle *self = _self;

	return new(Circle, self->base.x, self->base.y, self->rad);
}

static void circle_draw(const void *_self)
{
	const struct Circle *self = _self;

	printf("Circle: x: %d y: %d Rad: %d\n", self->base.x, self->base.y, self->rad);
}








