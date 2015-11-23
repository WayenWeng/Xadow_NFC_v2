/*
 * nfc_adapter.h
 *
 *  Created on: 2015Äê8ÔÂ12ÈÕ
 *      Author: Wayen
 */

#ifndef SOURCES_NFC_ADAPTER_H_
#define SOURCES_NFC_ADAPTER_H_


#include "ndef_message.h"
#include "nfc_tag.h"
#include "mifare_classic.h"
#include "mifare_ultralight.h"


#define TAG_TYPE_MIFARE_CLASSIC (0)
#define TAG_TYPE_1 (1)
#define TAG_TYPE_2 (2)
#define TAG_TYPE_3 (3)
#define TAG_TYPE_4 (4)
#define TAG_TYPE_UNKNOWN (99)

#define IRQ   (2)
#define RESET (3)  // Not connected by default on the NFC Shield


typedef struct
{
	unsigned char uid[7]; // Buffer to store the returned UID
	unsigned int uidLength; // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
}stru_NfcAdapter;


//
bool adapter_init(void);
// tagAvailable
bool tag_present(stru_NfcAdapter *tag, unsigned long timeout);
//
stru_nfcTag *adapter_read(stru_NfcAdapter *tag);
//
bool adapter_write(stru_NfcAdapter *tag, stru_message *ndefMessage);
// erase tag by writing an empty NDEF record
bool adapter_erase(stru_NfcAdapter *tag);
// format a tag as NDEF
bool adapter_format(stru_NfcAdapter *tag);
// reset tag back to factory state
bool adapter_clean(stru_NfcAdapter *tag);

unsigned int guess_tag_type(stru_NfcAdapter *tag);


#endif /* SOURCES_NFC_ADAPTER_H_ */
