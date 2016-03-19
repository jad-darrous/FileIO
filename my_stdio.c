#include "my_stdio.h"

#include <fcntl.h>
#include <sys/stat.h>

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>



/* Flags for the opennig mode */
#define READ_MODE 1
#define WRITE_MODE 2


/* Flags for the current state of the file */
#define OPENNED 1
#define CLOSED 2


/* We have to extern these functions to get rid of
   the "implicit function declaration warning" */
extern int read(int,void*,int);
extern int write(int,void*,int);
extern int close(int);
extern int isdigit(int);
extern int isspace(int);



/* The file data structure */
typedef struct _my_file {
    /* The buffer to cache file's bytes */
    char* buffer[MAX_BUFFER_SIZE];
    /* file descriptor, for the file API */
    int fd;
    /* The current reading index - used only in READ mode */
    int buf_p;
    /* The count of bytes into the buffer */
    int buf_len;
    /* The count of read bytes till now - used only in READ mode */
    int _size;
    /* the total size (#bytes) of the file */
    int size;
    /* READ_MODE / WRITE_MODE */
    int mode;
    /* OPENNED / CLOSED */
    int stat;
    /* if an end-of-ﬁle has been encountered during a previous read */
    int eof;
} MY_FILE;


/* A system call to read from the file */
void _read_file(MY_FILE *f) {
    f->buf_len = read(f->fd, f->buffer, MAX_BUFFER_SIZE);
    f->buf_p = 0;
}

/* A system call to write to the file */
void _write_file(MY_FILE *f) {
    write(f->fd, (char*)f->buffer, f->buf_p);
    f->size += f->buf_p;
    f->buf_p = 0;
}

/* pretty print - for debugging
void __pp_file(MY_FILE *f) {
    printf("MY_FILE: %p  fd=%d mode=%d size=%d buf_p=%d buf_len=%d\n",
        f, f->fd, f->mode, f->size, f->buf_p, f->buf_len);
}
*/

/* Utils */
char* _itoa(int i);

/*
    Create a new MY_FILE object and init it:
	- In reading mode; make the first read syscall
	- In writing mode; create file if doesn't exist
    Returns NULL if an error happens.
*/
MY_FILE *my_fopen(char *name, const char *mode) {
    MY_FILE *f = (MY_FILE *) malloc(sizeof(MY_FILE));

    if (!memcmp(mode, "r", 1)) {
        f->mode = READ_MODE;
        if ((f->fd = open(name, O_RDONLY, S_IRWXU)) == -1) {
            return NULL;
		}
        _read_file(f);
        struct stat buf;
        fstat(f->fd, &buf);
        f->size = buf.st_size;
        f->_size = 0;

    } else if (!memcmp(mode, "w", 1)) {
        f->mode = WRITE_MODE;
        if ((f->fd = open(name, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU)) == -1) {
            return NULL;
		}
        f->size = f->buf_p = 0;

    } else {
        fprintf(stderr, "openning mode err\n");
        return NULL;
    }
    f->eof = 0;
    f->stat = OPENNED;
    return f;
}

/* Close the file and free the allocated MY_FILE struct */
int my_fclose(MY_FILE *f) {
    if (f->stat == CLOSED) {
        return -1;
    }
    if (f->mode == WRITE_MODE) {
        _write_file(f);
    }
    f->stat = CLOSED;
    int fd = f->fd;
    free(f);
    return close(fd);
}

int my_fread(void *p, size_t size, size_t nbelem, MY_FILE *f) {
	//fprintf(stderr, "my_fread f->_size %d %d\n", f->_size, f->size);
    if (f->stat == CLOSED || f->mode == WRITE_MODE) {
        return -1;
    }
    if (f->size == f->_size) {
        f->eof = 1;
    }
    if (my_feof(f)) {
		return 0;
	}
	/* total bytes to read */
    int n = size * nbelem;
	/* the current read index in the user buffer */
	int p_ind = 0;
    while (f->buf_p + n > f->buf_len) {
		/* nb of bytes that are buffered but not yet copied to the user buffer */
        int rem_buf = f->buf_len - f->buf_p;
		if (rem_buf) {
        	// if (f->size == f->_size) return p_ind / size;
	        // _read_file(f);

//			fprintf(stderr, "rem_buf %d\n", rem_buf);
	        memcpy((char*)p + p_ind, (char*) f->buffer + f->buf_p, rem_buf);
	        f->buf_p += rem_buf;
	        f->_size += rem_buf;
	        p_ind += rem_buf;
//			fprintf(stderr, "p_ind %d\n", p_ind);

	        if (f->size == f->_size) return p_ind / size;
		}
        _read_file(f);
        if (f->buf_len == -1) {
			return -1;
		}
        if (f->buf_len == 0) {
            f->eof = 1;
            return p_ind / size;
        }
        n -= rem_buf;
    }
    memcpy((char*)p + p_ind, (char*) f->buffer + f->buf_p, n);
    f->buf_p += n;
    f->_size += n;
    return nbelem;
}

