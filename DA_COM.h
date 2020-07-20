/*
	Copyright (c) 2019 Friedl Jakob

	This project may just be used within the >DIPLOMARBEIT VERONIKA< at HTBLA-Wels and with agreement of the author
*/

#pragma once

#include <windows.h>
#include <string>
#include "logger.hpp"

namespace DA {
	class SerialPort
	{
	public:
		SerialPort(const char *portName);
		SerialPort();
		~SerialPort();

		//@readSerialPort: reads buf_size bytes into the buffer
		unsigned int readSerialPort(unsigned char *buffer, unsigned int buf_size);

		//@writeSerialPort: writes buf_size of buffer into Serialport
		bool writeSerialPort(const char *buffer, unsigned int buf_size);

		//@isConnected: returns true if the connection is established
		bool isConnected();
		unsigned int available();
		bool available(unsigned int n);
		unsigned int clearRest();
		void closePort();
		void printOutput();
		void openPort(const char *portName);

	private:
		HANDLE handler;
		bool connected;
		COMSTAT status;
		DWORD errors;
		LOG::Logger processLogger;
	};
}