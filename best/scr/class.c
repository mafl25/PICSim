#include "class.cr"
#include <stdlib.h>
#include <assert.h>

void *new(const void *_class, ...)
{
	const struct Class *class = _class;	
	void *p = malloc(class->size);
	NULL_RETURN(p)

	*(const struct Class **)p = class;
	if (class->destructor){
			va_list ap;
			va_start(ap, _class);
			p = class->constructor(p, &ap);
			va_end(ap);
	}
	return p;
}

void delete(void *self)
{
	const struct Class **p = self;

	if (self && *p && (*p)->destructor) 
		self = (*p)->destructor(self);
	free(self);
}

void *clone(const void *self)
{
	const struct Class * const *p = self;
	
	assert(self && *p && (*p)->clone);

	return (*p)->clone(self);	
}

bool equal(const void *_self, const void *_other)
{
	assert(_self && _other);

	const struct Class * const *self = _self;
	const struct Class * const *other = _other;

	if (self == other)
		return true;

	if ((*self) != (*other))
		return false;

	return (*self)->equal(self, other);
}








