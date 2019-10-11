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

#include <vector>
using namespace std;

namespace mm {

	class BigIntegerBitCollection_v1
	{
	public:
		typedef unsigned int DEGIT_TYPE;
		static constexpr const unsigned int SIZE_OF_BYTE = 8;
		static constexpr const unsigned int BITS_IN_ONE_DIGIT = sizeof(DEGIT_TYPE) * SIZE_OF_BYTE;
		static const unsigned int getNumDigits(const unsigned int dimension)
		{
			//dimension = 1 2 3 4...9...16...25...31 32 33...36...49...63 64 65...81...95 96 97...100
			//numDigits = 1 1 1 1...1... 1... 1... 1  1  2... 2... 2... 2  2  3... 3... 3  3  4...  4
			return ((dimension - 1u) / BITS_IN_ONE_DIGIT + 1);
			//OR
			//return (
			//			(dimension / BITS_IN_ONE_DIGIT) // One digit for each group of 32 dimensions/bits
			//			+ ((dimension % BITS_IN_ONE_DIGIT) > 0 ? 1 : 0) // One extra digit if there is additional bits unable to form a complete gruop of 32 bits
			//		); 
		}

		static const unsigned int getBitPositionInCurrentDigit(const unsigned int bitPosition)
		{
			return (bitPosition - 1u) % BITS_IN_ONE_DIGIT + 1;
		}

	public:
		//Constructors
		BigIntegerBitCollection_v1();
		explicit BigIntegerBitCollection_v1(const DEGIT_TYPE& digitValue);
		BigIntegerBitCollection_v1(const int numDigits, const DEGIT_TYPE& digitValue);

		//Copy constructor
		BigIntegerBitCollection_v1(const BigIntegerBitCollection_v1& copy);
		//Move constructor
		BigIntegerBitCollection_v1(BigIntegerBitCollection_v1&& copy);
		//Assignment operator
		BigIntegerBitCollection_v1& operator=(const BigIntegerBitCollection_v1& copy);
		//Move assignment operator
		BigIntegerBitCollection_v1& operator=(BigIntegerBitCollection_v1&& copy);

		void resize(const int numDigits, const DEGIT_TYPE& digitValue = 0);
		friend const BigIntegerBitCollection_v1 operator&(const BigIntegerBitCollection_v1& lhs, const BigIntegerBitCollection_v1& rhs);
		void operator&=(const BigIntegerBitCollection_v1& rhs);
		friend const BigIntegerBitCollection_v1 operator|(const BigIntegerBitCollection_v1& lhs, const BigIntegerBitCollection_v1& rhs);
		void operator|=(const BigIntegerBitCollection_v1& rhs);
		friend const BigIntegerBitCollection_v1 operator^(const BigIntegerBitCollection_v1& lhs, const BigIntegerBitCollection_v1& rhs);
		void operator^=(const BigIntegerBitCollection_v1& rhs);
		const BigIntegerBitCollection_v1 operator~() const;
		const BigIntegerBitCollection_v1 operator<<(const unsigned int shift) const;
		void setnthBitON(const unsigned int position);
		void setnthBitOFF(const unsigned int position);
		bool isnthBitON(const unsigned int position) const;
		//bool isnthBitOFF(const unsigned int position) const; //A separate function is not required. Make use of !isnthBitON(index)
		friend bool operator<(const BigIntegerBitCollection_v1& lhs, const BigIntegerBitCollection_v1& rhs);
		friend bool operator<=(const BigIntegerBitCollection_v1& lhs, const BigIntegerBitCollection_v1& rhs);
		friend bool operator>(const BigIntegerBitCollection_v1& lhs, const BigIntegerBitCollection_v1& rhs);
		friend bool operator>=(const BigIntegerBitCollection_v1& lhs, const BigIntegerBitCollection_v1& rhs);
		friend bool operator==(const BigIntegerBitCollection_v1& lhs, const BigIntegerBitCollection_v1& rhs);
		friend bool operator!=(const BigIntegerBitCollection_v1& lhs, const BigIntegerBitCollection_v1& rhs);
		int getOnBitPosition() const;
		bool isOnlyOneBitON();
		void setAllRightmostBitsON(const int size);
		void resetAllDigits(const DEGIT_TYPE& digitValue = 0);
		bool isZero();

		void printAllDigits();

		static void prepareBitMask(const int size);
		static vector<BigIntegerBitCollection_v1> bitMask;

		vector<DEGIT_TYPE>& getDigits()
		{
			return m_digits;
		}

		const vector<DEGIT_TYPE>& getDigits() const
		{
			return m_digits;
		}

	private:
		vector<DEGIT_TYPE> m_digits;

		size_t skipAllZerosAtLeft() const;
		const unsigned int getOnBitPosition_1(const unsigned int num) const;
		const unsigned int getOnBitPosition_2(const unsigned int num) const;
		const unsigned int getOnBitPosition_3(const unsigned int num) const;
		const unsigned int getOnBitPosition_4(const unsigned int num) const;
	};

	//Forward declaraton
	struct SudokuPuzzleBasicStats;

	class SudokuMatrix5
	{
	public:
		class Cell
		{
		public:
			Cell(const int size);
			Cell(const Cell& obj);
			int getOnBitPosition();

			bool isBitON(int index) const;
			void setBitOFF(int index);

			int m_value;
			BigIntegerBitCollection_v1 m_isPossible;
			int m_isPossibleCount;
		};

	public:
		static bool solve(const vector< vector<int> >& dataIn, vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions, SudokuPuzzleBasicStats& stats);

		SudokuMatrix5(const vector< vector<int> >& dataIn, SudokuPuzzleBasicStats& stats);
		bool assign(const int& row, const int& column, const int& value);
		bool removeFromPossibleValuesOfAllPeers(const int& row, const int& column, const int& value);
		bool removeFromPossibleValuesOfPeer(const int& row, const int& column, const int& value);
		bool checkIfOnlyOnePeerHasThisPossibleValue(const int& row, const int& column, const int& valuesToEliminate);
		bool solve(vector< vector< vector<int> > >& solutionSetsOut, const unsigned int numSolutions);
		void copyDataFromTo(const vector< vector<Cell> >& from, vector< vector<Cell> >& to);

		vector< vector<Cell> > m_data;
		bool m_isValid;
		int m_cluesCount;
		SudokuPuzzleBasicStats& m_stats;
	};

}
