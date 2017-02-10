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
	// To get the size of room x, use cellsInRoom[x].size().
	std::map<int, std::vector<std::pair<int, int>>> cellsInRoom;
	// Now fill in cellsInRoom.
	for (int i = 0; i < roomIds.size(); i++) {
		for (int j = 0; j < roomIds[i].size(); j++) {
			cellsInRoom[roomIds[i][j]].push_back({i, j});
		}
	}
	// TODO enforce contiguous rooms here? May also want to validate givens.
	// For now assuming valid input.
	
	// Easiest thing to do: fill in rooms of size 1.
	if (VERBOSITY) {
		std::cout << "Filling in any 1x1 rooms with 1s..." << std::endl;
	}
	for (const auto& roomAndCells : cellsInRoom) {
		if (roomAndCells.second.size() == 1) {
			int x, y;
			std::tie(x, y) = roomAndCells.second.front();
			if (board[x][y] == 0) {
				board[x][y] = 1;
				switch (VERBOSITY) {
					case 2:
						printBoard(board, roomIds);
					case 1:
						std::cout << "Filled in a forced 1 at (" << x + 1 << ", " << y + 1 << ")." << std::endl;
					default:
						break;
				}
			}
		}
	}
	
    return 0;
}
