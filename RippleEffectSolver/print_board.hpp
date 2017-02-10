//
//  print_board.hpp
//  RippleEffectSolver
//
//  Created by Hunter Knepshield on 2/9/17.
//  Copyright © 2017 Hunter Knepshield. All rights reserved.
//

#ifndef print_board_hpp
#define print_board_hpp

#include <vector>

void printBoard(const std::vector<std::vector<int>>& board,
				const std::vector<std::vector<int>>& roomIds);

void printBoardUgly(const std::vector<std::vector<int>>& board,
					const std::vector<std::vector<int>>& roomIds);

#endif /* print_board_hpp */
