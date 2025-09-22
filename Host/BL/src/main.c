#include "BL.h"
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci_lib.h>
#include<errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	
	int dev_id;										//	BL adapter ID
	int dd;												//	device descriptor -> aka socket fd
	extended_inquiry_info *ii; 		//	List result of device discovery (inquiry)		
	int max_ii = 1;							//	Number of max devices to return per scan
	int inquiry_len = 10;					//	Inquiry are blocking operations that last,	inquiry_len * 1.28 seconds



	// func aquires the ID of the first available bluetooth adapter
	if((dev_id = hci_get_route(NULL)) < 0){
		fprintf(stderr, "Could not aquire a BL adapter\n");
		exit(errno);
	}

	
	ii = calloc(max_ii, sizeof(extended_inquiry_info));

	// NULL for the lower address part (LAP) for internal use (or more control beyond my knowledge atm)
	// IREQ_CACHE_FLUSH forces a completly new scan every time
	int ii_count = hci_extended_inquiry(dev_id, inquiry_len, max_ii, NULL, &ii, IREQ_CACHE_FLUSH);



	if((dd=hci_open_dev(dev_id))<0){
		fprintf(stderr, "Could not open a BL socket\n");
		exit(errno);
	}

	
	printf("Read %d\n", ii_count);
	
	
	printf("\n\n");
	uint8_t* pointer = (uint8_t*) ii;
	for(int i=0; i<sizeof(extended_inquiry_info); i++){
		printf("%2.2x ", pointer[i]);	
	}		

	/*
	char *addr_str = calloc(32, sizeof(char));
	ba2str(&ii->bdaddr, addr_str);
	printf("%s\n", addr_str);
*/


	free(ii);
	
	fprintf(stdout, "Done\n");
	fflush(stdout);
	return 0;
}
