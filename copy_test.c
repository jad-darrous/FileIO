#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#ifdef CUSTOM
#include "my_stdio.h"
#endif


void _cpy_(FILE *f1, FILE *f2, void *p, size_t size, size_t nbelem) {

    int result;

    result = fread(p, size, nbelem, f1);

    while (result > 0) {
        result = fwrite(p, size, result, f2);
        if (result == -1)
            exit(-4);
        result = fread(p, size, nbelem, f1);
		// printf("\t result %d\n", result);
    }
    if (result == -1)
        exit(-5);
}

void cpy_one_char(FILE *f1, FILE *f2) {

    char c;
    _cpy_(f1, f2, &c, 1, 1);
}

void cpy_char_arr(FILE *f1, FILE *f2) {

    size_t nbelem = 1024;
    char c[1024];
   _cpy_(f1, f2, c, 1, nbelem);
}

void cpy_int_arr(FILE *f1, FILE *f2) {

    size_t nbelem = 10;
    int* T = malloc(nbelem * sizeof(int));
    _cpy_(f1, f2, T, sizeof(int), nbelem);
}


int main(int argc, char *argv[]) {

    FILE *f1;
    FILE *f2;

    if (argc != 4) {
		printf("usage: ./copy_test[_custom | _std] src-filename dest-filename copy-option\n");
		printf("\t copy-option is either\n");
		printf("\t\t 1: copy one char at time\n");
		printf("\t\t 2: copy 10 int at time\n");
		printf("\t\t 3: copy 1024 char at time\n");
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

    int param = atoi(argv[3]);
    if (param == 1) {
        cpy_one_char(f1, f2);
    } else if (param == 2) {
        cpy_int_arr(f1, f2);
    } else if (param == 3) {
        cpy_char_arr(f1, f2);
    } else {
        exit(5);
    }

    fclose(f1);
    fclose(f2);

    return 0;
}
