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

#include <windows.h> // to go throu all files in a directory

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream> //for file read/write
#include <cstdio> //for int rename( const char *old_filename, const char *new_filename );
#include <ctime>
#include <iomanip>      // std::setfill, std::setw
#include <cmath>
#include <chrono>
//using namespace std;

#include "Sudoku_Utils.h"
#include "Sudoku_Solver_Backtrack_v1.h"

namespace mm {

	const unsigned long long Sudoku_Utils::MAX_VALUES_TO_TRY_FOR_BRUTE_FORCE   = 10000000; // 10 million
	const unsigned long long Sudoku_Utils::MAX_VALUES_TO_TRY_FOR_DANCING_LINKS = 100000000; // 100 million

	/*
	vector<char> Sudoku_Utils::m_charSet(0);
	map<char, int> Sudoku_Utils::m_charToValueMap;
	// This method of using letters instead of numbers is obsolete now
	void Sudoku_Utils::initialize()
	{
		if (m_charSet.size() == 0)
		{
			m_charSet.resize(10 + 26 + 26);
			int index = -1;
			for (char ch = '0'; ch <= '9'; ++ch)
				m_charSet[++index] = ch;
			for (char ch = 'A'; ch <= 'Z'; ++ch)
				m_charSet[++index] = ch;
			for (char ch = 'a'; ch <= 'z'; ++ch)
				m_charSet[++index] = ch;
		}
		
		if (m_charToValueMap.size() == 0)
		{
			for (int i = 0; i < m_charSet.size(); ++i)
			{
				m_charToValueMap.insert({ m_charSet[i], i });
			}
		}
	}
	*/

	bool Sudoku_Utils::validateSudokuSolution(const vector<int>& dataIn)
	{
		int size = sqrt(dataIn.size());
		//unsigned int bits;
		//unsigned int initialVal = (1 << 9) - 1; // int having only righmost 9 bits ON, rest bits OFF

		BigIntegerBitCollection_v1 bits, initialVal;
		initialVal.resize(size / BigIntegerBitCollection_v1::BITS_IN_ONE_DIGIT + 1, 0);
		initialVal.setAllRightmostBitsON(size);

		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		// Validate each row
		int index = 0;
		for (int row = 0; row < size; ++row)
		{
			bits = initialVal;
			for (int i = 0; i < size && dataIn[index] > 0; ++i, ++index)
			{
				//bits &= ~(1 << (dataIn[row][i] - 1)); // Make rightmost (i + 1)th bit OFF
				bits.setnthBitOFF(dataIn[index]);
				//cout << "\nSet " << dataIn[row][i] << " bit off. All digits: ";
				//bits.printAllDigits();
			}
			if (bits != BigIntegerBitCollection_v1(0))
			{
				SetConsoleTextAttribute(hConsole, 9);
				Sudoku_Utils::getLogger() << "\nValidation on row " << row + 1 << " failed";
				//cout << "\nAll digits: ";
				//bits.printAllDigits();
				SetConsoleTextAttribute(hConsole, 15);
				return false;
			}
		}

		// Validate each column
		index = 0;
		for (int column = 0; column < size; ++column)
		{
			bits = initialVal;
			for (int i = 0; i < size && dataIn[index] > 0; ++i, ++index)
				//bits &= ~(1 << (dataIn[i][column] - 1)); // Make rightmost (i + 1)th bit OFF
				bits.setnthBitOFF(dataIn[index]);
			if (bits != BigIntegerBitCollection_v1(0))
			{
				SetConsoleTextAttribute(hConsole, 9);
				Sudoku_Utils::getLogger() << "\nValidation on column " << column + 1 << " failed";
				//cout << "\nAll digits: ";
				//bits.printAllDigits();
				SetConsoleTextAttribute(hConsole, 15);
				return false;
			}
		}

		// Validate each box
		int jump = sqrt(size);
		for (int rowStart = 0; rowStart < size; rowStart += jump)
			for (int columnStart = 0; columnStart < size; columnStart += jump)
			{
				bits = initialVal;
				bool isValid = true;
				for (int row = rowStart; row < rowStart + jump && isValid; ++row)
					for (int column = columnStart; column < columnStart + jump && (isValid = dataIn[row * size + column] > 0); ++column)
						//bits &= ~(1 << (dataIn[row][column] - 1)); // Make rightmost (i + 1)th bit OFF
						bits.setnthBitOFF(dataIn[row * size + column]);

				if (bits != BigIntegerBitCollection_v1(0))
				{
					SetConsoleTextAttribute(hConsole, 9);
					Sudoku_Utils::getLogger() << "\nValidation on box at (" << rowStart + 1 << ", " << columnStart + 1 << ") failed";
					//cout << "\nAll digits: ";
					//bits.printAllDigits();
					SetConsoleTextAttribute(hConsole, 15);
					return false;
				}
			}

		//Sudoku_Utils::getLogger() << "\nValidation successful!";
		return true;
	}

