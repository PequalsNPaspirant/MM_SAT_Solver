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


#pragma once

#include <array>
#include <vector>
#include <initializer_list>
#include <string>
#include <map>
#include <unordered_set>
using namespace std;

#include "Sudoku_Logger.h"
//#include "Puzzles_Sudoku/Puzzles_Sudoku_BigInteger.h"
//#include "Puzzles_Sudoku/Puzzles_Sudoku_StaticBitSet_1.h"

namespace mm {

	enum SudokuSolvingAlgoTypes
	{
		sat_v1,
		//bruteForce_Strategy_1 = 0,
		//bruteForce_Strategy_1And2,
		//bruteForce_Strategy_1AndOptimized2,
		//bruteForce_Strategy_withNakedAndHidden,
		//bruteForce_Fastest_1,
		//bruteForce_Fastest_2,
		//bruteForce_Fastest_3,
		//bruteForce_Fastest_4,
		//bruteForce_Fastest_5,
		//bruteForce_Fastest_6,
		//bruteForce_Fastest_7,
		//bruteForce_Fastest_14,
		//DancingLinks_Naive,
		//DancingLinks_NoMemoryAllocation,
		//DancingLinks_NoMemoryAllocationAndReadyStructure,
		//DancingLinks_Fastest_1,
		//Fsss_Solver,
		//JSolve,
		//bb_sudoku,

		totalAlgos
	};

	class Sudoku_Utils
	{
	public:

		static bool validateSudokuSolution(const vector< vector<int> >& dataIn);
		static int validateSudokuSolution(const vector< vector< vector<int> > >& dataSetsIn);
		static bool compareSudokuSolutions(const vector< vector<int> >& data1, const vector< vector<int> >& data2);
		static vector< vector<int> > convertToVectorOfVector(const string& str, const unsigned int dimension, const unsigned int numbersRangeStartsWith = 1);
		static vector< vector<int> > convertToVectorOfVector2(const string& str, const unsigned int dimension);
		static const string convertToSingleLineString(const vector< vector<int> >& source);
		static const string convertToSingleLineString2(const vector< vector<int> >& source);
		static const string convertToSingleLineString(const vector< vector< vector<int> > >& sourceSets);
		static void myRunTimeAssert(bool expression, const string& msg = "");
		static const int countNumberOfClues(const vector< vector<int> >& source)
		{
			//Count number of clues present
			int clues = 0;
			for (size_t i = 0; i < source.size(); ++i)
				for (size_t j = 0; j < source[i].size(); ++j)
					if (source[i][j] != 0)
						++clues;

			return clues;
		}
		static bool checkIfPerfectSquare(int number)
		{
			static unordered_set<int> perfectSquares({ 4, 9, 16, 25, 36, 49, 64, 81, 100});
			return perfectSquares.find(number) != perfectSquares.end();
		}

		static Logger& getLogger();
		static Logger& getLogger(unsigned int flags);
		static Logger& getLogger(ConsoleTextColour colour);
		static Logger& getLogger(unsigned int flags, ConsoleTextColour colour);
		
		static void printSudokuGrid(const vector<int>& solution, const vector<int>& refPuzzle = {});
		static void printSudokuGrid(const vector< vector<int> >& solutionSets, const vector<int>& refPuzzle = {});

		/*
		static void initialize();
		static vector<char> SudokuPuzzleUtils::m_charSet;
		static map<char, int> SudokuPuzzleUtils::m_charToValueMap;
		*/

		static const unsigned long long MAX_VALUES_TO_TRY_FOR_BRUTE_FORCE;
		static const unsigned long long MAX_VALUES_TO_TRY_FOR_DANCING_LINKS;

	private:
		Sudoku_Utils() = delete;
		~Sudoku_Utils() = delete;
	};

	struct SudokuPuzzleStats;
	struct SudokuPuzzleBasicStats
	{
		SudokuPuzzleBasicStats()
			: m_cellsTriedOrLinkUpdates(0),
			m_valuesTried(0),
			m_wrongGuesses(0),
			m_time(0),
			m_nakedSinglesFound(0),
			m_hiddenSinglesFound(0),
			m_nakedPairsFound(0),
			m_hiddenPairsFound(0),
			m_lockedCandidatesFound(0),
			m_solutions(""),
			m_numInvalidSolutions(0),
			m_numRecursiveCalls(0)
		{
		}

