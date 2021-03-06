//
//  read_input.cc
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/10/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#include "read_input.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "typedefs.h"

// Consumes a string of space-delimited integers and returns them in a vector.
std::vector<int> interpretString(const std::string& s) {
	std::vector<int> v;
	std::stringstream stream(s);
	while (true) {
		int i;
		stream >> i;
		if (!stream) {
			break;
		}
		v.push_back(i);
	}
	return v;
}

bool readCells(size_t* boardWidth, Board* cellValues) {
	std::string s;
	// Read in the cell values on the board (0 is unset) until an empty line is
	// reached.
	do {
		std::getline(std::cin, s);
		if (s.empty()) {
			if (cellValues->empty()) {
				std::cerr << "Board cannot be empty.";
				continue;
			}
			return true;
		}
		const std::vector<int>& rowValues = interpretString(s);
		if (*boardWidth == 0) {
			*boardWidth = rowValues.size();
		} else if (*boardWidth != rowValues.size()) {
			std::cerr << "Inconsistent line size. Terminating." << std::endl;
			return false;
		}
		cellValues->push_back(rowValues);
	} while (true);
}

bool readRooms(size_t boardWidth, size_t boardHeight, Board* roomIds) {
	std::string s;
	do {
		std::getline(std::cin, s);
		if (s.empty()) {
			if (roomIds->size() != boardHeight) {
				std::cerr
					<< "Failed to specify room IDs for all cells. Terminating."
					<< std::endl;
				return false;
			}
			return true;
		}
		const std::vector<int>& cellIds = interpretString(s);
		if (boardWidth != cellIds.size()) {
			std::cerr << "Inconsistent line size. Terminating." << std::endl;
			return false;
		}
		roomIds->push_back(cellIds);
	} while (true);
}
