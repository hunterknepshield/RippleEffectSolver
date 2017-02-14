//
//  validity_checks.cpp
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/10/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#include "validity_checks.hpp"

#include <algorithm>
#include <map>
#include <utility>
#include <vector>

#include "typedefs.h"

bool checkRow(const Cell& cell, int value,
			  const Board& cellValues) {
	// The row must be clear of the same value within `value` spaces.
	int leftBound = std::max(cell.second - value, 0);
	int rightBound = std::min(cell.second + value, (int)cellValues[cell.first].size() - 1);
	for (int c = leftBound; c <= rightBound; c++) {
		if (cellValues[cell.first][c] == value) {
			return false;
		}
	}
	return true;
}

bool checkColumn(const Cell& cell, int value,
				 const Board& cellValues) {
	// The column must be clear of the same value within `value` spaces.
	int lowerBound = std::max(cell.first - value, 0);
	int upperBound = std::min(cell.first + value, (int)cellValues.size() - 1);
	for (int r = lowerBound; r <= upperBound; r++) {
		if (cellValues[r][cell.second] == value) {
			return false;
		}
	}
	return true;
}

bool checkRoom(const Cell& cell, int value, const Board& cellValues,
			   const Board& roomIds, const std::map<int, CellList>& cellsInRoom) {
	// The room must be clear of the same value.
	int roomId = roomIds[cell.first][cell.second];
	for (const auto& roomCell : cellsInRoom.at(roomId)) {
		if (cellValues[roomCell.first][roomCell.second] == value) {
			return false;
		}
	}
	return true;
}
