#include <bluetooth/bluetooth.h>
#include <time.h>

/*
	"devlist_handler" is responsible for persisting
	string representations of devices that can unlock the machine.
	Internally, a device key is represented by a struct (defined below).
	Its string representation should adhere to the format:

	"<KEY_ID>: { <MAC_ADDR>, <USER>, <ADDED>, <LAST_SEEN> }"
	where
		<KEY_ID>[20]		a string identifying the device, e.g: "My Phone"
		<MAC_ADDR>  		a representation of the MAC address as "XX:XX:XX:XX:XX:XX"
		<USER>[20]			the user of the device (i.e.: "James")
		<ADDED>	    		the time when the device was added
		<LAST_SEEN>	    	last time the device was used to unlock the machine.

	The time fields should be of the format "dd/mm/yyyy HH:MM:SS". The max_length of the string representation
	should be 100 (97).
*/

#ifndef DEVLIST_HANDLER_H
#define DEVLIST_HANDLER_H
#define KEY_STORE_PATH "config/keystore"
#define DATE_FORMAT "%d/%m/%Y %H:%M:%S"
#define ID_LEN 20
#define USR_LEN 20
#define MAC_LEN 18
#define DATE_LEN 20
#define STORE_LEN 10
#define SERIAL_LEN 128


typedef struct {
	char* device_id;
	time_t added;
	time_t last_seen;
	char* user;
	bdaddr_t addr;
} key_device_t;
/*
	Functions for dealing with a single key device.
*/
char* serialize_key( key_device_t* key ); 	// key_device_t -> str
key_device_t* parse_key ( char* key_str); 	// str -> key_device_t
int register_key( key_device_t* key );		// persists a new key
int unregister_key( key_device_t* key );


struct node {
	key_device_t* key;
	struct node* next;
};
typedef struct node key_store;

/*
	Functions for manipulating the linked list
*/
key_store* append_node(key_store* store, key_device_t* to_insert);
key_store* delete_node(key_store* store, int position);
void print_list(key_store* store);
int get_list_length(key_store* list);

/*
	Functions for dealing with the store of key devices.
*/
void list_keys();							// prints the full key list
key_store* fetch_keys();
void persist_keys(key_store* keys);
int are_equal(key_device_t* key1, key_device_t* key2);
int check_existence(key_store* store,key_device_t* key);

#endif 
