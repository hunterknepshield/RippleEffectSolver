//
//  solve_puzzle.hpp
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/12/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#ifndef solve_puzzle_hpp
#define solve_puzzle_hpp

#include <map>
#include <utility>

#include "typedefs.h"

// Attempts to fill in known cells first, branching only if necessary. Returns
// true the first instant that a valid board is found. Returns false if no
// solution exists. To find multiple solutions, see TODO implement this later.
std::pair<bool, Board> findSingleSolution(
	Board /* intentional copy */ cellValues, const Board& roomIds,
	const std::map<int, CellList>& cellsInRoom,
	std::map<int, int> /* intentional copy */ cellsCompletedInRoom,
	int verbosity);

// Loops over a particular room until no cells can be filled in with 100%
// certainty. Returns the number of cells filled in.
int fillKnownCellsInRoom(Board& cellValues, const Board& roomIds, int room,
						 const CellList& cellsInRoom, int verbosity);

#endif /* solve_puzzle_hpp */
