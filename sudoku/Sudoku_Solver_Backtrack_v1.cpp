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


#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <cmath>
#include <unordered_map>
#include <limits>
using namespace std;

//#include <windows.h>

#include "Sudoku_Solver_Backtrack_v1.h"
#include "Sudoku_Utils.h"

namespace mm {

	const unsigned int BigIntegerBitCollection_v1::SIZE_OF_BYTE; // Initialized inside constructor
	const unsigned int BigIntegerBitCollection_v1::BITS_IN_ONE_DIGIT;
	//vector< BigIntegerBitCollection_v1 > Cell3::bitMask = { 0b1, 0b10, 0b100, 0b1000, 0b10000, 0b100000, 0b1000000, 0b10000000, 0b100000000 };
	vector< BigIntegerBitCollection_v1 > BigIntegerBitCollection_v1::bitMask(0);
	void BigIntegerBitCollection_v1::prepareBitMask(const int size)
	{
		const int oldSize = bitMask.size();
		if (oldSize < size)
		{
			bitMask.resize(size, BigIntegerBitCollection_v1());
			for (int i = oldSize; i < size; ++i)
			{
				BigIntegerBitCollection_v1 temp;
				temp.resize(i / BITS_IN_ONE_DIGIT + 1, 0);
				temp.setnthBitON(i + 1);
				//Cell3::bitMask.push_back(std::move(temp));
				bitMask[i] = std::move(temp);
			}
		}
	}

	//Constructors
	BigIntegerBitCollection_v1::BigIntegerBitCollection_v1()
		: m_digits(0)
	{

	}

	BigIntegerBitCollection_v1::BigIntegerBitCollection_v1(const DEGIT_TYPE& digitValue)
		: m_digits(1, digitValue)
	{

	}

	BigIntegerBitCollection_v1::BigIntegerBitCollection_v1(const int numDigits, const DEGIT_TYPE& digitValue)
		: m_digits(numDigits, digitValue)
	{

	}

	//Copy constructor
	BigIntegerBitCollection_v1::BigIntegerBitCollection_v1(const BigIntegerBitCollection_v1& copy)
		: m_digits(copy.m_digits)
	{
	}

	//Move constructor
	BigIntegerBitCollection_v1::BigIntegerBitCollection_v1(BigIntegerBitCollection_v1&& copy)
		: m_digits(0)
	{
		m_digits.swap(copy.m_digits);
	}

	//Assignment operator
	BigIntegerBitCollection_v1& BigIntegerBitCollection_v1::operator=(const BigIntegerBitCollection_v1& copy)
	{
		if (m_digits.size() == copy.m_digits.size())
			m_digits.assign(copy.m_digits.begin(), copy.m_digits.end());
		else
			m_digits = copy.m_digits;

		return *this;
	}

	//Move assignment operator
	BigIntegerBitCollection_v1& BigIntegerBitCollection_v1::operator=(BigIntegerBitCollection_v1&& copy)
	{
		m_digits.swap(copy.m_digits);
		return *this;
	}


	void BigIntegerBitCollection_v1::resize(const int numDigits, const DEGIT_TYPE& digitValue /*= 0*/)
	{
		m_digits.resize(numDigits, digitValue);
	}

	const BigIntegerBitCollection_v1 operator&(const BigIntegerBitCollection_v1& lhs, const BigIntegerBitCollection_v1& rhs)
	{
		BigIntegerBitCollection_v1 result(lhs);
		result &= rhs;
		return result;
	}

	void BigIntegerBitCollection_v1::operator&=(const BigIntegerBitCollection_v1& rhs)
	{
		unsigned int lhsIndex = m_digits.size();
		unsigned int rhsIndex = rhs.m_digits.size();
		// lhsIndex >= rhsIndex at this place
		for (; lhsIndex > 0 && rhsIndex > 0;)
			m_digits[--lhsIndex] &= rhs.m_digits[--rhsIndex];

		//Mark the remaining leftmost bits zero
		for (; lhsIndex > 0;)
			m_digits[--lhsIndex] = 0;
	}

	const BigIntegerBitCollection_v1 operator|(const BigIntegerBitCollection_v1& lhs, const BigIntegerBitCollection_v1& rhs)
	{
		BigIntegerBitCollection_v1 result(lhs);
		result |= rhs;
		return result;
	}

