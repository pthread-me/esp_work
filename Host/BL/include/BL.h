#include <sys/socket.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#ifndef EXTENDED_HCI
#define EXTENDED_HCI

#define HCI_META_DATA_SIZE 4
#define BA_AS_STR_SIZE 18

/*
 * Custome struct to identify the type of recieved hci packet when filter size > 1
 */
typedef struct hci_meta_data{
	uint8_t hci_packet_type;
	uint8_t hci_event_code;
	uint8_t hci_total_len;
	uint8_t number_of_responses;
} __attribute__ ((packed)) hci_meta_data;


int hci_extended_inquiry(int dev_id, uint8_t len, uint8_t nrsp, const uint8_t* lap, extended_inquiry_info** ii);

#endif