		unsigned long long m_cellsTriedOrLinkUpdates;
		unsigned long long m_valuesTried;
		unsigned long long m_wrongGuesses;
		unsigned long long m_time;
		unsigned long long m_nakedSinglesFound;
		unsigned long long m_hiddenSinglesFound;
		unsigned long long m_nakedPairsFound;
		unsigned long long m_hiddenPairsFound;
		unsigned long long m_lockedCandidatesFound;
		string m_solutions;
		unsigned int m_numInvalidSolutions;
		unsigned long long m_numRecursiveCalls; //TODO: Add recursive calls
	};

	struct SudokuPuzzleStats
	{
		SudokuPuzzleStats();

		string m_category;
		unsigned int m_clues;
		unsigned long long m_puzzleGenerationTime;
		unsigned int m_puzzleGenerationIterations;

		vector<SudokuPuzzleBasicStats> m_base;
	};

	class PendingAssignmentsCircularQueue
	{
	public:
		struct Cell
		{
			Cell(unsigned int rowIn, unsigned int columnIn, unsigned int valueIn)
				: row(rowIn), column(columnIn), value(valueIn)
			{
			}

			unsigned int row;
			unsigned int column;
			unsigned int value;
		};

		PendingAssignmentsCircularQueue(unsigned int size);
		void push(unsigned int row, unsigned int column, unsigned int value);
		const Cell& pop();
		const Cell& top();
		bool isEmpty();
		unsigned int count();
		void makeEmpty();

	private:
		vector<Cell> m_data;
		int m_start;
		int m_end;
	};

	/*=================================================================================================*/

	template <unsigned int size>
	class PendingAssignmentsCircularQueue2
	{
	public:
		struct Cell
		{
			//Cell(unsigned int cellIndexIn, unsigned int valueIn)
			//	: cellIndex(cellIndexIn), value(valueIn)
			//{
			//}

			Cell() : row(0), column(0), value(0) {}
			Cell(unsigned int rowIn, unsigned int columnIn, unsigned int valueIn);

			//unsigned int cellIndex;
			unsigned int row;
			unsigned int column;
			unsigned int value;
		};

		PendingAssignmentsCircularQueue2();
		//void push(unsigned int cellIndexIn, unsigned int valueIn);
		void push(unsigned int rowIn, unsigned int columnIn, unsigned int valueIn);
		typename const Cell& pop();
		typename const Cell& top();
		bool isEmpty();
		unsigned int count();
		void makeEmpty();

	private:
		array<Cell, size> m_data;
		int m_start;
		int m_end;
	};

	template <unsigned int size>
	PendingAssignmentsCircularQueue2<size>::PendingAssignmentsCircularQueue2()
	{

	}

	template <unsigned int size>
	PendingAssignmentsCircularQueue2<size>::Cell::Cell(unsigned int rowIn, unsigned int columnIn, unsigned int valueIn)
		: row(rowIn), column(columnIn), value(valueIn)
	{
	}

	//template <unsigned int size>
	//void PendingAssignmentsCircularQueue2<size>::push(unsigned int cellIndexIn, unsigned int valueIn)
	//{
	//	if (count() < m_data.size())
	//	{
	//		if (m_end == m_data.size())
	//			m_end = 0;

	//		m_data[m_end].cellIndex = cellIndexIn;
	//		m_data[m_end].value = valueIn;
	//		++m_end;
	//	}
	//	else
	//		MyAssert::myRunTimeAssert(false, "Queue size overflow");
	//}

	template <unsigned int size>
	void PendingAssignmentsCircularQueue2<size>::push(unsigned int rowIn, unsigned int columnIn, unsigned int valueIn)
	{
		if (count() < m_data.size())
		{
			if (m_end == m_data.size())
				m_end = 0;

			m_data[m_end].row = rowIn;
			m_data[m_end].column = columnIn;
			m_data[m_end].value = valueIn;
			++m_end;
		}
		else
			MyAssert::myRunTimeAssert(false, "Queue size overflow");
	}

