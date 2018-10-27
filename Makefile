CC = gcc
CFLAGS = -c -g -Wextra -Wall -Wvla -DNDEBUG -std=c99
LDFLAGS = -g

TAR_FILES = Makefile SudokuTree.c SudokuTree.h SudokuSolver.c GenericDFS.c extension.pdf

# All Target
all: libGenericDFS.a SudokuSolver

# Executables
libGenericDFS.a: GenericDFS.o
	ar rcs $@ $^

SudokuSolver: SudokuSolver.o SudokuTree.o libGenericDFS.a -lm
	$(CC) $(LDFLAGS) $^ -o $@

# Object Files
GenericDFS.o: GenericDFS.c GenericDFS.h
	$(CC) $(CFLAGS) $< -o $@

SudokuTree.o: SudokuTree.c SudokuTree.h
	$(CC) $(CFLAGS) $< -o $@

SudokuSolver.o: SudokuSolver.c SudokuTree.h
	$(CC) $(CFLAGS) $< -o $@

# Other Targets
tar: $(TAR_FILES)
	tar -cf ex3.tar $^

clean:
	rm -f *.o SudokuSolver libGenericDFS.a ex3.tar

# Phony
.PHONY: all clean tar