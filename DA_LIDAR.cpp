/*
	Copyright (c) 2019 Friedl Jakob

	This project may just be used within the >DIPLOMARBEIT VERONIKA< at HTBLA-Wels and with agreement of the author
*/

#include "DA_LIDAR.h"
#define RECCIVETIME 1 // in ms


DA::LIDAR::LIDAR(DA::SerialPort &COM) {
	this->COM = &COM;
	this->reboot();
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	this->updateHealth();
	if(this->health)
		printf_s("<LIDAR> INFO: LIDAR initialized\n");
	else
		printf_s("<LIDAR> ERROR: LIDAR failed to initialize\n");
}


DA::LIDAR::~LIDAR() {
	this->COM->writeSerialPort("\xA5\x65", 2);
}

//public:

void DA::LIDAR::startConnection(const char *portName) {
	this->COM->openPort(portName);
	this->reboot();
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	this->updateHealth();
	if (this->health)
		printf_s("<LIDAR> INFO: LIDAR initialized\n");
	else
		printf_s("<LIDAR> ERROR: LIDAR failed to initialize\n");
}


void DA::LIDAR::startConnection(DA::SerialPort &COM) {
	this->COM = &COM;
	this->reboot();
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	this->updateHealth();
	if (this->health)
		printf_s("<LIDAR> INFO: LIDAR initialized\n");
	else
		printf_s("<LIDAR> ERROR: LIDAR failed to initialize\n");
}

unsigned int DA::LIDAR::getScanCount() {
	return this->scanCount;
}

void DA::LIDAR::updateHealth() {

	if (COM->isConnected()) {
		bool commandFinished = 0;

		COM->clearRest();
		COM->writeSerialPort("\xA5\x91", 2);
		
		while (!commandFinished) {
			if (COM->available(2)) {
				unsigned char *startSign = new unsigned char[2];
				COM->readSerialPort(startSign, 2);

				if (startSign[0] == (unsigned char)'\xA5' && startSign[1] == (unsigned char)'\x5A') {
					unsigned char *header = new unsigned char[5];

					std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
					std::chrono::high_resolution_clock::time_point t2;

					std::this_thread::sleep_for(std::chrono::milliseconds(RECCIVETIME));

					COM->readSerialPort(header, 5);
					unsigned char contentLength = header[0];
					unsigned char *content = new unsigned char[contentLength];
					COM->readSerialPort(content, contentLength);
					this->health = !content[0];
					printf_s("<LIDAR> INFO: Health updated\n");

					commandFinished = true;
					delete[] content;
					delete[] header;
				}
				//else printf_s("<LIDAR> ERROR:\tInvalid garbadge in port buffer\n");
				delete[] startSign;
			}
		}
	}
	else printf_s("<LIDAR> ERROR:\tDevice not found\n");
}

