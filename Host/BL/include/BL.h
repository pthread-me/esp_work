#include <sys/socket.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#ifndef EXTENDED_HCI
#define EXTENDED_HCI

int hci_extended_inquiry(int dev_id, int len, uint8_t nrsp, const uint8_t* lap, extended_inquiry_info** ii, long flags);

#endif
