//
//  typedefs.h
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/12/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#ifndef typedefs_h
#define typedefs_h

#include <map>
#include <utility>
#include <vector>

typedef std::vector<std::vector<int>> Board;
// A (row, column) pair of indices into a Board.
typedef std::pair<int, int> Cell;
typedef std::vector<Cell> CellList;
// A map from room ID to the list of all cells in the room.
typedef std::map<int, CellList> RoomMap;

#endif /* typedefs_h */
