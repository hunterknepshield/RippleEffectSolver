//
//  main.cpp
//  RippleEffectCreator
//
//  Created by Hunter Knepshield on 3/5/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#include <chrono>
#include <iostream>
#include <map>
#include <thread>
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
	std::tie(roomMap, cellsCompletedInRoom) =
		generateRoomMapAndCompletedCellMap(cellValues, roomIds);

	if (!validateIncompleteBoard(cellValues, roomIds, roomMap)) {
		std::cerr << "Invalid initial board." << std::endl;
		return 1;
	}

	// To get the size of room n, use roomMap[n].size().
	// To get the value of cell (r, c), use cellValues[r][c].
	// To get the room ID of cell (r, c), use roomIds[r][c].

	bool solved;
	std::set<Board> boards;
	// The position of the previously modified cell within the loop below.
	int r = -1, c = -1;
	int overwrittenValue = -1;

	do {
		// Figure out what we know for sure right now.
		fillKnownCellsInBoard(cellValues, roomIds, roomMap,
							  cellsCompletedInRoom, VERBOSITY);
		std::cout << "Currently known cell values:" << std::endl;
		printBoard(cellValues, roomIds);

		std::cout << "Computing all solutions to the current board..."
				  << std::endl;
		bool* otherThreadFinished = new bool(false);
		// A cute little thread to only print the "this is taking a while"
		// message if it's actually taking a while. Cleans up after itself.
		std::thread printerThread([otherThreadFinished]() {
			// Prints a message about long-running operations after 10 seconds
			// if we're still computing all the solutions.
			std::this_thread::sleep_for(std::chrono::seconds(10));
			if (*otherThreadFinished) {
				// The main thread found all solutions before now and finished
				// already, so we need to clean up.
				delete otherThreadFinished;
			} else {
				// The main thread isn't done computing all the solutions yet,
				// so we let it clean up instead.
				*otherThreadFinished = true;
				std::cout << "If this takes a really long time, consider "
							 "filling in more cells and rerunning."
						  << std::endl;
			}
		});
		std::tie(solved, boards) = findAllSolutions(
			cellValues, roomIds, roomMap, cellsCompletedInRoom, VERBOSITY);
		if (*otherThreadFinished) {
			// The printer thread finished before us, so we clean up.
			delete otherThreadFinished;
		} else {
			// This finished before the printer thread, so we tell it we're done
			// and it will clean up.
			*otherThreadFinished = true;
			// Detach so we don't cause any issues when the stack variable goes
			// out of scope. C++11's threads don't support interrupts so this is
			// the best we can do. May cause minor memory leaks if the printer
			// thread gets killed by an exit before it deletes the bool.
			printerThread.detach();
		}
		if (boards.size() == 0) {
			std::cout << "No solutions to the current board." << std::endl;
			if (r == -1 && c == -1) {
				// We didn't even modify anything, this is just unsolvable based
				// on the original input.
				std::cout << "This puzzle cannot be solved." << std::endl;
				return 0;
			}
			std::cout << "Undoing previous value of " << cellValues[r][c]
					  << " at (" << (r + 1) << ", " << (c + 1) << ")."
					  << std::endl;
			cellValues[r][c] = overwrittenValue;  // Guaranteed not to be -1.
			cellsCompletedInRoom[roomIds[r][c]]--;
		} else if (boards.size() == 1) {
			std::cout << "Single solution to the current board:" << std::endl;
			printBoard(*boards.begin(), roomIds);
			return 0;
		} else {
			std::cout << "The puzzle currently has " << boards.size()
					  << " solutions." << std::endl;
			int solution = 0;
			for (const auto& board : boards) {
				std::cout << "Solution " << ++solution << ":" << std::endl;
				printBoard(board, roomIds);
			}
			std::cout << "Currently known cell values:" << std::endl;
			printBoard(cellValues, roomIds);
			int newValue;
		input:
			std::cout << "Choose a value to overwrite..." << std::endl;
		input_r:
			std::cout << "Value's row (1-indexed): ";
			std::cin >> r;
			r--;
			if (r < 0 || r >= cellValues.size()) {
				std::cerr << "Invalid location." << std::endl;
				goto input_r;  // Gasp!
			}
		input_c:
			std::cout << "Value's column (1-indexed): ";
			std::cin >> c;
			c--;
			if (c < 0 || c >= boardWidth) {
				std::cerr << "Invalid location." << std::endl;
				goto input_c;  // Oh my...
			}
		input_value:
			overwrittenValue = cellValues[r][c];
			std::cout << "Value (0 is unset, " << overwrittenValue
					  << " is current): ";
			std::cin >> newValue;
			if (newValue < 0 || newValue > roomMap[roomIds[r][c]].size()) {
				std::cerr << "Invalid value." << std::endl;
				goto input_value;  // What is the world coming to??
			} else if (newValue == overwrittenValue) {
				std::cerr << "Value already in place." << std::endl;
				goto input;  // Pretty sure I'm going to hell for this.
			}
			if (newValue != 0) {
				cellsCompletedInRoom[roomIds[r][c]]++;
			} else {
				cellsCompletedInRoom[roomIds[r][c]]--;
			}
			cellValues[r][c] = newValue;
		}
	} while (true);
}

int main(void) {
	std::cout
		<< "Generate random puzzle (g) or augment existing instance (a)? ";
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
