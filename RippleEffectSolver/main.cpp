//
//  main.cpp
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/9/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#define PRETTY_PRINT_BOARD true

// String constants for easy usage. N > S > E > W.
#define EW "─"
#define NS "│"
#define SE "┌"
#define SW "┐"
#define NE "└"
#define NW "┘"
#define NSE "├"
#define NSW "┤"
#define SEW "┬"
#define NEW "┴"
#define NSEW "┼"

void test() {
	auto string = "┃";
	std::cout << string << std::endl;
	std::cout << typeid(string).name() << std::endl;
}

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

void printBoard(const std::vector<std::vector<int>>& board,
				const std::vector<std::vector<int>>& roomIds) {
	// Top row of board symbols
	
	
	// Row of cell values, row of board symbols
	for (int i = 0; i < board.size(); i++) {
		for (int j = 0; j < board[i].size(); j++) {
			
		}
	}
}

void printBoardUgly(const std::vector<std::vector<int>>& board,
					const std::vector<std::vector<int>>& /* unused */ roomIds) {
	for (const auto& row : board) {
		for (int cell : row) {
			std::cout << cell << " ";
		}
		std::cout << std::endl;
	}
}

int main(void) {
	std::string s;
	// Read in the cell values on the board (0 is unset) until an empty line is reached
	size_t lineCount = -1;  // Unknown so far. Enforced to be the same for all rows.
	std::vector<std::vector<int>> board;
	do {
		std::getline(std::cin, s);
		if (s.empty())
			break;
		const std::vector<int>& cellValues = interpretString(s);
		if (lineCount == -1) {
			lineCount = cellValues.size();
		} else if (lineCount != cellValues.size()) {
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
		if (lineCount != cellIds.size()) {
			std::cerr << "Inconsistent line size. Terminating." << std::endl;
			return 1;
		}
		roomIds.push_back(cellIds);
	} while (true);
	
	PRETTY_PRINT_BOARD ? printBoard(board, roomIds) : printBoardUgly(board, roomIds);
	
	// Now some initial setup...
	// Maps room ID to a list of pairs of cell coordinates in the room.
	// To get the size of room x, use cellsInRoom[x].size().
	std::map<int, std::vector<std::pair<int, int>>> cellsInRoom;
	// Now fill in cellsInRoom.
	for (int i = 0; i < roomIds.size(); i++) {
		for (int j = 0; j < roomIds[i].size(); j++) {
			cellsInRoom[i].push_back({i, j});
		}
	}
	// TODO enforce contiguous rooms here? For now assuming valid input.
	
	
    return 0;
}
