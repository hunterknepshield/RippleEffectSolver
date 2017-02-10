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

#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "print_board.hpp"

std::vector<int> interpretString(const std::string& s) {
	std::vector<int> v;
	std::stringstream stream(s);
	while (true) {
		int i;
		stream >> i;
		if (!stream)
			break;
		v.push_back(i);
	}
	return v;
}

int main(void) {
	std::string s;
	// Read in the cell values on the board (0 is unset) until an empty line is reached
	size_t rowWidth = -1;  // Unknown so far. Enforced to be the same for all rows.
	std::vector<std::vector<int>> board;
	do {
		std::getline(std::cin, s);
		if (s.empty()) {
			if (board.empty()) {
				std::cerr << "Board cannot be empty.";
				continue;
			}
			break;
		}
		const std::vector<int>& cellValues = interpretString(s);
		if (rowWidth == -1) {
			rowWidth = cellValues.size();
		} else if (rowWidth != cellValues.size()) {
			std::cerr << "Inconsistent line size. Terminating." << std::endl;
			return 1;
		}
		board.push_back(cellValues);
	} while (true);
	
	// Now read in the room IDs. They can be arbitrary ints, not just sequential.
	std::vector<std::vector<int>> roomIds;
	do {
		std::getline(std::cin, s);
		if (s.empty()) {
			if (roomIds.size() != board.size()) {
				std::cerr << "Failed to specify room IDs for all cells. Terminating." << std::endl;
				return 1;
			}
			break;
		}
		const std::vector<int>& cellIds = interpretString(s);
		if (rowWidth != cellIds.size()) {
			std::cerr << "Inconsistent line size. Terminating." << std::endl;
			return 1;
		}
		roomIds.push_back(cellIds);
	} while (true);
	
	if (VERBOSITY > 0) {
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
	// TODO enforce contiguous rooms here? For now assuming valid input.
	
	// Easiest thing to do: fill in rooms of size 1.
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
