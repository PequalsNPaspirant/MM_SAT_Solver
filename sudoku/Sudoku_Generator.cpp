//=======================================================================================================//
//   Copyright (c) 2017 Maruti Mhetre                                                                    //
//   All rights reserved.                                                                                //
//=======================================================================================================//
//   Redistribution and use of this software in source and binary forms, with or without modification,   //
//   are permitted for personal, educational or non-commercial purposes provided that the following      //
//   conditions are met:                                                                                 //
//   1. Redistributions of source code must retain the above copyright notice, this list of conditions   //
//      and the following disclaimer.                                                                    //
//   2. Redistributions in binary form must reproduce the above copyright notice, this list of           //
//      conditions and the following disclaimer in the documentation and/or other materials provided     //
//      with the distribution.                                                                           //
//   3. Neither the name of the copyright holder nor the names of its contributors may be used to        //
//      endorse or promote products derived from this software without specific prior written            //
//      permission.                                                                                      //
//=======================================================================================================//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR      //
//   IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND    //
//   FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR          //
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL   //
//   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   //
//   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER  //
//   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT   //
//   OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                     //
//=======================================================================================================//


#include <vector>
#include <cmath>
#include <chrono>
using namespace std;

//#include "Timer/Timer.h"
//#include "Assert/MyAssert.h"
#include "Sudoku_Generator.h"
#include "Sudoku_Utils.h"
#include "Sudoku_Solver_Backtrack_v1.h" // // For sudoku validation while generating random sudoku puzzles

namespace mm {

	void SudokuPuzzleGenerator::generateSudokuPuzzle(int dimension, vector<int>& sudokuPuzzle, int& iterations, unsigned long long& puzzleGenerationTime)
	{
		if (!Sudoku_Utils::checkIfPerfectSquare(dimension))
			Sudoku_Utils::myRunTimeAssert(false, "can not create a Sudoku Puzzle with dimension which is not perfect square");

		std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
		SudokuPuzzleBasicStats stats;
		SudokuMatrix5 mat(vector< vector<int> >(dimension, vector<int>(dimension, 0)), stats);
		// 17 cluses are sufficient for Sudoku of dimension 9. For other dimensions, the number of clues will be selected in proportion of this
		const int numClues = ceil(dimension * dimension * 17.0 / 81.0);
		while (mat.m_cluesCount < numClues)
		{
			++iterations;
			int row = rand() % dimension;
			int column = rand() % dimension;
			while (mat.m_data[row][column].m_isPossibleCount == 1)
			{
				if (++column == dimension)
				{
					column = 0;
					if (++row == dimension)
						row = 0;
				}
			}

			vector< vector<mm::SudokuMatrix5::Cell> > backup(mat.m_data);

			int value = rand() % dimension + 1;
			int localValue = value - 1;
			bool success = false;
			while (!success && ++localValue <= dimension)
				success = mat.assign(row, column, localValue);
			if (!success)
				mat.m_data = backup;

			while (!success && --value > 0)
				success = mat.assign(row, column, value);			
			if(!success)
				mat.m_data = backup;

			// While generating puzzles of small dimension liek 4x4, the last assignment can solve the puzzles if using SudokuMatrix5
			if (mat.m_cluesCount == dimension * dimension)
			{
				mat.m_data = backup;
				break;
			}
		}

		//vector< vector<int> > data(dimension, vector<int>(dimension, 0));
		for (int i = 0; i < dimension; ++i)
			for (int j = 0; j < dimension; ++j)
				sudokuPuzzle[i * dimension + j] = mat.m_data[i][j].m_value;
				//data[i][j] = mat.m_data[i][j].getOnBitPosition();

		std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
		puzzleGenerationTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
		Sudoku_Utils::getLogger() << "\nTime required to generate sudoku puzzle (Dimension: " << dimension << " iterations = " << iterations << "): " << formatWithCommas(puzzleGenerationTime) << " nanoseconds";
		//return std::move(data); // Promote compiler optimization
	}
}
