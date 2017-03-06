CC = g++
CC_FLAGS = -std=c++11

# File names
SOLVER_EXEC = solver
SOLVER_SOURCES = $(filter-out creator_main.cc, $(wildcard *.cc))
SOLVER_OBJECTS = $(SOLVER_SOURCES:.cc=.o)

CREATOR_EXEC = creator
CREATOR_SOURCES = $(filter-out solver_main.cc, $(wildcard *.cc))
CREATOR_OBJECTS = $(CREATOR_SOURCES:.cc=.o)

# Main targets
$(SOLVER_EXEC): $(SOLVER_OBJECTS)
	$(CC) $(SOLVER_OBJECTS) -o $(SOLVER_EXEC)

$(CREATOR_EXEC): $(CREATOR_OBJECTS)
	$(CC) $(CREATOR_OBJECTS) -o $(CREATOR_EXEC)

# To obtain object files
%.o: %.cc
	$(CC) -c $(CC_FLAGS) $< -o $@

# To remove generated files
clean:
	rm -f $(SOLVER_OBJECTS) $(CREATOR_OBJECTS)

realclean:
	rm -f $(SOLVER_EXEC) $(SOLVER_OBJECTS) $(CREATOR_EXEC) $(CREATOR_OBJECTS)
