CC = gcc
DEBUG ?= 1
ifeq ($(DEBUG),1)
	CFLAGS = -Wall -g -Werror $(shell pkg-config --cflags glib-2.0) 
else
	CFLAGS = -DNDEBUG -O3 -Wall -Werror $(shell pkg-config --cflags glib-2.0) 
endif
LIBS = $(shell pkg-config --libs glib-2.0)
SOURCES = main.c class.c vector.c point.c circle.c vstring.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(OBJECTS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *o 
	rm -f $(EXECUTABLE)
