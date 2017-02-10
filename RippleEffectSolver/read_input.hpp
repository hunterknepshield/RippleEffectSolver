//
//  read_input.hpp
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/10/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#ifndef read_input_hpp
#define read_input_hpp

#include <vector>

bool readCells(size_t* boardWidth, std::vector<std::vector<int>>* board);

bool readRooms(size_t boardWidth, size_t boardHeight, std::vector<std::vector<int>>* roomIds);

#endif /* read_input_hpp */
