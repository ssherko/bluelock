#ifndef SCANNER_H
#define SCANNER_H
#define SCAN_DIST 1 //meters 
#define INQU_LEN 2// INQU_LENGTH * 1.28 sec 

typedef struct {
	char name[20];
	char addr[18];
} discovered_dev_t; 

int scan_nearby(int distance, int max_devices, discovered_dev_t* nearby);
char* disc_dev2str(discovered_dev_t* dev);
#endif