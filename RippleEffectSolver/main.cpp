//
//  main.cpp
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/9/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#include <bitset>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#define PRETTY_PRINT_BOARD true

// String constants for easy usage. N > S > E > W.
// Also #define'd away since they need the weird scary string types because these are Unicode characters, not boring ASCII.
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
	// Top row of board symbols.
	std::cout << SE;
	for (int i = 0; i < board[0].size() - 1; i++) {
		std::cout << EW;
		if (roomIds[0][i] != roomIds[0][i + 1]) {
			std::cout << SEW;
		} else {
			std::cout << EW;
		}
	}
	std::cout << EW << SW << std::endl;
	
	// Interior rows of cell values and board symbols.
	for (int i = 0; i < board.size() - 1; i++) {
		// Row of cell contents.
		std::cout << NS;
		for (int j = 0; j < board[i].size() - 1; j++) {
			std::cout << board[i][j];
			if (roomIds[i][j] != roomIds[i][j + 1]) {
				std::cout << NS;
			} else {
				std::cout << " ";
			}
		}
		std::cout << board[i].back() << NS << std::endl;
		
		// Row of board symbols.
		if (roomIds[i][0] != roomIds[i + 1][0]) {
			std::cout << NSE;
		} else {
			std::cout << NS;
		}
		for (int j = 0; j < board[i].size() - 1; j++) {
			if (roomIds[i][j] != roomIds[i + 1][j]) {
				std::cout << EW;
			} else {
				std::cout << " ";
			}
			
			// Space between four cells. Could be empty or anything except a corner. No easy way to do this.
			// Bit field; bit 0 = N, bit 1 = S, bit 2 = E, bit 3 = W.
			std::bitset<4> space;  // Empty for now
			if (roomIds[i][j] != roomIds[i][j + 1]) {
				space[0].flip();  // N
			}
			if (roomIds[i + 1][j] != roomIds[i + 1][j + 1]) {
				space[1].flip();  // S
			}
			if (roomIds[i][j + 1] != roomIds[i + 1][j + 1]) {
				space[2].flip();  // E
			}
			if (roomIds[i][j] != roomIds[i + 1][j]) {
				space[3].flip();  // W
			}
			switch (space.to_ulong()) {
				case 0:
					std::cout << " ";
					break;
				case 3:
					std::cout << NS;
					break;
				case 5:
					std::cout << NE;
					break;
				case 6:
					std::cout << SE;
					break;
				case 7:
					std::cout << NSE;
					break;
				case 9:
					std::cout << NW;
					break;
				case 10:
					std::cout << SW;
					break;
				case 11:
					std::cout << NSW;
					break;
				case 12:
					std::cout << EW;
					break;
				case 13:
					std::cout << NEW;
					break;
				case 14:
					std::cout << SEW;
					break;
				case 15:
					std::cout << NSEW;
					break;
				default:
					std::cerr << "Invalid value for calculating square of 4: " << space << std::endl;
					std::cout << "?";
			}
		}
		if (roomIds[i].back() != roomIds[i + 1].back()) {
			std::cout << EW << NSW;
		} else {
			std::cout << " " << NS;
		}
		std::cout << std::endl;
	}
	
	// Last row of cell contents.
	std::cout << NS;
	for (int i = 0; i < board.back().size() - 1; i++) {
		std::cout << board.back()[i];
		if (roomIds.back()[i] != roomIds.back()[i + 1]) {
			std::cout << NS;
		} else {
			std::cout << " ";
		}
	}
	std::cout << board.back().back() << NS << std::endl;
	
	// Last row of board symbols.
	std::cout << NE;
	for (int i = 0; i < board.back().size() - 1; i++) {
		std::cout << EW;
		if (roomIds.back()[i] != roomIds.back()[i + 1]) {
			std::cout << NEW;
		} else {
			std::cout << EW;
		}
	}
	std::cout << EW << NW << std::endl;
}

void printBoardUgly(const std::vector<std::vector<int>>& board,
					const std::vector<std::vector<int>>& /* unused */ roomIds) {
	for (const auto& row : board) {
		for (int i = 0; i < row.size() - 1; i++) {
			std::cout << row[i] << " ";
		}
		std::cout << row.back() << std::endl;
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
