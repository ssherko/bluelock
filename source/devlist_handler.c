#include "../headers/devlist_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <bluetooth/bluetooth.h>
#include <string.h>
/**
Returns a pointer to a string representing 
a device key. See "devlist_handler.h" for the format of the string.

@param	key 		key_device_t* to the internal representation of the device key to serialize
@return result		char* to the string representation of the device key.
**/
char* serialize_key( key_device_t* key ){
	char* result = (char*)malloc(sizeof(char)*128);
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

	for(; key_str[i] != ','; i++){
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


int register_key( key_device_t* key ){
	int result = -1; //error

	return result;
}


int unregister_key( key_device_t* key ){
	int result = -1; //error

	return result;
}


void list_keys(){

}