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
#include <set>
#include <utility>
#include <vector>

#include "typedefs.h"

// Attempts to fill in known cells first, branching only if necessary. Returns
// true the first instant that a valid board is found. Returns false if no
// solution exists. To find multiple solutions, see findAllSolutions.
std::pair<bool, Board> findSingleSolution(
	Board /* intentional copy */ cellValues, const Board& roomIds,
	const RoomMap& roomMap,
	std::map<int, int> /* intentional copy */ cellsCompletedInRoom,
	int verbosity);

// Uses the same logic as above, but returns only once all valid solutions to
// the supplied board are found.
std::pair<bool, std::set<Board>> findAllSolutions(
	Board /* intentional copy */ cellValues, const Board& roomIds,
	const RoomMap& roomMap,
	std::map<int, int> /* intentional copy */ cellsCompletedInRoom,
	int verbosity);

// Loops over a particular room until no cells can be filled in with 100%
// certainty. Returns the number of cells filled in.
int fillKnownCellsInRoom(Board& cellValues, const Board& roomIds, int room,
						 const CellList& cellsInRoom, int verbosity);

// Loops over the entire board until no cells can be filled in with 100%
// certainty. Updates cellsCompletedInRoom as appropriate.
void fillKnownCellsInBoard(Board& cellValues, const Board& roomIds,
						   const RoomMap& roomMap,
						   std::map<int, int>& cellsCompletedInRoom,
						   int verbosity);

#endif /* solve_puzzle_hpp */
