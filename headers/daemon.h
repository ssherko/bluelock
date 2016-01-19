/*
	"daemon" is responsible for running the lock/unlock service in the background.
	Based on the values stored in the "state_history" struct, a decision is made
	whether to lock or unlock the screen. "state_history" emulates a circular array A with binary entries,
	meaning:
		if A[i] = 0 -> no valid key device found at step i
		   A[i] = 1 -> a valid key device found at step i

	Since the bluetooth discovery service is (sometimes) unreliable, if A contains 1 non-zero entry,
	the daemon should unlock the screen. However, it should lock the screen iff all entries in A are 0.
*/

#ifndef DAEMON_H
#define DAEMON_H

#define MAX_DEVICES 5 
#include "devlist_handler.h"
#define HISTORY_LEN 3

typedef struct{
	int locks[HISTORY_LEN];
	int last_pos;
} state_history;

void start_daemon(int time_per_scan, key_store* store);
void init_history(state_history* history);
void update_history(state_history* history, int status );
void print_history(state_history* history);
void update_lock_status(state_history history, int* status);
int execute_status(int lock_status, int previous_status);

#endif