bool DA::LIDAR::updateScanPoints() {
	if (this->health) {

		if(COM->isConnected()) {
			unsigned int zeroPackageCount = 0;
			unsigned int scanBytes = 0;
			bool commandFinished = false;
			bool error = false;
			bool startRead = false;
			this->polarDistances.clear();
			this->polarAngles.clear();
			
			COM->clearRest();
			COM->writeSerialPort("\xA5\x60", 2);

			while (!commandFinished && COM->isConnected() || error) {

				std::this_thread::sleep_for(std::chrono::milliseconds(RECCIVETIME));

				if (COM->available(2)) {
					unsigned char *startSign = new unsigned char[2];		//The X4 packet is unified as 0xA55A, for scan package fixed at 0x55AA, low in front, high in back
					COM->readSerialPort(startSign, 2);

					if (startSign[0] == 0xA5 && startSign[1] == 0x5A) {
						this->scanCount++;
						unsigned char *packageHeader = new unsigned char[5];

						std::this_thread::sleep_for(std::chrono::milliseconds(RECCIVETIME));

						COM->readSerialPort(packageHeader, 5);
								
						delete packageHeader;
					}
					else if (startSign[0] == 0xAA && startSign[1] == 0x55) {
						unsigned char *ScanHeader = new unsigned char[8];

						std::this_thread::sleep_for(std::chrono::milliseconds(RECCIVETIME));

						COM->readSerialPort(ScanHeader, 8);

						if (ScanHeader[0] == 0x00) {
							scanBytes = (unsigned int)ScanHeader[1] * 2;
							unsigned int scanCount = scanBytes / 2;
							unsigned char *outputContent = new unsigned char[scanBytes];
									
							std::this_thread::sleep_for(std::chrono::milliseconds(RECCIVETIME));

							COM->readSerialPort(outputContent, scanBytes);

							if (startRead) {
								float angleFSA = calcAngleFSA(hexInDec(ScanHeader[2], ScanHeader[3]), hexInDec(outputContent[0], outputContent[1]) / 4);
								float angleLSA = calcAngleLSA(hexInDec(ScanHeader[4], ScanHeader[5]), hexInDec(outputContent[scanBytes - 1], outputContent[scanBytes]) / 4);

								unsigned int *distArr = new unsigned int[scanCount];
								float *angleArr = new float[scanCount];

								for (unsigned int i = 0; i < scanBytes; i += 2) {

									unsigned int dist = hexInDec(outputContent[i], outputContent[i + 1]) / 4;

									distArr[i / 2] = dist;

									float angle = calcAngleI(angleLSA - angleFSA, distArr[i / 2], angleFSA, angleLSA, scanCount, i / 2);

									angleArr[i / 2] = angle;

									if (dist > 0.f && angleLSA > angleFSA) {
										std::lock_guard<std::mutex> lk(m_Points);

										polarDistances.emplace_back(dist);
										polarAngles.emplace_back(angle);

										karthPoints.emplace_back(calcPoint(dist, angle));
									}
									else this->unvalidPoints++;
								}

								delete[] distArr;
								delete[] angleArr;
							}
							delete[] outputContent;
						}
						else if (ScanHeader[0] == 0x01) {
							zeroPackageCount++;
							
							unsigned char *outputContent = new unsigned char[2];  

							std::this_thread::sleep_for(std::chrono::milliseconds(RECCIVETIME));

							COM->readSerialPort(outputContent, 2);

							if (startRead) {

								unsigned int dist = hexInDec(outputContent[0], outputContent[1]) / 4;

								float angle = 360.f;
								
								if (dist > 0) {
									std::lock_guard<std::mutex> lk(m_Points);

									polarDistances.emplace_back(dist);
									polarAngles.emplace_back(angle);

									karthPoints.emplace_back(calcPoint(dist, angle));
								}
							}
							delete[] outputContent;
						}

						if (zeroPackageCount == 1) {
							startRead = true;
						}
						else if (zeroPackageCount >= 2) {
							startRead = false;
							commandFinished = true;
							zeroPackageCount = 0;
							COM->writeSerialPort("\xA5\x65", 2);
						}
						delete[] ScanHeader;
					}
					else {
						//printf_s("<LIDAR> ERROR:\tInvalid garbadge in port buffer\n");
						this->errorCount++;
						return false;
					}
					delete[] startSign;
				}
			}
		}
		else {
		printf_s("<LIDAR> ERROR:\tDevice not found\n");
		this->errorCount++;
		return false;
		}
	}
	else {
		printf_s("<LIDAR> ERROR:\tDevice error!\n\thealth != 0x00\n");
		this->errorCount++;
		this->reboot();
		return false;
	}
	return true;
}

bool DA::LIDAR::waitTilloutputIsAvailable(unsigned int n) {
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	std::chrono::high_resolution_clock::time_point t2;
	while (!COM->available(n)) {
		t2 = std::chrono::high_resolution_clock::now();

		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

		if (duration > TIMEOUT_1SEC) {
			this->errorCount++;
			this->reboot();
			return false;
		}
	}
	return true;
}

