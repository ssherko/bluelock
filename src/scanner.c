#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "scanner.h"
#include "logger.h"

/*
Scans for nearby bluetooth devices. It looks for a maximum of max_devices to be nearby.
The timeout for the discovery is set to (1.28*inquiry_length) seconds. Finally,
a reference to a discovered_dev_t (see scanner.h) array is passed, used to store the 
results of the scanning.

@param max_devices The number (maximum) of devices that can be discovered.
@param inquiry_length Time to wait for nearby devices to respond.
@param nearby Datastructure to hold information about the nearby/discovered devices.
@return nr_nearby The actual number of devices found nearby.
*/
int scan_nearby(int max_devices, int inquiry_length, discovered_dev_t* nearby){

	int adapter_id = hci_get_route(NULL);
	if(adapter_id < 0){
		log_event("<scan_nearby>", "Error accessing bluetooth device", ERRO);
		exit(1);
	}

	int socket = hci_open_dev(adapter_id);
	
	if(socket < 0){
		log_event("<scan_nearby>", "Error opening socket", ERRO);
		exit(2);
	}

	// Setting parameters for discovery.
	int status; 					// -1 if any error, else is set by HCI_INQUIRY to number of devices discovered
	long flags = IREQ_CACHE_FLUSH;
	inquiry_info* inq_inf = NULL; 	// will store data for discovered devices.
	
	inq_inf = (inquiry_info*)malloc(max_devices*sizeof(inquiry_info));
	if (inq_inf == NULL){
		log_event("<scan_nearby>", "Error allocating resources for inquiry_info", ERRO);
		exit(3);
	}

	status = hci_inquiry(adapter_id, inquiry_length, max_devices, NULL, &inq_inf,flags );
	if(status < 0){
		log_event("<scan_nearby>", "Error hci_inquiry", ERRO);
		exit(4);
	}
	
	int nr_nearby = status;
	int i;
	char name[20];

	for(i = 0; i < nr_nearby; i++){
		discovered_dev_t nearby_dev;
		memset(name,0,sizeof(name));
		bdaddr_t mac_addr = inq_inf[i].bdaddr;
		ba2str(&mac_addr,nearby_dev.addr);
		
		// try to get a name for the device ...
		status = hci_read_remote_name(socket, &mac_addr, sizeof(name),name,0);
		if(status < 0){
			strcpy(nearby_dev.name,"Unknown");
		}else{
			strcpy(nearby_dev.name,name);
		}

		nearby[i] = nearby_dev;

	}

	close(socket);
	free(inq_inf);
	return nr_nearby;

}

/*
Returns a string representation of a discovered device.

@param dev The device to be represented.
@return result The string representation.
*/
char* disc_dev2str(discovered_dev_t* dev){
	char* result = (char*)malloc(sizeof(dev->addr) + sizeof(dev->name) + 2);
	strcpy(result, dev->name);
	strcat(result, ", ");
	strcat(result,dev->addr);

	return result;
}