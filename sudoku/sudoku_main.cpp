#include <iostream>
#include <string>
#include <vector>
#include <chrono>

#include "Sudoku_Generator.h"
#include "Sudoku_Utils.h"

#include "Sudoku_Solver_SAT_v1.h"
#include "Sudoku_Solver_Backtrack_v1.h"

namespace mm
{
	void test()
	{
		int dimensionStart = 2;
		int dimensionEnd = 10;
		int numPuzzles = 1;
		for (int dimension = dimensionStart; dimension <= dimensionEnd; ++dimension)
		{
			for (int puzzleIndex = 0; puzzleIndex < numPuzzles; ++puzzleIndex)
			{
				int sudokuDimension = dimension * dimension;
				vector<int> sudokuPuzzle(sudokuDimension * sudokuDimension, 0);
				int iterations = 0;
				unsigned long long timeRequiredToGeneratePuzzle = 0;
				SudokuPuzzleGenerator::generateSudokuPuzzle(sudokuDimension, sudokuPuzzle, iterations, timeRequiredToGeneratePuzzle);
				Sudoku_Utils::printSudokuGrid(sudokuPuzzle);

				vector<int> sudokuSolution(sudokuDimension * sudokuDimension, 0);

				std::chrono::high_resolution_clock::time_point tps[5];
				std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
				bool result = test_SudokuSolverSAT_v1(sudokuDimension, sudokuPuzzle, sudokuSolution, tps);
				std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
				//unsigned long long duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
				//typedef ratio<1, 1000000000> nano;
				std::chrono::duration<unsigned long long, std::nano> time_taken = end - start;
				unsigned long long duration = time_taken.count();
				unsigned long long initialization  = std::chrono::duration_cast<std::chrono::nanoseconds>(tps[1] - tps[0]).count();
				unsigned long long applyBoard      = std::chrono::duration_cast<std::chrono::nanoseconds>(tps[2] - tps[1]).count();
				unsigned long long solving = 0;
				unsigned long long extractSolution = 0;
				if (result)
				{
					solving = std::chrono::duration_cast<std::chrono::nanoseconds>(tps[3] - tps[2]).count();
					extractSolution = std::chrono::duration_cast<std::chrono::nanoseconds>(tps[4] - tps[3]).count();
				}

				Sudoku_Utils::printSudokuGrid(sudokuSolution, sudokuPuzzle);
				std::cout << "\n";
				std::cout << "\nSudoku dimention: " << sudokuDimension << "x" << sudokuDimension;
				std::cout << "\nResult: " << (result ? "True" : "False");
				std::cout << "\nSolution found in " << formatWithCommas(duration) << " nanoseconds";
				std::cout << "\nTime duration break-up:";
				std::cout << "\n    Initialization  : " << formatWithCommas(initialization) << " nanoseconds";
				std::cout << "\n    applyBoard      : " << formatWithCommas(applyBoard) << " nanoseconds";
				std::cout << "\n    solving         : " << formatWithCommas(solving) << " nanoseconds";
				std::cout << "\n    extractSolution : " << formatWithCommas(extractSolution) << " nanoseconds";
				std::cout << "\n    Total           : " << formatWithCommas(initialization + applyBoard + solving + extractSolution) << " nanoseconds";
				bool success = Sudoku_Utils::validateSudokuSolution(sudokuSolution);
				std::cout << "\nSolution validation: " << (success ? "SUCCESS" : "FAILED");
				std::cout << "\n";
				std::cout << "\nPress any key to continue...";
				std::cin.get();
			}
		}
	}

}

int main() 
{
	mm::test();

	std::cin.get();
	return 0;
}
