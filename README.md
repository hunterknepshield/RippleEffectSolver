# Ripple Effect Solver & Creator

## Ripple Effect
Ripple effect is a Nikoli puzzle, see
http://nikoli.com/en/puzzles/ripple_effect/ for more info.

A board is divided into rooms of contiguous cells. Each room's must be filled
with a number from 1 to n, where n is the number of cells in the room. Cells
reachable in a cardinal direction from each other must either contain different
values, or must have at least that many spaces between them if they contain the
same value.

## Solving Strategies
The solver first iterates over the board's rooms, filling in any cell it can
guarantee to be correct. To do this, it inspects the missing values for a given
room, then takes two main actions:
- The solver tries to find any cell in the room that can only fit one value and
fills that cell in with said value.
- The solver tries to find any as of yet unused number for the room and looks if
that value only fits in a single cell.

Once it exhausts all the cells it can fill in for sure, the solver branches into
a DFS search, finding the first empty cell and filling it with all possible
values until one yields a solution further down the call stack.

I also wrote code that gathers all possible solutions to a puzzle, instead of
returning after finding just one. This is made super simple partially by C++'s
blood magic that for some reason gives `vector<vector<int>>` a default equality
check so they can be stuck into a `set` without concern.

## Creation Strategies
The creator cannot yet generate puzzles entirely on its own. Instead, it uses a
collaborative approach, requiring initial human input and occasional human input
thereafter. It takes a partially solved instance (which can be as bare as just a
room specification and entirely empty cells) and works from there. Once input is
read, general execution follows this pattern:
- Generate all possible solutions for the current configuration.
 - If no solutions exist, undo the previous operation or fail.
 - If one solution exists, congratulations! We've made a valid puzzle.
 - If multiple solutions exist, ask the user to specify another cell's value,
thereby reducing the number of possible solutions.

When asking the user which cell to modify next, the creator code attempts to
give useful information, such as the current number of solutions, known and
unknown cells, and the frequency with which possible values occur in the set of
valid solutions. This information can also be used to approximate puzzle
difficulty.

## Binary Options
There are a few options in the code to control output.
- In `solver_main.cc` and `creator_main.cc`, there is a VERBOSITY value to
control how much info gets printed.
- In `solver_main.cc`, there is a FIND_ALL_SOLUTIONS value to control whether
all solutions are found or not.
- In `print_board.h`, there is a `#define` that controls the `printBoard`
function. Specifically, un-commenting it prevents special Unicode grid
characters from printing. Xcode usually plays nice with them, but many fonts and
terminals usually don't.

## Building the Binaries
This code makes use of several C++11 features, so your compiler must support
that standard.

There is a makefile included, just open the appropriate directory in a terminal
and run `make`. It will build two binaries, `solver` and `creator`.

Alternatively, this project is already uploaded in Xcode's project format if you
run macOS.

## Running the Binaries
Command line arguments are ignored.

All input is read from stdin, with an additional empty line indicating an end to
that portion of input:
- First, the board's cell values are read, line by line, delimited by 1 or more
space. A value of 0 means that cell is not yet filled in.
- Then, the board's rooms are read in using a similar method, but each room is
assigned one ID for all cells in that room. Room IDs must be unique across the
entire board.

The creator code obviously expects more user input, and provides detailed output
and prompts.

See one of the sample puzzle files for more info.

Running the solver on one of the supplied puzzles is simple:
`./solver <Puzzle1.txt`

Running the creator on one of the supplied incomplete puzzles is also simple:
`./creator <MultipleSolutions1.txt`

