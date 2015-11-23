/*
 * mifare_ultralight.h
 *
 *  Created on: 2015Äê8ÔÂ14ÈÕ
 *      Author: Wayen
 */

#ifndef SOURCES_MIFARE_ULTRALIGHT_H_
#define SOURCES_MIFARE_ULTRALIGHT_H_

#include "nfc_tag.h"


#define ULTRALIGHT_PAGE_SIZE 4
#define ULTRALIGHT_READ_SIZE 4 // we should be able to read 16 bytes at a time

#define ULTRALIGHT_DATA_START_PAGE 4
#define ULTRALIGHT_MESSAGE_LENGTH_INDEX 1
#define ULTRALIGHT_DATA_START_INDEX 2
#define ULTRALIGHT_MAX_PAGE 63

#define NFC_FORUM_TAG_TYPE_2 ("NFC Forum Type 2")


typedef struct
{
	unsigned int tagCapacity;
	unsigned int messageLength;
	unsigned int bufferSize;
	unsigned int ndefStartIndex;
}stru_mifareUltralight;

stru_nfcTag *read_mifare_ultralight(unsigned char *uid, unsigned int uidLength);
bool write_mifare_ultralight(stru_message *ndefMessage, unsigned char *uid, unsigned int uidLength);
bool clean_mifare_ultralight();

bool is_unformatted();
void read_capability_container();
void find_ndef_message();
void calculate_buffer_size();


#endif /* SOURCES_MIFARE_ULTRALIGHT_H_ */
