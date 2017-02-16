//
//  validity_checks.h
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/10/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#ifndef validity_checks_h
#define validity_checks_h

#include "typedefs.h"

// Returns true if the value is a valid placement in the cell for the row.
bool checkRow(const Cell& cell, int value, const Board& cellValues);

// Returns true if the value is a valid placement in the cell for the column.
bool checkColumn(const Cell& cell, int value, const Board& cellValues);

// Returns true if the value is a valid placement in the cell for the room.
bool checkRoom(const Cell& cell, const CellList& cells, int value,
			   const Board& cellValues);

// Returns true if the entire board contains no empty spaces and satisfies all
// of the rules to solve it.
bool validateCompletedBoard(const Board& cellValues, const Board& roomIds,
							const RoomMap& roomMap);

#endif /* validity_checks_h */
