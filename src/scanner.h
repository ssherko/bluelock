/*
	
	'scanner' is responsible for discovering nearby
	bluetooth devices and representing them as entries
	in a discovered_dev_t array (see struct below).

*/

#ifndef SCANNER_H
#define SCANNER_H


typedef struct {
	char name[20];
	char addr[18]; // the MAC address in "XX:XX:...:XX" format
} discovered_dev_t; 


int scan_nearby(int max_devices, int inquiry_len, discovered_dev_t* nearby);
char* disc_dev2str(discovered_dev_t* dev);
#endif