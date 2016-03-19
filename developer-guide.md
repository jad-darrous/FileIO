# Developer guide

The library is called *my_stdio*; it consists – like every C library – of two files, the header file and the source file. The header file contains just the declaration of the data structure and the public functions. The implementation can be found in the source file.

## Data structure
The main data structure is a struct called `MY_FILE`, it contains a static array that represents the buffer, the current index of the buffer, the total bytes in the buffer, the total size of the file and the read bytes (Just in reading mode), the status of the file (READ_MODE or WRITE_MODE) and the state of the file (OPENED or CLOSED).

The size of the buffer is specified with MAX_BUFFER_SIZE which is defined when building/compiling the library. We set the buffer size to 4Kb (4096 bytes) which is equal to the page size. We think that this value can be optimized by the hardware for caching and virtual memory.

## Modifiers/Methods
All methods that start with underscore (`_`) are private; they are not declared in the header file.

### Public methods

`MY_FILE *my_fopen(char *name, const char *mode)`
* Create new MY_FILE object by allocating the required memory and initialize it;
	+ In reading mode; make the first read system call to fill the buffer.
	+ In writing mode; create file if it doesn't exist and truncate it if it already exists.
* Returns `NULL` if an error happens; if I can’t open the file or the mode parameter is invalid.

`int my_fclose(MY_FILE *f)`
* This method closes the file and frees its allocated memory. If the file is opened in writing mode, the buffer content is flushed to disk. Returns 0 upon success and -1 otherwise.

`int my_fread(void *p, size_t size, size_t nbelem, MY_FILE *f)`
* Reads the content from the buffer and copies it to the first parameter. First it computes the total amount of bytes (size * nbelem) and check if the required bytes is exist in the current buffer, if so, we don’t need a system call and we can immediately copy bytes from the buffer to the parameter (p). If the request can’t be satisfied with the current bytes of the buffer; the remaining bytes of the buffer is copied, then we do a system call to refill the buffer and copy again the remaining bytes. This operation can be done multiple times if the requested bytes count is greater than the MAX_BUFFER_SIZE.
* Returns the number of read elements that are completely copied to the destination parameter. Zero if there is nothing to read (EOF). -1 otherwise if case of an error or if the file is opened in writing mode or if it is closed.
* We can optimize this function by doing a direct system call without passing through the buffer if the total number of byte (size * nbelem) is greater than twice the size of the buffer. The request byte will be appended to the current content of the buffer and then returned to the user.

`int my_fwrite(void *p, size_t size, size_t nbelem, MY_FILE *f)`
* Write the content of the first parameter into the buffer. If the buffer has (size * nbelem) free byte or more, the element is just copied from the parameter (p) to the buffer, otherwise a system call must be issued to flush the content of the buffer to the disk.
* Returns the number of written elements which are completely copied from the source parameter. -1 otherwise if case of an error or if the file is opened in reading mode or if it is closed.
* We can optimize this function by doing a direct system call without passing through the buffer if the total number of byte (size * nbElem) is greater than twice the size of the buffer. First we write the current content of the buffer, and then we do a second system call to write the content from the first parameter directly to the file.

`int my_fscanf(MY_FILE *f, const char *format, ...)`
* Reads a formatted text from a text file, has the same API of the standard and famous scanf method but with fewer types; char (%c), int (%d) and string (%s). Its implementation is based on the my_fread method; after parsing the “format” string, calls to my_fread is done depending of the type of the parameter.
* When reading an integer we skip the leading spaces, and do so when reading a string. If case of the character we don’t skip any whitespace. We handle the minus sign of the integer.
* Returns the number of read parameters. Zero if there is nothing to read (EOF). -1 otherwise if the parameters type is unknown (different from c, d or s) or in case of an error or if the file is opened in writing mode or if it is closed.

`int my_fprintf(MY_FILE *f, const char *format, ...)`
* Writes a formatted text to a text file, has the same API of the standard and famous printf method but with fewer types; char (%c), int (%d) and string (%s). Its implementation is based on the my_write method; after parsing the “format” string, calls to my_write is done depending of the type of the parameter.
* Returns the number of well written parameters. -1 otherwise if the parameters type is unknown (different from c, d or s) or case of an error or if the file is opened in reading mode or if it is closed.

`int my_feof(MY_FILE *f)`
* Returns the value of the eof field.

`int my_file_flush(MY_FILE *f)`
* Flushes the buffer content to the disk if the file is opened in writing mode.
* Returns the number of flushed bytes. Returns -1 if the file is opened in reading mode.

`int my_file_size(MY_FILE *f)`
* Returns the size of a file, if it is opened in reading mode. If it is opened in writing mode; returns the number of flushed bytes to disk.

### Private Methods
`void _read_file(MY_FILE *f)`
* Performs a system call to fills the buffer.

`void _write_file(MY_FILE *f)`
* Perform a system call to flush the buffer.

`char _get_first_non_empty_char(MY_FILE *f)`
* This is a helper function that is used to skip spaces.
