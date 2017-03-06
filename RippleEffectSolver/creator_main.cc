//
//  main.cpp
//  RippleEffectCreator
//
//  Created by Hunter Knepshield on 3/5/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#include <iostream>
#include <utility>

#include "print_board.h"
#include "read_input.h"
#include "solve_puzzle.h"
#include "typedefs.h"
#include "validity_checks.h"

// Verbosity settings. 0 = silent, 1 = print message on action, 2 = print
// message and board on action.
int VERBOSITY = 0;

int generateRandomPuzzle() {
	int width, height;
	std::cout << "Specify the width of the puzzle to be created... ";
	std::cin >> width;
	std::cout << "Specify the height of the puzzle to be created... ";
	std::cin >> height;
	
	// TODO
	Board cellValues, roomIds;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			// cellValues[i][j];
		}
	}
	
	return 999;
}

int augmentExistingPuzzle() {
	std::cout << "Enter cell values and room IDs..." << std::endl;
	size_t boardWidth = 0;
	Board cellValues, roomIds;
	if (!readCells(&boardWidth, &cellValues) ||
		!readRooms(boardWidth, cellValues.size(), &roomIds)) {
		return 1;
	}
	std::cout << "Initial board state:" << std::endl;
	printBoard(cellValues, roomIds);
	
	// Now some initial setup...
	// Maps room ID to a list of pairs of cell coordinates in the room.
	RoomMap roomMap;
	// Maps room ID to the number of cells completed in the room.
	std::map<int, int> cellsCompletedInRoom;
	std::tie(roomMap, cellsCompletedInRoom) = generateRoomMapAndCompletedCellMap(cellValues, roomIds);
	
	if (!validateIncompleteBoard(cellValues, roomIds, roomMap)) {
		std::cerr << "Invalid initial board." << std::endl;
		return 1;
	}
	
	// To get the size of room n, use roomMap[n].size().
	// To get the value of cell (r, c), use cellValues[r][c].
	// To get the room ID of cell (r, c), use roomIds[r][c].

	bool solved;
	std::set<Board> boards;
	std::tie(solved, boards) = findAllSolutions(cellValues, roomIds, roomMap, cellsCompletedInRoom, VERBOSITY);
	int r = -1, c = -1;
	
	while (boards.size() != 1) {
		if (boards.size() == 0) {
			std::cout << "No solutions to the current board." << std::endl;
			if (r == -1 && c == -1) {
				std::cout << "This puzzle cannot be solved." << std::endl;
				return 0;
			}
			std::cout << "Undoing previous value of " << cellValues[r][c] << " at (" << r << ", " << c << ")." << std::endl;
			cellValues[r][c] = 0;
			cellsCompletedInRoom[roomIds[r][c]]--;
		} else {
			std::cout << "The puzzle currently has " << boards.size() << " solutions." << std::endl;
			int solution = 0;
			for (const auto& board : boards) {
				std::cout << "Solution " << ++solution << ":" << std::endl;
				printBoard(board, roomIds);
			}
			int newValue;
			std::cout << "Choose a value to overwrite..." << std::endl;
			std::cout << "Value's row: ";
			std::cin >> r;
			std::cout << "Value's column: ";
			std::cin >> c;
			std::cout << "Value: ";
			std::cin >> newValue;
			if (newValue < 0 || newValue > roomMap[roomIds[r][c]].size() + 1 || r < 0 || r > cellValues.size() || c < 0 || c > boardWidth) {
				std::cerr << "Invalid value." << std::endl;
				continue;
			} else if (newValue == cellValues[r][c]) {
				std::cerr << "Value already in place." << std::endl;
				continue;
			}
			if (newValue != 0) {
				cellsCompletedInRoom[roomIds[r][c]]++;
			} else {
				cellsCompletedInRoom[roomIds[r][c]]--;
			}
			cellValues[r][c] = newValue;
		}
		
		std::tie(solved, boards) = findAllSolutions(cellValues, roomIds, roomMap, cellsCompletedInRoom, VERBOSITY);
	}
	
	std::cout << "Single solution to the current puzzle:" << std::endl;
	printBoard(*boards.begin(), roomIds);

	return 0;
}

int main(void) {
	std::cout << "Generate random puzzle (g) or augment existing instance (a)? ";
	char choice = 'a';
	while (true) {
		// std::cin >> choice;
		if (choice == 'g' || choice == 'G') {
			return generateRandomPuzzle();
		} else if (choice == 'a' || choice == 'A') {
			return augmentExistingPuzzle();
		} else {
			std::cerr << "Invalid choice. Enter 'g' or 'a'... ";
		}
	}
}
