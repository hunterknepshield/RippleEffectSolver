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
blood magic that for some reason gives vector<vector<int>> a default equality
check so they can be stuck into a set without concern.

## Building the Binary
There is a makefile included, just open the appropriate directory in a terminal
and run `make`. It will build a single binary, `solver`.

## Running the Binary
Command line arguments are ignored.

All input is read from stdin.
- First, the board's cell values are read, line by line, delimited by 1 or more
space. A value of 0 means that cell is not yet filled in.
- Then, the board's rooms are read in using a similar method, but each room is
assigned one ID for all cells in that room. Room IDs must be unique across the
entire board.

See one of the sample puzzle files for more info.

Running the solver on one of the supplied puzzles is fairly simple:
```./solver <Puzzle1.txt```
