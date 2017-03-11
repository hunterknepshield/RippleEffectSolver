//
//  validity_checks.cc
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/10/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#include "validity_checks.h"

#include <algorithm>
#include <utility>
#include <vector>

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

bool validateRooms(const Board& roomIds, const RoomMap& roomMap) {
	// Graph traversal markers.
	std::vector<std::vector<bool>> visited;
	for (const auto& row : roomIds) {
		visited.emplace_back(row.size());
	}
	int roomId;
	int r, c;
	for (const auto& roomAndCells : roomMap) {
		roomId = roomAndCells.first;
		// If this vector is empty, something has gone horribly wrong elsewhere.
		CellList reachableCells = {roomAndCells.second.front()};
		while (!reachableCells.empty()) {
			// DFS because std::vector::pop_front() doesn't exist
			std::tie(r, c) = reachableCells.back();
			visited[r][c] = true;
			reachableCells.pop_back();
			if (r != 0) {
				// Check above
				if (roomIds[r - 1][c] == roomId && !visited[r - 1][c]) {
					visited[r - 1][c] = true;
					reachableCells.push_back({r - 1, c});
				}
			}
			if (r != roomIds.size() - 1) {
				// Check below
				if (roomIds[r + 1][c] == roomId && !visited[r + 1][c]) {
					visited[r + 1][c] = true;
					reachableCells.push_back({r + 1, c});
				}
			}
			if (c != 0) {
				// Check left
				if (roomIds[r][c - 1] == roomId && !visited[r][c - 1]) {
					visited[r][c - 1];
					reachableCells.push_back({r, c - 1});
				}
			}
			if (c != roomIds[r].size() - 1) {
				// Check right
				if (roomIds[r][c + 1] == roomId && !visited[r][c + 1]) {
					visited[r][c + 1] = true;
					reachableCells.push_back({r, c + 1});
				}
			}
		}
		// If every cell in the room is reachable from the first cell in the
		// room, it's a valid room.
		for (const auto& cell : roomAndCells.second) {
			std::tie(r, c) = cell;
			if (!visited[r][c]) {
				return false;
			}
		}
	}
	return true;
}

bool validateBoard(const Board& cellValues, const Board& roomIds,
				   const RoomMap& roomMap, bool allowEmpty) {
	if (!validateRooms(roomIds, roomMap)) {
		return false;
	}
	for (int r = 0; r < cellValues.size(); r++) {
		for (int c = 0; c < cellValues[r].size(); c++) {
			if (allowEmpty && cellValues[r][c] == 0) {
				continue;
			} else if (!allowEmpty && cellValues[r][c] == 0) {
				// We were expecting this board to be complete.
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
