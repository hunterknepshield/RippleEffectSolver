//
//  print_board.cpp
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/9/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#include "print_board.hpp"

#include <bitset>
#include <iostream>
#include <vector>

#ifdef UGLY_PRINT_BOARD

// The ugly, simple version of the printing routine. Doesn't show room borders,
// just cells' values.
void printBoard(const std::vector<std::vector<int>>& board,
				const std::vector<std::vector<int>>& /* unused */ roomIds) {
	for (const auto& row : board) {
		for (int i = 0; i < row.size() - 1; i++) {
			std::cout << row[i] << ' ';
		}
		std::cout << row.back() << std::endl;
	}
}

#else /* !UGLY_PRINT_BOARD */

// String constants for easy usage. N > S > E > W in names.
// Also #define'd away since they need the weird scary string types because
// these are Unicode characters, not boring ASCII (i.e. not normal `char`).
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

// The pretty version of the board printing routine. Uses special Unicode
// characters to show room borders in addition to the cells' values. However,
// these characters don't play nice with all fonts.
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
			if (board[i][j]) {
				std::cout << board[i][j];
			} else {
				std::cout << ' ';
			}
			if (roomIds[i][j] != roomIds[i][j + 1]) {
				std::cout << NS;
			} else {
				std::cout << ' ';
			}
		}
		if (board[i].back()) {
			std::cout << board[i].back();
		} else {
			std::cout << ' ';
		}
		std::cout << NS << std::endl;
		
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
				std::cout << ' ';
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
					std::cout << ' ';
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
			std::cout << ' ' << NS;
		}
		std::cout << std::endl;
	}
	
	// Last row of cell contents.
	std::cout << NS;
	for (int i = 0; i < board.back().size() - 1; i++) {
		if (board.back()[i]) {
			std::cout << board.back()[i];
		} else {
			std::cout << ' ';
		}
		if (roomIds.back()[i] != roomIds.back()[i + 1]) {
			std::cout << NS;
		} else {
			std::cout << ' ';
		}
	}
	if (board.back().back()) {
		std::cout << board.back().back();
	} else {
		std::cout << ' ';
	}
	std::cout << NS << std::endl;
	
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

#endif /* UGLY_PRINT_BOARD */