	void BigIntegerBitCollection_v1::operator|=(const BigIntegerBitCollection_v1& rhs)
	{
		// we need to resize lhs if it's size is less
		if (m_digits.size() < rhs.m_digits.size())
		{
			BigIntegerBitCollection_v1 temp(*this);
			m_digits.resize(rhs.m_digits.size(), 0);
			int startIndex = m_digits.size() - temp.m_digits.size() - 1;
			for (DEGIT_TYPE num : temp.m_digits)
				m_digits[++startIndex] = num;
		}

		unsigned int lhsIndex = m_digits.size();
		unsigned int rhsIndex = rhs.m_digits.size();
		// lhsIndex >= rhsIndex at this place
		for (; lhsIndex > 0 && rhsIndex > 0;)
			m_digits[--lhsIndex] |= rhs.m_digits[--rhsIndex];
	}

	const BigIntegerBitCollection_v1 operator^(const BigIntegerBitCollection_v1& lhs, const BigIntegerBitCollection_v1& rhs)
	{
		BigIntegerBitCollection_v1 result(lhs);
		result ^= rhs;
		return result;
	}

	void BigIntegerBitCollection_v1::operator^=(const BigIntegerBitCollection_v1& rhs)
	{
		// we need to resize lhs if it's size is less
		if (m_digits.size() < rhs.m_digits.size())
		{
			BigIntegerBitCollection_v1 temp(*this);
			m_digits.resize(rhs.m_digits.size(), 0);
			int startIndex = m_digits.size() - temp.m_digits.size() - 1;
			for (DEGIT_TYPE num : temp.m_digits)
				m_digits[++startIndex] = num;
		}

		unsigned int lhsIndex = m_digits.size();
		unsigned int rhsIndex = rhs.m_digits.size();
		// lhsIndex >= rhsIndex at this place
		for (; lhsIndex > 0 && rhsIndex > 0;)
			m_digits[--lhsIndex] ^= rhs.m_digits[--rhsIndex];

		//Mark the remaining leftmost bits  // No need to handle it: n ^ 0 = n
		//for (; lhsIndex > 0;)
		//	m_digits[--lhsIndex] ^= 0;
	}

	const BigIntegerBitCollection_v1 BigIntegerBitCollection_v1::operator~() const
	{
		BigIntegerBitCollection_v1 result(*this);
		for (DEGIT_TYPE& num : result.m_digits)
			num = ~num;

		return result;
	}

	const BigIntegerBitCollection_v1 BigIntegerBitCollection_v1::operator<<(const unsigned int shift) const
	{
		BigIntegerBitCollection_v1 result;
		result.resize(m_digits.size(), 0);
		int localShift = shift % BITS_IN_ONE_DIGIT;
		int digitShift = shift / BITS_IN_ONE_DIGIT;
		int carry = 0;
		for (size_t i = digitShift; i < m_digits.size(); ++i)
		{
			result.m_digits[i] |= carry;
			result.m_digits[i] = m_digits[i - digitShift] << localShift;
			carry = m_digits[i + digitShift] >> (sizeof(DEGIT_TYPE) - localShift);
		}

		return result;
	}

	void BigIntegerBitCollection_v1::setnthBitON(const unsigned int position)
	{
		//int index = m_digits.size() - 1 - (position - 1u) / BITS_IN_ONE_DIGIT;
		int index = (position - 1u) / BITS_IN_ONE_DIGIT;
		//int localPosition = (position - 1) % BITS_IN_ONE_DIGIT;
		int localPosition = (position - 1u) - (index * BITS_IN_ONE_DIGIT);
		m_digits[index] |= (1u << localPosition);

		//*this |= BigIntegerBitCollection_v1::bitMask[position - 1u];
	}

	void BigIntegerBitCollection_v1::setnthBitOFF(const unsigned int position)
	{
		//int index = m_digits.size() - 1 - (position - 1u) / BITS_IN_ONE_DIGIT;
		int index = (position - 1u) / BITS_IN_ONE_DIGIT;
		//int localPosition = (position - 1) % BITS_IN_ONE_DIGIT;
		int localPosition = (position - 1u) - (index * BITS_IN_ONE_DIGIT);
		m_digits[index] &= ~(1u << localPosition);
	}

