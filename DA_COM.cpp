/*
	Copyright (c) 2019 Friedl Jakob

	This project may just be used within the >DIPLOMARBEIT VERONIKA< at HTBLA-Wels and with agreement of the author
*/

#include "DA_COM.h"

DA::SerialPort::SerialPort(const char *portName) {
	this->openPort(portName);
}

DA::SerialPort::SerialPort() {}

DA::SerialPort::~SerialPort()
{
	if (this->connected) {
		this->connected = false;
		CloseHandle(this->handler);
	}
}


void DA::SerialPort::openPort(const char *portName) {

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 12);

	this->connected = false;

	this->handler = CreateFileA(static_cast<LPCSTR>(portName),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (this->handler == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {

			printf("<COM>\tERROR:\tHandle was not attached. Reason: %s not available\n", portName);
		}
		else
		{
			printf("<COM>\tERROR:\tunkown error!\n");
		}
	}
	else {
		DCB dcbSerialParameters = { 0 };

		if (!GetCommState(this->handler, &dcbSerialParameters)) {
			printf("<COM>\tERROR:\tfailed to get current serial parameters\n");
		}
		else {
			dcbSerialParameters.BaudRate = CBR_128000;
			dcbSerialParameters.ByteSize = 8;
			dcbSerialParameters.StopBits = ONESTOPBIT;
			dcbSerialParameters.Parity = NOPARITY;
			dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

			if (!SetCommState(handler, &dcbSerialParameters))
			{
				printf("<COM>\tALERT:\tcould not set Serial port parameters\n");
			}
			else {
				this->connected = true;
				PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
			}
		}
	}
	SetConsoleTextAttribute(hConsole, 15);
}

void DA::SerialPort::closePort() {
	if (this->connected) {
		this->connected = false;
		CloseHandle(this->handler);
	}
}

unsigned int DA::SerialPort::readSerialPort(unsigned char *buffer, unsigned int buf_size)
{
	DWORD bytesRead;
	unsigned int toRead = 0;

	ClearCommError(this->handler, &this->errors, &this->status);

	if (this->status.cbInQue > 0) {
		if (this->status.cbInQue > buf_size) {
			toRead = buf_size;
		}
		else toRead = this->status.cbInQue;
	}
	else return 0;

	if (ReadFile(this->handler, buffer, toRead, &bytesRead, NULL)) return bytesRead;

	return 0;
}

bool DA::SerialPort::writeSerialPort(const char *buffer, unsigned int buf_size)
{
	DWORD bytesSend;

	if (!WriteFile(this->handler, (void*)buffer, buf_size, &bytesSend, 0)) {
		ClearCommError(this->handler, &this->errors, &this->status);
		return false;
	}
	else return true;
}

bool DA::SerialPort::isConnected()
{
	return this->connected;
}

bool DA::SerialPort::available(unsigned int n)
{
	ClearCommError(this->handler, &this->errors, &this->status);
	if (this->status.cbInQue >= n)
	{
		return true;
	}
	else return false;
}

unsigned int DA::SerialPort::clearRest() {
	ClearCommError(this->handler, &this->errors, &this->status);
	unsigned char *rest = new unsigned char[this->status.cbInQue];
	SerialPort::readSerialPort(rest, this->status.cbInQue);
	delete[] rest;
	return this->status.cbInQue;
}

unsigned int DA::SerialPort::available() {
	ClearCommError(this->handler, &this->errors, &this->status);
	return this->status.cbInQue;
}

void DA::SerialPort::printOutput() {
	if (this->available()) {
		printf_s("Serial OUT:\n");
		while (this->available()) {
			for (unsigned int i = 0; i < 10; i++) {
				if (this->available()) {
					unsigned char *outByte = new unsigned char[1];
					this->readSerialPort(outByte, 1);

					printf_s("%hx\t", (unsigned int)outByte);

					delete[] outByte;
				}
				else break;
			}
			printf_s("\n");
		}
	}
}