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
	while (true) {
		int width, height;
		std::cout << "Specify the width of the puzzle to be created... ";
		std::cin >> width;
		std::cout << "Specify the height of the puzzle to be created... ";
		std::cin >> height;

		Board cellValues, roomIds;
		for (int r = 0; r < height; r++) {
			cellValues.emplace_back(width);
			roomIds.emplace_back(width);
		}

		std::string s;
		std::cout << "Input seed string." << std::endl;
		std::cin >> s;
		int seed = 0;
		for (const char& c : s) {
			seed += c;
		}
		std::cout << "Seed is " << seed << std::endl;
		std::srand(seed);

		int newRoomId = 1;

		int maxInt = std::numeric_limits<int>::max();
		// We will expand a room 50% of the time. If rand() returns a value
		// below this threshold, we'll expand an existing room.
		int defaultLimit = maxInt / 4 * 3;
		// Puzzles with huge amounts of big numbers aren't very exciting.
		// Prevent any rooms larger than this.
		int maxRoomSize = std::min(width, height) * 2 / 3;
		// Once we get to rooms of this size, make it harder and harder to have
		// a larger room.
		int beginningDifficultyIncrease = std::min(width, height) / 3;
		std::cout << "Default limit is " << defaultLimit << std::endl;
		std::cout << "Maximum room size is " << maxRoomSize
				  << ", difficulty begins increasing at "
				  << beginningDifficultyIncrease << std::endl;
		RoomMap roomMap;
		for (int r = 0; r < height; r++) {
			for (int c = 0; c < width; c++) {
				// Fill in from left to right, top to bottom.
				int roomId = 0;
				if (c == 0 && r == 0) {
					// We have to start a new room here for obvious reasons.
					roomIds[r][c] = newRoomId++;
				} else if (c == 0) {
					roomId = roomIds[r - 1][c];
				} else if (r == 0) {
					roomId = roomIds[r][c - 1];
				} else {
					// Roll to determine if we take from above or below.
					roomId = std::rand() < maxInt / 2 ? roomIds[r - 1][c]
													  : roomIds[r][c - 1];
				}
				if (roomId != 0) {
					int roomSize = (int)roomMap[roomId].size();
					bool expand;
					if (roomSize >= beginningDifficultyIncrease) {
						// Expansion chance scales according to the current room
						// size. Bigger rooms need to be  harder harder to
						// expand, so they get lower limits. Slightly weird
						// order of operations to ensure the most accurate value
						// since this is integer division.
						int adjustedLimit = defaultLimit / maxRoomSize *
											(maxRoomSize - roomSize);
						std::cout << "Room " << roomId
								  << " is currently of size " << roomSize
								  << ". To expand, need to be less than "
								  << adjustedLimit << std::endl;
						expand = std::rand() < adjustedLimit;
					} else {
						expand = std::rand() < defaultLimit;
					}
					roomIds[r][c] = expand ? roomId : newRoomId++;
				}
				roomMap[roomIds[r][c]].push_back({r, c});
			}
		}

		std::cout << "Board before smoothing:" << std::endl;
		printBoard(cellValues, roomIds);

		// We now have a fairly decent looking board, but it may still have
		// nasty invalid bits:
		// - Neighboring 1x1 rooms.
		// - Neighboring 2x2 rooms with all 4 cells in a straight line. (TODO)
		for (int r = 0; r < height; r++) {
			for (int c = 0; c < width; c++) {
				int roomId = roomIds[r][c];
				if (roomMap[roomId].size() == 1) {
					bool singleBelow = false, singleRight = false;
					// Check neighboring rooms. If any of them are also 1x1, we
					// need to do something to fix this invalid state.
					if (r != height - 1) {
						int roomIdBelow = roomIds[r + 1][c];
						if (roomMap[roomIdBelow].size() == 1) {
							std::cout << "Cell (" << r + 2 << ", " << c + 1
									  << ") is 1x1 too." << std::endl;
							singleBelow = true;
						}
					}
					if (c != width - 1) {
						int roomIdRight = roomIds[r][c + 1];
						if (roomMap[roomIdRight].size() == 1) {
							std::cout << "Cell (" << r + 1 << ", " << c + 2
									  << ") is 1x1 too." << std::endl;
							singleRight = true;
						}
					}
					if (singleBelow && singleRight) {
						// Merge all 3, take the ID of the one to the right.
						int roomIdBelow = roomIds[r + 1][c],
							roomIdRight = roomIds[r][c + 1];
						roomMap.erase(roomIdBelow);
						roomMap.erase(roomId);
						roomIds[r][c] = roomIdRight;
						roomIds[r + 1][c] = roomIdRight;
						roomMap[roomIdRight].push_back({r, c});
						roomMap[roomIdRight].push_back({r + 1, c});
					} else if (singleBelow || singleRight) {
						// Merge with the other cell.
						int mergedRoomId;
						if (singleBelow) {
							mergedRoomId = roomIds[r + 1][c];
						} else {
							mergedRoomId = roomIds[r][c + 1];
						}
						roomMap.erase(roomId);
						roomIds[r][c] = mergedRoomId;
						roomMap[mergedRoomId].push_back({r, c});
					}
				}
			}
		}

		std::cout << "Board after smoothing:" << std::endl;
		printBoard(cellValues, roomIds);
#ifndef SIMPLE_PRINT_BOARD
		switch (VERBOSITY) {
			case 2:
			case 1:
				uglyPrintBoard(cellValues, roomIds);
			default:
				break;
		}
#endif /* SIMPLE_PRINT_BOARD */

		// Rebuild some info and attempt the puzzle.
		std::map<int, int> cellsCompletedInRoom;
		std::tie(roomMap, cellsCompletedInRoom) =
			generateRoomMapAndCompletedCellMap(cellValues, roomIds);

		if (!validateIncompleteBoard(cellValues, roomIds, roomMap)) {
			std::cerr << "Invalid initial board." << std::endl;
			break;
		} else {
			std::cout
				<< "This board appears to be valid. Computing all solutions..."
				<< std::endl;
			const auto& solvedWithBoards = findAllSolutions(
				cellValues, roomIds, roomMap, cellsCompletedInRoom, VERBOSITY);
			if (!solvedWithBoards.first) {
				std::cout << "This board can't be solved." << std::endl;
			} else {
				std::cout << "Solved this board with "
						  << solvedWithBoards.second.size() << " solutions."
						  << std::endl;
				int solution = 0;
				for (const auto& board : solvedWithBoards.second) {
					std::cout << "Solution " << ++solution << ":" << std::endl;
					printBoard(board, roomIds);
				}
			}
		}

		std::cout << "Enter 'q' to stop (anything else to continue)."
				  << std::endl;
		char c;
		std::cin >> c;
		if (c == 'q') break;
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

	// The position of the last modified cell within the loop below.
	int r = -1, c = -1;
	int overwrittenValue = -1;

	// This copy keeps track of the ground truth that the user gives us, no
	// inferred cells; the canonical board.
	Board originalBoard = cellValues;

	do {
		// Wipe out any cells that we filled in without the user specifying a
		// value. This prevents issues with things like the user attempting to
		// overwrite an inferred cell, which might produce a bad (read:
		// unsolvable) puzzle state.
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
			// We've run into a dead end. Undo if we can, otherwise fail.
			std::cout << "No solutions to the current board." << std::endl;
			if (r == -1 && c == -1) {
				// We didn't even modify anything, this is just unsolvable based
				// on the original input.
				std::cout << "This puzzle cannot be solved." << std::endl;
				return 0;
			}
			std::cerr << "Undoing previous value of " << cellValues[r][c]
					  << " at (" << (r + 1) << ", " << (c + 1) << ")."
					  << std::endl;
			originalBoard[r][c] = overwrittenValue;  // Guaranteed not to be -1.
			cellsCompletedInRoom[roomIds[r][c]]--;
			r = -1;
			c = -1;
			overwrittenValue = -1;
		} else if (boards.size() == 1) {
			// We've finally reached a single solution. Dump out the pertinent
			// stuff then we're done.
			std::cout << "Single solution to the current board:" << std::endl;
			printBoard(*boards.begin(), roomIds);
#ifndef SIMPLE_PRINT_BOARD
			std::cout << "Raw solution:" << std::endl;
			uglyPrintBoard(*boards.begin(), roomIds);
#endif /* SIMPLE_PRINT_BOARD */
			std::cout << "Initial board:" << std::endl;
			printBoard(originalBoard, roomIds);
#ifndef SIMPLE_PRINT_BOARD
			std::cout << "Raw initial board:" << std::endl;
			uglyPrintBoard(originalBoard, roomIds);
#endif /* SIMPLE_PRINT_BOARD */
			return 0;
		} else {
			// This is the longest case by far. We have multiple solutions.
			// First, give the user some info like what we know for sure,
			// then ask them to modify a cell and loop.
			switch (VERBOSITY) {
				case 2:
				case 1: {
					int solution = 1;
					for (const auto& board : boards) {
						std::cout << "Solution " << solution++ << ":"
								  << std::endl;
						printBoard(board, roomIds);
					}
				}
				default:
					break;
			}

			// Now that we've computed all the possible solutions, we can look
			// at what cells are the same across every one of them.
			int canonicalCells = countKnownCells(originalBoard);
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
				std::cout << "Currently known cell values:" << std::endl;
				printBoard(cellValues, roomIds);
			} else {
				std::cout << "No new known cells after aggregation."
						  << std::endl;
			}

			// General difficulty-esque info
			std::cout << "There " << (afterAggregation == 1 ? "is" : "are")
					  << " currently " << afterAggregation << " known cell"
					  << (afterAggregation == 1 ? "" : "s") << ", "
					  << canonicalCells << " of which "
					  << (canonicalCells == 1 ? "was" : "were")
					  << " given as input, and " << aggregationDifference
					  << " of which "
					  << (aggregationDifference == 1 ? "was" : "were")
					  << " determined only after aggregating all solutions."
					  << std::endl;
			int unknownCells = (int)(cellValues.size() * cellValues[0].size()) -
							   afterAggregation;
			std::cout << "There " << (unknownCells == 1 ? "is" : "are")
					  << " currently " << unknownCells << " unknown cell"
					  << (unknownCells == 1 ? "" : "s") << "." << std::endl;
			std::cout << "The puzzle currently has " << boards.size()
					  << " solutions." << std::endl;

			const auto& valueFrequencyForCell =
				generateValueFrequencies(boards);

		// Now ask for a cell to overwrite.
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
				// User wants all value frequencies.
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
				// User is asking for suggestions of cells to fill.
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
									<< " to eliminate "
									<< boards.size() - valueAndFrequency.second
									<< " other solutions, leaving "
									<< valueAndFrequency.second
									<< " remaining solutions." << std::endl;
								suggested = true;
							}
						}
					}
				}
				if (!suggested) {
					// Suggest all cells that had a value with the lowest
					// frequency seen across all solutions. This obviously
					// exists, even if we didn't suggest something smarter from
					// above.
					for (int i = 0; i < suggestedRows.size(); i++) {
						std::cout << "Cell (" << suggestedRows[i] + 1 << ", "
								  << suggestedCols[i] + 1
								  << ") can be filled with a value of "
								  << suggestedValues[i]
								  << " to eliminate the largest number of "
									 "other solutions, leaving "
								  << minFrequency << " remaining solutions."
								  << std::endl;
					}
				}
				goto input;
			} else if (raw_r == "i") {
				// User wants the initial board state.
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
			// User gave a row to modify.
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

			overwrittenValue = originalBoard[r][c];
			int newValue;
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
			} else if (newValue == 0 && overwrittenValue == 0 &&
					   cellValues[r][c] != 0) {
				std::cerr << "This cell was inferred, not set by the user. "
							 "Clearing it will have no effect."
						  << std::endl;
				goto input_value;
				// If the user attempts to override an inferred value with some
				// other non-zero value, the solver will most likely fail and
				// undo it automatically. I have yet to find an instance that
				// won't cause this to happen.
			} else if (newValue == overwrittenValue) {
				std::cerr << "That value already in place." << std::endl;
				goto input_value;  // Pretty sure I'm going to hell for this.
			}

			if (newValue == 0) {
				cellsCompletedInRoom[roomIds[r][c]]--;
			} else {
				cellsCompletedInRoom[roomIds[r][c]]++;
			}
			originalBoard[r][c] = newValue;  // This is canonical input.
		}
	} while (true);
}

int main(void) {
	//	std::cout
	//		<< "Generate random puzzle (g) or augment existing instance (a)? ";
	char choice = 'g';
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
