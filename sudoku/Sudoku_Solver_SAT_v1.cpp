#include <iostream>
#include <string>
#include <chrono>

#include "Sudoku_Solver_SAT_v1.h"

//namespace {

//const int rows = 9;
//const int columns = 9;
//const int values = 9;

Minisat::Var SudokuSolverSAT_v1::toVar_sudoku(int row, int column, int value) const
{
	assert(row >= 0 && row < rows_	&& "Attempt to get var for nonexistant row");
	assert(column >= 0 && column < columns_ && "Attempt to get var for nonexistant column");
	assert(value >= 0 && value < values_   && "Attempt to get var for nonexistant value");
	return row * columns_ * values_ + column * values_ + value;
}

//namespace {


/**
* Input patterns:
* [1-9] means that square has a digit assigned,
* . means empty square
* Example:
*
*  123...456
*  .........
*  .........
*  456...789
*  .........
*  .........
*  789...123
*  .........
*  .........
*
*/
std::vector<int> SudokuSolverSAT_v1::read_board(std::istream& in)
{
	std::vector<int> parsed(9, 0);
	int lines = 1;
	std::string line;
	while (std::getline(in, line) && lines <= 9) 
	{
		if (line.size() != 9) 
		{
			throw std::runtime_error("Line #" + std::to_string(lines) + " has invalid size.");
		}
		for (size_t ci = 0; ci < line.size(); ++ci) 
		{
			char c = line[ci];
			if (c == '.') 
			{
				continue;
			}
			else if (c >= '0' && c <= '9') 
			{
				parsed[(lines - 1) * 9 + ci] = c - '0';
			}
			else 
			{
				throw std::runtime_error("Line #" + std::to_string(lines) + "contains invalid character: '" + c + "'");
			}
		}
		++lines;
	}
	if (lines != 10) 
	{
		throw std::runtime_error("The input is missing a line");
	}

	return parsed;
}


//} // end anonymous namespace

bool SudokuSolverSAT_v1::is_valid_board(const std::vector<int>& b)
{
	if (b.size() != rows_ * columns_) 
	{
		return false;
	}
	int index = 0;
	for (int row = 0; row < rows_; ++row) 
	{
		//if (b[row].size() != columns_) 
		//{
		//	return false;
		//}
		for (int col = 0; col < columns_; ++col, ++index)
		{
			auto value = b[index];
			if (value < 0 || value > values_) 
			{
				return false;
			}
		}
	}
	return true;
}

void log_var_sudoku(Minisat::Lit lit) 
{
	if (sign(lit)) 
	{
		std::clog << '-';
	}
	std::clog << var(lit) + 1 << ' ';
}

void log_clause_sudoku(Minisat::vec<Minisat::Lit> const& clause) 
{
	for (int i = 0; i < clause.size(); ++i) 
	{
		log_var_sudoku(clause[i]);
	}
	std::clog << "0\n";
}

void log_clause_sudoku(Minisat::Lit lhs, Minisat::Lit rhs) 
{
	log_var_sudoku(lhs); log_var_sudoku(rhs);
	std::clog << "0\n";
}


//} //end anonymous namespace

void SudokuSolverSAT_v1::init_variables() 
{
	if (m_write_dimacs) 
	{
		std::clog << "c (row, column, value) = variable\n";
	}
	for (int r = 0; r < rows_; ++r) 
	{
		for (int c = 0; c < columns_; ++c) 
		{
			for (int v = 0; v < values_; ++v) 
			{
				auto var = solver.newVar();
				if (m_write_dimacs) 
				{
					std::clog << "c (" << r << ", " << c << ", " << v + 1 << ") = " << var + 1 << '\n';
				}
			}
		}
	}
	std::clog << std::flush;
}


void SudokuSolverSAT_v1::exactly_one_true(Minisat::vec<Minisat::Lit> const& literals) 
{
	if (m_write_dimacs) 
	{
		log_clause_sudoku(literals);
	}
	solver.addClause(literals);
	for (size_t i = 0; i < literals.size(); ++i) 
	{
		for (size_t j = i + 1; j < literals.size(); ++j) 
		{
			if (m_write_dimacs) 
			{
				log_clause_sudoku(~literals[i], ~literals[j]);
			}
			solver.addClause(~literals[i], ~literals[j]);
		}
	}
}



void SudokuSolverSAT_v1::one_square_one_value() 
{
	for (int row = 0; row < rows_; ++row) 
	{
		for (int column = 0; column < columns_; ++column) 
		{
			Minisat::vec<Minisat::Lit> literals;
			for (int value = 0; value < values_; ++value) 
			{
				literals.push(Minisat::mkLit(toVar_sudoku(row, column, value)));
			}
			exactly_one_true(literals);
		}
	}
}

