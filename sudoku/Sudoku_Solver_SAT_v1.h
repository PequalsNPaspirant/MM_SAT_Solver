#pragma once

#include <minisat/core/Solver.h>

#include <vector>

//using board = std::vector<std::vector<int>>;

class SudokuSolverSAT_v1 {
private:
    const bool m_write_dimacs = false;
    Minisat::Solver solver;

public:
	SudokuSolverSAT_v1(int dimension, bool write_dimacs = false);
	std::vector<int> read_board(std::istream& in);
    // Returns true if applying the board does not lead to UNSAT result
	bool apply_board(const std::vector<int>& sudokuPuzzle);
    // Returns true if the sudoku has a solution
    bool solve();
	void get_solution(std::vector<int>& solution) const;

private:
    void one_square_one_value();
    void non_duplicated_values();
    void exactly_one_true(Minisat::vec<Minisat::Lit> const& literals);
    void init_variables();

	Minisat::Var toVar_sudoku(int row, int column, int value) const;
	bool is_valid_board(const std::vector<int>& b);

	const int rows_;
	const int columns_;
	const int values_;
};

bool test_SudokuSolverSAT_v1(int dimension, const std::vector<int>& sudokuPuzzle, std::vector<int>& sudokuSolution, std::chrono::high_resolution_clock::time_point tps[]);
