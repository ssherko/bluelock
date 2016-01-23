#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <time.h>
#include <unistd.h>

#include "daemon.h"
#include "devlist_handler.h"
#include "scanner.h"
#include "messagebus.h"
#include "logger.h"
#include "util.h"

/*
Begins scanning for nearby bluetooth devices. If any, it checks the keystore whether 
they're valid or not. Locks/unlocks the screen appropriately.

@param time_per_scan 		int 	How long to wait for bluetooth devices to respond.
@param key_store* 			store   The keystore containing valid bluetooth key devices.
@return None
*/
void start_daemon(int time_per_scan, key_store* store){
	discovered_dev_t* nearby = (discovered_dev_t*)malloc(sizeof(discovered_dev_t)*MAX_DEVICES);
	state_history history;
	init_history(&history);

	bool unlock_status = 1;
	bool previous_status = 1;
	while(1){
		int found = scan_nearby(MAX_DEVICES, time_per_scan, nearby);
		int i;
		bool status = FALSE;
		key_device_t* found_device = NULL;

		for(i = 0; i < found; i++){
			key_device_t* current = (key_device_t*)malloc(sizeof(key_device_t));
			discovered_dev_t dev = nearby[i];
			current -> device_id = (char*)malloc(sizeof(char)*ID_LEN);
			str2ba(dev.addr, &(current->addr));
			int pos = -1;
			if((pos = check_existence(store,current)) >= 0){
				status = 1;
				update_key(store, pos);
				found_device = fetch_key(store,pos);
				break;
			}
			free(current);
		}
		update_history(&history,status);
		update_lock_status(history,&unlock_status);
		previous_status = execute_status(unlock_status,previous_status, found_device);
		
		if(unlock_status == 1){
			usleep(2000);
			continue;
		}
		usleep(250);

	}
}

/*
Initializes (in-place) the state_history struct to mean
that a device has been found in every step.

@param state_history*		history 	The struct to initialize.
@return None
*/
void init_history(state_history* history){
	int i = 0;
	for(; i < HISTORY_LEN; i++){
		history -> locks[i] = 1;
	}
	history -> last_pos = 0;
}

/*
Updates the state_history struct with one more step.

@param state_history* 		history 	The current history of finding valid keys nearby.
@param int 					status 		The new status (0 -> no nearby keys, 1 -> at least 1 nearby key) 		
*/
void update_history(state_history* history, bool status){
	int pos = (history -> last_pos) % HISTORY_LEN;
	history -> locks[pos] = status;
	(history->last_pos)++;
}

/*
Updates the status (by reference) upon which the screen will be locked or unlocked. 
See 'daemon.h' for further details.

@param state_history		history 	The current history.
@param int* 				status 		Reference to the lock/unlock status.
@return None
*/
void update_lock_status(state_history history, bool* status){
	int lock = 0;
	int i = 0;
	for(;i < HISTORY_LEN; i++){
		lock = lock + history.locks[i];
	}

	if(lock >= 1){ 
		*status = TRUE;
	}

	if(lock == 0){
		*status = FALSE;
	}
}

/*
Issues a command to lock/unlock the screen whenever the lock_status toggles.

@param int 		lock_status 		The current lock status
@param int 		previous_status		The previous lock status
@return int 	lock_status 		The new lock status.
*/
bool execute_status(bool lock_status, bool previous_status, key_device_t* key){
	if(lock_status == previous_status){
		return lock_status;
	}
	
	if(lock_status == FALSE){
		bus_send_message("LOCK"); // log after check.
		log_event("<execute_status>","Locked screen, no valid keys nearby",INFO);

		
	}else{
		bus_send_message("UNLOCK"); // same here
		char* user = key -> user;
		char* msg = (char*)malloc(sizeof(char) * (USR_LEN + 50));
		strcpy(msg,"Unlocked screen, valid key nearby (");
		strcat(msg,user);
		strcat(msg,")");

		log_event("<execute_status>",msg,INFO);
		free(msg);
	}
	return lock_status;
}

void print_history(state_history* history){
	int i;
	for(i = 0; i< HISTORY_LEN; i++){
		printf("%d, ",history -> locks[i]);
	}
	printf("\n");
}