#include <iostream>
#include <string>

#include "Sudoku_SAT_solver.hpp"

//namespace {

const int rows = 9;
const int columns = 9;
const int values = 9;

Minisat::Var toVar_sudoku(int row, int column, int value) {
    assert(row >= 0 && row < rows    && "Attempt to get var for nonexistant row");
    assert(column >= 0 && column < columns && "Attempt to get var for nonexistant column");
    assert(value >= 0 && value < values   && "Attempt to get var for nonexistant value");
    return row * columns * values + column * values + value;
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
board SudokuSolver::read_board(std::istream& in) {
	board parsed(9, std::vector<int>(9));
	int lines = 1;
	std::string line;
	while (std::getline(in, line) && lines <= 9) {
		if (line.size() != 9) {
			throw std::runtime_error("Line #" + std::to_string(lines) + " has invalid size.");
		}
		for (size_t ci = 0; ci < line.size(); ++ci) {
			char c = line[ci];
			if (c == '.') {
				continue;
			}
			else if (c >= '0' && c <= '9') {
				parsed[lines - 1][ci] = c - '0';
			}
			else {
				throw std::runtime_error("Line #" + std::to_string(lines) + "contains invalid character: '" + c + "'");
			}
		}
		++lines;
	}
	if (lines != 10) {
		throw std::runtime_error("The input is missing a line");
	}

	return parsed;
}


//} // end anonymous namespace

bool is_valid_board(board const& b) {
    if (b.size() != rows) {
        return false;
    }
    for (int row = 0; row < rows; ++row) {
        if (b[row].size() != columns) {
            return false;
        }
        for (int col = 0; col < columns; ++col) {
            auto value = b[row][col];
            if (value < 0 || value > 9) {
                return false;
            }
        }
    }
    return true;
}

void log_var_sudoku(Minisat::Lit lit) {
    if (sign(lit)) {
        std::clog << '-';
    }
    std::clog << var(lit) + 1 << ' ';
}

void log_clause_sudoku(Minisat::vec<Minisat::Lit> const& clause) {
    for (int i = 0; i < clause.size(); ++i) {
		log_var_sudoku(clause[i]);
    }
    std::clog << "0\n";
}

void log_clause_sudoku(Minisat::Lit lhs, Minisat::Lit rhs) {
	log_var_sudoku(lhs); log_var_sudoku(rhs);
    std::clog << "0\n";
}


//} //end anonymous namespace

void SudokuSolver::init_variables() {
    if (m_write_dimacs) {
        std::clog << "c (row, column, value) = variable\n";
    }
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < columns; ++c) {
            for (int v = 0; v < values; ++v) {
                auto var = solver.newVar();
                if (m_write_dimacs) {
                    std::clog << "c (" << r << ", " << c << ", " << v + 1 << ") = " << var + 1 << '\n';
                }
            }
        }
    }
    std::clog << std::flush;
}


void SudokuSolver::exactly_one_true(Minisat::vec<Minisat::Lit> const& literals) {
    if (m_write_dimacs) {
		log_clause_sudoku(literals);
    }
    solver.addClause(literals);
    for (size_t i = 0; i < literals.size(); ++i) {
        for (size_t j = i + 1; j < literals.size(); ++j) {
            if (m_write_dimacs) {
				log_clause_sudoku(~literals[i], ~literals[j]);
            }
            solver.addClause(~literals[i], ~literals[j]);
        }
    }
}



void SudokuSolver::one_square_one_value() {
    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            Minisat::vec<Minisat::Lit> literals;
            for (int value = 0; value < values; ++value) {
                literals.push(Minisat::mkLit(toVar_sudoku(row, column, value)));
            }
            exactly_one_true(literals);
        }
    }
}

void SudokuSolver::non_duplicated_values() {
    // In each row, for each value, forbid two column sharing that value
    for (int row = 0; row < rows; ++row) {
        for (int value = 0; value < values; ++value) {
            Minisat::vec<Minisat::Lit> literals;
            for (int column = 0; column < columns; ++column) {
                literals.push(Minisat::mkLit(toVar_sudoku(row, column, value)));
            }
            exactly_one_true(literals);
        }
    }
    // In each column, for each value, forbid two rows sharing that value
    for (int column = 0; column < columns; ++column) {
        for (int value = 0; value < values; ++value) {
            Minisat::vec<Minisat::Lit> literals;
            for (int row = 0; row < rows; ++row) {
                literals.push(Minisat::mkLit(toVar_sudoku(row, column, value)));
            }
            exactly_one_true(literals);
        }
    }
    // Now forbid sharing in the 3x3 boxes
    for (int r = 0; r < 9; r += 3) {
        for (int c = 0; c < 9; c += 3) {
            for (int value = 0; value < values; ++value) {
                Minisat::vec<Minisat::Lit> literals;
                for (int rr = 0; rr < 3; ++rr) {
                    for (int cc = 0; cc < 3; ++cc) {
                        literals.push(Minisat::mkLit(toVar_sudoku(r + rr, c + cc, value)));
                    }
                }
                exactly_one_true(literals);
            }
        }
    }
}

SudokuSolver::SudokuSolver(bool write_dimacs):
    m_write_dimacs(write_dimacs) {
    // Initialize the board
    init_variables();
    one_square_one_value();
    non_duplicated_values();
}

bool SudokuSolver::apply_board(board const& b) {
    assert(is_valid_board(b) && "Provided board is not valid!");
    bool ret = true;
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < columns; ++col) {
            auto value = b[row][col];
            if (value != 0) {
                ret &= solver.addClause(Minisat::mkLit(toVar_sudoku(row, col, value - 1)));
            }
        }
    }
    return ret;
}

bool SudokuSolver::solve() {
    return solver.solve();
}

board SudokuSolver::get_solution() const {
    board b(rows, std::vector<int>(columns));
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < columns; ++col) {
            int found = 0;
            for (int val = 0; val < values; ++val) {
                if (solver.modelValue(toVar_sudoku(row, col, val)).isTrue()) {
                    ++found;
                    b[row][col] = val + 1;
                }
            }
            assert(found == 1 && "The SAT solver assigned one position more than one value");
            (void)found;
        }
    }
    return b;
}
