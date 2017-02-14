//
//  main.cpp
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/9/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

// If your console or font doesn't play nice with the pretty board symbols,
// uncomment this #define and it will print only numbers. The caveat is that it
// then won't print any sort of room borders.
// #define UGLY_PRINT_BOARD

// Verbosity settings. 0 = silent, 1 = print message on action, 2 = print
// message and board on action.
int VERBOSITY = 2;

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "print_board.hpp"
#include "read_input.hpp"
#include "solve_puzzle.hpp"
#include "typedefs.h"
#include "validity_checks.hpp"

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
	std::map<int, CellList> cellsInRoom;
	// Maps room ID to the number of cells completed in the room.
	std::map<int, int> cellsCompletedInRoom;
	for (int r = 0; r < roomIds.size(); r++) {
		for (int c = 0; c < roomIds[r].size(); c++) {
			// TODO enforce contiguous rooms here? May also want to validate givens.
			// For now assuming valid input.
			cellsInRoom[roomIds[r][c]].push_back({r, c});
			if (cellValues[r][c]) {
				cellsCompletedInRoom[roomIds[r][c]]++;
			}
		}
	}
	// To get the size of room n, use cellsInRoom[n].size().
	// To get the value of cell (r, c), use cellValues[r][c].
	// To get the room ID of cell (r, c), use roomIds[r][c].
	
	if (VERBOSITY) {
		std::cout << "Inspecting each room in attempt to brute force..." << std::endl;
	}
	// Tracker to watch whether something changed on this iteration or not.
	// If this is false at the end of the loop, we need to break out and try
	// something else.
	bool modifiedBoard;
	do {
		modifiedBoard = false;
		for (const auto& roomAndCells : cellsInRoom) {
			if (cellsCompletedInRoom[roomAndCells.first] == roomAndCells.second.size()) {
				// This room is already complete, don't waste time here.
				continue;
			}
			int cellsFilled = fillKnownCellsInRoom(cellValues, roomIds, roomAndCells.first,
												   roomAndCells.second, VERBOSITY);
			cellsCompletedInRoom[roomAndCells.first] += cellsFilled;
			if (cellsFilled > 0) {
				modifiedBoard = true;
			}
		}
	} while (modifiedBoard);
	
	bool completed = true;
	for (const auto& roomAndCells : cellsInRoom) {
		if (cellsCompletedInRoom[roomAndCells.first] != roomAndCells.second.size()) {
			completed = false;
			break;
		}
	}
	if (completed) {
		// We finished everything.
		std::cout << "Solved the puzzle. Final state:" << std::endl;
		printBoard(cellValues, roomIds);
	}
	
    return 0;
}
