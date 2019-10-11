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

				std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
				test_SudokuSolverSAT_v1(sudokuDimension, sudokuPuzzle, sudokuSolution);
				std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
				//unsigned long long duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
				//typedef ratio<1, 1000000000> nano;
				std::chrono::duration<unsigned long long, std::nano> time_taken = end - start;
				unsigned long long duration = time_taken.count();

				Sudoku_Utils::printSudokuGrid(sudokuSolution, sudokuPuzzle);
				std::cout << "\n\nSolution found in " << formatWithCommas(duration) << " nanoseconds\n";
				std::cout << "\nSolution validation: " << (Sudoku_Utils::validateSudokuSolution(sudokuSolution) ? "SUCCESS" : "FAILED");
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