	bool BigIntegerBitCollection_v1::isnthBitON(const unsigned int position) const
	{
		//return (m_isPossible & SudokuPuzzleUtils::bitMask[index]) > BigIntegerBitCollection_v1(0);

		//int index = m_digits.size() - 1 - (position - 1u) / BITS_IN_ONE_DIGIT;
		int index = (position - 1u) / BITS_IN_ONE_DIGIT;
		//int localPosition = (position - 1) % BITS_IN_ONE_DIGIT;
		int localPosition = (position - 1u) - (index * BITS_IN_ONE_DIGIT);
		return (m_digits[index] & (1u << localPosition)) > 0;
	}

	/*
	bool BigIntegerBitCollection_v1::isnthBitOFF(const unsigned int position) const
	{
	int index = m_digits.size() - 1 - (position - 1u) / BITS_IN_ONE_DIGIT;
	//int localPosition = (position - 1) % BITS_IN_ONE_DIGIT;
	int localPosition = (position - 1u) - (index * BITS_IN_ONE_DIGIT);
	return (m_digits[index] & (1u << localPosition)) == 0;
	}
	*/

	bool operator<(const BigIntegerBitCollection_v1& lhs, const BigIntegerBitCollection_v1& rhs)
	{
		size_t lhsStart = lhs.skipAllZerosAtLeft();
		size_t rhsStart = rhs.skipAllZerosAtLeft();

		if ((lhs.m_digits.size() - lhsStart) != (rhs.m_digits.size() - rhsStart))
			return (lhs.m_digits.size() - lhsStart) < (rhs.m_digits.size() - rhsStart);

		for (; lhsStart < lhs.m_digits.size(); ++lhsStart, ++rhsStart)
			if (lhs.m_digits[lhsStart] > rhs.m_digits[rhsStart])
				return false;
			else if (lhs.m_digits[lhsStart] < rhs.m_digits[rhsStart])
				return true;

		return false;
	}

	bool operator<=(const BigIntegerBitCollection_v1& lhs, const BigIntegerBitCollection_v1& rhs)
	{
		//return *this < rhs || *this == rhs;
		return !(lhs > rhs);
	}

	bool operator>(const BigIntegerBitCollection_v1& lhs, const BigIntegerBitCollection_v1& rhs)
	{
		return rhs < lhs;
	}

	bool operator>=(const BigIntegerBitCollection_v1& lhs, const BigIntegerBitCollection_v1& rhs)
	{
		//return *this > rhs || *this == rhs;
		return !(lhs < rhs);
	}

	bool operator==(const BigIntegerBitCollection_v1& lhs, const BigIntegerBitCollection_v1& rhs)
	{
		size_t lhsStart = lhs.skipAllZerosAtLeft();
		size_t rhsStart = rhs.skipAllZerosAtLeft();

		if ((lhs.m_digits.size() - lhsStart) != (rhs.m_digits.size() - rhsStart))
			return false;

		for (; lhsStart < lhs.m_digits.size(); ++lhsStart, ++rhsStart)
			if (lhs.m_digits[lhsStart] != rhs.m_digits[rhsStart])
				return false;

		return true;
	}

	bool operator!=(const BigIntegerBitCollection_v1& lhs, const BigIntegerBitCollection_v1& rhs)
	{
		return !(lhs == rhs);
	}

	int BigIntegerBitCollection_v1::getOnBitPosition() const
	{
		unsigned int numDigits = m_digits.size();
		if (numDigits == 1)
			return getOnBitPosition_4(m_digits[0]);

		unsigned int carryPositions = 0;
		unsigned int digitIndex = 0;
		for (; digitIndex < numDigits && m_digits[digitIndex] == 0; ++carryPositions, ++digitIndex);
		return carryPositions * BITS_IN_ONE_DIGIT + getOnBitPosition_4(m_digits[digitIndex]);
	}

	bool BigIntegerBitCollection_v1::isOnlyOneBitON()
	{
		unsigned int numNonZeroDigits = 0;
		unsigned int nonZeroDigitIndex = 0;
		for (unsigned int i = 0; i < m_digits.size(); ++i)
			if (m_digits[i] != 0)
			{
				++numNonZeroDigits;
				nonZeroDigitIndex = i;
			}

		return (numNonZeroDigits == 1) && ((m_digits[nonZeroDigitIndex] & (m_digits[nonZeroDigitIndex] - 1)) == 0);
	}

