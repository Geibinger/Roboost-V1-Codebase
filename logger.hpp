#pragma once

#include <iostream>
#include <mutex>
#include <fstream>
#include <ctime>
#include <string>
#include "timer.hpp"

namespace LOG
{
	class Logger
	{
	public:
		// parametrized ctor
		Logger(std::string fileName){
			mStream.open(fileName);

			if (mStream.fail())
			{
				throw std::iostream::failure("Cannot open file: " + fileName);
			}

			std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			char timestamp[26];
			ctime_s(timestamp, sizeof timestamp, &now);
			std::string timestampWithoutEndl(timestamp);
			timestampWithoutEndl = timestampWithoutEndl.substr(0, 24);
			mStream << timestampWithoutEndl.c_str() << std::endl;
		}

		Logger(){}

		// disable copy ctor and copy assignment
		Logger(const Logger&) = delete;
		Logger& operator= (const Logger&) = delete;

		// move ctor and move assignment
		Logger(Logger&& other)
		{
			mStream.close();
			mStream = move(other.mStream);
		}

		Logger& operator=(Logger&& other)
		{
			mStream.close();
			mStream = move(other.mStream);

			return *this;
		}

		// dtor
		~Logger()
		{
			mStream.close();
		}

		void start(std::string fileName) {
			mStream.open(fileName);

			if (mStream.fail())
			{
				throw std::iostream::failure("Cannot open file: " + fileName);
			}

			std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			char timestamp[26];
			ctime_s(timestamp, sizeof timestamp, &now);
			std::string timestampWithoutEndl(timestamp);
			timestampWithoutEndl = timestampWithoutEndl.substr(0, 24);
			mStream << timestampWithoutEndl.c_str() << std::endl;
		}

		// write to log file
		void writeLine(std::string content){
			std::lock_guard<std::mutex> lock(mMutex);

			mStream << std::to_string(this->timer.elapsed()) << ": " << content.c_str() << std::endl;
		}

	private:
		std::ofstream mStream;
		std::mutex mMutex;
		Timer timer;
	};
}