int my_fwrite(void *p, size_t size, size_t nbelem, MY_FILE *f) {
    if (f->stat == CLOSED || f->mode == READ_MODE) {
        return -1;
    }
	/* total bytes to write */
    int n = size * nbelem;
	/* the current write index in the user buffer */
	int p_ind = 0;
    while (f->buf_p + n > MAX_BUFFER_SIZE) {
        int empty_buf = MAX_BUFFER_SIZE - f->buf_p;
        memcpy((char*)f->buffer + f->buf_p, (char*)p + p_ind, empty_buf);
        f->buf_p += empty_buf;
        _write_file(f);
        n -= empty_buf;
        p_ind += empty_buf;
    }
    memcpy((char*)f->buffer + f->buf_p, (char*)p + p_ind, n);
    f->buf_p += n;
    return nbelem;
}


void _reput_char(MY_FILE *f, char c) {
    ((char*)f->buffer)[--f->buf_p] = c;
    f->_size--;
}

char _get_first_non_empty_char(MY_FILE *f) {
    int result;
    char chr;
    while ((result = my_fread(&chr, sizeof(char), 1, f)) == 1 && isspace(chr));
    return (result == 1 ? chr : 0);
}


/* formated read - std scanf style */
int my_fscanf(MY_FILE *f, const char *format, ...) {
    if (f->stat == CLOSED || f->mode == WRITE_MODE) {
        return -1;
    }
    if (my_feof(f)) {
		return 0;
	}
    /* the number of arguments successfully written */
    int c = 0;
    /* the list holing the args */
    va_list arguments;
    /* Initializing arguments to store all values after format */
    va_start(arguments, format);
    /* Sum all the inputs; we still rely on the function caller to tell us how
     * many there are */
    const char *str = format;
    while (*str) {
        if (*str == '%') {
            str++;
            char tt = *str, chr;
            if (tt == 'c') {
                void* ch = va_arg(arguments, void*);
                if (my_fread(ch, sizeof(char), 1, f))
                    c++;
            } else if (tt == 'd') {
                int* x = va_arg(arguments, int*);
                chr = _get_first_non_empty_char(f);
                if (chr == 0) return -1;
                char num[12] = {chr};
                int num_p = 1;
                while (my_fread(&chr, sizeof(char), 1, f) == 1 && isdigit(chr) && num_p < 12) {
                    num[num_p++] = chr;
                }
                _reput_char(f, chr);
                *x = atoi(num);
                c++;
            } else if (tt == 's') {
                char* ss = va_arg(arguments, char*);
                char* p = ss;
                *p = _get_first_non_empty_char(f);
                p++;
                while (my_fread(&chr, sizeof(char), 1, f) == 1 && !isspace(chr)) {
                    *p = chr;
                    p++;
                }
                /* append the NULL character to the string */
                *p = 0;
                _reput_char(f, chr);
                c++;
            } else {
                return -1;
            }
        } else {
        }
        str++;
    }
    va_end(arguments);
    return c;
}

/* formated write - std printf style */
int my_fprintf(MY_FILE *f, const char *format, ...) {
    if (f->stat == CLOSED || f->mode == READ_MODE) {
        return -1;
    }
    /* the number of arguments successfully written */
    int c = 0;
    /* the list holing the args */
    va_list arguments;
    /* Initializing arguments to store all values after format */
    va_start(arguments, format);
    /* Sum all the inputs; we still rely on the function caller to tell us how
     * many there are */
    const char *str = &format[0];
    while (*str) {
        if (*str == '%') {
            str++;
            char tt = *str;
            if (tt == 'c') {
                char ch = va_arg(arguments, int);
                if (my_fwrite(&ch, sizeof(char), 1, f))
                    c++;
            } else if (tt == 'd') {
                int x = va_arg(arguments, int);
                char* bb = _itoa(x);
                if (my_fwrite(bb, strlen(bb), 1, f))
                    c++;
            } else if (tt == 's') {
                char* ss = va_arg(arguments, char*);
                if (my_fwrite(ss, strlen(ss), 1, f))
                    c++;
            } else {
                return -1;
            }
        } else {
            char ch = *str;
            my_fwrite(&ch, sizeof(char), 1, f);
        }
        str++;
    }
    va_end(arguments);
    return c;
}

/* is end of file (reading mode) */
int my_feof(MY_FILE *f) {
    return f->eof;
}

/* Flush the buffer content to the disk (writing mode) */
int my_file_flush(MY_FILE *f) {
    if (f->stat == CLOSED || f->mode == READ_MODE) {
        return -1;
    }
    int bytes = f->buf_len;
    _write_file(f);
    return bytes;
}

/* Returns the total size of a file if openned in reading mode
   Or the number of flushed bytes to disk of a file if openned
   in writing mode */
int my_file_size(MY_FILE *f) {
    return f->size;
}


/* Utils */

/* Convert an Integer to a list of char */
char* _itoa(int i) {
    char* b = malloc(12);
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}