	void BigIntegerBitCollection_v1::setAllRightmostBitsON(const int size)
	{
		unsigned int localSize = size, i = 0;
		for (; i < m_digits.size() && localSize >= BITS_IN_ONE_DIGIT; localSize -= BITS_IN_ONE_DIGIT, ++i)
			m_digits[i] = numeric_limits<DEGIT_TYPE>::max();

		if (localSize > 0)
			m_digits[i] = (1 << localSize) - 1;
	}

	void BigIntegerBitCollection_v1::resetAllDigits(const DEGIT_TYPE& digitValue /*= 0*/)
	{
		for (size_t i = 0; i < m_digits.size(); ++i)
			m_digits[i] = digitValue;
	}

	bool BigIntegerBitCollection_v1::isZero()
	{
		for (size_t i = 0; i < m_digits.size(); ++i)
			if (m_digits[i] != 0)
				return false;

		return true;
	}

	size_t BigIntegerBitCollection_v1::skipAllZerosAtLeft() const
	{
		size_t i = 0;
		for (; i < m_digits.size() && m_digits[i] == 0; ++i);
		return i;
	}

	const unsigned int BigIntegerBitCollection_v1::getOnBitPosition_1(const unsigned int num) const
	{
		switch (num)
		{
		case 1: return 1;
		case 2: return 2;
		case 4: return 3;
		case 8: return 4;
		case 16: return 5;
		case 32: return 6;
		case 64: return 7;
		case 128: return 8;
		case 256: return 9;
		}

		return -1;
	}

	const unsigned int BigIntegerBitCollection_v1::getOnBitPosition_2(const unsigned int num) const
	{
		// Max 3 comparisons needed in worst case due to binary search
		if (num < 32u) // 1 or 2 or 4 or 8 or 16
		{
			if (num < 8u) // 1 or 2 or 4
			{
				if (num != 4u) // 1 or 2
					return num;
				else // 4
					return 3u;
			}
			else // 8 or 16
			{
				if (num == 8u) // 8
					return 4u;
				else // 16
					return 5u;
			}
		}
		else // 32 or 64 or 128 or 256
		{
			if (num < 128u) // 32 or 64
			{
				if (num == 32u) // 32
					return 6u;
				else // 64
					return 7u;
			}
			else // 128 or 256
			{
				if (num == 128u) // 128
					return 8u;
				else // 256
					return 9u;
			}
		}
	}

	const unsigned int BigIntegerBitCollection_v1::getOnBitPosition_3(const unsigned int num) const
	{
		static unordered_map<unsigned int, unsigned int> valueOnBitPositionHashMap(0);

		if (valueOnBitPositionHashMap.empty())
		{
			valueOnBitPositionHashMap.insert({
				{ 1u << 0, 1 },
				{ 1u << 1, 2 },
				{ 1u << 2, 3 },
				{ 1u << 3, 4 },
				{ 1u << 4, 5 },
				{ 1u << 5, 6 },
				{ 1u << 6, 7 },
				{ 1u << 7, 8 },
				{ 1u << 8, 9 },
				{ 1u << 9, 10 },
				{ 1u << 10, 11 },
				{ 1u << 11, 12 },
				{ 1u << 12, 13 },
				{ 1u << 13, 14 },
				{ 1u << 14, 15 },
				{ 1u << 15, 16 },
				{ 1u << 16, 17 },
				{ 1u << 17, 18 },
				{ 1u << 18, 19 },
				{ 1u << 19, 20 },
				{ 1u << 20, 21 },
				{ 1u << 21, 22 },
				{ 1u << 22, 23 },
				{ 1u << 23, 24 },
				{ 1u << 24, 25 },
				{ 1u << 25, 26 },
				{ 1u << 26, 27 },
				{ 1u << 27, 28 },
				{ 1u << 28, 29 },
				{ 1u << 29, 30 },
				{ 1u << 30, 31 },
				{ 1u << 31, 32 }
				});
		}

		unordered_map<unsigned int, unsigned int>::iterator it = valueOnBitPositionHashMap.find(num);
		if (it == valueOnBitPositionHashMap.end())
			Sudoku_Utils::myRunTimeAssert(false, "Bit position value entry missing from map");

		return it->second;
	}

	// This is fastest implementation of all
	const unsigned int BigIntegerBitCollection_v1::getOnBitPosition_4(const unsigned int num) const
	{
		return static_cast<unsigned int>(log2(num)) + 1;
	}

	void BigIntegerBitCollection_v1::printAllDigits()
	{
		for (size_t i = 0; i < m_digits.size(); ++i)
			cout << m_digits[i] << ", ";
	}

