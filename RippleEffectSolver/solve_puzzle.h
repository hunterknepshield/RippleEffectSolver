//
//  solve_puzzle.h
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/12/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#ifndef solve_puzzle_h
#define solve_puzzle_h

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
// the supplied board are found. If `solutionCount` is not null, a message will
// print with the current total number of solutions found every time a new
// valid configuration is found.
std::pair<bool, std::set<Board>> findAllSolutions(
	Board /* intentional copy */ cellValues, const Board& roomIds,
	const RoomMap& roomMap,
	std::map<int, int> /* intentional copy */ cellsCompletedInRoom,
	int verbosity, int* solutionCount = nullptr);

// Scans all supplied boards, returning an aggregate board containing only
// values that appear the same in every single one. Assumes that at least one
// board is supplied.
Board aggregateBoards(const std::set<Board>& boards);

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

// Generates two maps, one mapping room ID to a list of cells in the room, and
// the other mapping room ID to the count of completed cells in the room so far.
std::pair<RoomMap, std::map<int, int>> generateRoomMapAndCompletedCellMap(
	const Board& cellValues, const Board& roomIds);

// Counts the number of known (non-zero) cells in the current board.
int countKnownCells(const Board& board);

// Returns a mapping of cell location to the frequency of its various values
// across all solutions supplied in `boards`.
std::vector<std::vector<std::map<int, int>>> generateValueFrequencies(
	const std::set<Board>& boards);

#endif /* solve_puzzle_h */
