//
//  generate_puzzle.cc
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 3/16/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#include "generate_puzzle.h"

#include <iostream>
#include <numeric>
#include <set>
#include <utility>

#include "print_board.h"
#include "typedefs.h"

std::pair<Board, Board> generatePuzzle(int width, int height, int seed,
									   int verbosity, int solvingVerbosity) {
	Board cellValues, roomIds;
	for (int r = 0; r < height; r++) {
		cellValues.emplace_back(width);
		roomIds.emplace_back(width);
	}
	std::srand(seed);

	int newRoomId = 1;

	int maxInt = std::numeric_limits<int>::max();
	// We will expand a room 50% of the time. If rand() returns a value
	// below this threshold, we'll expand an existing room.
	int defaultLimit = maxInt / 5 * 4;
	// Puzzles with huge amounts of big numbers aren't very exciting.
	// Prevent any rooms larger than this.
	int maxRoomSize = std::min(width, height) * 2 / 3;
	// Once we get to rooms of this size, make it harder and harder to have
	// a larger room.
	int beginningDifficultyIncrease = std::min(width, height) / 2;
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
					int adjustedLimit =
						defaultLimit / maxRoomSize * (maxRoomSize - roomSize);
					expand = std::rand() < adjustedLimit;
				} else {
					expand = std::rand() < defaultLimit;
				}
				roomIds[r][c] = expand ? roomId : newRoomId++;
			}
			roomMap[roomIds[r][c]].push_back({r, c});
		}
	}
	switch (verbosity) {
		case 2:
		case 1:
			std::cout << "Initial random room generation:" << std::endl;
			printBoard(cellValues, roomIds);
			std::cout << "Beginning smoothing..." << std::endl;
		default:
			break;
	}

	// We now have a fairly decent looking board, but it may still have
	// nasty invalid bits:
	// - Adjacent 1x1 rooms.
	// - Adjacent 2x1 rooms that form a 4x1 line.
	// - 3 or more contiguous 2x1 rooms.
	// - A 1x1 in-between two 2x1 rooms.
	// Since merging 1x1 rooms may produce 2x1 rooms, these steps must be
	// done in sequence.

	// Adjacent 1x1s
	bool smoothed = false;
	for (int r = 0; r < height; r++) {
		for (int c = 0; c < width; c++) {
			int roomId = roomIds[r][c];
			if (roomMap[roomId].size() == 1) {
				// Find neighboring 1x1 rooms, if any, and merge them.
				bool singleBelow = false, singleRight = false;
				// Check neighboring rooms. If any of them are also 1x1, we
				// need to do something to fix this invalid state.
				if (r != height - 1) {
					int roomIdBelow = roomIds[r + 1][c];
					if (roomMap[roomIdBelow].size() == 1) {
						singleBelow = true;
					}
				}
				if (c != width - 1) {
					int roomIdRight = roomIds[r][c + 1];
					if (roomMap[roomIdRight].size() == 1) {
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
					smoothed = true;
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
					smoothed = true;
				}
			}
		}
	}
	switch (verbosity) {
		case 2:
		case 1:
			if (smoothed) {
				std::cout << "Board after smoothing 1x1s:" << std::endl;
				printBoard(cellValues, roomIds);
			} else {
				std::cout << "No change after smoothing 1x1s." << std::endl;
			}
		default:
			break;
	}

	// Linear 2x1s
	smoothed = false;
	for (int r = 0; r < height; r++) {
		for (int c = 0; c < width; c++) {
			int roomId = roomIds[r][c];
			if (roomMap[roomId].size() == 2) {
				// Look for adjacent 2x1 rooms that align with this one.
				const auto& cells = roomMap[roomId];
				const auto &first = cells.front(), &second = cells.back();
				bool searchInRow = (first.first == second.first);
				int largerInDirection =
					std::max(searchInRow ? first.second : first.first,
							 searchInRow ? second.second : second.first);
				if (largerInDirection < (searchInRow ? width : height) - 2) {
					// We can still fit another 2x1 room in this space, so
					// we have to check it.
					int nextRoomId = searchInRow
										 ? roomIds[r][largerInDirection + 1]
										 : roomIds[largerInDirection + 1][c];
					if (roomMap[nextRoomId].size() == 2) {
						// Ensure that it's the same orientation as the
						// current room.
						const auto& nextRoomCells = roomMap[nextRoomId];
						const auto &next1 = nextRoomCells.front(),
								   &next2 = nextRoomCells.back();
						bool sameAlignment =
							(searchInRow ? next1.first == next2.first
										 : next1.second == next2.second);
						if (sameAlignment) {
							// Now we merge these two rooms, trashing the
							// current room ID.
							roomIds[first.first][first.second] = nextRoomId;
							roomIds[second.first][second.second] = nextRoomId;
							roomMap[nextRoomId].push_back(first);
							roomMap[nextRoomId].push_back(second);
							roomMap.erase(roomId);
							smoothed = true;
						}
					}
				}
			}
		}
	}
	switch (verbosity) {
		case 2:
		case 1:
			if (smoothed) {
				std::cout << "Board after smoothing linear 2x1s:" << std::endl;
				printBoard(cellValues, roomIds);
			} else {
				std::cout << "No change after smoothing linear 2x1s."
						  << std::endl;
			}
		default:
			break;
	}

	// 3+ adjacent 2x1s
	smoothed = false;
	std::vector<int> twoByOnes;
	for (const auto& roomAndCells : roomMap) {
		if (roomAndCells.second.size() == 2)
			twoByOnes.push_back(roomAndCells.first);
	}
	for (const auto& thisTwoByOne : twoByOnes) {
		// Check if we can reach two other 2x1s from here.
		const auto& cellsInRoom = roomMap[thisTwoByOne];
		std::set<int> otherTwoByOnes;
		for (const auto& cell : cellsInRoom) {
			int otherRoomId;
			if (cell.first != 0) {
				otherRoomId = roomIds[cell.first - 1][cell.second];
				if (otherRoomId != thisTwoByOne &&
					roomMap[otherRoomId].size() == 2) {
					otherTwoByOnes.insert(otherRoomId);
				}
			}
			if (cell.first != height - 1) {
				otherRoomId = roomIds[cell.first + 1][cell.second];
				if (otherRoomId != thisTwoByOne &&
					roomMap[otherRoomId].size() == 2) {
					otherTwoByOnes.insert(otherRoomId);
				}
			}
			if (cell.second != 0) {
				otherRoomId = roomIds[cell.first][cell.second - 1];
				if (otherRoomId != thisTwoByOne &&
					roomMap[otherRoomId].size() == 2) {
					otherTwoByOnes.insert(otherRoomId);
				}
			}
			if (cell.second != width - 1) {
				otherRoomId = roomIds[cell.first][cell.second + 1];
				if (otherRoomId != thisTwoByOne &&
					roomMap[otherRoomId].size() == 2) {
					otherTwoByOnes.insert(otherRoomId);
				}
			}
		}
		if (otherTwoByOnes.size() > 1) {
			// Make every cell have this room's ID
			// TODO investigate if all 3+ must be merged, or just taking out
			// 2 would do the trick.
			int r, c;
			// for (const auto& otherTwoByOne : otherTwoByOnes) {
			int otherTwoByOne = *otherTwoByOnes.begin();
			const auto& otherCells = roomMap[otherTwoByOne];
			for (const auto& otherCell : otherCells) {
				std::tie(r, c) = otherCell;
				roomIds[r][c] = thisTwoByOne;
				roomMap[thisTwoByOne].push_back(otherCell);
			}
			roomMap.erase(otherTwoByOne);
			// }
			smoothed = true;
		}
	}
	switch (verbosity) {
		case 2:
		case 1:
			if (smoothed) {
				std::cout << "Board after smoothing 3+ adjacent 2x1s:"
						  << std::endl;
				printBoard(cellValues, roomIds);
			} else {
				std::cout << "No change after smoothing 3+ adjacent 2x1s."
						  << std::endl;
			}
		default:
			break;
	}

	// 1x1 in-between two 2x1s
	smoothed = false;
	for (const auto& roomAndCells : roomMap) {
		if (roomAndCells.second.size() == 1) {
			int r, c;
			std::tie(r, c) = roomAndCells.second.front();
			int mergeRoomId1 = 0, mergeRoomId2 = 0;
			if (r != 0 && r != height - 1) {
				// Check above and below
				int roomAbove = roomIds[r - 1][c];
				int roomBelow = roomIds[r + 1][c];
				if (roomMap[roomAbove].size() == 2 &&
					roomMap[roomBelow].size() == 2) {
					mergeRoomId1 = roomAbove;
					mergeRoomId2 = roomBelow;
				}
			}
			if (c != 0 && c != width - 1) {
				// Check right and left
				int roomLeft = roomIds[r][c - 1];
				int roomRight = roomIds[r][c + 1];
				if (roomMap[roomLeft].size() == 2 &&
					roomMap[roomRight].size() == 2) {
					mergeRoomId1 = roomLeft;
					mergeRoomId2 = roomRight;
				}
			}
			if (mergeRoomId1) {
				// Merge these two rooms into the current one.
				// TODO determine if merging all 3 or just two is sufficient
				// to remove most invalidities.
				int roomId = roomAndCells.first;
				for (const auto& cell : roomMap[mergeRoomId1]) {
					std::tie(r, c) = cell;
					roomIds[r][c] = roomId;
					roomMap[roomId].push_back(cell);
				}
				roomMap.erase(mergeRoomId1);
				for (const auto& cell : roomMap[mergeRoomId2]) {
					std::tie(r, c) = cell;
					roomIds[r][c] = roomId;
					roomMap[roomId].push_back(cell);
				}
				roomMap.erase(mergeRoomId2);
				std::tie(r, c) = roomAndCells.second.front();
				smoothed = true;
			}
		}
	}
	switch (verbosity) {
		case 2:
		case 1:
			if (smoothed) {
				std::cout << "Board after smoothing 2x1-1x1-2x1s:" << std::endl;
				printBoard(cellValues, roomIds);
			} else {
				std::cout << "No change after smoothing 2x1-1x1-2x1s."
						  << std::endl;
			}
		default:
			break;
	}

	return {cellValues, roomIds};
}