	SudokuMatrix5::Cell::Cell(const int size)
		: m_value(0), 
		m_isPossible(),  // 0b111111111 = 0x1FF = 511 = 2^9 - 1 = (1 << 9) - 1; // int having only righmost 9 bits ON, rest bits OFF1
		m_isPossibleCount(size)
	{
		m_isPossible.resize(size / BigIntegerBitCollection_v1::BITS_IN_ONE_DIGIT + 1, 0);
		m_isPossible.setAllRightmostBitsON(size);
	}

	SudokuMatrix5::Cell::Cell(const Cell& obj)
		: m_value(obj.m_value),
		m_isPossible(obj.m_isPossible),
		m_isPossibleCount(obj.m_isPossibleCount)
	{
	}

	int SudokuMatrix5::Cell::getOnBitPosition()
	{
		//Ideally there should be only one bit ON
		//MyAssert::myRunTimeAssert((m_isPossible & (m_isPossible - 1)) == 0);
		//MyAssert::myRunTimeAssert(m_isPossible.isOnlyOneBitON());
		if (m_isPossible.isOnlyOneBitON())
			return m_isPossible.getOnBitPosition();
		else
			return 0;
	}

	bool SudokuMatrix5::Cell::isBitON(int index) const
	{
		return m_isPossible.isnthBitON(index);
	}

	void SudokuMatrix5::Cell::setBitOFF(int index)
	{
		m_isPossible.setnthBitOFF(index);
	}

