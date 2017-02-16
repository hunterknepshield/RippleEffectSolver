//
//  validity_checks.cc
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/10/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#include "validity_checks.h"

#include <algorithm>

#include "typedefs.h"

bool checkRow(const Cell& cell, int value, const Board& cellValues) {
	// The row must be clear of the same value within `value` spaces.
	int leftBound = std::max(cell.second - value, 0);
	int rightBound =
		std::min(cell.second + value, (int)cellValues[cell.first].size() - 1);
	for (int c = leftBound; c <= rightBound; c++) {
		if (c == cell.second) {
			// This cell either needs to be empty or already contain the value.
			int existing = cellValues[cell.first][cell.second];
			if (existing != 0 && existing != value) {
				return false;
			}
			continue;
		}
		if (cellValues[cell.first][c] == value) {
			return false;
		}
	}
	return true;
}

bool checkColumn(const Cell& cell, int value, const Board& cellValues) {
	// The column must be clear of the same value within `value` spaces.
	int lowerBound = std::max(cell.first - value, 0);
	int upperBound = std::min(cell.first + value, (int)cellValues.size() - 1);
	for (int r = lowerBound; r <= upperBound; r++) {
		if (r == cell.first) {
			// This cell either needs to be empty or already contain the value.
			int existing = cellValues[cell.first][cell.second];
			if (existing != 0 && existing != value) {
				return false;
			}
			continue;
		}
		if (cellValues[r][cell.second] == value) {
			return false;
		}
	}
	return true;
}

bool checkRoom(const Cell& cell, const CellList& cells, int value,
			   const Board& cellValues) {
	// No other cell in the room may contain the same value.
	for (const auto& roomCell : cells) {
		if (roomCell == cell) {
			// This cell either needs to be empty or already contain the value.
			int existing = cellValues[cell.first][cell.second];
			if (existing != 0 && existing != value) {
				return false;
			}
			continue;
		}
		if (cellValues[roomCell.first][roomCell.second] == value) {
			return false;
		}
	}
	return true;
}

bool validateCompletedBoard(const Board& cellValues, const Board& roomIds,
							const RoomMap& roomMap) {
	for (int r = 0; r < cellValues.size(); r++) {
		for (int c = 0; c < cellValues[r].size(); c++) {
			if (!cellValues[r][c]) {
				// Empty cell. This isn't a complete board.
				return false;
			}
			if (!checkRow({r, c}, cellValues[r][c], cellValues)) {
				return false;
			}
			if (!checkColumn({r, c}, cellValues[r][c], cellValues)) {
				return false;
			}
			if (!checkRoom({r, c}, roomMap.at(roomIds[r][c]), cellValues[r][c],
						   cellValues)) {
				return false;
			}
		}
	}
	return true;
}
