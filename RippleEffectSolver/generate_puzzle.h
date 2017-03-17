//
//  generate_puzzle.h
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 3/16/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#ifndef generate_puzzle_h
#define generate_puzzle_h

#include <utility>

#include "typedefs.h"

// Generates a board using the supplied dimensions. Uses `seed` in an srand()
// call. The returned puzzle is not guaranteed to be solvable.
std::pair<Board, Board> generatePuzzle(int width, int height, int seed,
									   int verbosity, int solvingVerbosity);

#endif /* generate_puzzle_h */
