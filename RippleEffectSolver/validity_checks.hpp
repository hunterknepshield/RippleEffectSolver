//
//  validity_checks.hpp
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/10/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#ifndef validity_checks_hpp
#define validity_checks_hpp

#include <map>
#include <utility>
#include <vector>

// Returns true if the value is a valid placement in the cell for the row.
bool checkRow(const std::pair<int, int>& cell, int value,
			  const std::vector<std::vector<int>>& board);

// Returns true if the value is a valid placement in the cell for the column.
bool checkColumn(const std::pair<int, int>& cell, int value,
				 const std::vector<std::vector<int>>& board);

// Returns true if the value is a valid placement in the cell for the room.
bool checkRoom(const std::pair<int, int>& cell, int value,
			   const std::vector<std::vector<int>>& board,
			   const std::vector<std::vector<int>>& roomIds,
			   const std::map<int, std::vector<std::pair<int, int>>>& cellsInRoom);

#endif /* validity_checks_hpp */
