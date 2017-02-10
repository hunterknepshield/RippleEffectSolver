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
#define VERBOSITY 2

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "read_input.hpp"
#include "print_board.hpp"

int main(void) {
	size_t boardWidth = 0;
	std::vector<std::vector<int>> board, roomIds;
	if (!readCells(&boardWidth, &board) ||
		!readRooms(boardWidth, board.size(), &roomIds)) {
		return 1;
	}
	
	if (VERBOSITY) {
		std::cout << "Initial board state:" << std::endl;
		printBoard(board, roomIds);
	}
	
	// Now some initial setup...
	// Maps room ID to a list of pairs of cell coordinates in the room.
	std::map<int, std::vector<std::pair<int, int>>> cellsInRoom;
	// Maps room ID to the number of cells completed in the room.
	std::map<int, int> cellsCompletedInRoom;
	for (int r = 0; r < roomIds.size(); r++) {
		for (int c = 0; c < roomIds[r].size(); c++) {
			// TODO enforce contiguous rooms here? May also want to validate givens.
			// For now assuming valid input.
			cellsInRoom[roomIds[r][c]].push_back({r, c});
			if (board[r][c]) {
				cellsCompletedInRoom[roomIds[r][c]]++;
			}
		}
	}
	// To get the size of room n, use cellsInRoom[n].size().
	// To get the value of cell (r, c), use board[r][c].
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
			for (const auto& cell : roomAndCells.second) {
				std::tie(r, c) = cell;
				if (board[r][c]) {
					completedNumbers[board[r][c] - 1] = true;
				}
			}
			// The easiest thing to do is complete a room that has only one
			// empty cell. This also covers 1x1 rooms that must be 1s.
			if (std::count(completedNumbers.begin(), completedNumbers.end(), false) == 1) {
				auto valueIt = std::find(completedNumbers.begin(), completedNumbers.end(), false);
				int index = (int)(valueIt - completedNumbers.begin());
				int value = index + 1;
				auto cellIt = std::find_if(roomAndCells.second.begin(), roomAndCells.second.end(),
										   [&board](const std::pair<int, int>& cell) {
											   return board[cell.first][cell.second] == 0;
										   });
				std::tie(r, c) = *cellIt;
				board[r][c] = value;
				cellsCompletedInRoom[roomIds[r][c]]++;
				modifiedBoard = true;
				switch (VERBOSITY) {
					case 2:
						printBoard(board, roomIds);
					case 1:
						std::cout << "Filled in a " << value << " at (" << r + 1 << ", " << c + 1 << ") to complete the room." << std::endl;
					default:
						break;
				}
			}
		}
	} while (modifiedBoard);
	
    return 0;
}
