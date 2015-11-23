/*
 * mifare_classic.h
 *
 *  Created on: 2015Äê8ÔÂ14ÈÕ
 *      Author: Wayen
 */

#ifndef SOURCES_MIFARE_CLASSIC_H_
#define SOURCES_MIFARE_CLASSIC_H_

#include "nfc_tag.h"

#define BLOCK_SIZE 16
#define LONG_TLV_SIZE 4
#define SHORT_TLV_SIZE 2

#define MIFARE_CLASSIC ("Mifare Classic")


stru_nfcTag *read_mifare_classic(unsigned char *uid, unsigned int uidLength);
bool write_mifare_classic(stru_message *ndefMessage, unsigned char *uid, unsigned int uidLength);
bool format_mifare_classic_ndef(unsigned char *uid, unsigned int uidLength);
bool format_mifare_classic(unsigned char *uid, unsigned int uidLength);

int get_mifare_classic_buffer_size(int messageLength);
int get_mifare_classic_ndef_start_index(unsigned char *data);
bool decode_mifare_classic_tlv(unsigned char *data, int *messageLength, int *messageStartIndex);


#endif /* SOURCES_MIFARE_CLASSIC_H_ */
