//
//  main.cpp
//  RippleEffectCreator
//
//  Created by Hunter Knepshield on 3/5/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#include <iostream>

#include "augment_puzzle.h"
#include "generate_puzzle.h"
#include "read_input.h"
#include "solve_puzzle.h"
#include "typedefs.h"
#include "validity_checks.h"

// Verbosity settings. 0 = silent, 1 = print message and board on modification.
// Not supplied as a parameter to solving routines, since those produce
// comparatively huge amounts of output.
int VERBOSITY = 1;
// Verbosity settings that are passed to solving routines. 0 = silent, 1 = print
// message on action, 2 = print message and board on action.
int SOLVING_VERBOSITY = 0;

int main(void) {
	while (true) {
		//	std::cout
		//		<< "Generate random puzzle (g) or augment existing instance (a)?
		//";
		char choice = 'g';
		// std::cin >> choice;
		switch (choice) {
			case 'g':
			case 'G':
				// Generate a room configuration according to the user-specified
				// width and height. Loop until a solvable board is generated or
				// the user quits.
				while (true) {
					int width, height;
					std::cout << "Specify the width of the puzzle to be "
								 "created (-1 to quit): ";
					std::cin >> width;
					if (width == -1) return 1;
					std::cout
						<< "Specify the height of the puzzle to be created: ";
					std::cin >> height;

					int seed;
					std::cout << "Input seed (integer): ";
					std::cin >> seed;

					Board cellValues, roomIds;
					std::tie(cellValues, roomIds) = generatePuzzle(
						width, height, seed, VERBOSITY, SOLVING_VERBOSITY);

					// Attempt to solve the puzzle.
					RoomMap roomMap;
					std::map<int, int> cellsCompletedInRoom;
					std::tie(roomMap, cellsCompletedInRoom) =
						generateRoomMapAndCompletedCellMap(cellValues, roomIds);

					if (!validateIncompleteBoard(cellValues, roomIds,
												 roomMap)) {
						std::cerr << "Invalid initial board." << std::endl;
						break;
					} else {
						std::cout
							<< "This board is valid. Checking for a solution..."
							<< std::endl;
						const auto& solvedWithBoard = findSingleSolution(
							cellValues, roomIds, roomMap, cellsCompletedInRoom,
							SOLVING_VERBOSITY);
						if (!solvedWithBoard.first) {
							std::cout
								<< "This board can't be solved. Trying again..."
								<< std::endl;
						} else {
							std::cout << "This board is solvable. Beginning "
										 "augmentation..."
									  << std::endl;
							return augmentPuzzle(cellValues, roomIds, VERBOSITY,
												 SOLVING_VERBOSITY);
						}
					}
				}
				return 1;
			case 'a':
			case 'A': {
				// Read in a partially solved board from the user then begin to
				// augment.
				std::cout << "Enter cell values and room IDs..." << std::endl;
				size_t boardWidth = 0;
				Board cellValues, roomIds;
				if (!readCells(&boardWidth, &cellValues) ||
					!readRooms(boardWidth, cellValues.size(), &roomIds)) {
					return 1;
				}
				return augmentPuzzle(cellValues, roomIds, VERBOSITY,
									 SOLVING_VERBOSITY);
			}
			default:
				std::cerr << "Invalid choice.";
		}
	}
}
