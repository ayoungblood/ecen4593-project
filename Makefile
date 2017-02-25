# Makefile
# Loosely based on https://stackoverflow.com/questions/1484817/how-do-i-make-a-simple-makefile-for-gcc-on-linux
TARGET = sim
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Wshadow -m64 -std=c11 -Wpointer-arith -masm=intel -save-temps -fverbose-asm -Wstrict-prototypes -Wmissing-prototypes
# -Wall -Wextra -pedantic: stricter warnings
# -Wshadow: warn if a local shadows something else
# -m64: Target x86-64
# -std=c99: ISO/IEC 9899:2011, current C standard
# -save-temps -fverbose-asm -masm=intel: make prettier disassembly
# -Wpointer-arith: warn on silly pointer operations
# --Wstrict-prototypes -Wmissing-prototypes: be strict about function prototypes
LIBS =

.PHONY: clean
.PRECIOUS: $(TARGET) $(OBJECTS)

# Get all the header files and object files
HEADERS = $(wildcard src/*.h)
OBJECTS = $(patsubst %.c, %.o, $(wildcard src/*.c))

# Build all the object files
%.o: %.c $(HEADERS)
		$(CC) $(CFLAGS) -c $< -o $@

# Build the target
$(TARGET): $(OBJECTS)
		$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

all: $(TARGET)

test: $(OBJECTS)
		$(CC) -Wall $(LIBS) -o test/alu-test test/alu-test.c
		test/alu-test

clean:
		-rm -f *.bc *.i *.s
		-rm -f src/*.o
		-rm -f *.gch src/*.gch
		-rm -f $(TARGET)
