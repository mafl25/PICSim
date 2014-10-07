CC = gcc
CFLAGS = -Wall -g $(shell pkg-config --cflags gtk+-3.0)
LIBS = $(shell pkg-config --libs gtk+-3.0)
SOURCES = outputbuffer.c filedialog.c filemanager.c main.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = program

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(OBJECTS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *o 
	rm -f $(EXECUTABLE)
