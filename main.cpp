/*
	Copyright (c) 2019 Friedl Jakob

	This project may only be used within the >DIPLOMARBEIT VERO< at HTBLA-Wels and with agreement of the author

	last edit:					18.02.2020 00:14
	approximate working time:	288:45h


	TODO:
			-Map erstellen lassen (SLAM)
			-Motoransteuerung
*/

#pragma once
#include "SYSTEM.hpp"

DA::SYSTEM sys;

int main(void) {
	std::cout << "Diplomarbeit der HTL-Wels Mechatronik" << std::endl << "Autor:\tFriedl Jakob" << std::endl;


	sys.setupGUI();
	sys.startTesting();
	//sys.startLiveConnection();

	std::cin.get();

	return 0;
}
