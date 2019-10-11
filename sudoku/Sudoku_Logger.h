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

#include <string>
#include <fstream> //for file read/write
#include <sstream> // for std::stringstream
using namespace std;

//#include "Sudoku_Utils.h"

namespace mm {

	template<class T>
	std::string formatWithCommas(T value)
	{
		std::stringstream ss;
		ss.imbue(std::locale(""));
		ss << std::fixed << value;
		return ss.str();
	}

	string getCurrentLocalTimeInNanoSeconds();

	enum ConsoleTextColour
	{
		Blue = 1,
		Green = 2,
		Cyan,
		Red,
		Purple,
		DarkYellow,
		White,
		Gray,
		BrightBlue,
		BrigthGreen,
		BrightCyan,
		BrightRed,
		Magenta,
		Yellow,
		BrightWhite//,
		//Default = 30
	};

	class Logger
	{
	public:
		Logger(const string& logFileFullName, bool printInLogFile = true, bool printOnScreen = true, bool addTime = true);
		Logger(const string& logFileFullName, unsigned int flags);
		~Logger();

		void setAddTime(bool printOnScreen);
		void setPrintOnScreen(bool printOnScreen);
		void setPrintInLogFile(bool printInLogFile);
		void setFlags(unsigned int flags);
		void setConsoleTextColour(ConsoleTextColour colour);

		void log(const string& logMessage);
		template<typename T>
		friend Logger& operator<< (Logger &logger, T object);
		void close();

	private:
		ofstream m_logFile;
		//string m_logFileName;
		//string m_logLocation;
		//Timer m_timer;
		static const unsigned int addTimeFlag;
		static const unsigned int printOnScreenFlag;
		static const unsigned int printInLogFileFlag;
		unsigned int m_flags;
		ConsoleTextColour m_consoleTextColour;

		std::stringstream m_logFileBuffer;
		std::ostream& m_screenBuffer;
	};


	template<typename T>
	Logger& operator<< (Logger &logger, T object)
	{
		if (logger.m_flags & Logger::printInLogFileFlag)
		{
			logger.m_logFileBuffer << object;
			string logMessage(logger.m_logFileBuffer.str());

			if (logger.m_logFile.is_open() && logMessage.length() > 0)
			{
				if (logger.m_flags & Logger::addTimeFlag)
				{
					string time("\n" + getCurrentLocalTimeInNanoSeconds() + ": ");
					logger.m_logFile.write(time.c_str(), time.length());
				}

				logger.m_logFile.write(logMessage.c_str(), logMessage.length());
				//flush() will ensure than the contents are written to file on disk immediately, Otherwise we need to wait until close() is called on it
				logger.m_logFile.flush();
				logger.m_logFileBuffer.str(std::string());
				logger.m_logFileBuffer.clear();
			}
		}

		if (logger.m_flags & Logger::printOnScreenFlag)
			logger.m_screenBuffer << object;

		return logger;
	}

}