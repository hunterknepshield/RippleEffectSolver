# Ripple Effect Solver
## Hunter Knepshield

## Ripple Effect
Ripple effect is a Nikoli puzzle, see
http://nikoli.com/en/puzzles/ripple_effect/ for more info.

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

## Binary Options
There are a few options in the code to control output.
- In `main.cc`, there is a VERBOSITY value to control how much info gets 
printed.
- In `main.cc`, there is a FIND_ALL_SOLUTIONS value to control whether all
solutions are found or not.
- In `print_board.h`, there is a `#define` that controls the `printBoard`
function. Specifically, un-commenting it prevents special Unicode grid
characters from printing. Xcode usually plays nice with them, but many fonts and
terminals usually don't.

## Building the Binary
This code makes use of several C++11 features, so your compiler must support
that standard.

There is a makefile included, just open the appropriate directory in a terminal
and run `make`. It will build a single binary, `solver`.

## Running the Binary
Command line arguments are ignored.

All input is read from stdin, with an additional empty line indicating an end to
that portion of input:
- First, the board's cell values are read, line by line, delimited by 1 or more
space. A value of 0 means that cell is not yet filled in.
- Then, the board's rooms are read in using a similar method, but each room is
assigned one ID for all cells in that room. Room IDs must be unique across the
entire board.

See one of the sample puzzle files for more info.

Running the solver on one of the supplied puzzles is fairly simple:
`./solver <Puzzle1.txt`

## Sample Run
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

## Sample Puzzles
Puzzle 1 was taken from Nikoli's site and has an "easy" rating. Author Hammy.

Puzzle 2 was created by me and is slightly more difficult than puzzle 1.

Puzzle 3 was also my creation and is similar to puzzle 2's difficulty level.

Puzzle 4 was also from Nikoli's site and has a "hard" rating. Author T.Karino.
