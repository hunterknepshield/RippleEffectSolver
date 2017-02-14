//
//  validity_checks.hpp
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/10/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#ifndef validity_checks_hpp
#define validity_checks_hpp

#include "typedefs.h"

// Returns true if the value is a valid placement in the cell for the row.
bool checkRow(const Cell& cell, int value, const Board& cellValues);

// Returns true if the value is a valid placement in the cell for the column.
bool checkColumn(const Cell& cell, int value, const Board& cellValues);

// Returns true if the value is a valid placement in the cell for the room.
bool checkRoom(const CellList& cells, int value, const Board& cellValues);

#endif /* validity_checks_hpp */
