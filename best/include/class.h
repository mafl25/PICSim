#ifndef CLASS_H
#define CLASS_H

#include <stdbool.h>

struct Class;

void *new(const void *class, ...);
void delete(void *self);
void *clone(const void *self);
bool equal(const void *self, const void *other);


#endif
