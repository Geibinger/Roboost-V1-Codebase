/*
	Copyright (c) 2019 Friedl Jakob

	This project may just be used within the >DIPLOMARBEIT VERONIKA< at HTBLA-Wels and with agreement of the author
*/

#pragma once

#include "DA_COM.h"
#include "DA_Types.hpp"
#include "logger.hpp"
#include <chrono>
#include <math.h>
#include <vector>
#include <mutex>

#define PI 3.1415926535f
#define TIMEOUT_1SEC 1000u

//for debug purpose
#include <iostream>
//Serial.h required!

namespace DA {
	class LIDAR {
	public:
		LIDAR(DA::SerialPort &COM);
		~LIDAR();

		void startConnection(const char * portName);

		void startConnection(DA::SerialPort &COM);

		//@updateHealth: updates the private bool health
		void updateHealth();

		//@updateScanPoints: updates the private distance and angle vectors according to the recent scan -- resetPoints before usage
		bool updateScanPoints();

		//@reboot: sends reboot command to LIDAR
		void reboot();

		//@printScanedPoints: prints out the current points
		//@command: prints wether karthesian or polar koordinats
		void printScanedPoints(const char command);

		//@stop: sends stop command to LIDAR
		void stop();

		//@resetPoints: clears the current angle and distance vectors
		void resetPoints();

		//@getDistanceArr: returns the distance vector as unsigned int array
		unsigned int* getDistanceArr();

		//@getAngleArr: returns the angle vector as float array
		float* getAngleArr();

		//@getPointQuantity: outputs the number of valid measurements -- secure check included
		unsigned int getPointQuantity();

		//@getPointQuantity: outputs the number of unvalid measurements
		unsigned int getUnvalidPoints();

		//@isConnected returns true if the LIDAR is ready to recceve commands
		bool isConnected();

		//@getPointX: returns x value of point i
		float getPointX(const unsigned int i);

		//@getPointX: returns y value of point i
		float getPointY(const unsigned int i);

		//@getScanCount returns number of current scan
		unsigned int getScanCount();

		//@getStatus returns true if LIDAR is OK
		bool getStatus();

		//@getPoints returns recent Point vector
		std::vector<DA::Point> getPoints();

		//@getErrorCount returns current # of errors
		unsigned int getErrorCount();

		//@printHexOutput prints serial output in hex
		void printHexOutput();

		//@waitForOutput waits till n bytes are availiable
		bool waitTilloutputIsAvailable(unsigned int n);

	private:
		std::mutex m_Points;

		SerialPort *COM;

		bool health = false; //status OK if health true

		std::vector<unsigned int> polarDistances;
		std::vector<float> polarAngles;

		std::vector<DA::Point> karthPoints;

		unsigned int scanCount = 0;
		unsigned int errorCount = 0;

		unsigned int unvalidPoints;

		//calculations according to https://www.elecrow.com/download/X4_Lidar_Development_Manual.pdf
		float calcAngleFSA(const int &angleUncorr, const unsigned int &dist1);
		float calcAngleLSA(const int &angleUncorr, const unsigned int &distLSN);
		float calcAngleI(const float &angleDiff, const unsigned int &distI, const float &angleFSA, const float &angleLSA, const unsigned int &sampleQuant, const unsigned int &count);
		unsigned int hexInDec(const unsigned char &LSA, const unsigned char &FSA);

		//@calcPoint: takes polar koordinates as input and outputs a point with karthesian koordinates  
		DA::Point calcPoint(const unsigned int &polarDistance, const float &polarAngle);
	};
}

