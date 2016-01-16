#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>

#include "../headers/daemon.h"
#include "../headers/util.h"
#include "../headers/devlist_handler.h"
#include "../headers/scanner.h"

void start_daemon(){
	
	discovered_dev_t* nearby = (discovered_dev_t*)malloc(sizeof(discovered_dev_t)*MAX_DEVICES);
	int found = scan_nearby(SCAN_DIST, MAX_DEVICES, nearby);

	int i;
	for(i = 0; i < found; i++){
		discovered_dev_t dev = nearby[i];
		printf("%s\n",disc_dev2str(&dev));
	}

	free(nearby);
}