void DA::LIDAR::printScanedPoints(const char command) {

	if (command == 'p') {
		if (this->polarDistances.size() != 0) {
			if (this->polarAngles.size() == this->polarDistances.size()) {
				printf_s("Saved points (polar):\n");
				for (unsigned int i = 0; i < this->polarAngles.size(); i++) {
					printf_s("\tPoint %d:\t%.2f°\t%dmm\n", i, this->polarAngles.at(i), this->polarDistances.at(i));
				}
			}
			else printf_s("<LIDAR> ERROR:\tangle and distance array length not equal!!");
		}
		else printf_s("<LIDAR> ERROR:\tno points availiable");
	}
	else if (command == 'k') {
		printf_s("Saved points (karthesian):\n");
		for (unsigned int i = 0; i < this->karthPoints.size(); i++) {
			printf_s("%.2f:%.2f:%.2f\n",this->karthPoints.at(i).x, this->karthPoints.at(i).y, this->polarAngles.at(i));
		}
	}
	else printf_s("<LIDAR> ERROR:\tinvalid command");
}

void DA::LIDAR::reboot() {
	COM->writeSerialPort("\xA5\x65", 2);
	COM->writeSerialPort("\xA5\x40", 2);
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	COM->clearRest();
}

void DA::LIDAR::stop() {
	COM->closePort();
}

void DA::LIDAR::resetPoints() {
	this->polarDistances.clear();
	this->polarAngles.clear();
	this->karthPoints.clear();
	this->unvalidPoints = 0;
}

unsigned int* DA::LIDAR::getDistanceArr() {
	return &this->polarDistances[0];
}

float* DA::LIDAR::getAngleArr() {
	return &this->polarAngles[0];
}

float DA::LIDAR::getPointX(unsigned int i) {
	return this->karthPoints.at(i).x;
}

float DA::LIDAR::getPointY(unsigned int i) {
	return this->karthPoints.at(i).y;
}

unsigned int DA::LIDAR::getPointQuantity() {
	if (this->polarAngles.size() == this->polarDistances.size()) {
		return this->karthPoints.size();
	}
	else return 0;
}

unsigned int DA::LIDAR::getUnvalidPoints() {
	return this->unvalidPoints;
}

bool DA::LIDAR::isConnected() {
	return this->COM->isConnected();
}

bool DA::LIDAR::getStatus() {
	return this->health;
}

std::vector<DA::Point> DA::LIDAR::getPoints() {
	return this->karthPoints;
}

unsigned int DA::LIDAR::getErrorCount() {
	return this->errorCount;
}

//private:
float DA::LIDAR::calcAngleFSA(const int &angleUncorr, const unsigned int &dist1) {

	if (dist1 == 0) {
		return ((angleUncorr >> 1)) / 64.f;
	}
	else {
		return (((float)(angleUncorr >> 1)) / 64.f) + (atan(21.8f*((155.3f - (float)dist1) / (155.3f*(float)dist1))));
	}
}

float DA::LIDAR::calcAngleLSA(const int &angleUncorr, const unsigned int &distLSN) {
	if (distLSN == 0) {
		return ((float)(angleUncorr >> 1)) / 64.f;
	}
	else {
		return (((float)(angleUncorr >> 1)) / 64.f) + (atan(21.8f*((155.3f - (float)distLSN) / (155.3f*(float)distLSN))));
	}
}

float DA::LIDAR::calcAngleI(const float &angleDiff, const unsigned int &distI, const float &angleFSA, const float &angleLSA, const unsigned int &sampleQuant, const unsigned int &count) {
	if (distI == 0.f) {
		return (((angleDiff)) / (sampleQuant -1.f))*(count) + angleFSA;
	}
	else return (((angleDiff)) / (sampleQuant - 1.f))*(count)+angleFSA + float(atan(21.8*((155.3 - (float)distI) / (155.3*(float)distI))));
}

unsigned int DA::LIDAR::hexInDec(const unsigned char &LSA, const unsigned char &FSA) {
	return LSA + (unsigned int)(FSA * pow(16, 2));
}


DA::Point DA::LIDAR::calcPoint(const unsigned int &polarDistance, const float &polarAngle) {
	DA::Point point;

	point.x = (float)polarDistance * sin((float)polarAngle * (PI / 180.f));
	point.y = -(float)polarDistance * cos((float)polarAngle * (PI / 180.f));
	return point;
}

void DA::LIDAR::printHexOutput() {
	COM->printOutput();
}