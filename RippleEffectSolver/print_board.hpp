//
//  print_board.hpp
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/9/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#ifndef print_board_hpp
#define print_board_hpp

#include "typedefs.h"

// A single function that prints the board. If SIMPLE_PRINT_BOARD is defined in
// main.cpp, only the raw numbers are printed. Otherwise, room borders are
// printed using special Unicode grid printing characters.
void printBoard(const Board& cellValues, const Board& roomIds);

#endif /* print_board_hpp */