	template <unsigned int size>
	typename const PendingAssignmentsCircularQueue2<size>::Cell& PendingAssignmentsCircularQueue2<size>::pop()
	{
		if (!isEmpty())
		{
			if (m_start == m_data.size())
				m_start = 0;

			const Cell& retVal = m_data[m_start];
			++m_start;

			return retVal;
		}
		else
			MyAssert::myRunTimeAssert(false, "Queue Empty");
	}

	template <unsigned int size>
	typename const PendingAssignmentsCircularQueue2<size>::Cell& PendingAssignmentsCircularQueue2<size>::top()
	{
		if (!isEmpty())
			return m_data[m_start];
		else
			MyAssert::myRunTimeAssert(false, "Queue Empty");
	}

	template <unsigned int size>
	bool PendingAssignmentsCircularQueue2<size>::isEmpty()
	{
		return (count() == 0);
	}

	template <unsigned int size>
	unsigned int PendingAssignmentsCircularQueue2<size>::count()
	{
		if (m_start <= m_end)
			return m_end - m_start;
		else
			return (m_end - 0 - 1) + (m_data.size() - m_start);
	}

	template <unsigned int size>
	void PendingAssignmentsCircularQueue2<size>::makeEmpty()
	{
		m_start = 0;
		m_end = 0;
	}

	//====================== Compile time square root =============================================================

	//Does not work for unsigned types
	template <typename T>
	constexpr T fabsCompileTime(T value)
	{
		return value > 0 ? value : -value;
	}

	template <typename T>
	constexpr T getPositiveDifferenceCompileTime(T value1, T value2)
	{
		return value1 > value2 ? value1 - value2 : value2 - value1;
	}

#define MID(x, y) ((x + y) / 2)

	template <typename T>
	constexpr T sqrtUsingBinarySearch(T x, T lo, T hi, T precision)
	{
		// works
		const T mid = (lo + hi) / 2;
		if (getPositiveDifferenceCompileTime(x, mid * mid) <= precision)
			return mid;

		if (x < mid * mid)
			return sqrtUsingBinarySearch<T>(x, lo, mid, precision);
		else
			return sqrtUsingBinarySearch<T>(x, mid, hi, precision);

		// works
		//return
		//	(x > MID(lo, hi) * MID(lo, hi) ? (x - MID(lo, hi) * MID(lo, hi)) <= precision : (MID(lo, hi) * MID(lo, hi) - x) <= precision)
		//	? MID(lo, hi)
		//	: x < MID(lo, hi) * MID(lo, hi)
		//		? sqrtByBinarySearch<T>(x, lo, MID(lo, hi), precision)
		//		: sqrtByBinarySearch<T>(x, MID(lo, hi), hi, precision);
	}

	template <typename T>
	constexpr T sqrtUsingNewtonRaphsonMethod(T x, T currGuess, T precision)
	{
		// fabs is not constexpr
		//T newGuess = 0.5 * (currGuess + x / currGuess);
		//return fabs(newGuess - currGuess) <= precision
		//	? newGuess
		//	: sqrtUsingNewtonRaphsonMethod(x, newGuess, precision);

		// unary - operator applied to unsigned type
		//return fabsCompileTime(currGuess * currGuess - x) <= precision
		//		? currGuess
		//		: sqrtUsingNewtonRaphsonMethod(x, static_cast<T>(0.5 * (currGuess + x / currGuess)), precision);

		// works
		return getPositiveDifferenceCompileTime(currGuess * currGuess, x) <= precision
			? currGuess
			: sqrtUsingNewtonRaphsonMethod(x, static_cast<T>(0.5 * (currGuess + x / currGuess)), precision);

		// works
		//return (currGuess * currGuess > x) ? (currGuess * currGuess - x) <= precision : (x - currGuess * currGuess) <= precision
		//	? currGuess
		//	: sqrtUsingNewtonRaphsonMethod(x, static_cast<T>(0.5 * (currGuess + x / currGuess)), precision);
	}

