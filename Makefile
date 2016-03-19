CC=gcc

WARNINGS = -Wall -Wshadow -Wcast-align -Waggregate-return -Wstrict-prototypes \
			-Wredundant-decls -Wnested-externs -Wpointer-arith -Wwrite-strings \
			-finline-functions -Wextra -Werror

CFLAGS = -g -O2 $(WARNINGS)



main: main.c my_stdio.o
	$(CC) $(CFLAGS) -o $@ $^

main_static: main.c my_stdio.a
	$(CC) $(CFLAGS) -o $@ -L. $^

main_dynamic: main.c my_stdio.so
	$(CC) $(CFLAGS) -L. -Wl,-rpath=. -o $@ $< -lmy_stdio



my_stdio.o: my_stdio.c
	# $(CC) -DMAX_BUFFER_SIZE=65536 $(CFLAGS) -fpic -c $<
	# $(CC) -DMAX_BUFFER_SIZE=4096 $(CFLAGS) -fpic -c $<
	$(CC) -DMAX_BUFFER_SIZE=16 $(CFLAGS) -fpic -c $<

my_stdio.a: my_stdio.o
	ar q $@ $<

my_stdio.so: my_stdio.o
	$(CC) -shared -o lib$@ $<



copy_test_custom: copy_test.c my_stdio.o
	$(CC) $(CFLAGS) -DCUSTOM $^ -o $@

copy_test_std: copy_test.c
	$(CC) $(CFLAGS) $^ -o $@

test_copy: EinsteinQuotes.txt copy_test_custom copy_test_std
	./copy_test_custom $< EQ-copy-custom.out 3
	./copy_test_std    $< EQ-copy-std.out    3
	@if diff EQ-copy-custom.out EQ-copy-std.out >/dev/null; then \
	  echo "**** Test $@ Passed *****"; \
	else \
	  echo "**** Test $@ FAILED *****"; \
	  diff -y $^ ;\
	fi


split_test_custom: split_test.c my_stdio.o
	$(CC) $(CFLAGS) -DCUSTOM $^ -o $@

split_test_std: split_test.c
	$(CC) $(CFLAGS) $^ -o $@

test_split: EinsteinQuotes.txt split_test_custom split_test_std
	./split_test_custom $< EQ-split-custom.out 2
	./split_test_std    $< EQ-split-std.out    2
	@if diff EQ-split-custom.out EQ-split-std.out >/dev/null; then \
	  echo "**** Test $@ Passed *****"; \
	else \
	  echo "**** Test $@ FAILED *****"; \
	  diff -y $^ ;\
	fi


tests: test_copy test_split


clean:
	rm -f *~ *.a *.o *.so *.out main main_* *_test_*


# http://gd.tuwien.ac.at/languages/c/programming-bbrown/c_075.htm
