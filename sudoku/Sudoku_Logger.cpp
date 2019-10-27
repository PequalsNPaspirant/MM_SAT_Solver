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

#include <windows.h>

#include <iostream>
#include <iomanip>
#include <locale>
#include <string>
#include <chrono>
using namespace std;

//#include "Timer/Timer.h"
#include "Sudoku_Logger.h"
#include "Sudoku_Utils.h"

namespace mm {

	string getCurrentLocalTimeInNanoSeconds()
	{
		using SystemClock = std::chrono::system_clock;

		//This function takes aorund 50 to 70 microseconds to execute
		SystemClock::time_point now = SystemClock::now();

		std::time_t now_t = chrono::system_clock::to_time_t(now);
		std::tm nowTm;
		localtime_s(&nowTm, &now_t);
		std::stringstream bufferStream;
		bufferStream << std::put_time(&nowTm, "%Y-%m-%d_%H-%M-%S");

		long long duration = (chrono::duration_cast<chrono::nanoseconds>(now.time_since_epoch())).count() % (long long)1000000000;

		//duration is in nanoseconds, so it can have max 9 digits, 2 commas and 1 dot
		string fractional_seconds(".000,000,000");
		for (int index = 11; duration > 0; --index)
		{
			if (index == 8 || index == 4)
				continue;
			fractional_seconds[index] = '0' + (duration % 10);
			duration /= 10;
		}

		string retVal(bufferStream.str() + fractional_seconds);
		return std::move(retVal);
	}

	const unsigned int Logger::addTimeFlag = 1;
	const unsigned int Logger::printOnScreenFlag = 2;
	const unsigned int Logger::printInLogFileFlag = 4;

	Logger::Logger(const string& logFileFullName, bool printInLogFile /*= true*/, bool printOnScreen /*= true*/, bool addTime /* = true*/)
		: m_flags((printInLogFile ? printOnScreenFlag : 0) | (printOnScreen ? printInLogFileFlag : 0) | (addTime ? addTimeFlag : 0)),
		m_consoleTextColour(ConsoleTextColour::White),
		m_screenBuffer(cout)
	{
		string logFileName(logFileFullName);

		try
		{
			size_t start = logFileFullName.find_last_of('/') + 1;
			//logFileName = logFileName.substr(start, logFileFullName.length() - start);
			string fullFileNameWithTimestamp(logFileFullName + "_" + getCurrentLocalTimeInNanoSeconds() + ".txt");
			m_logFile.open(fullFileNameWithTimestamp);
		}
		catch (std::ofstream::failure &writeErr)
		{
			cout << "\nERROR: Can not open file: " << logFileFullName << endl;
		}
		catch (...)
		{
			cout << "\nUNKNOWN ERROR while opening file: " << logFileFullName << endl;
		}
	}

	Logger::Logger(const string& logFileFullName, unsigned int flags)
		:Logger(logFileFullName, flags & printInLogFileFlag, flags & printOnScreenFlag, flags & addTimeFlag)
	{

	}

	Logger::~Logger()
	{
		close();
	}

	void Logger::close()
	{
		if (m_logFile.is_open())
			m_logFile.close();
	}

	void Logger::setPrintOnScreen(bool printOnScreen)
	{
		if(printOnScreen)
			m_flags |= printOnScreenFlag;
		else
			m_flags &= ~printOnScreenFlag;
	}

	void Logger::setPrintInLogFile(bool printInLogFile)
	{
		if (printInLogFile)
			m_flags |= printInLogFileFlag;
		else
			m_flags &= ~printInLogFileFlag;
	}

	void Logger::setAddTime(bool addTime)
	{
		if (addTime)
			m_flags |= addTimeFlag;
		else
			m_flags &= ~addTimeFlag;
	}

	void Logger::setFlags(unsigned int flags)
	{
		m_flags = flags;
	}

	void Logger::setConsoleTextColour(ConsoleTextColour colour)
	{
		m_consoleTextColour = colour;
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, m_consoleTextColour);
	}

	void Logger::log(const string& logMessage)
	{
		if (m_flags & addTimeFlag)
		{
			string time("\n" + getCurrentLocalTimeInNanoSeconds() + ": ");
			m_logFile.write(time.c_str(), time.length());
		}
			
		if (m_flags & printInLogFileFlag)
		{
			if (m_logFile.is_open())
			{
				m_logFile.write(logMessage.c_str(), logMessage.length());
				//flush() will ensure than the contents are written to file on disk immediately, Otherwise we need to wait until close() is called on it
				m_logFile.flush();
			}
		}

		if (m_flags & printOnScreenFlag)
			cout << logMessage;
	}

	template<>
	Logger& operator<< (Logger &logger, const string& msg)
	{
		logger.log(msg);
		return logger;
	}

}