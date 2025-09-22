#include <BL.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>

int hci_extended_inquiry(int dev_id, int inquiry_len, uint8_t nrsp, const uint8_t* lap, 
		extended_inquiry_info** ii, long flags)
{
	
	int dd;									//	device descriptor -> aka socket fd
	int ret;
	uint8_t dev_mode = -1;	//	init dev_mode
	int time_out = 1000;		
	
	struct hci_inquiry_req *ir;
	void *buf;


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

	buf = malloc((sizeof(struct hci_inquiry_req))+(sizeof(extended_inquiry_info) * nrsp));	
	memset(buf, 0, sizeof(struct hci_inquiry_req) + (sizeof(extended_inquiry_info)*nrsp));
	
	ir = buf;	
	
	// Lower Adderess Part, copied as in bluez hci_inquiry
	if(lap){
		memcpy(ir->lap, lap, 3);
	}else{
		ir->lap[0] = 0x33;
		ir->lap[1] = 0x8b;
		ir->lap[2] = 0x9e;
	}

	ir->dev_id = dev_id;
	ir->num_rsp = nrsp;
	ir->length = inquiry_len;
	ir->flags = flags;


	ret = ioctl(dd, HCIINQUIRY, buf);
	int size = sizeof(struct hci_inquiry_req) + (sizeof(extended_inquiry_info)*nrsp);

	uint8_t* pointer = (uint8_t*) buf;
	for(int i=0; i<size; i++){
		printf("%2.2x ", pointer[i]);	
	}		



	if(ret > -1){
		if(!*ii){
			*ii = malloc(sizeof(hci_extended_inquiry) * ir->num_rsp);	
		}else{
			memcpy(*ii, buf+sizeof(struct hci_inquiry_req), sizeof(hci_extended_inquiry)* (ir->num_rsp));
			ret = ir->num_rsp;
		}
		if(hci_close_dev(dd)<0){
			fprintf(stderr, "Could not close dev %d\n", dev_id);
			return -1;
		}
	}

	free(buf);
	hci_close_dev(dd);
	
	return ret;
	
}
