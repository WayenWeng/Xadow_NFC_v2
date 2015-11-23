/*
 * nfc_adapter.c
 *
 *  Created on: 2015Äê8ÔÂ12ÈÕ
 *      Author: Wayen
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "pn532_i2c.h"
#include "pn532.h"
#include "nfc_adapter.h"


bool adapter_init(void)
{
	uint32_t versiondata;

	wake_up();

	versiondata = getFirmwareVersion();
    if(!versiondata)
    {
        return false;
    }

    return SAMConfig();
}

bool tag_present(stru_NfcAdapter *tag, unsigned long timeout)
{
    unsigned char success;

    tag->uidLength = 0;

    if(timeout == 0)
    {
        success = readPassiveTargetID(PN532_MIFARE_ISO14443A, (unsigned char*)tag->uid, (unsigned char*)&(tag->uidLength), 0, 0);
    }
    else
    {
        success = readPassiveTargetID(PN532_MIFARE_ISO14443A, (unsigned char*)tag->uid, (unsigned char*)&(tag->uidLength), timeout, 0);
    }
    return success;
}

stru_nfcTag *adapter_read(stru_NfcAdapter *tag)
{
    unsigned char type = guess_tag_type(tag);

    if(type == TAG_TYPE_MIFARE_CLASSIC)
    {
        return read_mifare_classic(tag->uid, tag->uidLength);
    }
    else if(type == TAG_TYPE_2)
    {
        return read_mifare_ultralight(tag->uid, tag->uidLength);
    }
    else if(type == TAG_TYPE_UNKNOWN)
    {
        return create_nfc_tag_b(tag->uid, tag->uidLength);
    }
    else
    {
        // should set type here
        return create_nfc_tag_b(tag->uid, tag->uidLength);
    }
}

bool adapter_write(stru_NfcAdapter *tag, stru_message *ndefMessage)
{
    bool success;
    unsigned char type = guess_tag_type(tag);

    if(type == TAG_TYPE_MIFARE_CLASSIC)
    {
        success = write_mifare_classic(ndefMessage, tag->uid, tag->uidLength);
    }
    else if(type == TAG_TYPE_2)
    {
        success = write_mifare_ultralight(ndefMessage, tag->uid, tag->uidLength);
    }
    else if(type == TAG_TYPE_UNKNOWN)
    {
        success = false;
    }
    else
    {
        success = false;
    }

    return success;
}

bool adapter_erase(stru_NfcAdapter *tag)
{
    bool success;
    stru_message *message = create_message();
    add_empty_record(message);
    success = adapter_write(tag, message);
    free_message(message);
    return success;
}

bool adapter_format(stru_NfcAdapter *tag)
{
    bool success;
    if(tag->uidLength == 4)
    {
        success = format_mifare_classic_ndef(tag->uid, tag->uidLength);
    }
    else
    {
        success = false;
    }
    return success;
}

bool adapter_clean(stru_NfcAdapter *tag)
{
    unsigned char type = guess_tag_type(tag);

    if(type == TAG_TYPE_MIFARE_CLASSIC)
    {
        return format_mifare_classic(tag->uid, tag->uidLength);
    }
    else if(type == TAG_TYPE_2)
    {
        return clean_mifare_ultralight();
    }
    else
    {
        return false;
    }
}

// this should return a Driver MifareClassic, MifareUltralight, Type 4, Unknown
// Guess Tag Type by looking at the ATQA and SAK values
// Need to follow spec for Card Identification. Maybe AN1303, AN1305 and ???
unsigned int guess_tag_type(stru_NfcAdapter *tag)
{
    // 4 byte id - Mifare Classic
    // - ATQA 0x4 && SAK 0x8
    // 7 byte id
    // - ATQA 0x44 && SAK 0x8 - Mifare Classic
    // - ATQA 0x44 && SAK 0x0 - Mifare Ultralight NFC Forum Type 2
    // - ATQA 0x344 && SAK 0x20 - NFC Forum Type 4

    if(tag->uidLength == 4)
    {
        return TAG_TYPE_MIFARE_CLASSIC;
    }
    else
    {
        return TAG_TYPE_2;
    }
}
