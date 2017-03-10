//
//  print_board.cc
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/9/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#include "print_board.h"

#include <bitset>
#include <iostream>
#include <vector>

#include "typedefs.h"

void uglyPrintBoard(const Board& cellValues, const Board& roomIds) {
	for (const auto& board : {cellValues, roomIds}) {
		for (const auto& row : board) {
			for (int c = 0; c < row.size() - 1; c++) {
				std::cout << row[c] << ' ';
			}
			std::cout << row.back() << std::endl;
		}
		std::cout << std::endl;
	}
}

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

void prettyPrintBoard(const Board& cellValues, const Board& roomIds) {
	// Top row of board symbols.
	std::cout << SE;
	for (int c = 0; c < cellValues[0].size() - 1; c++) {
		std::cout << EW;
		if (roomIds[0][c] != roomIds[0][c + 1]) {
			std::cout << SEW;
		} else {
			std::cout << EW;
		}
	}
	std::cout << EW << SW << std::endl;

	// Interior rows of cell values and board symbols.
	for (int r = 0; r < cellValues.size() - 1; r++) {
		// Row of cell contents.
		std::cout << NS;
		for (int c = 0; c < cellValues[r].size() - 1; c++) {
			if (cellValues[r][c]) {
				std::cout << cellValues[r][c];
			} else {
				std::cout << ' ';
			}
			if (roomIds[r][c] != roomIds[r][c + 1]) {
				std::cout << NS;
			} else {
				std::cout << ' ';
			}
		}
		if (cellValues[r].back()) {
			std::cout << cellValues[r].back();
		} else {
			std::cout << ' ';
		}
		std::cout << NS << std::endl;

		// Row of board symbols.
		if (roomIds[r][0] != roomIds[r + 1][0]) {
			std::cout << NSE;
		} else {
			std::cout << NS;
		}
		for (int c = 0; c < cellValues[r].size() - 1; c++) {
			if (roomIds[r][c] != roomIds[r + 1][c]) {
				std::cout << EW;
			} else {
				std::cout << ' ';
			}

			// Space between four cells. Could be any of the special characters.
			// No particularly easy way to do this.
			std::bitset<4> space;  // Empty for now
			if (roomIds[r][c] != roomIds[r][c + 1]) {
				space[0].flip();  // N
			}
			if (roomIds[r + 1][c] != roomIds[r + 1][c + 1]) {
				space[1].flip();  // S
			}
			if (roomIds[r][c + 1] != roomIds[r + 1][c + 1]) {
				space[2].flip();  // E
			}
			if (roomIds[r][c] != roomIds[r + 1][c]) {
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
					std::cerr << "Invalid value for calculating square of 4: "
							  << space << std::endl;
					std::cout << "?";
			}
		}
		if (roomIds[r].back() != roomIds[r + 1].back()) {
			std::cout << EW << NSW;
		} else {
			std::cout << ' ' << NS;
		}
		std::cout << std::endl;
	}

	// Last row of cell contents.
	std::cout << NS;
	for (int c = 0; c < cellValues.back().size() - 1; c++) {
		if (cellValues.back()[c]) {
			std::cout << cellValues.back()[c];
		} else {
			std::cout << ' ';
		}
		if (roomIds.back()[c] != roomIds.back()[c + 1]) {
			std::cout << NS;
		} else {
			std::cout << ' ';
		}
	}
	if (cellValues.back().back()) {
		std::cout << cellValues.back().back();
	} else {
		std::cout << ' ';
	}
	std::cout << NS << std::endl;

	// Last row of board symbols.
	std::cout << NE;
	for (int c = 0; c < cellValues.back().size() - 1; c++) {
		std::cout << EW;
		if (roomIds.back()[c] != roomIds.back()[c + 1]) {
			std::cout << NEW;
		} else {
			std::cout << EW;
		}
	}
	std::cout << EW << NW << std::endl;
}