	bool SudokuMatrix5::solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats)
	{
		SudokuMatrix5 obj(dataIn, stats);
		if (obj.m_isValid && obj.solve(solutionSetsOut, numSolutions))
			return true;
		else
			return false;
	}

	SudokuMatrix5::SudokuMatrix5(const vector< vector<int> >& dataIn, SudokuPuzzleBasicStats& stats)
	: m_data(dataIn.size(), vector<Cell>(dataIn.size(), Cell(dataIn.size()))), m_isValid(true), m_cluesCount(0),
		m_stats(stats)
	{
		const int size = dataIn.size();
		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < size; ++j)
			{
				if (dataIn[i][j] != 0)
				{
					if(!assign(i, j, dataIn[i][j]))
					{
						m_isValid = false;
						return;
					}
				}
			}
		}

		//vector< vector<int> > solution(9, vector<int>(9));
		//for (int i = 0; i < size; ++i)
		//	for (int j = 0; j < size; ++j)
		//		solution[i][j] = m_data[i][j].m_value;
		//cout << "\nAfter initialization is done:";
		//SudokuPuzzleUtils::printSudokuGrid(solution, dataIn);
	}

	void SudokuMatrix5::copyDataFromTo(const vector< vector<Cell> >& from, vector< vector<Cell> >& to)
	{
		int size = from.size();
		for (int i = 0; i < size; ++i)
		{
			for (int j = 0; j < size; ++j)
			{
				to[i][j].m_value = from[i][j].m_value;
				to[i][j].m_isPossibleCount = from[i][j].m_isPossibleCount;
				to[i][j].m_isPossible = from[i][j].m_isPossible;
			}
		}
	}

	bool SudokuMatrix5::removeFromPossibleValuesOfPeer(const int& row, const int& column, const int& value)
	{
		if (m_data[row][column].isBitON(value))
		{
			m_data[row][column].setBitOFF(value);
			m_data[row][column].m_isPossibleCount -= 1;

			if (m_data[row][column].m_isPossibleCount == 0)
				return false;
			else if (m_data[row][column].m_isPossibleCount == 1)
			{
				int currentValue = m_data[row][column].getOnBitPosition();
				if (!assign(row, column, currentValue))
					return false;
			}

			//if (!checkIfOnlyOnePeerHasThisPossibleValue(row, column, value))
			//	return false;
		}

		return true;
	}

	bool SudokuMatrix5::removeFromPossibleValuesOfAllPeers(const int& row, const int& column, const int& value)
	{
		int size = m_data.size();
		// Check row
		for (int k = 0; k < size; ++k)
			if (k != column && !removeFromPossibleValuesOfPeer(row, k, value))
				return false;

		// Check column
		for (int k = 0; k < size; ++k)
			if (k != row && !removeFromPossibleValuesOfPeer(k, column, value))
				return false;

		// Check box
		int boxSize = sqrt(size);
		//int a = row - row % 3;
		//int a = (row / 3) * 3;
		const int boxRowStart = row - row % boxSize;
		const int boxColumnStart = column - column % boxSize;
		int boxRow = boxRowStart;
		for (int i = 0; i < boxSize; ++i, ++boxRow)
		{
			int boxColumn = boxColumnStart;
			for (int j = 0; j < boxSize; ++j, ++boxColumn)
				if (boxRow != row && boxColumn != column && !removeFromPossibleValuesOfPeer(boxRow, boxColumn, value))
					return false;
		}

		int targetRowIndex = 0;
		int targetColumnIndex = 0;

		//Check all rows
		int counter = 0;
		for (int i = 0; i < size; ++i)
		{
			if (i == row)
				continue;
			counter = 0;
			for (int k = 0; k < size && counter < 2; ++k)
				if (m_data[i][k].isBitON(value))
				{
					++counter;
					targetColumnIndex = k;
				}

			if (counter == 0)
				return false;

			if (counter == 1)// && m_data[row][targetColumnIndex].m_isPossibleCount > 1)
				if (!assign(i, targetColumnIndex, value))
					return false;
		}

		//Check all columns
		for (int i = 0; i < size; ++i)
		{
			if (i == column)
				continue;
			counter = 0;
			for (int k = 0; k < size && counter < 2; ++k)
				if (m_data[k][i].isBitON(value))
				{
					++counter;
					targetRowIndex = k;
				}

			if (counter == 0)
				return false;

			if (counter == 1)// && m_data[row][targetColumnIndex].m_isPossibleCount > 1)
				if (!assign(targetRowIndex, i, value))
					return false;
		}

		//Check only relevant boxes
		//int a = row < 3 ? 0 : row < 6 ? 3 : 6;
		//int b = column < 3 ? 0 : column < 6 ? 3 : 6;
		for (int i = 0; i < size; i += boxSize)
		{
			for (int j = 0; j < size; j += boxSize)
			{
				if ((i == boxRowStart && j != boxColumnStart) || (i != boxRowStart && j == boxColumnStart))
				{
					counter = 0;
					for (int p = i; p < i + boxSize && counter < 2; ++p)
						for (int q = j; q < j + boxSize && counter < 2; ++q)
							if (m_data[p][q].isBitON(value))
							{
								++counter;
								targetRowIndex = p;
								targetColumnIndex = q;
							}

					if (counter == 0)
						return false;

					if (counter == 1)// && m_data[row][targetColumnIndex].m_isPossibleCount > 1)
						if (!assign(targetRowIndex, targetColumnIndex, value))
							return false;
				}
			}
		}

		return true;
	}

	bool SudokuMatrix5::checkIfOnlyOnePeerHasThisPossibleValue(const int& row, const int& column, const int& value)
	{
		int size = m_data.size();
		int targetRowIndex = 0;
		int targetColumnIndex = 0;

		// Check row
		int counter = 0;
		for (int k = 0; k < size && counter < 2; ++k)
			//if (k != column && m_data[row][k].isBitON(bitIndex))
			if (m_data[row][k].isBitON(value))
			{
				++counter;
				targetColumnIndex = k;
			}

		if (counter == 0)
			return false;

		if (counter == 1) // && m_data[row][targetColumnIndex].m_isPossibleCount > 1)
			if (!assign(row, targetColumnIndex, value))
				return false;

		// Check column
		counter = 0;
		for (int k = 0; k < size && counter < 2; ++k)
			//if (k != row && m_data[k][column].isBitON(bitIndex))
			if (m_data[k][column].isBitON(value))
			{
				++counter;
				targetRowIndex = k;
			}

		if (counter == 0)
			return false;

		if (counter == 1) // && m_data[targetRowIndex][column].m_isPossibleCount > 1)
			if (!assign(targetRowIndex, column, value))
				return false;

		// Check box
		counter = 0;
		int boxSize = sqrt(size);
		//int a = row - row % 3;
		//int a = (row / 3) * 3;
		const int boxRowStart = row - row % boxSize;
		const int boxColumnStart = column - column % boxSize;
		int boxRow = boxRowStart;
		for (int i = 0; i < boxSize && counter < 2; ++i, ++boxRow)
		{
			int boxColumn = boxColumnStart;
			for (int j = 0; j < boxSize && counter < 2; ++j, ++boxColumn)
				//if (a != row && b != column && m_data[a][b].isBitON(bitIndex))
				if (m_data[boxRow][boxColumn].isBitON(value))
				{
					++counter;
					targetRowIndex = boxRow;
					targetColumnIndex = boxColumn;
				}
		}

		if (counter == 0)
			return false;
		if (counter == 1) // && m_data[targetRowIndex][targetColumnIndex].m_isPossibleCount > 1)
			if (!assign(targetRowIndex, targetColumnIndex, value))
				return false;

		return true;
	}

	bool SudokuMatrix5::assign(const int& row, const int& column, const int& value)
	{
		/*
		The logic behind the optimization in this class is explained in function SudokuMatrix4::eliminate() in file Puzzles_Sudoku_SolverUsingBruteForce_5.cpp
		*/

		if (m_data[row][column].m_value != 0) // Already assigned
			return true;

		//This is similar to above. We can avoid dependency on m_value
		//if (m_data[row][column].m_isPossibleCount == 1 && m_data[row][column].isBitON(value))
		//	return true;

		if (!m_data[row][column].isBitON(value))
			return false;
		//else if(m_data[row][column].m_isPossibleCount == 1)
		//	return true;

		m_data[row][column].m_value = value;
		// Set only m_value'th bit OFF
		m_data[row][column].setBitOFF(value);
		BigIntegerBitCollection_v1 valuesToEliminate = m_data[row][column].m_isPossible;
		// Set all bits OFF as this cell is solved
		m_data[row][column].m_isPossible.resetAllDigits();
		m_data[row][column].m_isPossible.setnthBitON(value);
		m_data[row][column].m_isPossibleCount = 1;

		if (!removeFromPossibleValuesOfAllPeers(row, column, value))
			return false;

		int size = m_data.size();
		for (int bitIndex = 1; bitIndex <= size; ++bitIndex)
		{
			if (/*bitIndex != value && */ valuesToEliminate.isnthBitON(bitIndex)
				&& !checkIfOnlyOnePeerHasThisPossibleValue(row, column, bitIndex))
				return false;
		}

		++m_cluesCount;
		return true;
	}

	bool SudokuMatrix5::solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions)
	{
		int size = m_data.size();
		int minPossibleCount = size + 1;
		std::pair<int, int> obj(-1, -1);
		for (int i = 0; i < size && minPossibleCount != 2; ++i)
		{
			for (int j = 0; j < size && minPossibleCount != 2; ++j)
			{
				if (m_data[i][j].m_isPossibleCount > 1 && m_data[i][j].m_isPossibleCount < minPossibleCount)
				{
					obj = std::pair<int, int>(i, j);
					minPossibleCount = m_data[i][j].m_isPossibleCount;
				}
			}
		}

		if (obj.first == -1 || m_stats.m_valuesTried >= Sudoku_Utils::MAX_VALUES_TO_TRY_FOR_BRUTE_FORCE)
		{
			vector< vector<int> > solution(size, vector<int>(size, 0));
			for (int i = 0; i < size; ++i)
				for (int j = 0; j < size; ++j)
					solution[i][j] = m_data[i][j].m_value;

			solutionSetsOut.push_back(std::move(solution));
			return true;
		}

		bool success = false;
		int row = obj.first;
		int column = obj.second;

		++m_stats.m_cellsTriedOrLinkUpdates;
		//SudokuPuzzleUtils::getLogger() << "\nTrying cell[ " << row << ", " << column << "]";

		//int currentPossibleValues = Cell.m_isPossible.size();
		//vector< vector<Cell> > copy(dataIn);
		//SudokuMatrix5 s(*this);
		for (int i = 0; i < size; ++i)
		{
			if (m_data[row][column].isBitON(i + 1))
			{
				++m_stats.m_valuesTried;

				//if (m_countValuesTried > SudokuPuzzleUtils::MAX_VALUES_TO_TRY_FOR_BRUTE_FORCE)
				//	return false;

				//SudokuPuzzleUtils::getLogger() << "\nTrying Value: " << i + 1;

				//copyDataFromTo(m_data, s.m_data);
				//vector< vector<Cell> > copy(dataIn);
				SudokuMatrix5 s(*this);

				if (s.assign(row, column, i + 1)
					&& (success = s.solve(solutionSetsOut, numSolutions))
					&& solutionSetsOut.size() == numSolutions)
					break;

				++m_stats.m_wrongGuesses;
			}
		}

		return success;
	}

}