	template <typename T>
	constexpr T getCompileTimeSquareRoot(T x)
	{
		constexpr T precision = static_cast<T>(0.00001);
		const T sqrt1 = sqrtUsingBinarySearch<T>(x, 0, x / 2 + 1, precision);
		const T sqrt2 = sqrtUsingNewtonRaphsonMethod(x, x, precision);

		//std::static_assert(fabs(sqrt1 - sqrt2) < 0.00001);
		//MyAssert::myStaticAssert<(getPositiveDifferenceCompileTime(sqrt1, sqrt2) <= precision)>();
		//constexpr static bool result = (getPositiveDifferenceCompileTime(sqrt1, sqrt2) <= precision);
		//MyAssert::myStaticAssert<result>();
		//return sqrt1;

		return
			(getPositiveDifferenceCompileTime(sqrt1, sqrt2) <= precision)
			? sqrt1
			: 0;
	}

	
	//================================================================================================================

	template<unsigned int size>
	class PendingAssignmentsCircularQueue4
	{
	public:
		struct Cell
		{
			//Cell(unsigned int cellIndexIn, unsigned int valueIn)
			//	: cellIndex(cellIndexIn), value(valueIn)
			//{
			//}

			Cell() : row(0), column(0) {}
			Cell(unsigned int rowIn, unsigned int columnIn);

			//unsigned int cellIndex;
			unsigned int row;
			unsigned int column;
		};

		PendingAssignmentsCircularQueue4();
		//void push(unsigned int cellIndexIn, unsigned int valueIn);
		void push(unsigned int rowIn, unsigned int columnIn);
		typename const Cell& pop();
		typename const Cell& top();
		bool isEmpty();
		unsigned int count();
		void makeEmpty();

	private:
		array<Cell, size * size> m_data;
		int m_start;
		int m_end;
	};

	template <unsigned int size>
	PendingAssignmentsCircularQueue4<size>::PendingAssignmentsCircularQueue4()
	{

	}

	template <unsigned int size>
	PendingAssignmentsCircularQueue4<size>::Cell::Cell(unsigned int rowIn, unsigned int columnIn)
		: row(rowIn), column(columnIn)
	{
	}

	//template <unsigned int size>
	//void PendingAssignmentsCircularQueue2<size>::push(unsigned int cellIndexIn, unsigned int valueIn)
	//{
	//	if (count() < m_data.size())
	//	{
	//		if (m_end == m_data.size())
	//			m_end = 0;

	//		m_data[m_end].cellIndex = cellIndexIn;
	//		m_data[m_end].value = valueIn;
	//		++m_end;
	//	}
	//	else
	//		MyAssert::myRunTimeAssert(false, "Queue size overflow");
	//}

	template <unsigned int size>
	void PendingAssignmentsCircularQueue4<size>::push(unsigned int rowIn, unsigned int columnIn)
	{
		if (count() < m_data.size())
		{
			if (m_end == m_data.size())
				m_end = 0;

			m_data[m_end].row = rowIn;
			m_data[m_end].column = columnIn;
			++m_end;
		}
		else
			MyAssert::myRunTimeAssert(false, "Queue size overflow");
	}

	template <unsigned int size>
	typename const PendingAssignmentsCircularQueue4<size>::Cell& PendingAssignmentsCircularQueue4<size>::pop()
	{
		if (!isEmpty())
		{
			if (m_start == m_data.size())
				m_start = 0;

			const Cell& retVal = m_data[m_start];
			++m_start;

			return retVal;
		}
		else
			MyAssert::myRunTimeAssert(false, "Queue Empty");
	}

	template <unsigned int size>
	typename const PendingAssignmentsCircularQueue4<size>::Cell& PendingAssignmentsCircularQueue4<size>::top()
	{
		if (!isEmpty())
			return m_data[m_start];
		else
			MyAssert::myRunTimeAssert(false, "Queue Empty");
	}

	template <unsigned int size>
	bool PendingAssignmentsCircularQueue4<size>::isEmpty()
	{
		return (count() == 0);
	}

	template <unsigned int size>
	unsigned int PendingAssignmentsCircularQueue4<size>::count()
	{
		if (m_start <= m_end)
			return m_end - m_start;
		else
			return (m_end - 0 - 1) + (m_data.size() - m_start);
	}

	template <unsigned int size>
	void PendingAssignmentsCircularQueue4<size>::makeEmpty()
	{
		m_start = 0;
		m_end = 0;
	}
}

