//
//  solve_puzzle.hpp
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/12/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#ifndef solve_puzzle_hpp
#define solve_puzzle_hpp

#include <vector>

#include "typedefs.h"

// Returns the number of cells filled in.
int fillKnownCellsInRoom(Board* cellValues, const Board& roomIds, int room, const CellList& cellsInRoom, int verbosity);

#endif /* solve_puzzle_hpp */
