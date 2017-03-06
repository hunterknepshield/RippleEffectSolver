//
//  main.cpp
//  RippleEffectCreator
//
//  Created by Hunter Knepshield on 3/5/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#include <iostream>

#include "print_board.h"
#include "read_input.h"
#include "solve_puzzle.h"
#include "typedefs.h"
#include "validity_checks.h"

// Verbosity settings. 0 = silent, 1 = print message on action, 2 = print
// message and board on action.
int VERBOSITY = 0;

void generateRandomPuzzle() {
	int width, height;
	std::cout << "Specify the width of the puzzle to be created... ";
	std::cin >> width;
	std::cout << "Specify the height of the puzzle to be created... ";
	std::cin >> height;
	
	Board cellValues, roomIds;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			// cellValues[i][j];
		}
	}
}

void augmentExistingPuzzle() {
	
}

int main(void) {
	std::cout << "Generate random puzzle (g) or augment existing instance (a)? ";
	char choice = 'a';
	bool generateRandom;
	while (true) {
		// std::cin >> choice;
		if (choice == 'g' || choice == 'G') {
			generateRandomPuzzle();
			return 0;
		} else if (choice == 'a' || choice == 'A') {
			augmentExistingPuzzle();
			return 0;
		} else {
			std::cerr << "Invalid choice. Enter 'g' or 'a'... ";
		}
	}
}
