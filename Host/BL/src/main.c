#include "BL.h"
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci_lib.h>
#include<errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(void){
	int dev_id;										//	BL adapter ID
	int dd;												//	device descriptor -> aka socket fd
	extended_inquiry_info *ii; 		//	List result of device discovery (inquiry)		
	uint8_t nrsp = 10;					//	Number of max devices to return per scan
	uint8_t inquiry_len = 10;			//	Inquiry are blocking operations that last,	inquiry_len * 1.28 seconds



	// func aquires the ID of the first available bluetooth adapter
	if((dev_id = hci_get_route(NULL)) < 0){
		fprintf(stderr, "Could not aquire a BL adapter\n");
		exit(errno);
	}

	


	if((dd=hci_open_dev(dev_id))<0){
		fprintf(stderr, "Could not open a BL socket\n");
		exit(errno);
	}

	/*
	ii = calloc(nrsp, sizeof(extended_inquiry_info*));
	for(int i=0; i<nrsp; i++){
		ii[i] = calloc(1, EXTENDED_INQUIRY_INFO_SIZE);
	}
*/

	ii = malloc(EXTENDED_INQUIRY_INFO_SIZE * nrsp);
	memset(ii, 0, EXTENDED_INQUIRY_INFO_SIZE);
	// IREQ_CACHE_FLUSH forces a completly new scan every time
	int ii_count = hci_extended_inquiry(dev_id, inquiry_len, nrsp, NULL, &ii);


	printf("\nRead %d\n", ii_count);
	for(int i=0; i<ii_count; i++){
		uint8_t* byte = (uint8_t*) &ii[i];
		printf("%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X\n",
				byte[0],byte[1],byte[2],byte[3],byte[4],byte[5]);
		fflush(stdout);
	}		

	free(ii);
	
	fprintf(stdout, "Done\n");
	return 0;
}
