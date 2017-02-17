OBJS = print_board.o read_input.o solve_puzzle.o typedefs.o validity_checks.o
CC = g++
CC_FLAGS = -std=c++11

# File names
EXEC = solver
SOURCES = $(wildcard *.cc)
OBJECTS = $(SOURCES:.cc=.o)

# Main target
$(EXEC): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

# To obtain object files
%.o: %.cc
	$(CC) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(OBJECTS)

realclean:
	rm -f $(EXEC) $(OBJECTS)
