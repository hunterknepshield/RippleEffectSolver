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

bool checkRow(const std::pair<int, int>& cell, int value,
			  const std::vector<std::vector<int>>& board) {
	// The row must be clear of the same value within `value` spaces.
	int leftBound = std::max(cell.second - value, 0);
	int rightBound = std::min(cell.second + value, (int)board[cell.first].size() - 1);
	for (int c = leftBound; c <= rightBound; c++) {
		if (board[cell.first][c] == value) {
			return false;
		}
	}
	return true;
}

bool checkColumn(const std::pair<int, int>& cell, int value,
				 const std::vector<std::vector<int>>& board) {
	// The column must be clear of the same value within `value` spaces.
	int lowerBound = std::max(cell.first - value, 0);
	int upperBound = std::min(cell.first + value, (int)board.size() - 1);
	for (int r = lowerBound; r <= upperBound; r++) {
		if (board[r][cell.second] == value) {
			return false;
		}
	}
	return true;
}

bool checkRoom(const std::pair<int, int>& cell, int value,
			   const std::vector<std::vector<int>>& board,
			   const std::vector<std::vector<int>>& roomIds,
			   const std::map<int, std::vector<std::pair<int, int>>>& cellsInRoom) {
	// The room must be clear of the same value.
	int roomId = roomIds[cell.first][cell.second];
	for (const auto& roomCell : cellsInRoom.at(roomId)) {
		if (board[roomCell.first][roomCell.second] == value) {
			return false;
		}
	}
	return true;
}
