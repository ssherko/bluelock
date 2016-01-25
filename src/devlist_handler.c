#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <bluetooth/bluetooth.h>
#include <string.h>

#include "devlist_handler.h"
#include "logger.h"
#include "util.h"

/****************************************
*  FUNCTIONS DEALING WITH KEY_DEVICE_T  *
*****************************************/

/*
Returns a pointer to a string representing 
a device key. See "devlist_handler.h" for the format of the string.

@param key Reference to the internal representation of the device key to serialize.
@return result Reference to the string representation of the device key.
*/
char* serialize_key( key_device_t* key ){
	char* result = (char*)malloc(sizeof(char)*SERIAL_LEN);

	if(result == NULL){
		return result; // error.
	}

	char added_time[DATE_LEN];
	char last_seen[DATE_LEN];
	struct tm* local_time_a;
	struct tm* local_time_l;

	local_time_a = localtime(&(key->added));
	strftime(added_time, sizeof(added_time) , DATE_FORMAT, local_time_a);
	
	local_time_l = localtime(&(key->last_seen));
	strftime(last_seen, sizeof(last_seen) , DATE_FORMAT, local_time_l);

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

/*
Parses a string representation of a device key into
its internal representation.

@param key_str Reference to the string representation.
@return key Reference to the internal representation.
*/
key_device_t* parse_key ( char* key_str ){
	key_device_t* key = (key_device_t*)malloc(sizeof(key_device_t));

	if(key == NULL){
		log_event("<parse_key>", "Error allocating memory", ERRO);
		exit(EXIT_ERR_ALLOC_MEM);
	}
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

	struct tm local_time_a;
	memset(&local_time_a,0,sizeof(local_time_a));
	strptime(added_time, DATE_FORMAT, &local_time_a);
	key->added = mktime(&local_time_a);

	struct tm local_time_l;
	memset(&local_time_l,0,sizeof(local_time_l));
	strptime(last_seen, DATE_FORMAT, &local_time_l);
	key->last_seen = mktime(&local_time_l);

	key->user = (char*)malloc(sizeof(user));
	strcpy(key->user,user);

	bdaddr_t addr;
	str2ba(mac_addr, &addr);
	key->addr = addr;

	return key;
}

/*
Checks whether two keys are equal based on their MAC addresses.

@param key1 The first operand.
@param key2	The second operand.
@return status (TRUE) if equal, (FALSE) otherwise.
*/
bool are_equal(key_device_t* key1, key_device_t* key2){

	bdaddr_t addr1 = key1 -> addr;
	bdaddr_t addr2 = key2 -> addr;

	int i;
	for(i = 0; i < 6; i++){ // bdaddr_t is uint8_t [6]
		uint8_t a1,a2;
		a1 = addr1.b[i];
		a2 = addr2.b[i];
		if(a1 != a2){
			return FALSE;
		}

	}
	return TRUE;
}

/****************************************
*   FUNCTIONS DEALING WITH KEY_STORE    *
*****************************************/

/*
Returns a linked list of type key_store. The list is 
constructed from the KEYSTORE file.

@return store Representation of the list of valid keys.
*/
key_store* fetch_keys(){
	FILE* key_file = fopen(KEY_STORE_PATH, "rb");
	
	if(key_file == NULL){
		return NULL;
	}
	
	key_store* store = NULL;
	size_t read;
	char* current_key = (char*)malloc(sizeof(char)*SERIAL_LEN);
	if(current_key == NULL){
		log_event("<fetch_keys>", "Error allocating memory", ERRO);
		fclose(key_file);
		exit(EXIT_ERR_ALLOC_MEM);
	}
	while((read = fread(current_key,sizeof(char)*SERIAL_LEN,1,key_file)) != 0){
		key_device_t* key = parse_key(current_key);

		store = append_node(store,key);
		memset(current_key,0,sizeof(current_key));
	}
	fclose(key_file);
	return store;
}

/*
Writes a linked list of type key_store onto the KEYSTORE file.

@param store Representation of the list of valid keys.
*/
void persist_keys(key_store* store){
	FILE* key_file = fopen(KEY_STORE_PATH, "wb");
	if(key_file == NULL){
		log_event("<persist_keys>", "Error opening keystore", ERRO);
		fclose(key_file);
		exit(EXIT_ERR_OPEN_KEYSTORE);
	}

	fwrite(store, 0,0 , key_file); // reset file

	if(store == NULL){
		fclose(key_file);
		return;
	}

	key_store* cursor = store;
	char* current_key;
	do{
		current_key = serialize_key(cursor -> key);
		fwrite(current_key, sizeof(char)*SERIAL_LEN,1, key_file);

	}while( (cursor = cursor ->next ) != NULL);

	fclose(key_file);
}	

/*
Checks whether a key is present in the store (i.e.: is valid).
See are_equal(...) function.

@param store Representation of the (linked) list of valid keys.
@param key The key to be checked.
@return pos (-1) if not in store, else the position in which it appears in the linked list.
*/
int check_existence(key_store* store,key_device_t* key){
	if(store == NULL){
		return -1;
	}
	
	int pos = 0;
	key_store* cursor = store;
	do{
		key_device_t* cursor_key = cursor -> key;
		if(are_equal(cursor_key,key)){
			return pos;
		}
		pos ++;
	}while((cursor = cursor -> next) != NULL);

	return -1;
}

/*
Adds (and persists) a new device as a valid bluetooth key.
It doesn't allow duplicates.

@param key The key to be registered.
@return status (TRUE) if device is registered successfully, (FALSE) otherwise.
*/
bool register_key( key_device_t* key ){

	key_store* store = fetch_keys();
	if(check_existence(store,key) < 0){
		store = append_node(store,key);
		persist_keys(store);
		return TRUE;
	}
	return FALSE;
}

/*
Removes a key from the keystore (and persists the rest).

@param key The key to be removed.
@return status (TRUE) if deletion was successful, (FALSE) otherwise.
*/
bool unregister_key( key_device_t* key ){

	key_store* store = fetch_keys();
	int position = check_existence(store, key);

	if(position < 0){
		return FALSE; // Key not found in store.
	}
	store = delete_node(store,position);
	persist_keys(store);
	return TRUE; // successful delete

}

/*
Updates the last_seen field of the key_device_t located at 
the given position in the store. Note that the new keystore (with the updated key)
is persisted, and that the function assumes the position falls within the linked list's length.

@param store The store holding the valid keys.
@param position The position (in the keystore) of the key we want to update.
@return result (TRUE) if key successfully updated, (FALSE) otherwise.
*/
bool update_key(key_store* store, int position){
	key_device_t* to_update = fetch_key(store,position);
	to_update -> last_seen = time(NULL);
	persist_keys(store);
	return TRUE; // success

}

/*
Returns a reference to a key in specified position in the keystore. (i.e.: The i-th valid key)
Note that it doesn't validate the "position" argument. 

@param store The store holding the valid keys.
@param position The position of the key in the store.
*/
key_device_t* fetch_key(key_store* store, int position){
	if(store == NULL){
		return NULL;
	}

	int curr_pos = 0;
	key_store* cursor = store;
	while(curr_pos < position){
		cursor = cursor -> next;
		curr_pos++;
	}

	key_device_t* to_return = cursor -> key;
	return to_return;

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

	free(store);
}


/*****************************************
* FUNCTIONS DEALING WITH THE LINKED LIST *
******************************************/
/*
The usual linked list business.
*/
key_store* append_node(key_store* list, key_device_t* to_insert){
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

key_store* delete_node(key_store* store, int position){
	key_store* cursor = store;
	if(cursor == NULL){
		return NULL;
	}
	if(position < 0){
		return store;
	}
	int curr_pos = 0;
	key_store* prev = cursor;
	while(cursor -> next != NULL && curr_pos < position ){
		prev = cursor;
		cursor = cursor -> next;
		curr_pos ++;
	}

	if(curr_pos == 0){
		store = store -> next;
		return store;
	}

	prev -> next = cursor -> next;
	cursor = NULL;

	return store;

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
