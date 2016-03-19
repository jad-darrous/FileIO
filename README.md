# FileIO
A C wrapper to do buffered IO operations on files.

In this project I implemented a library to deal with binary and text files in C. All the read and write requests are **buffered**. This means that the library uses a buffer to minimize the *system calls* that will in turn increase the overall performance.

The goal is not to build the fastest, most memory efficient or the best multi-threaded file-io library. The goal is just to have fun and understand more about the GNU Libc File-IO API.

## Architecture

To read more in details about the API and the internal implementation please take a look on the [Developer guide](developer-guide.md)

## Compile and Build

We provide a MakeFile in order to build the library as an object file, a static and a shared dynamic object.

The library is compiled with the flag "-fpic" (Position Independent Code) so that the generated address can be mapped later.

This tutorial provides a nice explanation on dynamic libraries in C-language:
http://www.cprogramming.com/tutorial/shared-libraries-linux-gcc.html

### Static vs Dynamic (Shared) library

When using a static library, the library is included in final application binary. This coupling complicates the software development because the application should be recompiled if any library have to be changed.
Shared libraries solve the problem, so the library could be replaced without recompiling the main program.

To see the difference in the binary size and the effect of including a static library, run the following command

```
make main main_static main_dynamic
ls -la main main_* my_stdio.a my_stdio.o libmy_stdio.so | awk '{ print $1, $5, $9}'
```

> Please note that Loadable libraries, sometime called Dynamic, is a different thing; Loadable libraries can be loaded and closed while the program is running.

## Run and Test

In addition to the `main.c` which presents an example of the library API, two tests are provided:
* `copy_test`: copy a file, similar to `cp` program, but with additional argument that specify the block size.
* `split_test`: reads a text file and split it char by char or word by word.

To build and run the two tests, open the terminal and type
```
make tests
```

To build and run the main file
```
make main
./main temp-file
cat temp-file
```

> please note that these tests are not exhaustive and do not cover all the execution paths. They are used just for demonstration.
