#include <BL.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


int hci_extended_inquiry(int dev_id, uint8_t inquiry_len, uint8_t nrsp, const uint8_t* lap, 
		extended_inquiry_info** ii){
	
	int dd;										//	device descriptor -> aka socket fd
	int recieved = 0;					// # of EIR recieved
	uint8_t dev_mode = 0;			//	init dev_mode
	int time_out = 1000;

	struct hci_filter filter;	// socket filter
	inquiry_cp *ir;						// inquiry request parameter
	
	uint8_t *buf;
	size_t buf_size = (EXTENDED_INQUIRY_INFO_SIZE + HCI_META_DATA_SIZE);
	hci_meta_data meta_data;

	
	if(*ii == NULL){
		printf("init ii\n");
		*ii = calloc(nrsp, EXTENDED_INQUIRY_INFO_SIZE);
	}


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

	
	// clearing hci cache, equivalent to hci_inquiry with the IREQ_CACHE_FLUSH flag
	hci_send_cmd(dd, OGF_HOST_CTL, OCF_FLUSH, 0, NULL);

	// filtering for inq extended and complete packets 
	hci_filter_clear(&filter);
	hci_filter_set_ptype(HCI_EVENT_PKT, &filter);
	hci_filter_set_event(EVT_EXTENDED_INQUIRY_RESULT,  &filter);
	hci_filter_set_event(EVT_INQUIRY_COMPLETE,  &filter);
	setsockopt(dd, SOL_HCI, HCI_FILTER, &filter, sizeof(filter));

	hci_send_cmd(dd, OGF_LINK_CTL, OCF_INQUIRY, sizeof(*ir), ir);
	

	buf = malloc(buf_size);
	memset(buf, 0, buf_size);

	for(int i=0; i<nrsp; i++){
		read(dd, buf, EXTENDED_INQUIRY_INFO_SIZE+HCI_META_DATA_SIZE);
		memcpy(&meta_data, buf, HCI_META_DATA_SIZE);
		
		if(meta_data.hci_packet_type != HCI_EVENT_PKT){
			fprintf(stderr, "Expected packet type: 0x04 (HCI_EVENT_PKT) but recieved: %2.2X\n.", meta_data.hci_packet_type);
			free(ir);
			hci_close_dev(dd);
			return -1;
		}
		
		if(meta_data.hci_event_code == EVT_INQUIRY_COMPLETE){
			break;
		}else if(meta_data.hci_event_code != EVT_EXTENDED_INQUIRY_RESULT){
			fprintf(stderr, "Expected event type: 0x2F (HCI_EXTENDED_INQUIRY_RESULT) but recieved: %2.2X\n.", meta_data.hci_event_code);
			free(ir);
			hci_close_dev(dd);
			return -1;
		}
	

		memcpy(&(*ii)[i], &(buf[HCI_META_DATA_SIZE]), EXTENDED_INQUIRY_INFO_SIZE);
		recieved += 1;
	}
		
	
	free(buf);
	free(ir);
	hci_close_dev(dd);
	
	return recieved;
	
}
