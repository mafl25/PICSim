#include "point.cr"
#include "class.cr"
#include <assert.h>
#include <stdio.h>

//Method prototypes
static void *point_constructor(void *_self, va_list *pa);
static void *point_destructor(void *_self);
static void *point_clone(const void *_self);

static void point_draw(const void *self);

//Class description

static struct point_vtable vtable = 
{
	point_draw
};

static const struct Class _Point =
{
	sizeof(struct Point),
	point_constructor,
	point_destructor,
	point_clone
};

const void *Point = &_Point;

//Class methods
static void *point_constructor(void *_self, va_list *pa)
{
	struct Point *self = _self;

	self->superClass = Point;
	self->vtable = &vtable;
	self->x = va_arg(*pa, int);
	self->y = va_arg(*pa, int);
	
	return self;
}

static void *point_destructor(void *_self)
{
	return _self;
}

static void *point_clone(const void *_self)
{
	const struct Point *self = _self;

	return new(Point, self->x, self->y);
}


//Point functions and methods
void draw(const void *_self)
{
	const struct Point *self = _self;

	assert(self->superClass == Point);

	self->vtable->draw(self);
}

static void point_draw(const void *_self)
{
	const struct Point *self = _self;

	printf("Point x: %d y: %d\n", self->x, self->y);
}



