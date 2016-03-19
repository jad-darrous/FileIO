#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#ifdef CUSTOM
#include "my_stdio.h"
#endif


int main(int argc, char *argv[]) {

    FILE *f1, *f2;

    if (argc != 4) {
		printf("usage: ./split_test[_custom | _std] src-filename dest-filename split-option\n");
		printf("\t split-option is either\n");
		printf("\t\t 1: char by char\n");
		printf("\t\t 2: word by word \n");
	    exit(1);
	}

	if (!strcmp(argv[1], argv[2])) {
		printf("The src-filename is the same as the dest-filename\n");
	    exit(2);
	}

    f1 = fopen(argv[1], "r");
    if (f1 == NULL) {
		printf("Can not open src-filename\n");
	    exit(3);
	}

    f2 = fopen(argv[2], "w");
    if (f2 == NULL) {
		printf("Can not open dest-filename\n");
	    exit(4);
	}

    fprintf(f2, "Input file: %s\n", argv[1]);

    int param = atoi(argv[3]);
    if (param == 1) {
        char c;
		if (!fscanf(f1, "%c", &c)) {};
        while  (!feof(f1))
        {
            fprintf(f2, "Character %c read, its ASCII code is %d\n", c, c);
            if (!fscanf(f1, "%c", &c)) break;
        }
    } else if (param == 2) {
        char s[22];
        while (!feof(f1))
        {
            if (!fscanf(f1, "%s", s)) break;
            fprintf(f2, "Word %s read, its length is %d\n", s, (int) strlen(s));
        }
    } else {
        exit(5);
    }

    fclose(f1);
    fclose(f2);

    return 0;
}
