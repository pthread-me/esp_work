#include <BL.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BA_AS_STR_SIZE 18

int hci_extended_inquiry(int dev_id, int inquiry_len, uint8_t nrsp, const uint8_t* lap, 
		extended_inquiry_info** ii, long flags)
{
	
	int dd;										//	device descriptor -> aka socket fd
	int ret;
	uint8_t dev_mode = -1;		//	init dev_mode
	int time_out = 1000;

	struct hci_filter filter;	// socket filter
	inquiry_cp *ir;						// inquiry request parameter
	uint8_t *buf;


	if((dd=hci_open_dev(dev_id)) < 0){
		fprintf(stderr, "Could not connect a socket to the %d bluetooth adapter\n", dev_id);
		return -1;
	}

	if(hci_read_inquiry_mode(dd, &dev_mode, time_out)<0){

		fprintf(stderr, "Error while checking dev inquiry mode\n");
		return -1;
	}


	if(dev_mode != 2){
		if(hci_write_inquiry_mode(dd, 2, time_out)<0){
			fprintf(stderr, "Cant set inquiry mode to extended\n");
			return -1;
		}
	}



	buf = malloc(HCI_MAX_EIR_LENGTH * nrsp);	
	memset(buf, 0, HCI_MAX_EIR_LENGTH*nrsp);
	ir  = calloc(1, sizeof(inquiry_cp));

	// Lower Adderess Part, copied as in bluez hci_inquiry
	if(lap){
		memcpy(ir->lap, lap, 3);
	}else{
		ir->lap[0] = 0x33;
		ir->lap[1] = 0x8b;
		ir->lap[2] = 0x9e;
	}
	ir->num_rsp = nrsp;
	ir->length = inquiry_len;




	hci_filter_clear(&filter);
	hci_filter_set_ptype(HCI_EVENT_PKT, &filter);
	hci_filter_set_event(EVT_EXTENDED_INQUIRY_RESULT,  &filter);
	hci_filter_set_event(EVT_INQUIRY_COMPLETE,  &filter);
	setsockopt(dd, SOL_HCI, HCI_FILTER, &filter, sizeof(filter));

	hci_send_cmd(dd, OGF_LINK_CTL, OCF_INQUIRY, sizeof(*ir), ir);


	read(dd, buf, HCI_MAX_EIR_LENGTH);
	
	
	printf("%2.2X\n", buf[1]);
	for(int i=1; (i<HCI_MAX_EIR_LENGTH*nrsp) && buf[i]==EVT_EXTENDED_INQUIRY_RESULT;
			i+=HCI_MAX_EIR_LENGTH){
	
		// start of EIR
		int j = i+3;

		extended_inquiry_info *eir = (extended_inquiry_info*) &buf[j];
		char *addr_str = calloc(BA_AS_STR_SIZE, sizeof(char));

		ba2str(&eir->bdaddr, addr_str);
		memcpy(&addr_str[17], "\0", 1);
		printf("Addr: %s\n", addr_str);

		uint8_t *data = eir->data;
		printf("data:\n");
		for(int k=0; k<HCI_MAX_EIR_LENGTH; k++){
			printf("%2.2X ", data[k]);
		}
		printf("\n");
	}

	free(buf);
	free(ir);
	hci_close_dev(dd);
	
	return ret;
	
}
