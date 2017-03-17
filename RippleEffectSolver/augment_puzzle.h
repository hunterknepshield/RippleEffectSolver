//
//  augment_puzzle.h
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 3/16/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#ifndef augment_puzzle_h
#define augment_puzzle_h

#include "typedefs.h"

// Main interactive routine. General flow:
// - Generate all possible solutions.
//   - If no solutions, undo the previous action if possible, otherwise fail.
//   - If one solution, we're done and we've generated a puzzle.
//   - If multiple solutions, ask the user for a new cell to fill in.
int augmentPuzzle(Board& cellValues, const Board& roomIds, int verbosity,
				  int solvingVerbosity);

#endif /* augment_puzzle_h */
