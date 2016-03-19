#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "my_stdio.h"


int main(int argc, char *argv[]) {

    if (argc != 2) {
		printf("usage: ./main filename\n");
	    exit(1);
	}

    MY_FILE *fr;
	MY_FILE *fw;

    int x1, x2, y;
    char ch;
    char s1[100]={},s2[100]={};


    fw = my_fopen(argv[1], "w");
    my_fprintf(fw, "This %c%c my %dst try: %s .\n", 'i', 's', 1, "Hello World!!");
    my_fprintf(fw, "Mathematical excercise\n");
    my_fprintf(fw, "for the %c %s\n", '+', "operation");
    my_fprintf(fw, "\t%d+%d= %d\n", -99, 77, 77-99);
	my_fwrite(argv[1], 1, strlen(argv[1]), fw);
    my_fprintf(fw, "\n");
    my_fclose(fw);


    fr = my_fopen(argv[1], "r");

	my_fread(s1, sizeof(char), 36, fr);
    printf("Preamble: %s\n", s1);

    my_fscanf(fr, "%s", s1); // "Mathematical"
    my_fscanf(fr, "%s", s2); // "excercise"
	printf("The title is [%s %s]\n", s1, s2);

    my_fscanf(fr, "%s", s1); // "for"
    my_fscanf(fr, "%s", s2); // "the"
    my_fscanf(fr, "%c", &ch); // ' '
    my_fscanf(fr, "%c", &ch); // '+'
    printf("The operation is '[%c]'\n", ch);

    my_fscanf(fr, "%s", s2); // "operation"
	ch = 'm';
    my_fscanf(fr, "%d", &x1);
    my_fscanf(fr, "%c", &ch); // '+'
    my_fscanf(fr, "%d", &x2);
    my_fscanf(fr, "%c", &ch); // '='
    my_fscanf(fr, "%d", &y);
    printf("x1=[%d] x2=[%d] y=[%d]\n", x1, x2, y);

    my_fclose(fr);

    return 0;
}
