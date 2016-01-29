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

@param time_per_scan Time to wait for bluetooth devices to respond.
@param store The keystore containing valid bluetooth key devices.
*/
void start_daemon(int time_per_scan, key_store* store){
	discovered_dev_t* nearby = (discovered_dev_t*)malloc(sizeof(discovered_dev_t)*NR_MAX_DISCOVERED_DEVICES);
	state_history history;
	init_history(&history);

	bool unlock_status = 1;
	bool previous_status = 1;
	while(TRUE){
		int found = scan_nearby(NR_MAX_DISCOVERED_DEVICES, TIME_PER_SCAN, nearby);
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
			usleep(SLEEP_TIME*1000);
			continue;
		}
		usleep(250);

	}
}

/*
Initializes (in-place) the state_history struct to mean
that a device has been found in every step.

@param history The struct to initalize.
*/
void init_history(state_history* history){
	int i = 0;
	history -> locks = (bool*)malloc(sizeof(bool)*MAX_HISTORY_LEN);
	for(; i < MAX_HISTORY_LEN; i++){
		history -> locks[i] = 1;
	}
	history -> last_pos = 0;
}

/*
Updates the state_history struct with one more step.

@param history The current history of finding valid keys nearby.
@param status The new status (0 -> no nearby keys, 1 -> at least 1 nearby key) 		
*/
void update_history(state_history* history, bool status){
	int pos = (history -> last_pos) % MAX_HISTORY_LEN;
	history -> locks[pos] = status;
	(history->last_pos)++;
}

/*
Updates the status (by reference) upon which the screen will be locked or unlocked. 
See 'daemon.h' for further details.

@param history The current history.
@param status Reference to the lock/unlock status.
*/
void update_lock_status(state_history history, bool* status){
	int lock = 0;
	int i = 0;
	for(;i < MAX_HISTORY_LEN; i++){
		lock = lock + history.locks[i];
	}
	float cutoff = MAX_HISTORY_LEN / 3;
	
	if(lock >= cutoff){ 
		*status = TRUE;
	}

	if(lock == 0){
		*status = FALSE;
	}
}

/*
Issues a command to lock/unlock the screen whenever the lock_status toggles.

@param lock_status The current lock status
@param previous_status The previous lock status
@return lock_status The new lock status.
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
	for(i = 0; i< MAX_HISTORY_LEN; i++){
		printf("%d, ",history -> locks[i]);
	}
	printf("\n");
}