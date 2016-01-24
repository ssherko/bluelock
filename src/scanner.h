#ifndef SCANNER_H
#define SCANNER_H


typedef struct {
	char name[20];
	char addr[18];
} discovered_dev_t; 


int scan_nearby(int max_devices, int inquiry_len, discovered_dev_t* nearby);
char* disc_dev2str(discovered_dev_t* dev);
#endif