#include "vstring.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	struct VString *m = new(VString, NULL, 4);

	FILE *fp = fopen("hola.txt", "r");

	get_string_file(m, fp);
	
	print_string(m, true);

	char *sent = "0xfffff";
	int length = utf8_char_number(sent, -1);
	struct VString *h = new(VString, sent, length);

	print_string(h, true);
	printf("%lu\n", string_to_hex(h));

	hex_to_string(h, 0xac341f);
	print_string(h, true);

	FILE *num = fopen("numero.txt", "w");
	write_string_file(h, num);

	delete(m);
	delete(h);
	fclose(fp);
	fclose(num);

	return 0;
}