## Sample Solver Run
```
$ ./solver <Puzzle2.txt
5 0 0 0 0 0
0 3 0 0 0 0
0 0 1 0 0 0
6 4 0 0 2 0
0 0 1 0 0 0
0 3 0 0 0 0
5 0 0 0 0 0

 1  2  2  3  3  3
 1  1  4  4  4  3
 5  1  1  4  4  3
 5  5  5  6  6  7
 5  5  8  9  9 10
11  8  8 12  9 10
 8  8 12 12  9 10

Initial board state:
┌─┬───┬─────┐
│5│   │     │
│ └─┬─┴───┐ │
│  3│     │ │
├─┐ └─┐   │ │
│ │  1│   │ │
│ └───┼───┼─┤
│6 4  │  2│ │
│   ┌─┼───┼─┤
│   │1│   │ │
├─┬─┘ ├─┐ │ │
│ │3  │ │ │ │
├─┘ ┌─┘ │ │ │
│5  │   │ │ │
└───┴───┴─┴─┘
Solved the puzzle. Final state:
┌─┬───┬─────┐
│5│1 2│1 3 2│
│ └─┬─┴───┐ │
│4 3│5 2 1│4│
├─┐ └─┐   │ │
│1│2 1│3 4│5│
│ └───┼───┼─┤
│6 4 3│1 2│1│
│   ┌─┼───┼─┤
│2 5│1│4 3│2│
├─┬─┘ ├─┐ │ │
│1│3 4│2│1│3│
├─┘ ┌─┘ │ │ │
│5 2│1 3│2│1│
└───┴───┴─┴─┘
```

