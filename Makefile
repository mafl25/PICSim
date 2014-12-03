CC = gcc

SCR_DIR = scr/
SCR = main.c outputbuffer.c filedialog.c filemanager.c commoncallbacks.c \
	  settings.c customstring.c datastructure.c menubar.c toolbar.c variabletreeview.c

VECTOR_DIR = best/scr/
VECTOR_SCR = vstring.c class.c vector.c 

CFLAGS = -Wall -g -Werror $(shell pkg-config --cflags gtk+-3.0 gtksourceview-3.0)
CPPFLAGS = -I./include -I./best/include
LIBS = $(shell pkg-config --libs gtk+-3.0 gtksourceview-3.0)

OBJECTS = $(patsubst %.c, $(SCR_DIR)obj/%.o, $(SCR)) \
		  $(patsubst %.c, $(VECTOR_DIR)obj/%.o, $(VECTOR_SCR)) 

EXECUTABLE = bin/program.out

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(OBJECTS) $(LIBS)

$(SCR_DIR)obj/%.o: $(SCR_DIR)%.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(VECTOR_DIR)obj/%.o: $(VECTOR_DIR)%.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) 
	rm -f $(EXECUTABLE)
