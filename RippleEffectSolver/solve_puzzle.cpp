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
#include <map>
#include <utility>
#include <vector>

#include "print_board.hpp"
#include "typedefs.h"
#include "validity_checks.hpp"

std::pair<bool, Board> findSingleSolution(
	Board /* intentional copy */ cellValues, const Board& roomIds,
	const std::map<int, CellList>& cellsInRoom,
	std::map<int, int> /* intentional copy */ cellsCompletedInRoom,
	int VERBOSITY) {
	// Tracker to watch whether something changed on this iteration or not.
	// If this is false at the end of the loop, we need to break out and try
	// something else.
	bool modifiedBoard;
	do {
		modifiedBoard = false;
		for (const auto& roomAndCells : cellsInRoom) {
			if (cellsCompletedInRoom[roomAndCells.first] ==
				roomAndCells.second.size()) {
				// This room is already complete, don't waste time here.
				continue;
			}
			int cellsFilled =
				fillKnownCellsInRoom(cellValues, roomIds, roomAndCells.first,
									 roomAndCells.second, VERBOSITY);
			cellsCompletedInRoom[roomAndCells.first] += cellsFilled;
			if (cellsFilled > 0) {
				modifiedBoard = true;
			}
		}
	} while (modifiedBoard);

	// At this point, we're either done the puzzle or need to branch.
	if (validateCompletedBoard(cellValues, roomIds, cellsInRoom)) {
		return {true, cellValues};
	}
	// Now, we need to make a choice. Find the first empty cell and fill it with
	// its first possibility, then recurse. If that returns a valid solution,
	// return that. Otherwise, try the next value until one is found or all are
	// exhausted.
	switch (VERBOSITY) {
		case 2:
		case 1:
			std::cout << "Unable to fill in any more cells with certainty. "
						 "Beginning to branch."
					  << std::endl;
		default:
			break;
	}
	for (int r = 0; r < cellValues.size(); r++) {
		for (int c = 0; c < cellValues[r].size(); c++) {
			if (cellValues[r][c]) {
				continue;
			}
			// Now determine which values are possible here.
			int room = roomIds[r][c];
			std::vector<bool> usedNumber(cellsInRoom.at(room).size(), false);
			for (const auto& cell : cellsInRoom.at(room)) {
				int value = cellValues[cell.first][cell.second];
				if (value) {
					usedNumber[value - 1] = true;
				}
			}
			std::vector<int> possibleValues;
			for (int i = 0; i < usedNumber.size(); i++) {
				if (!usedNumber[i]) {
					possibleValues.push_back(i + 1);
				}
			}
			for (int possibility : possibleValues) {
				if (checkRow({r, c}, possibility, cellValues) &&
					checkColumn({r, c}, possibility, cellValues)) {
					// Now we attempt this completion, and undo it if it doesn't
					// work.
					cellValues[r][c] = possibility;
					cellsCompletedInRoom[room]++;
					switch (VERBOSITY) {
						case 2:
							printBoard(cellValues, roomIds);
						case 1:
							std::cout << "Branching by filling (" << r + 1
									  << ", " << c + 1 << ") with value "
									  << possibility << "." << std::endl;
						default:
							break;
					}
					const auto& resultAndBoard =
						findSingleSolution(cellValues, roomIds, cellsInRoom,
										   cellsCompletedInRoom, VERBOSITY);
					if (resultAndBoard.first) {
						// This is a valid completion.
						return resultAndBoard;
					}
					cellValues[r][c] = 0;
					cellsCompletedInRoom[room]--;
				}
			}
			// We've exhausted every possibility for this cell without finding
			// a valid one, which means either this board is unsolvable or we
			// branched incorrectly somewhere up the call stack.
			return {false, {}};
		}
	}
	return {false, {}};
}

int fillKnownCellsInRoom(Board& cellValues, const Board& roomIds, int room,
						 const CellList& cellsInRoom, int VERBOSITY) {
	int cellsFilled = 0;
	bool modifiedRoom;
	do {
		modifiedRoom = false;
		// First, we need to figure out which numbers haven't been used and
		// which cells are empty.
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

		// If we have any empty cells, start looking through their combinations
		// of possibilities.
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
						std::cout
							<< "Filled in a " << validPossibility << " at ("
							<< r + 1 << ", " << c + 1
							<< ") since it's the only possibility for the cell."
							<< std::endl;
					default:
						break;
				}
				// We've now taken care of this possibility.
				possibleValues.erase(
					std::remove(possibleValues.begin(), possibleValues.end(),
								validPossibility),
					possibleValues.end());
			}
		}
		// We need to remove the cells we just filled in, if any.
		emptyCells.erase(
			std::remove_if(emptyCells.begin(), emptyCells.end(),
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
						std::cout
							<< "Filled in a " << possibleValue << " at ("
							<< r + 1 << ", " << c + 1
							<< ") since it's the only cell that will fit it."
							<< std::endl;
					default:
						break;
				}
				// We've now taken care of this cell.
				emptyCells.erase(
					std::remove(emptyCells.begin(), emptyCells.end(),
								std::make_pair(r, c)),
					emptyCells.end());
			}
		}
		// Note to self: trim possibilities vector if doing more work after this
	} while (modifiedRoom);
	return cellsFilled;
}
