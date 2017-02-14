//
//  solve_puzzle.cpp
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/12/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#include "solve_puzzle.hpp"

#include <algorithm>
#include <iostream>
#include <vector>
#include <utility>

#include "print_board.hpp"
#include "typedefs.h"
#include "validity_checks.hpp"

int fillKnownCellsInRoom(Board& cellValues, const Board& roomIds, int room,
						 const CellList& cellsInRoom, int VERBOSITY) {
	int cellsFilled = 0;
	bool modifiedRoom;
	do {
		modifiedRoom = false;
		// First, we need to figure out which numbers haven't been used and which cells are empty.
		int r, c;
		std::vector<bool> usedNumber(cellsInRoom.size(), false);
		CellList emptyCells;
		for (const auto& cell : cellsInRoom) {
			std::tie(r, c) = cell;
			if (cellValues[r][c]) {
				usedNumber[cellValues[r][c] - 1] = true;
			} else {
				emptyCells.push_back(cell);
			}
		}
		
		// If we have any empty cells, start looking through their combinations of possibilities.
		std::vector<int> possibleValues;
		for (int i = 0; i < usedNumber.size(); i++) {
			if (!usedNumber[i]) {
				possibleValues.push_back(i + 1);
			}
		}
		
		// Are there any empty cells that have just one possible value?
		int validPossibility;
		for (const auto& cell : emptyCells) {
			// This will be non-zero if there's exactly 1 possible value for
			// this cell.
			validPossibility = 0;
			std::tie(r, c) = cell;
			for (int possibleValue : possibleValues) {
				if (checkRow(cell, possibleValue, cellValues) &&
					checkColumn(cell, possibleValue, cellValues)) {
					if (validPossibility) {
						// Multiple possibilities, so do nothing.
						validPossibility = 0;
						break;
					}
					validPossibility = possibleValue;
				}
			}
			if (validPossibility) {
				cellValues[r][c] = validPossibility;
				cellsFilled++;
				modifiedRoom = true;
				switch (VERBOSITY) {
					case 2:
						printBoard(cellValues, roomIds);
					case 1:
						std::cout << "Filled in a " << validPossibility
							<< " at (" << r + 1 << ", " << c + 1
							<< ") since it's the only possibility." << std::endl;
					default:
						break;
				}
				// We've now taken care of this possibility.
				possibleValues.erase(std::remove(possibleValues.begin(),
												 possibleValues.end(),
												 validPossibility),
									 possibleValues.end());
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
		for (int possibleValue : possibleValues) {
			r = -1;
			c = -1;
			for (const auto& cell : emptyCells) {
				if (checkRow(cell, possibleValue, cellValues) &&
					checkColumn(cell, possibleValue, cellValues)) {
					if (r != -1) {
						// Multiple possibilities, so do nothing.
						r = -1;
						break;
					}
					std::tie(r, c) = cell;
				}
			}
			if (r != -1) {
				cellValues[r][c] = possibleValue;
				cellsFilled++;
				modifiedRoom = true;
				switch (VERBOSITY) {
					case 2:
						printBoard(cellValues, roomIds);
					case 1:
						std::cout << "Filled in a " << possibleValue << " at (" << r + 1 << ", " << c + 1 << ") since it's the only cell that will fit it." << std::endl;
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
		// Note to self: trim possibilities vector if doing more work after this
	} while (modifiedRoom);
	return cellsFilled;
}
