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

// Validate room IDs, ensuring they are all contiguous.
bool validateRooms(const Board& roomIds, const RoomMap& roomMap);

// Returns true if the entire board satisfies all of the puzzle's rules. Also
// checks that all rooms are contiguous.
bool validateBoard(const Board& cellValues, const Board& roomIds,
				   const RoomMap& roomMap, bool allowEmpty);

// Validate a board and expect it to be complete. Same return conditions as
// above, but will also return false if an empty space is encountered.
inline bool validateCompleteBoard(const Board& cellValues, const Board& roomIds,
								  const RoomMap& roomMap) {
	return validateBoard(cellValues, roomIds, roomMap, false);
}

// Validate a board, also allowing empty spaces.
inline bool validateIncompleteBoard(const Board& cellValues,
									const Board& roomIds,
									const RoomMap& roomMap) {
	return validateBoard(cellValues, roomIds, roomMap, true);
}

#endif /* validity_checks_h */
