#include "../headers/devlist_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <bluetooth/bluetooth.h>
#include <string.h>

/****************************************
*  FUNCTIONS DEALING WITH KEY_DEVICE_T  *
*****************************************/

/**
Returns a pointer to a string representing 
a device key. See "devlist_handler.h" for the format of the string.

@param	key 		key_device_t* to the internal representation of the device key to serialize
@return result		char* to the string representation of the device key.
**/
char* serialize_key( key_device_t* key ){
	char* result = (char*)malloc(sizeof(char)*SERIAL_LEN);

	if(result == NULL){
		return result; // error.
	}

	char added_time[DATE_LEN];
	char last_seen[DATE_LEN];
	struct tm* local_time;

	local_time = localtime(&(key->added));
	strftime(added_time, sizeof(added_time) , DATE_FORMAT, local_time);

	local_time = localtime(&(key->last_seen));
	strftime(last_seen, sizeof(last_seen) , DATE_FORMAT, local_time);

	strcpy(result,key->device_id);
	strcat(result, ": { ");

	strcat(result,key->user);
	strcat(result,", A:");


	strcat(result, added_time);
	strcat(result, ", L:");

	strcat(result,last_seen);
	strcat(result, ", ");

	char mac_addr[MAC_LEN];
	ba2str(&(key->addr),mac_addr);
	strcat(result, mac_addr);

	strcat(result, " }");
	
	return result;
} 

/**
Parses a string representation of a device key into
its internal representation.

@param key_str 		char* to the string representation.
@return key 		key_device_t* to the internal representation.
**/
key_device_t* parse_key ( char* key_str ){
	key_device_t* key = (key_device_t*)malloc(sizeof(key_device_t));

	char device_id[ID_LEN];
	int j = 0;

	int i;
	for(i = 0; key_str[i] != ':'; i++){
		device_id[j] = key_str[i];
		j++;
	}
	device_id[j] = '\0';

	while(key_str[i] == ' ' || key_str[i] == ':' || key_str[i] == '{')
		i++;
	
	char user[USR_LEN];
	j = 0;

	for(; key_str[i] != ','; i++){
		user[j] = key_str[i];
		j++;
	}
	user[j] = '\0';

	i = i + 4 ; // skip over a comma, whitespace, a character and ":"

	char added_time[DATE_LEN];
	j = 0;

	for(; key_str[i] != ','; i++){
		added_time[j] = key_str[i];
		j++;
	}
	added_time[j] = '\0';
	i = i + 4; // skip again

	char last_seen[DATE_LEN];
	j = 0;

	for(; key_str[i] != ','; i++){
		last_seen[j] = key_str[i];
		j++;
	}
	last_seen[j] = '\0';

	char mac_addr[MAC_LEN];
	j = 0;

	i = i + 2; // skip over a comma and a whitespace.

	for(; key_str[i] != ' '; i++){
		mac_addr[j] = key_str[i];
		j++;
	}
	mac_addr[j] = '\0';
	
	// build the actual key_device_t struct
	key->device_id = (char*)malloc(sizeof(device_id));
	strcpy(key->device_id, device_id);

	struct tm local_time;
	strptime(added_time, DATE_FORMAT, &local_time);
	key->added = mktime(&local_time);

	strptime(last_seen, DATE_FORMAT, &local_time);
	key->last_seen = mktime(&local_time);

	key->user = (char*)malloc(sizeof(user));
	strcpy(key->user,user);

	bdaddr_t addr;
	str2ba(mac_addr, &addr);
	key->addr = addr;

	return key;
}

/****************************************
*   FUNCTIONS DEALING WITH KEY_STORE    *
*****************************************/

key_store* fetch_keys(){
	FILE* key_file = fopen(KEY_STORE_PATH, "rb");
	if(key_file == NULL){
		return NULL;
	}
	key_store* store = NULL;
	size_t read;
	char* current_key = (char*)malloc(sizeof(char)*SERIAL_LEN);
	while((read = fread(current_key,sizeof(char)*SERIAL_LEN,1,key_file)) != 0){
		key_device_t* key = parse_key(current_key);
		store = insert_node(store,key);
		memset(current_key,0,sizeof(current_key));
	}
	
	return store;
}

void persist_keys(key_store* store){
	FILE* key_file = fopen(KEY_STORE_PATH, "wb");
	// TODO: error checking

	key_store* cursor = store;
	char* current_key;
	do{
		current_key = serialize_key(cursor -> key);
		fwrite(current_key, sizeof(char)*SERIAL_LEN,1, key_file);

	}while( (cursor = cursor ->next ) != NULL);

	fclose(key_file);
}	

void register_key( key_device_t* key ){
	key_store* store = fetch_keys();
	store = insert_node(store,key);
	persist_keys(store);
}


void unregister_key( key_device_t* key ){

}


void list_keys(){
	key_store* store = fetch_keys();
	key_store* cursor = store;
	int i = 0;
	if(cursor == NULL){
		printf(" - No keys have been added.\n");
		return;
	}
	do{
		printf(" - [%d] %s\n", i, serialize_key(cursor -> key));
		i++;
	}while((cursor = cursor -> next) != NULL);

}

/*****************************************
* FUNCTIONS DEALING WITH THE LINKED LIST *
******************************************/

key_store* insert_node(key_store* list, key_device_t* to_insert){
	key_store* cursor = list;

	key_store* tail = (key_store*)malloc(sizeof(key_store));
	tail->next = NULL;
	tail->key = to_insert;

	if(list == NULL){
		list = tail;
	}else{
		while(cursor -> next != NULL){
			cursor = cursor -> next;
		}

		cursor -> next = tail;
	}
	return list;
}

void delete_node(key_store* list, int position){

}

int get_list_length(key_store* list){
	if(list == NULL){
		return 0;
	}
	
	int length = 1;
	key_store* cursor = list;
	while((cursor = cursor -> next) != NULL){
		length++;
	}

	return length;
}

void print_list(key_store* list){
	if(list == NULL){
		return; 
	}

	key_store* cursor = list;
	do{
		printf("%s\n",serialize_key(cursor->key));
	}while((cursor = cursor -> next) != NULL);
}