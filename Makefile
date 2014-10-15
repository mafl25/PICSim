CC = gcc
CFLAGS = -Wall -g $(shell pkg-config --cflags gtk+-3.0 gtksourceview-3.0)
LIBS = $(shell pkg-config --libs gtk+-3.0 gtksourceview-3.0)
SOURCES = main.c outputbuffer.c filedialog.c filemanager.c commoncallbacks.c settings.c customstring.c datastructure.c menubar.c toolbar.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = program.out

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(OBJECTS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *o 
	rm -f $(EXECUTABLE)
