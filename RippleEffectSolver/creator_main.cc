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
#include <numeric>
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
		int solutionCount = 0;
		std::tie(solved, boards) = findAllSolutions(
			cellValues, roomIds, roomMap, cellsCompletedInRoom, VERBOSITY,
			VERBOSITY > 0 ? &solutionCount : nullptr);
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
			int solution = 0;
			for (const auto& board : boards) {
				std::cout << "Solution " << ++solution << ":" << std::endl;
				printBoard(board, roomIds);
			}
			// Now that we've computed all the possible solutions, we can look
			// at what cells are the same across every one of them.
			// TODO is this valid to assume that these are "known"? Pretty sure.
			int beforeAggregation = countKnownCells(cellValues);
			std::cout << "Currently known cell values:" << std::endl;
			printBoard(cellValues, roomIds);
			std::cout << "Aggregating cells across all solutions to see if we "
						 "know anything else..."
					  << std::endl;
			cellValues = aggregateBoards(boards);
			int afterAggregation = countKnownCells(cellValues);
			int aggregationDifference = afterAggregation - beforeAggregation;
			if (aggregationDifference > 0) {
				std::tie(roomMap, cellsCompletedInRoom) =
					generateRoomMapAndCompletedCellMap(cellValues, roomIds);
				std::cout << "Determined " << aggregationDifference
						  << " more known cell"
						  << (aggregationDifference == 1 ? "" : "s")
						  << " after aggregating all solutions." << std::endl;
				std::cout << "Currently known cell values:" << std::endl;
				printBoard(cellValues, roomIds);
			} else {
				std::cout << "No new known cells after aggregation."
						  << std::endl;
			}
			std::cout << "There " << (afterAggregation == 1 ? "is" : "are")
					  << " currently " << afterAggregation << " known cell"
					  << (afterAggregation == 1 ? "" : "s") << "." << std::endl;
			int unknownCells = (int)(cellValues.size() * cellValues[0].size()) -
							   afterAggregation;
			std::cout << "There " << (unknownCells == 1 ? "is" : "are")
					  << " currently " << unknownCells << " unknown cell"
					  << (unknownCells == 1 ? "" : "s") << "." << std::endl;
			std::cout << "The puzzle currently has " << boards.size()
					  << " solutions." << std::endl;

			const auto& valueFrequencyForCell = analyzeSolutions(boards);

			int newValue;
		input:
			std::cout << "Choose a cell to overwrite..." << std::endl;
		input_r:
			std::cout << "Enter cell's row (1-" << cellValues.size()
					  << ", '?' to display value frequencies for all unknown "
						 "cells): ";
			std::string raw_r;
			std::cin >> raw_r;
			if (raw_r == "?") {
				std::cout << "Value frequencies for unknown cells:"
						  << std::endl;
				for (const auto& cellAndValueFrequency :
					 valueFrequencyForCell) {
					if (cellAndValueFrequency.second.size() == 1)
						continue;  // This is a known cell, we don't need to
								   // bother printing this out.
					const auto& cellLocation = cellAndValueFrequency.first;
					std::cout << "Value frequency for cell ("
							  << cellLocation.first + 1 << ", "
							  << cellLocation.second + 1 << "):" << std::endl;
					for (const auto& valueAndFrequency :
						 cellAndValueFrequency.second) {
						std::cout
							<< '\t' << "Solutions with "
							<< valueAndFrequency.first
							<< " in this cell: " << valueAndFrequency.second
							<< std::endl;
					}
				}
				goto input;  // I solemnly swear to never use this elsewhere.
			}
			if (raw_r.empty() ||
				!std::accumulate(raw_r.begin(), raw_r.end(), true,
								 [](bool so_far, char c) {
									 return so_far && std::isdigit(c);
								 })) {
				std::cerr << "Invalid input." << std::endl;
				goto input_r;
			}
			r = std::stoi(raw_r);
			r--;
			if (r < 0 || r >= cellValues.size()) {
				std::cerr << "Invalid row." << std::endl;
				goto input_r;  // Gasp!
			}
		input_c:
			std::cout << "Enter cell's column (1-" << cellValues[r].size()
					  << "): ";
			std::cin >> c;
			c--;
			if (c < 0 || c >= boardWidth) {
				std::cerr << "Invalid column." << std::endl;
				goto input_c;  // Oh my...
			}
			std::cout << "Value frequency for cell (" << r + 1 << ", " << c + 1
					  << "):" << std::endl;
			const auto& valueFrequency = valueFrequencyForCell.at({r, c});
			for (const auto& valueAndFrequency : valueFrequency) {
				std::cout << '\t' << "Solutions with "
						  << valueAndFrequency.first
						  << " in this cell: " << valueAndFrequency.second
						  << std::endl;
			}
			overwrittenValue = cellValues[r][c];
		input_value:
			std::cout << "Enter new value (0 clears the cell, "
					  << overwrittenValue
					  << " is the current value, -1 to cancel and choose a "
						 "different cell): ";
			std::cin >> newValue;
			if (newValue == -1) {
				goto input;  // The more I use it, the more useful it seems...
			} else if (newValue < 0 ||
					   newValue > roomMap[roomIds[r][c]].size()) {
				std::cerr << "Invalid value." << std::endl;
				goto input_value;  // What is the world coming to??
			} else if (newValue == overwrittenValue) {
				std::cerr << "That value already in place." << std::endl;
				goto input_value;  // Pretty sure I'm going to hell for this.
			}
			if (newValue == 0) {
				// Warn the user just in case.
				std::cout << "Warning! Overwriting already-known cells may "
							 "invalidate other already-known cells, but that "
							 "will not be reflected here."
						  << std::endl;
				cellsCompletedInRoom[roomIds[r][c]]--;
			} else {
				cellsCompletedInRoom[roomIds[r][c]]++;
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
		switch (choice) {
			case 'g':
			case 'G':
				return generateRandomPuzzle();
			case 'a':
			case 'A':
				return augmentExistingPuzzle();
			default:
				std::cerr << "Invalid choice. Enter 'g' or 'a'... ";
		}
	}
}
