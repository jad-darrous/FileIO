#ifndef		_MY_STDIO_H_
#define		_MY_STDIO_H_

#include <stddef.h>

typedef struct _my_file MY_FILE;

MY_FILE *my_fopen(char *name, const char *mode);
int my_fclose(MY_FILE *f);
int my_fread(void *p, size_t size, size_t nbelem, MY_FILE *f);
int my_fwrite(void *p, size_t size, size_t nbelem, MY_FILE *f);

int my_fprintf(MY_FILE *f, const char *format, ...);
int my_fscanf(MY_FILE *f, const char *format, ...);

int my_feof(MY_FILE *f);
int my_file_flush(MY_FILE *f);
int my_file_size(MY_FILE *f);


#ifdef CUSTOM
#define FILE MY_FILE
#define fopen my_fopen
#define fclose my_fclose
#define fread my_fread
#define fwrite my_fwrite
#define feof my_feof
#define fscanf my_fscanf
#define fprintf my_fprintf
#endif

#endif
