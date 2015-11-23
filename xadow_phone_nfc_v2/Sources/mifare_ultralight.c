/*
 * mifare_ultralight.c
 *
 *  Created on: 2015Äê8ÔÂ14ÈÕ
 *      Author: Wayen
 */




#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "pn532.h"
#include "ndef_message.h"
#include "mifare_ultralight.h"


stru_mifareUltralight mifareUltralight;


stru_nfcTag *read_mifare_ultralight(unsigned char *uid, unsigned int uidLength)
{
    if(is_unformatted())
    {
        return create_nfc_tag_c(uid, uidLength, NFC_FORUM_TAG_TYPE_2);
    }

    read_capability_container(); // meta info for tag
    find_ndef_message();
    calculate_buffer_size();

    if(mifareUltralight.messageLength == 0) // data is 0x44 0x03 0x00 0xFE
    {
    	stru_message *message = create_message();
        add_empty_record(message);
        return create_nfc_tag_d(uid, uidLength, NFC_FORUM_TAG_TYPE_2, message);
    }

    bool success;
    unsigned char page;
    unsigned char index = 0;
    unsigned char buffer[mifareUltralight.bufferSize];
    for(page = ULTRALIGHT_DATA_START_PAGE; page < ULTRALIGHT_MAX_PAGE; page++)
    {
        // read the data
        success = mifareultralight_ReadPage(page, &buffer[index]);
        if(success)
        {

        }
        else
        {
            // error handling
        	mifareUltralight.messageLength = 0;
            break;
        }

        if(index >= (mifareUltralight.messageLength + mifareUltralight.ndefStartIndex))
        {
            break;
        }

        index += ULTRALIGHT_PAGE_SIZE;
    }

    stru_message *ndefMessage = create_message_b(&buffer[mifareUltralight.ndefStartIndex], mifareUltralight.messageLength);
    return create_nfc_tag_d(uid, uidLength, NFC_FORUM_TAG_TYPE_2, ndefMessage);
}

bool write_mifare_ultralight(stru_message *ndefMessage, unsigned char *uid, unsigned int uidLength)
{
    if(is_unformatted())
    {
        return false;
    }
    read_capability_container(); // meta info for tag

    mifareUltralight.messageLength  = get_encoded_message_size(ndefMessage);
    mifareUltralight.ndefStartIndex =  mifareUltralight.messageLength < 0xFF ? 2 : 4;
    calculate_buffer_size();

    if(mifareUltralight.bufferSize > mifareUltralight.tagCapacity)
    {
    	return false;
    }

    unsigned char encoded[mifareUltralight.bufferSize];
    unsigned char * src = encoded;
    unsigned int position = 0;
    uint8_t page = ULTRALIGHT_DATA_START_PAGE;

    // Set message size. With ultralight should always be less than 0xFF but who knows?

    encoded[0] = 0x3;
    if(mifareUltralight.messageLength < 0xFF)
    {
        encoded[1] = mifareUltralight.messageLength;
    }
    else
    {
        encoded[1] = 0xFF;
        encoded[2] = ((mifareUltralight.messageLength >> 8) & 0xFF);
        encoded[3] = (mifareUltralight.messageLength & 0xFF);
    }

    encode_message(ndefMessage, encoded + mifareUltralight.ndefStartIndex);
    // this is always at least 1 byte copy because of terminator.
    memset(encoded + mifareUltralight.ndefStartIndex + mifareUltralight.messageLength, 0, mifareUltralight.bufferSize - mifareUltralight.ndefStartIndex - mifareUltralight.messageLength);
    encoded[mifareUltralight.ndefStartIndex + mifareUltralight.messageLength] = 0xFE; // terminator

    while(position < mifareUltralight.bufferSize) //bufferSize is always times pagesize so no "last chunk" check
    {
        // write page
        if(!mifareultralight_WritePage(page, src))return false;
        page ++;
        src += ULTRALIGHT_PAGE_SIZE;
        position += ULTRALIGHT_PAGE_SIZE;
    }
    return true;
}

bool clean_mifare_ultralight()
{
    read_capability_container(); // meta info for tag

    unsigned char pages = (mifareUltralight.tagCapacity / ULTRALIGHT_PAGE_SIZE) + ULTRALIGHT_DATA_START_PAGE;

    // factory tags have 0xFF, but OTP-CC blocks have already been set so we use 0x00
    uint8_t data[4] = { 0x00, 0x00, 0x00, 0x00 };

    for(int i = ULTRALIGHT_DATA_START_PAGE; i < pages; i++)
    {
        if(!mifareultralight_WritePage(i, data))
        {
            return false;
        }
    }
    return true;
}

bool is_unformatted()
{
    unsigned char page = 4;
    unsigned char data[ULTRALIGHT_READ_SIZE];
    bool success = mifareultralight_ReadPage(page, data);
    if(success)
    {
        return (data[0] == 0xFF && data[1] == 0xFF && data[2] == 0xFF && data[3] == 0xFF);
    }
    else
    {
        return false;
    }
}

// page 3 has tag capabilities
void read_capability_container()
{
    unsigned char data[ULTRALIGHT_PAGE_SIZE];
    int success = mifareultralight_ReadPage (3, data);
    if(success)
    {
        // See AN1303 - different rules for Mifare Family byte2 = (additional data + 48)/8
    	mifareUltralight.tagCapacity = data[2] * 8;
        // future versions should get lock information
    }
}

void find_ndef_message()
{
    int page;
    unsigned char data[12]; // 3 pages
    unsigned char* data_ptr = &data[0];

    // the nxp read command reads 4 pages, unfortunately adafruit give me one page at a time
    bool success = true;
    for(page = 4; page < 6; page++)
    {
        success = success && mifareultralight_ReadPage(page, data_ptr);
        data_ptr += ULTRALIGHT_PAGE_SIZE;
    }

    if(success)
    {
        if (data[0] == 0x03)
        {
        	mifareUltralight.messageLength = data[1];
        	mifareUltralight.ndefStartIndex = 2;
        }
        else if(data[5] == 0x3) // page 5 byte 1
        {
            // should really read the lock control TLV to ensure byte[5] is correct
        	mifareUltralight.messageLength = data[6];
        	mifareUltralight.ndefStartIndex = 7;
        }
    }
}

void calculate_buffer_size()
{
    // TLV terminator 0xFE is 1 byte
	mifareUltralight.bufferSize = mifareUltralight.messageLength + mifareUltralight.ndefStartIndex + 1;

    if(mifareUltralight.bufferSize % ULTRALIGHT_READ_SIZE != 0)
    {
        // buffer must be an increment of page size
    	mifareUltralight.bufferSize = ((mifareUltralight.bufferSize / ULTRALIGHT_READ_SIZE) + 1) * ULTRALIGHT_READ_SIZE;
    }
}