	int Sudoku_Utils::validateSudokuSolution(const vector< vector<int> >& dataSetsIn)
	{
		int numInvalidSolutions = 0;
		for (size_t i = 0; i < dataSetsIn.size(); ++i)
		{
			if (!Sudoku_Utils::validateSudokuSolution(dataSetsIn[i]))
				++numInvalidSolutions;
		}

		return numInvalidSolutions;
	}

	bool Sudoku_Utils::compareSudokuSolutions(const vector< vector<int> >& data1, const vector< vector<int> >& data2)
	{
		int size = data1.size();
		for (int row = 0; row < size; row++)
		{
			for (int column = 0; column < size; column++)
			{
				if (data1[row][column] != data2[row][column])
					return false;
			}
		}

		return true;
	}

	vector< vector<int> > Sudoku_Utils::convertToVectorOfVector2(const string& str, const unsigned int dimension)
	{
		vector< vector<int> > destination(dimension, vector<int>(dimension, 0));
		int columnIndex = -1;
		int rowIndex = 0;
		for (size_t i = 0; i < str.length(); i++)
		{
			if (columnIndex == dimension - 1)
			{
				columnIndex = -1;
				++rowIndex;

				if (rowIndex == dimension)
					break;
			}
			if (str[i] == '.' || str[i] == '0')
				destination[rowIndex][++columnIndex] = 0;
			else if (str[i] >= '0' && str[i] <= '9')
			{
				//map<char, int>::iterator it = m_charToValueMap.find(str[i]);
				//if(it != m_charToValueMap.end())
				//	destination[rowIndex][++columnIndex] = it->second;
				destination[rowIndex][++columnIndex] = (str[i] - '0');
			}
		}

		//return std::move(destination);
		return destination; // Give a way for return value optimization which is at least as efficient as move, and is more efficient most of the times
	}

	vector< vector<int> > Sudoku_Utils::convertToVectorOfVector(const string& str, const unsigned int dimension, const unsigned int numbersRangeStartsWith /* = 1*/)
	{
		if (dimension < 10)
			return Sudoku_Utils::convertToVectorOfVector2(str, dimension);

		vector< vector<int> > destination(dimension, vector<int>(dimension, 0));
		int columnIndex = -1;
		int rowIndex = 0;
		unsigned int value = 0;
		bool valueRead = false;
		for (size_t i = 0; i < str.length(); ++i)
		{
			if (columnIndex == dimension - 1)
			{
				columnIndex = -1;
				++rowIndex;

				//if (rowIndex == dimension)
				//	break;
			}

			if (str[i] >= '0' && str[i] <= '9')
			{
				valueRead = true;
				value = value * 10 + (str[i] - '0');
			}
			else //We encountered some other character than ['0'-'9']
			{
				if (valueRead) //If we have some value captured till now, store it
				{
					//map<char, int>::iterator it = m_charToValueMap.find(str[i]);
					//if (it != m_charToValueMap.end())
					destination[rowIndex][++columnIndex] = value + (1 - numbersRangeStartsWith);
					value = 0;
					valueRead = false;
				}

				if (columnIndex == dimension - 1)
				{
					columnIndex = -1;
					++rowIndex;

					//if (rowIndex == dimension)
					//	break;
				}

				if (str[i] == '.') //If we encountered '.', store zero in its place
				{
					destination[rowIndex][++columnIndex] = 0;
					value = 0;
				}
			}
		}

		// Add the value of last cell
		if (valueRead) //(value != 0)
			destination[dimension - 1][dimension - 1] = value;

		//return std::move(destination);
		return destination; // Give a way for return value optimization which is at least as efficient as move, and is more efficient most of the times
	}

	const string Sudoku_Utils::convertToSingleLineString2(const vector< vector<int> >& source)
	{
		const size_t size = source.size();
		string out(size * size + (size - 1), '.');
		for (size_t row = 0; row < size; ++row)
		{
			for (size_t column = 0; column < size; ++column)
				if (source[row][column] != 0)
					out[row * (size + 1) + column] = '0' + source[row][column];
			if (row != (size - 1))
				out[row * (size + 1) + size] = '|';
		}

		return out;
	}

