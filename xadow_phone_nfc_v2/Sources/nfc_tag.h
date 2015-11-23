/*
 * nfc_tag.h
 *
 *  Created on: 2015Äê8ÔÂ14ÈÕ
 *      Author: Wayen
 */

#ifndef SOURCES_NFC_TAG_H_
#define SOURCES_NFC_TAG_H_

#include "ndef_message.h"


typedef struct
{
	unsigned char *uid;
	unsigned int uidLength;
	char *tagType; // Mifare Classic, NFC Forum Type {1,2,3,4}, Unknown
	stru_message *ndefMessage;
}stru_nfcTag;


unsigned char get_uid_length(stru_nfcTag *nfcTag);
void get_uid(stru_nfcTag *nfcTag, unsigned char *uid, unsigned int uidLength);
char *get_uid_string(stru_nfcTag *nfcTag);
char *get_tag_type(stru_nfcTag *nfcTag);
bool has_ndef_message(stru_nfcTag *nfcTag);
stru_message *get_ndef_message(stru_nfcTag *nfcTag);

stru_nfcTag *create_nfc_tag_a();
stru_nfcTag *create_nfc_tag_b(unsigned char *uid, unsigned int uidLength);
stru_nfcTag *create_nfc_tag_c(unsigned char *uid, unsigned int uidLength, char *tagType);
stru_nfcTag *create_nfc_tag_d(unsigned char *uid, unsigned int uidLength, char *tagType, stru_message *ndefMessage);
stru_nfcTag *create_nfc_tag_e(unsigned char *uid, unsigned int uidLength, char *tagType, const unsigned char *ndefData, const int ndefDataLength);
void free_nfc_tag(stru_nfcTag *nfcTag);
void print_nfc_tag(stru_nfcTag *nfcTag);


#endif /* SOURCES_NFC_TAG_H_ */