## Sample Creator Run
```
$ ./creator <MultipleSolutions1.txt
5 0 0 0 0 0
0 0 0 0 0 0
0 0 0 0 0 0
6 4 0 0 0 0
0 0 0 0 0 0
0 0 0 0 0 0
5 0 0 0 0 0

1  2  2  3  3  3
1  1  4  4  4  3
5  1  1  4  4  3
5  5  5  6  6  7
5  5  8  9  9 10
11  8  8 12  9 10
8  8 12 12  9 10

Initial board state:
┌─┬───┬─────┐
│5│   │     │
│ └─┬─┴───┐ │
│   │     │ │
├─┐ └─┐   │ │
│ │   │   │ │
│ └───┼───┼─┤
│6 4  │   │ │
│   ┌─┼───┼─┤
│   │ │   │ │
├─┬─┘ ├─┐ │ │
│ │   │ │ │ │
├─┘ ┌─┘ │ │ │
│5  │   │ │ │
└───┴───┴─┴─┘
Currently known cell values:
┌─┬───┬─────┐
│5│   │     │
│ └─┬─┴───┐ │
│   │     │ │
├─┐ └─┐   │ │
│ │   │   │ │
│ └───┼───┼─┤
│6 4  │1 2│1│
│   ┌─┼───┼─┤
│   │ │   │ │
├─┬─┘ ├─┐ │ │
│1│   │ │ │ │
├─┘ ┌─┘ │ │ │
│5  │   │ │ │
└───┴───┴─┴─┘
Computing all solutions to the current board...
Currently known cell values:
┌─┬───┬─────┐
│5│   │     │
│ └─┬─┴───┐ │
│   │     │ │
├─┐ └─┐   │ │
│ │   │   │ │
│ └───┼───┼─┤
│6 4  │1 2│1│
│   ┌─┼───┼─┤
│   │ │   │ │
├─┬─┘ ├─┐ │ │
│1│   │ │ │ │
├─┘ ┌─┘ │ │ │
│5  │   │ │ │
└───┴───┴─┴─┘
Aggregating cells across all solutions to see if any other cells are known...
Currently known cell values:
┌─┬───┬─────┐
│5│1 2│1 3  │
│ └─┬─┴───┐ │
│4  │     │ │
├─┐ └─┐   │ │
│ │   │  4│5│
│ └───┼───┼─┤
│6 4  │1 2│1│
│   ┌─┼───┼─┤
│   │ │4 3│2│
├─┬─┘ ├─┐ │ │
│1│3 4│2│1│3│
├─┘ ┌─┘ │ │ │
│5  │   │2│1│
└───┴───┴─┴─┘
There are currently 25 known cells, 4 of which were given as input, and 17 of which were determined only after aggregating all solutions.
There are currently 17 unknown cells.
The puzzle currently has 3 solutions.
Choose a cell to overwrite...
Enter cell's row (1-7, 'f' to display value frequencies for all unknown cells, 's' to display suggested cells to modify, 'i' to display initial board state [i.e. only what the user has supplied as input]): s
Cell (1, 6) can be filled with a value of 4 to eliminate all other solutions.
Cell (2, 2) can be filled with a value of 3 to eliminate all other solutions.
Cell (2, 3) can be filled with a value of 5 to eliminate all other solutions.
Cell (2, 4) can be filled with a value of 2 to eliminate all other solutions.
Cell (2, 5) can be filled with a value of 1 to eliminate all other solutions.
Cell (2, 6) can be filled with a value of 2 to eliminate all other solutions.
Cell (3, 1) can be filled with a value of 1 to eliminate all other solutions.
Cell (3, 2) can be filled with a value of 2 to eliminate all other solutions.
Cell (3, 3) can be filled with a value of 1 to eliminate all other solutions.
Cell (3, 4) can be filled with a value of 3 to eliminate all other solutions.
Cell (4, 3) can be filled with a value of 3 to eliminate all other solutions.
Cell (5, 1) can be filled with a value of 2 to eliminate all other solutions.
Cell (5, 2) can be filled with a value of 5 to eliminate all other solutions.
Cell (5, 3) can be filled with a value of 1 to eliminate all other solutions.
Cell (7, 2) can be filled with a value of 2 to eliminate all other solutions.
Cell (7, 3) can be filled with a value of 1 to eliminate all other solutions.
Cell (7, 4) can be filled with a value of 3 to eliminate all other solutions.
Choose a cell to overwrite...
Enter cell's row (1-7, 'f' to display value frequencies for all unknown cells, 's' to display suggested cells to modify, 'i' to display initial board state [i.e. only what the user has supplied as input]): 1
Enter cell's column (1-6): 6
Value frequency for cell (1, 6):
Solutions with 2 in this cell: 2
Solutions with 4 in this cell: 1
Enter new value (0 clears the cell, 0 is the current value, -1 to cancel and choose a different cell): 4
Currently known cell values:
┌─┬───┬─────┐
│5│   │    4│
│ └─┬─┴───┐ │
│   │     │ │
├─┐ └─┐   │ │
│ │   │   │ │
│ └───┼───┼─┤
│6 4  │1 2│1│
│   ┌─┼───┼─┤
│   │ │   │ │
├─┬─┘ ├─┐ │ │
│1│   │ │ │ │
├─┘ ┌─┘ │ │ │
│5  │   │ │ │
└───┴───┴─┴─┘
Computing all solutions to the current board...
Single solution to the current board:
┌─┬───┬─────┐
│5│1 2│1 3 4│
│ └─┬─┴───┐ │
│4 2│1 3 5│2│
├─┐ └─┐   │ │
│2│1 3│2 4│5│
│ └───┼───┼─┤
│6 4 5│1 2│1│
│   ┌─┼───┼─┤
│3 1│2│4 3│2│
├─┬─┘ ├─┐ │ │
│1│3 4│2│1│3│
├─┘ ┌─┘ │ │ │
│5 1│3 1│2│1│
└───┴───┴─┴─┘
Initial board:
┌─┬───┬─────┐
│5│   │    4│
│ └─┬─┴───┐ │
│   │     │ │
├─┐ └─┐   │ │
│ │   │   │ │
│ └───┼───┼─┤
│6 4  │   │ │
│   ┌─┼───┼─┤
│   │ │   │ │
├─┬─┘ ├─┐ │ │
│ │   │ │ │ │
├─┘ ┌─┘ │ │ │
│5  │   │ │ │
└───┴───┴─┴─┘
Raw initial board:
5 0 0 0 0 4
0 0 0 0 0 0
0 0 0 0 0 0
6 4 0 0 0 0
0 0 0 0 0 0
0 0 0 0 0 0
5 0 0 0 0 0

1 2 2 3 3 3
1 1 4 4 4 3
5 1 1 4 4 3
5 5 5 6 6 7
5 5 8 9 9 10
11 8 8 12 9 10
8 8 12 12 9 10
```

## Sample Puzzles
Puzzle 1 was taken from Nikoli's site and has an "easy" rating. Author Hammy.

Puzzle 2 was created by me and is slightly more difficult than puzzle 1. A
modified version of this puzzle for creation purposes can be found in
`MultipleSolutions1.txt`.

Puzzle 3 was also my creation and is similar to puzzle 2's difficulty level.

Puzzle 4 was also from Nikoli's site and has a "hard" rating. A modified version
of this puzzle for creation purposes can be found in `MultipleSolutions2.txt`.
Author T. Karino.
