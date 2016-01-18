#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../headers/util.h"
#include "../headers/entrypoint.h"
#include "../headers/daemon.h"
#include "../headers/devlist_handler.h"
#include "../headers/scanner.h"

int main(int argc, char** argv){

	cmd_args args;
	args = parse_cmd(argc, argv);
	print_header();
	if(args.add){ // The program has been invoked with the -a option (ADD A NEW KEY)
		
		printf("Scanning for nearby devices. Please wait ... \n");
		discovered_dev_t* nearby = (discovered_dev_t*)malloc(sizeof(discovered_dev_t)*MAX_DEVICES);
		int found = scan_nearby(SCAN_DIST, MAX_DEVICES, nearby);
		if(found < 1){
			printf("No nearby devices found. Exiting.\n");
			return 0;
		}

		//list the found devices and prompt the user.
		printf("Found devices: %d\n", found);
		int i;
		for(i = 0; i < found; i++ ){
			discovered_dev_t nearby_dev = nearby[i];
			printf(" - [%d] %s\n",i,disc_dev2str(&nearby_dev));
		}
		printf("Please, select the [<id>] to add as a key. Enter '-1' to abort: ");
		int choice;
		scanf("%d",&choice);

		if(choice < 0){
			printf("User aborted. Exiting.\n");
			return 0;
		}

		if(choice >= found){
			printf("Invalid [<id>]. Exiting.\n");
			return 0;
		}

		char username[USR_LEN];
		printf("Adding device [%d] as a new key. Please, enter the name of the device's user: ",choice);
		scanf("%s",&username[0]);

		discovered_dev_t to_add = nearby[choice];
		key_device_t key;
		key.added =  time(NULL);
		key.last_seen = time(NULL);
		key.device_id = to_add.name;
		key.user = username;
		str2ba(to_add.addr, &(key.addr));

		int status = register_key(&key); // Add to store
		if(status == 0){
			printf("Device [%d] with ID(%s) and ADDR(%s) already exists.\n",choice,to_add.name,to_add.addr);
			printf("Exiting.\n");
			return 0;
		}

		printf("Added device [%d] belonging to '%s' as a new key to the keystore.\n",choice,username);
		printf(" - New Key ( %s )\n",serialize_key(&key));
		printf("Exiting.\n");
		return 0;
	}

	if(args.del){ // The program has been invoked with the -d option (DELETE AN EXISTING KEY)
		key_store* store = fetch_keys();
		int store_len = get_list_length(store);

		printf("Currently recognized as valid bluetooth keys: (%d) \n",store_len);
		list_keys();
		if(store_len == 0){
			printf("No keys to delete. Exiting.\n");
			return 0;
		}

		int choice;
		printf("Please, select the [<id>] of the key to be deleted. Enter '-1' to abort: ");
		scanf("%d",&choice);
		if(choice < 0){
			printf("User aborted. Exiting.\n");
			free(store);
			return 0;
		}

		if(choice >= store_len){
			printf("Invalid [<id>]. Exiting.\n");
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
			printf("Could not delete device [%d].\n", choice);
			free(store);
			return 0;
		}
		
		printf("Deleted device (%s) from the keystore.\n",serialize_key(to_delete->key));
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

	if(args.disp_help){ // The program has been invoked with the -h option (DISPLAY HELP)
		print_help();
		return 0;
	}

	start_daemon();


	return 0;
}
