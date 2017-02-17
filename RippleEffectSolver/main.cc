//
//  main.cc
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/9/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

// If your console or font doesn't play nice with the pretty board symbols,
// uncomment this #define and it will print only numbers. The caveat is that it
// then won't print any sort of room borders.
// #define SIMPLE_PRINT_BOARD

// Verbosity settings. 0 = silent, 1 = print message on action, 2 = print
// message and board on action.
int VERBOSITY = 2;

// Multi-solution settings. false = find single solution (or none), true = find
// all solutions (or none).
bool FIND_ALL_SOLUTIONS = false;

#include <iostream>
#include <map>
#include <string>

#include "print_board.h"
#include "read_input.h"
#include "solve_puzzle.h"
#include "typedefs.h"
#include "validity_checks.h"

int main(void) {
	size_t boardWidth = 0;
	Board cellValues, roomIds;
	if (!readCells(&boardWidth, &cellValues) ||
		!readRooms(boardWidth, cellValues.size(), &roomIds)) {
		return 1;
	}

	if (VERBOSITY) {
		std::cout << "Initial board state:" << std::endl;
		printBoard(cellValues, roomIds);
	}

	// Now some initial setup...
	// Maps room ID to a list of pairs of cell coordinates in the room.
	RoomMap roomMap;
	// Maps room ID to the number of cells completed in the room.
	std::map<int, int> cellsCompletedInRoom;
	for (int r = 0; r < roomIds.size(); r++) {
		for (int c = 0; c < roomIds[r].size(); c++) {
			// TODO enforce contiguous rooms here? May also want to validate
			// givens. For now assuming valid input.
			roomMap[roomIds[r][c]].push_back({r, c});
			if (cellValues[r][c]) {
				cellsCompletedInRoom[roomIds[r][c]]++;
			}
		}
	}
	// To get the size of room n, use roomMap[n].size().
	// To get the value of cell (r, c), use cellValues[r][c].
	// To get the room ID of cell (r, c), use roomIds[r][c].

	if (FIND_ALL_SOLUTIONS) {
		const auto& solvedWithBoards = findAllSolutions(
			cellValues, roomIds, roomMap, cellsCompletedInRoom, VERBOSITY);
		if (solvedWithBoards.first) {
			std::cout << "The puzzle has " << solvedWithBoards.second.size()
					  << " solution"
					  << (solvedWithBoards.second.size() > 1 ? "s" : "") << "."
					  << std::endl;
			int solution = 0;
			for (const auto& board : solvedWithBoards.second) {
				std::cout << "Solution " << ++solution << ":" << std::endl;
				printBoard(board, roomIds);
			}
		} else {
			std::cout << "No solutions." << std::endl;
		}
	} else {
		const auto& solvedWithBoard = findSingleSolution(
			cellValues, roomIds, roomMap, cellsCompletedInRoom, VERBOSITY);
		if (solvedWithBoard.first) {
			std::cout << "Solved the puzzle. Final state:" << std::endl;
			printBoard(solvedWithBoard.second, roomIds);
		} else {
			std::cout << "No solution." << std::endl;
		}
	}

	return 0;
}
