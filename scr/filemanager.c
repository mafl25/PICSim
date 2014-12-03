#include "filemanager.h"
#include "outputbuffer.h"
#include <gtk/gtk.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

//This function closes a file. It checks if the file pointer is a NULL or if it is closed already
//in which case it will do nothing and will return FALSE. If the file pointer is valid and the file
//is indeed open, it will close it. If the closing was successful, it will return TRUE and set isOpen to
//FALSE. If it fails, it will print a message to the output buffer and return FALSE, leaving isOpen as true.
//Eventually, add truth table
gboolean fileClose(FILE **file, const gchar *filename, gboolean print_output)
{
	gboolean returnValue = FALSE;

	if(*file != NULL){
		int output = fclose(*file);
		if(output == EOF){
			if(print_output){
				output_print("Error: File \"", FALSE);
				if(filename != NULL)
					output_print(filename, FALSE);
				output_print("\" could not be closed.", TRUE);
			}
		}else{
			*file = NULL;
			returnValue = TRUE;
		}
	}

	return returnValue;
}

gboolean fileOpen(FILE **file, const gchar *filename, gboolean print_output)
{
	gboolean returnValue = FALSE;

	if(filename != NULL){
		*file = fopen(filename, "r");
		if(*file == NULL){
			if(print_output){
				char *error = strerror(errno);
				output_print("Error opening file \"", FALSE);
				output_print(filename, FALSE);
				output_print("\": ", FALSE);
				output_print(error, TRUE);
			}
		}else{
			returnValue = TRUE;
		}
	}else{
		*file = NULL;
		if(print_output)
			output_print("Error opening file: No filename provided.", TRUE);
	}

	return returnValue;
}

gboolean fileWrite(FILE **file, const gchar *filename, gboolean print_output)
{
	gboolean returnValue = FALSE;

	if(filename != NULL){
		*file = fopen(filename, "w");
		if(*file == NULL){
			if(print_output){
				char *error = strerror(errno);
				output_print("Error writting file \"", FALSE);
				output_print(filename, FALSE);
				output_print("\": ", FALSE);
				output_print(error, TRUE);
			}
		}else{
			returnValue = TRUE;
		}
	}else{
		*file = NULL;
		if(print_output)
			output_print("Error writting file: No filename provided.", TRUE);
	}

	return returnValue;
}

off_t fileSize(const gchar *filename)
{
	struct stat file;

	if(stat(filename, &file) == 0)
		return file.st_size;

	return -1;
}