	const string Sudoku_Utils::convertToSingleLineString(const vector< vector<int> >& source)
	{
		const size_t size = source.size();
		if (size < 10)
			return Sudoku_Utils::convertToSingleLineString2(source);

		size_t digitsInEachValue = 0;
		size_t tempDimension = size;
		while (tempDimension > 0)
		{
			++digitsInEachValue;
			tempDimension /= 10;
		}
		string out((digitsInEachValue + 1) * size * size + (size - 1), ' ');
		size_t index = -1;
		for (size_t row = 0; row < size; ++row)
		{
			for (size_t column = 0; column < size; ++column)
			{
				++index; // leave space before adding a new number
				if (source[row][column] == 0)
				{
					//for (size_t i = 1; i < digitsInEachValue; ++i)
					//	++index; // leave spaces
					index += (digitsInEachValue - 1u);
					out[++index] = '.';
				}
				else
				{
					string str(to_string(source[row][column]));
					//for (size_t i = 0; i < (digitsInEachValue - str.length()); ++i)
					//	++index; // leave spaces
					if (digitsInEachValue >= str.length())
					{
						index += (digitsInEachValue - str.length());
						for (size_t i = 0; i < str.length(); ++i)
							out[++index] = str[i];
					}
					else // if we have string lenth more than digitsInEachValue, there is some wrong number in grid
					{
						index += (digitsInEachValue - 1u);
						out[++index] = '.';
					}
				}
			}
			if (row != (size - 1))
				out[++index] = '|';
		}

		return out;
	}

	const string Sudoku_Utils::convertToSingleLineString(const vector< vector< vector<int> > >& sourceSets)
	{
		string out;
		for (size_t i = 0; i < sourceSets.size(); ++i)
		{
			if (i != 0)
				out += " AND ";
			out += convertToSingleLineString(sourceSets[i]);
		}

		return out;
	}

	Logger& Sudoku_Utils::getLogger()
	{
		static Logger sudokuSolverLogger("data/Sudoku/logs/test", true, true, false);
		return sudokuSolverLogger;
	}

	Logger& Sudoku_Utils::getLogger(unsigned int flags)
	{
		Logger& sudokuSolverLogger = Sudoku_Utils::getLogger();
		sudokuSolverLogger.setFlags(flags);
		return sudokuSolverLogger;
	}

	Logger& Sudoku_Utils::getLogger(ConsoleTextColour colour)
	{
		Logger& sudokuSolverLogger = Sudoku_Utils::getLogger();
		sudokuSolverLogger.setConsoleTextColour(colour);
		return sudokuSolverLogger;
	}

	Logger& Sudoku_Utils::getLogger(unsigned int flags, ConsoleTextColour colour)
	{
		Logger& sudokuSolverLogger = Sudoku_Utils::getLogger();
		sudokuSolverLogger.setFlags(flags);
		sudokuSolverLogger.setConsoleTextColour(colour);
		return sudokuSolverLogger;
	}

