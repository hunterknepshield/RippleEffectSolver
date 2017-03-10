//
//  print_board.h
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/9/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#ifndef print_board_h
#define print_board_h

#include "typedefs.h"

// If your console or font doesn't play nice with the pretty board symbols,
// uncomment this #define and it will print only numbers. The caveat is that it
// then won't print any sort of room borders.
// #define SIMPLE_PRINT_BOARD

// The ugly, simple version of the printing routine. Doesn't show room borders,
// just cells' values and their room IDs as two separate boards.
void uglyPrintBoard(const Board& cellValues, const Board& roomIds);

// The pretty version of the board printing routine. Uses special Unicode
// characters to show room borders in addition to the cells' values. However,
// these characters don't play nice with all fonts.
void prettyPrintBoard(const Board& cellValues, const Board& roomIds);

// A single function that prints the board. If SIMPLE_PRINT_BOARD is defined,
// only the raw numbers are printed. Otherwise, room borders are printed using
// special Unicode grid printing characters.
inline void printBoard(const Board& cellValues, const Board& roomIds) {
#ifdef SIMPLE_PRINT_BOARD
	uglyPrintBoard(cellValues, roomIds);
#else  /* !SIMPLE_PRINT_BOARD */
	prettyPrintBoard(cellValues, roomIds);
#endif /* SIMPLE_PRINT_BOARD */
}

#endif /* print_board_h */
