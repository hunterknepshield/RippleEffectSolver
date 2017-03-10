//
//  main.cpp
//  RippleEffectCreator
//
//  Created by Hunter Knepshield on 3/5/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#include <iostream>
#include <map>
#include <numeric>
#include <utility>
#include <vector>

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

	Board cellValues, roomIds;
	for (int r = 0; r < height; r++) {
		cellValues.emplace_back();
		roomIds.emplace_back();
		for (int c = 0; c < width; c++) {
			cellValues[r].emplace_back();
			roomIds[r].emplace_back();
		}
	}

	// TODO ensure that there aren't 2 1-cell rooms directly adjacent

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

	// The position of the last modified cell within the loop below.
	int r = -1, c = -1;
	int overwrittenValue = -1;

	// This copy keeps track of the ground truth that the user gives us, no
	// inferred cells.
	Board originalBoard = cellValues;

	do {
		// Wipe out any cells that we filled in without the user specifying a
		// value.
		cellValues = originalBoard;
		std::tie(roomMap, cellsCompletedInRoom) =
			generateRoomMapAndCompletedCellMap(cellValues, roomIds);
		// Figure out what we know for sure right now.
		fillKnownCellsInBoard(cellValues, roomIds, roomMap,
							  cellsCompletedInRoom, VERBOSITY);
		std::cout << "Currently known cell values:" << std::endl;
		printBoard(cellValues, roomIds);

		std::cout << "Computing all solutions to the current board..."
				  << std::endl;
		bool solved;
		std::set<Board> boards;
		int solutionCount = 0;
		std::tie(solved, boards) = findAllSolutions(
			cellValues, roomIds, roomMap, cellsCompletedInRoom, VERBOSITY,
			VERBOSITY > 0 ? &solutionCount : nullptr);
		if (!solved) {
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
			originalBoard[r][c] = overwrittenValue;  // Guaranteed not to be -1.
			cellsCompletedInRoom[roomIds[r][c]]--;
			r = -1;
			c = -1;
			overwrittenValue = -1;
		} else if (boards.size() == 1) {
			std::cout << "Single solution to the current board:" << std::endl;
			printBoard(*boards.begin(), roomIds);
			std::cout << "Initial board:" << std::endl;
			printBoard(originalBoard, roomIds);
#ifndef SIMPLE_PRINT_BOARD
			std::cout << "Raw initial board:" << std::endl;
			uglyPrintBoard(originalBoard, roomIds);
#endif /* SIMPLE_PRINT_BOARD */
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
			std::cout << "Aggregating cells across all solutions to see if any "
						 "other cells are known..."
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

			const auto& valueFrequencyForCell =
				generateValueFrequencies(boards);

			int newValue;
		input:
			std::cout << "Choose a cell to overwrite..." << std::endl;
		input_r:
			std::cout << "Enter cell's row (1-" << cellValues.size()
					  << ", 'f' to display value frequencies for all unknown "
						 "cells, 's' to display suggested cells to modify, 'i' "
						 "to display initial board state [i.e. only what the "
						 "user has supplied as input]): ";
			std::string raw_r;
			std::cin >> raw_r;
			if (raw_r == "f") {
				std::cout << "Value frequencies for unknown cells:"
						  << std::endl;
				for (int row = 0; row < valueFrequencyForCell.size(); row++) {
					for (int col = 0; col < valueFrequencyForCell[row].size();
						 col++) {
						if (valueFrequencyForCell[row][col].size() == 1)
							continue;  // This is a known cell, we don't need to
									   // bother printing this out.

						std::cout << "Value frequency for cell (" << row + 1
								  << ", " << col + 1 << "):" << std::endl;
						for (const auto& valueAndFrequency :
							 valueFrequencyForCell[row][col]) {
							std::cout
								<< '\t' << "Solutions with "
								<< valueAndFrequency.first
								<< " in this cell: " << valueAndFrequency.second
								<< std::endl;
						}
					}
				}
				goto input;  // I solemnly swear to never use this elsewhere.
			} else if (raw_r == "s") {
				bool suggested = false;
				int minFrequency = std::numeric_limits<int>::max();
				std::vector<int> suggestedRows, suggestedCols, suggestedValues;
				for (int row = 0; row < valueFrequencyForCell.size(); row++) {
					for (int col = 0; col < valueFrequencyForCell[row].size();
						 col++) {
						if (valueFrequencyForCell[row][col].size() == 1)
							continue;  // This is a known cell, we don't need to
									   // bother printing this out.

						for (const auto& valueAndFrequency :
							 valueFrequencyForCell[row][col]) {
							if (valueAndFrequency.second < minFrequency) {
								minFrequency = valueAndFrequency.second;
								suggestedRows.clear();
								suggestedCols.clear();
								suggestedValues.clear();
								suggestedRows.push_back(row);
								suggestedCols.push_back(col);
								suggestedValues.push_back(
									valueAndFrequency.first);
							} else if (valueAndFrequency.second ==
									   minFrequency) {
								suggestedRows.push_back(row);
								suggestedCols.push_back(col);
								suggestedValues.push_back(
									valueAndFrequency.first);
							}
							if (valueAndFrequency.second == 1) {
								std::cout
									<< "Cell (" << row + 1 << ", " << col + 1
									<< ") can be filled with a value of "
									<< valueAndFrequency.first
									<< " to eliminate all other solutions."
									<< std::endl;
								suggested = true;
							} else if (valueAndFrequency.second <=
									   boards.size() / 10) {
								std::cout
									<< "Cell (" << row + 1 << ", " << col + 1
									<< ") can be filled with a value of "
									<< valueAndFrequency.first
									<< " to eliminate >90% of other solutions."
									<< std::endl;
								suggested = true;
							}
						}
					}
				}
				if (!suggested) {
					for (int i = 0; i < suggestedRows.size(); i++) {
						std::cout << "Cell (" << suggestedRows[i] + 1 << ", "
								  << suggestedCols[i] + 1
								  << ") can be filled with a value of "
								  << suggestedValues[i]
								  << " to eliminate the largest number of "
									 "other solutions."
								  << std::endl;
					}
				}
				goto input;
			} else if (raw_r == "i") {
				std::cout << "Current initial board:" << std::endl;
				printBoard(originalBoard, roomIds);
				goto input;
			} else if (raw_r.empty() ||
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
			const auto& valueFrequencies = valueFrequencyForCell[r][c];
			for (const auto& valueAndFrequency : valueFrequencies) {
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
			} else if (newValue == 0 && originalBoard[r][c] == 0) {
				std::cerr << "This cell was inferred, not set by the user. "
							 "Clearing it will have no effect."
						  << std::endl;
				goto input_value;
			}
			if (newValue == 0) {
				cellsCompletedInRoom[roomIds[r][c]]--;
			} else {
				cellsCompletedInRoom[roomIds[r][c]]++;
			}
			originalBoard[r][c] = newValue;
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
				// TODO unimplemented
				return generateRandomPuzzle();
			case 'a':
			case 'A':
				return augmentExistingPuzzle();
			default:
				std::cerr << "Invalid choice. Enter 'g' or 'a'... ";
		}
	}
}
