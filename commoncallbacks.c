#include "commoncallbacks.h"
#include "datastructure.h"
#include "filedialog.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

gboolean set_saved_cb(GtkWidget *widget, gpointer data)
{
	textStruct *label = (textStruct *)data;
	label->isSaved = TRUE;

	return TRUE;
}

//Tengo que ponerle que si no esta salvado, que no lo cambie
//Creo que aqui hay un bug, revisar que una vez caqmbiado, no este llamando constantemente
//a esta funcion, posiblemente lograr que se reactive solo cuando haya salvado el documento
//Bueno, ya recorde el set Saved, pero revisar luego
gboolean program_changed_cd(GtkWidget *widget, gpointer data)
{
	textStruct *label = (textStruct *)data;
	gchar *name ;

	if(label->isSaved){
		label->isSaved = FALSE;
		if(label->filename == NULL)
			gtk_label_set_text(GTK_LABEL(label->label), UNSAVED_FILE_MOD);
		else{
			name = g_filename_display_basename(label->filename);
			int length = strlen(name);
			length = length + 2;
			gchar *newName = calloc(1, length);
			newName[0] = '*';
			strcat(&newName[1], name);
			gtk_label_set_text(GTK_LABEL(label->label), newName);
			g_free(newName);
			g_free(name);
		}
	}
	
	return TRUE;
}
