#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#include "util.h"
#include "entrypoint.h"
#include "daemon.h"
#include "devlist_handler.h"
#include "scanner.h"
#include "logger.h"


int main(int argc, char** argv){

	print_header();
	
	// Init stuff.
	cmd_args args;
	DATA_PATH = check_persistent_data(); // defined in entrypoint.h
	KEY_STORE_PATH = (char*)malloc(sizeof(char) * 50);
	LOGS_PATH = (char*)malloc(sizeof(char) * 50);
	strcpy(KEY_STORE_PATH,DATA_PATH);
	strcat(KEY_STORE_PATH, "/keystore");
	strcpy(LOGS_PATH, DATA_PATH);
	strcat(LOGS_PATH, "/logs");
	fetch_settings(DATA_PATH);
	args = parse_cmd(argc, argv);
	int scanf_status = 0; // annoying scanf warning ... 

	// register our signal handler
	if(signal(SIGINT, signal_handler) == SIG_ERR){
		log_event("<main>", "Error setting signal handler for SIGINT. Proceeding.", WARN);
	}
	if(signal(SIGTERM, signal_handler) == SIG_ERR){
		log_event("<main>", "Error setting signal handler for SIGTERM. Proceeding.", WARN);
	}    


	if(args.add){ // The program has been invoked with the -a option (ADD A NEW KEY)
		
		printf("Scanning for nearby devices. Please wait ... \n");
		discovered_dev_t* nearby = (discovered_dev_t*)malloc(sizeof(discovered_dev_t)*NR_MAX_DISCOVERED_DEVICES);
		int found = scan_nearby(NR_MAX_DISCOVERED_DEVICES, TIME_PER_SCAN, nearby);
		if(found < 1){
			printf("No nearby devices found.\n");
			printf("Exiting.\n");
			return 0;
		}

		printf("Found devices: %d\n", found);
		int i;
		for(i = 0; i < found; i++ ){
			discovered_dev_t nearby_dev = nearby[i];
			printf(" - [%d] %s\n",i,disc_dev2str(&nearby_dev));
		}
		printf("Please, select the [<id>] to add as a key. Enter '-1' to abort: ");
		int choice;
		scanf_status = scanf("%d",&choice);

		if(scanf_status == 0){
			log_event("<main>", "No user input",WARN);
			return 0;
		}

		if(choice < 0){
			printf("User aborted.\n"); 
			printf("Exiting.\n");
			return 0;
		}

		if(choice >= found){
			printf("Invalid [<id>].\n");
			printf("Exiting.\n");
			return 0;
		}

		char username[USR_LEN];
		printf("Adding device [%d] as a new key. Please, enter the name of the device's user: ",choice);
		scanf_status = scanf("%s",&username[0]);

		if(scanf_status == 0){
			log_event("<main>", "No user input",WARN);
			return 0;
		}

		discovered_dev_t to_add = nearby[choice];
		key_device_t key;
		key.added =  time(NULL);
		key.last_seen = time(NULL);
		key.device_id = to_add.name;
		key.user = username;
		str2ba(to_add.addr, &(key.addr));

		int status = register_key(&key);
		if(status == 0){
			printf("Device [%d] with ID(%s) and ADDR(%s) already exists.\n",choice,to_add.name,to_add.addr);
			printf("Exiting.\n");
			return 0;
		}

		printf("Added device [%d] belonging to '%s' as a new key to the keystore.\n",choice,username);
		printf(" - New Key ( %s )\n",serialize_key(&key));
		printf("Exiting.\n");

		char* log_msg = (char*)malloc(sizeof(char)*100);
		strcpy(log_msg, "Added new key device, belonging to '");
		strcat(log_msg, username);
		strcat(log_msg,"'");

		log_event("<main>", log_msg, INFO);
		free(log_msg);
		return 0;
	}

	if(args.del){ // The program has been invoked with the -d option (DELETE AN EXISTING KEY)
		key_store* store = fetch_keys();
		int store_len = get_list_length(store);

		printf("Currently recognized as valid bluetooth keys: (%d) \n",store_len);
		list_keys();
		if(store_len == 0){
			printf("No keys to delete.\n");
			printf("Exiting.\n");
			return 0;
		}

		int choice;
		printf("Please, select the [<id>] of the key to be deleted. Enter '-1' to abort: ");
		scanf_status = scanf("%d",&choice);

		if(scanf_status == 0){
			log_event("<main>", "No user input",WARN);
			return 0;
		}
		
		if(choice < 0){
			printf("User aborted.\n");
			printf("Exiting.\n");
			free(store);
			return 0;
		}

		if(choice >= store_len){
			printf("Invalid [<id>].\n");
			printf("Exiting.\n");
			free(store);
			return 0;
		}

		int curr_pos = 0;
		key_store* to_delete = store;
		while(curr_pos < choice){
			to_delete = to_delete -> next;
			curr_pos++;
		}
		int status = unregister_key(to_delete -> key);
		if(status == 0){
			printf("Could not delete key [%d].\n", choice);
			free(store);
			return 0;
		}
		
		printf("Deleted key (%s) from the keystore.\n",serialize_key(to_delete->key));

		char* log_msg = (char*)malloc(sizeof(char)*100);
		strcpy(log_msg, "Deleted device belonging to '");
		strcat(log_msg, to_delete->key->user);
		strcat(log_msg,"'");

		log_event("<main>", log_msg, INFO);
		free(log_msg);

		free(store);
		return 0;
	}

	if(args.list_keys){ // The program has been invoked with the -l option (LIST EXISTING KEYS)
		key_store* store = fetch_keys();
		int store_len = get_list_length(store);
		printf("Currently recognized as valid bluetooth keys: (%d) \n",store_len);
		list_keys();
		free(store);
		return 0;
	}

	if(args.list_params){ // The program has been invoked with the -s option (LIST PARAMETERS)
		list_params();
		printf("Exiting.\n");
		return 0;

	}

	if(args.edit_param){ // The program has been invoked with the -e option (EDIT PARAMETER)
		printf("Note: It is advised not to change the default daemon parameters.\n");
		list_params();
		int choice;
		printf("Please, select the [<id>] of the parameter to edit. Enter '-1' to abort:  ");
		scanf_status = scanf("%d",&choice);

		if(scanf_status == 0){
			log_event("<main>", "No user input",WARN);
			return 0;
		}
		
		if(choice < 0){
			printf("User aborted.\n");
			printf("Exiting.\n");
			return 0;
		}

		if(choice > 3){
			printf("Invalid [<id>].\n");
			printf("Exiting.\n");
			return 0;
		}

		int new_value;

		switch(choice){
			case 0:
				printf("Editing 'NR_MAX_DISCOVERED_DEVICES'. Enter new value: ");
				scanf_status = scanf("%d",&new_value);
				NR_MAX_DISCOVERED_DEVICES = new_value;
				break;

			case 1:
				printf("Editing 'MAX_HISTORY_LEN'. Enter new value: ");
				scanf_status = scanf("%d",&new_value);
				MAX_HISTORY_LEN = new_value;
				break;
			
			case 2:
				printf("Editing 'SLEEP_TIME'. Enter new value: ");
				scanf_status = scanf("%d",&new_value);
				SLEEP_TIME = new_value;
				break;
			case 3:
				printf("Editing 'TIME_PER_SCAN'. Enter new value: ");
				scanf_status = scanf("%d",&new_value);
				TIME_PER_SCAN = new_value;
				break;
		}

		if(scanf_status == 0){
			log_event("<main>", "No user input",WARN);
			return 0;
		}
		
		if(validate_value(new_value)){
			persist_settings(DATA_PATH);
			printf("Successfully modified daemon parameters.\n");
			list_params();
			printf("Exiting.\n");
			return 0;
		}

		printf("The new value should be between 1 and 99999.\n");
		printf("Daemon parameters not modified.\n");
		printf("Exiting.\n");
		return 0;
	}

	if(args.disp_help){ // The program has been invoked with the -h option (DISPLAY HELP)
		print_help();
		return 0;
	}

	if(args.list_logs){
		printf("Printing logs.\n");
		print_logs(LOGS_PATH);
		printf("Exiting.\n");
		return 0;
	}

	key_store* store = fetch_keys();
	int store_len = get_list_length(store);

	if(store_len == 0){
		printf("No keys have been added to the keystore. Daemon cannot be run until keys are added.\n");
		printf("To add keys, invoke the program with the -a option (-h to list help).\n");
		printf("Exiting.\n");
		return 0;
	}
	
	log_event("<main>", "Started running service as a daemon", INFO);
	start_daemon(TIME_PER_SCAN,store);


	return 0;
}
