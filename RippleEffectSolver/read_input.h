//
//  read_input.h
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/10/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#ifndef read_input_h
#define read_input_h

#include "typedefs.h"

// Reads cell values from stdin. Updates boardWidth as appropriate. Enforces
// that all rows must be the same width. Returns false if that is not the case,
// otherwise true.
bool readCells(size_t* boardWidth, Board* cellValues);

// Reads cells' room IDs from stdin. Enforces that the values must fit in the
// same footprint as the board from readCells.
bool readRooms(size_t boardWidth, size_t boardHeight, Board* roomIds);

#endif /* read_input_h */
