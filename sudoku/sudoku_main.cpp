#include <iostream>
#include <string>
#include <vector>
#include <chrono>

#include "Sudoku_SAT_solver.hpp"

int main() 
{
    try {
		SudokuSolver s;
        auto board = s.read_board(std::cin);
        auto t1 = std::chrono::high_resolution_clock::now();
		
        if (!s.apply_board(board)) {
            std::clog << "There is a contradiction in the parsed!\n";
            return 2;
        }
        if (s.solve()) {
            std::chrono::duration<double, std::milli> time_taken = std::chrono::high_resolution_clock::now() - t1;
            std::clog << "Solution found in " << time_taken.count() << " ms\n";

            auto solution = s.get_solution();
            for (auto const& row : solution) {
                for (auto const& col : row) {
                    std::cout << col << ' ';
                }
                std::cout << '\n';
            }
        } else {
            std::clog << "Solving the provided parsed is not possible\n";
        }
    } catch(std::exception const& ex) {
        std::clog << "Failed parsing because: " << ex.what() << std::endl;
        return 1;
    }

	std::cin.get();
	return 0;
}
