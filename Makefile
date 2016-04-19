# Usage:
# Run 'make test' to execute the test program.
# Run 'make valgrind' to run the test program in Valgrind.
# Run 'make clean' to remove compiled files.

#LIB source files.
LIBSRC =  prefs.c preftest.c

LIBOBJ = $(LIBSRC:.c=.o)

LIBOUT = cfgrdwr

#APP source files.
APPSRC =  preftest.c
APPOBJ = $(APPSRC:.c=.o)
APPOUT = preftest

# include directories
INCLUDES = -I. -I/usr/local/include

# C compiler flags (-g -O2 -Wall)
CCFLAGS = -g -Wall -Wextra -std=c99 -D_XOPEN_SOURCE=500

# compiler
CCC = gcc

.SUFFIXES: .c

default: $(LIBOUT) $(APPOUT)

.c.o:
	$(CCC) $(INCLUDES) $(CCFLAGS) -c $< -o $@

$(LIBOUT): $(LIBOBJ)
	ar rcs lib$(LIBOUT).a $(LIBOBJ)

$(APPOUT): $(APPOBJ)
	$(CCC) $(CFLAGS) -static $(APPOBJ) -L. -l$(LIBOUT)  -o $(APPOUT)
clean:
	rm -f $(APPOBJ) $(APPOUT) $(LIBOBJ) lib$(LIBOUT).a

valgrind:
	valgrind --tool=memcheck ./$(APPOUT)