	void Sudoku_Utils::printSudokuGrid(const vector<int>& puzzleOrSolution, const vector<int>& refPuzzle /*= {}*/)
	{
		/*
		SetConsoleTextAttribute() second argument:
		1: Blue
		2: Green
		3: Cyan
		4: Red
		5: Purple
		6: Yellow (Dark)
		7: Default white
		8: Gray/Grey
		9: Bright blue
		10: Brigth green
		11: Bright cyan
		12: Bright red
		13: Pink/Magenta
		14: Yellow
		15: Bright white
		etc...
		*/

		// Start printing on a new line
		Sudoku_Utils::getLogger() << "\n";
		int cluesPresent = 0;
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		size_t solutionSize = sqrt(puzzleOrSolution.size());
		size_t boxSize = sqrt(solutionSize);
		size_t refPuzzleSize = sqrt(refPuzzle.size());
		size_t spaceBeforeEachChar = 1;
		size_t digitsInEachValue = 0;
		size_t tempDimension = solutionSize;
		while (tempDimension > 0)
		{
			++digitsInEachValue;
			tempDimension /= 10;
		}
		const string sep((spaceBeforeEachChar + digitsInEachValue) * boxSize + 1, '-');
		for (size_t i = 0; i < solutionSize; i++)
		{
			if (i > 0 && i % boxSize == 0)
			{
				for (size_t k = 1; k < boxSize; ++k)
					Sudoku_Utils::getLogger() << sep << "+";
				Sudoku_Utils::getLogger() << sep << "\n";
			}
			//else
			//	Sudoku_Utils::getLogger() << " ";

			for (size_t j = 0; j < solutionSize; j++)
			{
				Sudoku_Utils::getLogger() << " ";

				if (j > 0 && j % boxSize == 0)
					Sudoku_Utils::getLogger() << "| ";
				if (refPuzzleSize != 0 && refPuzzle[i * solutionSize + j] == 0)
					SetConsoleTextAttribute(hConsole, 12);
				if (puzzleOrSolution[i * solutionSize + j] == 0)
				{
					for (size_t i = 1; i < digitsInEachValue; ++i)
						Sudoku_Utils::getLogger() << " ";
					Sudoku_Utils::getLogger() << ".";
				}
				else
				{
					string str(to_string(puzzleOrSolution[i * solutionSize + j]));
					for (size_t i = 0; i < (digitsInEachValue - str.length()); ++i)
						Sudoku_Utils::getLogger() << " ";
					Sudoku_Utils::getLogger() << str;
					++cluesPresent;
				}

				if (refPuzzleSize != 0 && refPuzzle[i * solutionSize + j] == 0)
					SetConsoleTextAttribute(hConsole, 15); // //set 15 to black background and white text
			}
			Sudoku_Utils::getLogger() << "\n";
		}
		//if (refPuzzleSize == 0)
		{
			Sudoku_Utils::getLogger() << "Cells fixed   : " << cluesPresent << "\n";
			Sudoku_Utils::getLogger() << "Cells to find : " << (solutionSize * solutionSize) - cluesPresent;
		}
	}

	void Sudoku_Utils::printSudokuGrid(const vector< vector<int> >& solutionSets, const vector<int>& refPuzzle /*= {}*/)
	{
		for (size_t i = 0; i < solutionSets.size(); ++i)
		{
			Sudoku_Utils::getLogger() << "\nSolution no.: " << i + 1;
			printSudokuGrid(solutionSets[i], refPuzzle);
		}
	}

	void Sudoku_Utils::myRunTimeAssert(bool expression, const string& msg /* = "" */)
	{
		if (!expression)
		{
			cout << msg;
			int* nullPointer = nullptr;
			*nullPointer = 0;
		}
	}


	SudokuPuzzleStats::SudokuPuzzleStats()
		: m_category(""),
		m_clues(0),
		m_puzzleGenerationTime(0),
		m_puzzleGenerationIterations(0),
		m_base(SudokuSolvingAlgoTypes::totalAlgos, SudokuPuzzleBasicStats())
	{
	}


	PendingAssignmentsCircularQueue::PendingAssignmentsCircularQueue(unsigned int size)
		: m_data(size, Cell(0, 0, 0)), m_start(0), m_end(0)
	{

	}

	void PendingAssignmentsCircularQueue::push(unsigned int row, unsigned int column, unsigned int value)
	{
		if (count() == m_data.size())
			Sudoku_Utils::myRunTimeAssert(false, "Queue size overflow");
		else
		{
			if (m_end == m_data.size())
				m_end = 0;

			m_data[m_end].row = row;
			m_data[m_end].column = column;
			m_data[m_end].value = value;
			++m_end;
		}					
	}

	const PendingAssignmentsCircularQueue::Cell& PendingAssignmentsCircularQueue::pop()
	{
		if (isEmpty())
			Sudoku_Utils::myRunTimeAssert(false, "Queue Empty");
		else
		{
			if (m_start == m_data.size())
				m_start = 0;

			const Cell& retVal = m_data[m_start];
			++m_start;

			return retVal;
		}
	}

	const PendingAssignmentsCircularQueue::Cell& PendingAssignmentsCircularQueue::top()
	{
		if (!isEmpty())
			return m_data[m_start];
		else
			Sudoku_Utils::myRunTimeAssert(false, "Queue Empty");
	}

	bool PendingAssignmentsCircularQueue::isEmpty()
	{
		return (count() == 0);
	}

	unsigned int PendingAssignmentsCircularQueue::count()
	{
		if (m_start <= m_end)
			return m_end - m_start;
		else
			return (m_end - 0 - 1) + (m_data.size() - m_start);
	}

	void PendingAssignmentsCircularQueue::makeEmpty()
	{
		m_start = 0;
		m_end = 0;
	}
}
