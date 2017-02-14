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
			int r, c;
			std::vector<bool> completedNumbers(roomAndCells.second.size(), false);
			CellList emptyCells;
			for (const auto& cell : roomAndCells.second) {
				std::tie(r, c) = cell;
				if (cellValues[r][c]) {
					completedNumbers[cellValues[r][c] - 1] = true;
				} else {
					emptyCells.push_back(cell);
				}
			}
			// The easiest thing to do is complete a room that has only one
			// empty cell. This also covers 1x1 rooms that must therefore be 1s.
			if (emptyCells.size() == 1) {
				auto valueIt = std::find(completedNumbers.begin(), completedNumbers.end(), false);
				int value = (int)(valueIt - completedNumbers.begin()) + 1;
				std::tie(r, c) = emptyCells.front();
				cellValues[r][c] = value;
				cellsCompletedInRoom[roomIds[r][c]]++;
				modifiedBoard = true;
				switch (VERBOSITY) {
					case 2:
						printBoard(cellValues, roomIds);
					case 1:
						std::cout << "Filled in a " << value << " at (" << r + 1 << ", " << c + 1 << ") to complete the room." << std::endl;
					default:
						break;
				}
				// We're now done with this room.
				continue;
			}
			
			// If we have more than 1 empty cell, start looking through each one
			// and see if we can fill it in with exactly one missing number.
			std::vector<int> possibilities;
			for (int i = 0; i < roomAndCells.second.size(); i++) {
				if (!completedNumbers[i]) {
					possibilities.push_back(i + 1);
				}
			}
			
			// Are there any cells in this room that have only one possible
			// valid completion?
			int validPossibility;
			for (const auto& cell : emptyCells) {
				// This will be non-zero if there's exactly 1 possible value for
				// this cell.
				validPossibility = 0;
				std::tie(r, c) = cell;
				for (int possibility : possibilities) {
					if (checkRow(cell, possibility, cellValues) && checkColumn(cell, possibility, cellValues)) {
						if (validPossibility) {
							// Multiple possibilities, so do nothing.
							validPossibility = 0;
							break;
						}
						validPossibility = possibility;
					}
				}
				if (validPossibility) {
					cellValues[r][c] = validPossibility;
					cellsCompletedInRoom[roomIds[r][c]]++;
					modifiedBoard = true;
					switch (VERBOSITY) {
						case 2:
							printBoard(cellValues, roomIds);
						case 1:
							std::cout << "Filled in a " << validPossibility << " at (" << r + 1 << ", " << c + 1 << ") since it's the only possibility." << std::endl;
						default:
							break;
					}
					// We've now taken care of this possibility.
					possibilities.erase(std::remove(possibilities.begin(), possibilities.end(), validPossibility), possibilities.end());
				}
			}
			// We need to remove the cells we just filled in, if any.
			emptyCells.erase(std::remove_if(emptyCells.begin(), emptyCells.end(),
											[&cellValues](const Cell& cell) {
												return cellValues[cell.first][cell.second] != 0;
											}),
							 emptyCells.end());
			// Are there any missing values for this room that fit in only one
			// cell?
			for (int possibility : possibilities) {
				r = -1;
				c = -1;
				for (const auto& cell : emptyCells) {
					if (checkRow(cell, possibility, cellValues) &&
						checkColumn(cell, possibility, cellValues)) {
						if (r != -1) {
							// Multiple possibilities, so do nothing.
							r = -1;
							break;
						}
						std::tie(r, c) = cell;
					}
				}
				if (r != -1) {
					cellValues[r][c] = possibility;
					cellsCompletedInRoom[roomIds[r][c]]++;
					modifiedBoard = true;
					switch (VERBOSITY) {
						case 2:
							printBoard(cellValues, roomIds);
						case 1:
							std::cout << "Filled in a " << possibility << " at (" << r + 1 << ", " << c + 1 << ") since it's the only cell that will fit it." << std::endl;
						default:
							break;
					}
					// We've now taken care of this cell.
					emptyCells.erase(std::remove(emptyCells.begin(),
												 emptyCells.end(),
												 std::make_pair(r, c)),
									 emptyCells.end());
				}
			}
			// TODO trim possibilities vector if doing more work after this
		}
	} while (modifiedBoard);
	
    return 0;
}