void SudokuSolverSAT_v1::non_duplicated_values() 
{
	// In each row, for each value, forbid two column sharing that value
	for (int row = 0; row < rows_; ++row) 
	{
		for (int value = 0; value < values_; ++value) 
		{
			Minisat::vec<Minisat::Lit> literals;
			for (int column = 0; column < columns_; ++column) 
			{
				literals.push(Minisat::mkLit(toVar_sudoku(row, column, value)));
			}
			exactly_one_true(literals);
		}
	}
	// In each column, for each value, forbid two rows sharing that value
	for (int column = 0; column < columns_; ++column) 
	{
		for (int value = 0; value < values_; ++value) 
		{
			Minisat::vec<Minisat::Lit> literals;
			for (int row = 0; row < rows_; ++row) 
			{
				literals.push(Minisat::mkLit(toVar_sudoku(row, column, value)));
			}
			exactly_one_true(literals);
		}
	}
	// Now forbid sharing in the 3x3 boxes
	int rowBox = sqrt(rows_);
	int colBox = sqrt(columns_);
	for (int r = 0; r < rows_; r += rowBox)
	{
		for (int c = 0; c < columns_; c += colBox)
		{
			for (int value = 0; value < values_; ++value) 
			{
				Minisat::vec<Minisat::Lit> literals;
				for (int rr = 0; rr < rowBox; ++rr)
				{
					for (int cc = 0; cc < colBox; ++cc)
					{
						literals.push(Minisat::mkLit(toVar_sudoku(r + rr, c + cc, value)));
					}
				}
				exactly_one_true(literals);
			}
		}
	}
}

SudokuSolverSAT_v1::SudokuSolverSAT_v1(int dimension, bool write_dimacs /* = false*/)
	:
	rows_(dimension),
	columns_(dimension),
	values_(dimension),
	m_write_dimacs(write_dimacs)
{
	// Initialize the board
	init_variables();
	one_square_one_value();
	non_duplicated_values();
}

bool SudokuSolverSAT_v1::apply_board(const std::vector<int>& sudokuPuzzle)
{
	assert(is_valid_board(sudokuPuzzle) && "Provided board is not valid!");
	bool ret = true;
	int index = 0;
	for (int row = 0; row < rows_; ++row) 
	{
		for (int col = 0; col < columns_; ++col, ++index) 
		{
			auto value = sudokuPuzzle[index];
			if (value != 0) 
			{
				ret &= solver.addClause(Minisat::mkLit(toVar_sudoku(row, col, value - 1)));
			}
		}
	}
	return ret;
}

bool SudokuSolverSAT_v1::solve() 
{
	return solver.solve();
}

void SudokuSolverSAT_v1::get_solution(std::vector<int>& solution) const
{
	//std::vector<int> b(rows_ * columns_, 0);
	int index = 0;
	for (int row = 0; row < rows_; ++row) 
	{
		for (int col = 0; col < columns_; ++col, ++index)
		{
			int found = 0;
			for (int val = 0; val < values_; ++val) 
			{
				if (solver.modelValue(toVar_sudoku(row, col, val)).isTrue()) 
				{
					++found;
					solution[index] = val + 1;
				}
			}
			assert(found == 1 && "The SAT solver assigned one position more than one value");
			(void)found;
		}
	}
	//return b;
}

void test_SudokuSolverSAT_v1(int dimension, const std::vector<int>& sudokuPuzzle, std::vector<int>& sudokuSolution)
{
	SudokuSolverSAT_v1 s(dimension);
	if (!s.apply_board(sudokuPuzzle))
	{
		std::cout << "\nThere is a contradiction in the parsed!\n";
		return;
	}

	s.solve();
	s.get_solution(sudokuSolution);
}

int sudoku_solver_SAT_main()
{
	try {
		SudokuSolverSAT_v1 s(9);
		std::vector<int> board = s.read_board(std::cin);
		auto t1 = std::chrono::high_resolution_clock::now();

		if (!s.apply_board(board)) 
		{
			std::clog << "There is a contradiction in the parsed!\n";
			return 2;
		}
		if (s.solve()) 
		{
			std::chrono::duration<double, std::milli> time_taken = std::chrono::high_resolution_clock::now() - t1;
			std::clog << "Solution found in " << time_taken.count() << " ms\n";

			std::vector<int> solution(9 * 9, 0);
			s.get_solution(solution);
			//for (auto const& row : solution) 
			//{
			//	for (auto const& col : row) 
			//	{
			//		std::cout << col << ' ';
			//	}
			//	std::cout << '\n';
			//}
		}
		else 
		{
			std::clog << "Solving the provided parsed is not possible\n";
		}
	}
	catch (std::exception const& ex) 
	{
		std::clog << "Failed parsing because: " << ex.what() << std::endl;
		return 1;
	}

	return 0;
}
