/*
 * mifare_classic.c
 *
 *  Created on: 2015Äê8ÔÂ14ÈÕ
 *      Author: Wayen
 */


#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "pn532.h"
#include "mifare_classic.h"


stru_nfcTag *read_mifare_classic(unsigned char *uid, unsigned int uidLength)
{
	unsigned char key[6] = { 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 };
	int currentBlock = 4;
	int messageStartIndex = 0;
	int messageLength = 0;
	unsigned char data[BLOCK_SIZE];

	// read first block to get message length
	int success = mifareclassic_AuthenticateBlock(uid, uidLength, currentBlock, 0, key);
	if(success)
	{
		success = mifareclassic_ReadDataBlock(currentBlock, data);
		if(success)
		{
			if(!decode_mifare_classic_tlv(data, &messageLength, &messageStartIndex))
			{
				return create_nfc_tag_c(uid, uidLength, "ERROR"); // should the error message go in NfcTag?
			}
		}
		else
		{
			return create_nfc_tag_c(uid, uidLength, MIFARE_CLASSIC);
		}
	}
	else
	{
		// set tag.isFormatted = false
		return create_nfc_tag_c(uid, uidLength, MIFARE_CLASSIC);
	}

	// this should be nested in the message length loop
	int index = 0;
	int bufferSize = get_mifare_classic_buffer_size(messageLength);
	unsigned char buffer[bufferSize];

	while(index < bufferSize)
	{

		// authenticate on every sector
		if(mifareclassic_IsFirstBlock(currentBlock))
		{
			success = mifareclassic_AuthenticateBlock(uid, uidLength, currentBlock, 0, key);
			if(!success)
			{
				// error handling
				success = false;
				return create_nfc_tag_c(uid, uidLength, "ERROR"); // ???
			}
		}

		// read the data
		success = mifareclassic_ReadDataBlock(currentBlock, &buffer[index]);
		if(!success)
		{
			success = false;
			return create_nfc_tag_c(uid, uidLength, "ERROR"); // ???
		}

		index += BLOCK_SIZE;
		currentBlock++;

		// skip the trailer block
		if(mifareclassic_IsTrailerBlock(currentBlock))
		{
			currentBlock++;
		}
	}

	return create_nfc_tag_e(uid, uidLength, MIFARE_CLASSIC, &buffer[messageStartIndex], messageLength);
}

bool write_mifare_classic(stru_message *ndefMessage, unsigned char *uid, unsigned int uidLength)
{
	unsigned char encoded[get_encoded_message_size(ndefMessage)];
	encode_message(ndefMessage, encoded);

	unsigned char buffer[get_mifare_classic_buffer_size(sizeof(encoded))];
	memset(buffer, 0, sizeof(buffer));

	if(sizeof(encoded) < 0xFF)
	{
		buffer[0] = 0x3;
		buffer[1] = sizeof(encoded);
		memcpy(&buffer[2], encoded, sizeof(encoded));
		buffer[2+sizeof(encoded)] = 0xFE; // terminator
	}
	else
	{
		buffer[0] = 0x3;
		buffer[1] = 0xFF;
		buffer[2] = ((sizeof(encoded) >> 8) & 0xFF);
		buffer[3] = (sizeof(encoded) & 0xFF);
		memcpy(&buffer[4], encoded, sizeof(encoded));
		buffer[4+sizeof(encoded)] = 0xFE; // terminator
	}

	// Write to tag
	int index = 0;
	int currentBlock = 4;
	uint8_t key[6] = { 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 }; // this is Sector 1 - 15 key

	while(index < sizeof(buffer))
	{

		if(mifareclassic_IsFirstBlock(currentBlock))
		{
			int success = mifareclassic_AuthenticateBlock(uid, uidLength, currentBlock, 0, key);
			if(!success)
			{
				return false;
			}
		}

		int write_success = mifareclassic_WriteDataBlock(currentBlock, &buffer[index]);
		if(write_success)
		{

		}
		else
		{

			return false;
		}
		index += BLOCK_SIZE;
		currentBlock++;

		if(mifareclassic_IsTrailerBlock(currentBlock))
		{
			// can't write to trailer block

			currentBlock++;
		}

	}

	return true;
}

int get_mifare_classic_buffer_size(int messageLength)
{
	int bufferSize = messageLength;

	// TLV header is 2 or 4 bytes, TLV terminator is 1 byte.
	if(messageLength < 0xFF)
	{
		bufferSize += SHORT_TLV_SIZE + 1;
	}
	else
	{
		bufferSize += LONG_TLV_SIZE + 1;
	}

	// bufferSize needs to be a multiple of BLOCK_SIZE
	if(bufferSize % BLOCK_SIZE != 0)
	{
		bufferSize = ((bufferSize / BLOCK_SIZE) + 1) * BLOCK_SIZE;
	}

	return bufferSize;
}

int get_mifare_classic_ndef_start_index(unsigned char *data)
{
    for(int i = 0; i < BLOCK_SIZE; i++)
    {
        if(data[i] == 0x0)
        {
            // do nothing, skip
        }
        else if(data[i] == 0x3)
        {
            return i;
        }
        else
        {
            return -2;
        }
    }

    return -1;
}

// Decode the NDEF data length from the Mifare TLV
// Leading null TLVs (0x0) are skipped
// Assuming T & L of TLV will be in the first block
// messageLength and messageStartIndex written to the parameters
// success or failure status is returned
//
// { 0x3, LENGTH }
// { 0x3, 0xFF, LENGTH, LENGTH }
bool decode_mifare_classic_tlv(unsigned char *data, int *messageLength, int *messageStartIndex)
{
    int i = get_mifare_classic_ndef_start_index(data);

    if(i < 0 || data[i] != 0x3)
    {
        return false;
    }
    else
    {
        if(data[i+1] == 0xFF)
        {
            *messageLength = ((0xFF & data[i+2]) << 8) | (0xFF & data[i+3]);
            *messageStartIndex = i + LONG_TLV_SIZE;
        }
        else
        {
            *messageLength = data[i+1];
            *messageStartIndex = i + SHORT_TLV_SIZE;
        }
    }

    return true;
}

// Intialized NDEF tag contains one empty NDEF TLV 03 00 FE - AN1304 6.3.1
// We are formatting in read/write mode with a NDEF TLV 03 03 and an empty NDEF record D0 00 00 FE - AN1304 6.3.2
bool format_mifare_classic_ndef(unsigned char *uid, unsigned int uidLength)
{
    unsigned char keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    unsigned char emptyNdefMesg[16] = {0x03, 0x03, 0xD0, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char sectorbuffer0[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned char sectorbuffer4[16] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7, 0x7F, 0x07, 0x88, 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    bool success = mifareclassic_AuthenticateBlock(uid, uidLength, 0, 0, keya);
    if(!success)
    {
        return false;
    }
    success = mifareclassic_FormatNDEF();
    if(!success)
    {

    }
    else
    {
        for(int i=4; i<64; i+=4)
        {
            success = mifareclassic_AuthenticateBlock(uid, uidLength, i, 0, keya);

            if (success)
            {
                if(i == 4)  // special handling for block 4
                {
                    if(!(mifareclassic_WriteDataBlock(i, emptyNdefMesg)))
                    {

                    }
                }
                else
                {
                    if(!(mifareclassic_WriteDataBlock(i, sectorbuffer0)))
                    {

                    }
                }

                if(!(mifareclassic_WriteDataBlock(i+1, sectorbuffer0)))
                {

                }
                if(!(mifareclassic_WriteDataBlock(i+2, sectorbuffer0)))
                {

                }
                if(!(mifareclassic_WriteDataBlock(i+3, sectorbuffer4)))
                {

                }
            }
            else
            {
                unsigned int iii = uidLength;
                readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, (unsigned char*)&iii, 0, 0);
            }
        }
    }
    return success;
}

#define NR_SHORTSECTOR          (32)    // Number of short sectors on Mifare 1K/4K
#define NR_LONGSECTOR           (8)     // Number of long sectors on Mifare 4K
#define NR_BLOCK_OF_SHORTSECTOR (4)     // Number of blocks in a short sector
#define NR_BLOCK_OF_LONGSECTOR  (16)    // Number of blocks in a long sector

// Determine the sector trailer block based on sector number
#define BLOCK_NUMBER_OF_SECTOR_TRAILER(sector) (((sector)<NR_SHORTSECTOR)? \
  ((sector)*NR_BLOCK_OF_SHORTSECTOR + NR_BLOCK_OF_SHORTSECTOR-1):\
  (NR_SHORTSECTOR*NR_BLOCK_OF_SHORTSECTOR + (sector-NR_SHORTSECTOR)*NR_BLOCK_OF_LONGSECTOR + NR_BLOCK_OF_LONGSECTOR-1))

bool format_mifare_classic(unsigned char *uid, unsigned int uidLength)
{
    // The default Mifare Classic key
    unsigned char KEY_DEFAULT_KEYAB[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    unsigned char blockBuffer[16];                          // Buffer to store block contents
    unsigned char blankAccessBits[3] = { 0xff, 0x07, 0x80 };
    unsigned char idx = 0;
    unsigned char numOfSector = 16;                         // Assume Mifare Classic 1K for now (16 4-block sectors)
    bool success = false;

    for(idx = 0; idx < numOfSector; idx++)
    {
        // Step 1: Authenticate the current sector using key B 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
        success = mifareclassic_AuthenticateBlock(uid, uidLength, BLOCK_NUMBER_OF_SECTOR_TRAILER(idx), 1, (unsigned char *)KEY_DEFAULT_KEYAB);
        if(!success)
        {
            return false;
        }

        // Step 2: Write to the other blocks
        if(idx == 0)
        {
            memset(blockBuffer, 0, sizeof(blockBuffer));
            if(!(mifareclassic_WriteDataBlock((BLOCK_NUMBER_OF_SECTOR_TRAILER(idx)) - 2, blockBuffer)))
            {

            }
        }
        else
        {
            memset(blockBuffer, 0, sizeof(blockBuffer));
            // this block has not to be overwritten for block 0. It contains Tag id and other unique data.
            if(!(mifareclassic_WriteDataBlock((BLOCK_NUMBER_OF_SECTOR_TRAILER(idx)) - 3, blockBuffer)))
            {

            }
            if(!(mifareclassic_WriteDataBlock((BLOCK_NUMBER_OF_SECTOR_TRAILER(idx)) - 2, blockBuffer)))
            {

            }
        }

        memset(blockBuffer, 0, sizeof(blockBuffer));

        if(!(mifareclassic_WriteDataBlock((BLOCK_NUMBER_OF_SECTOR_TRAILER(idx)) - 1, blockBuffer)))
        {

        }

        // Step 3: Reset both keys to 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
        memcpy(blockBuffer, KEY_DEFAULT_KEYAB, sizeof(KEY_DEFAULT_KEYAB));
        memcpy(blockBuffer + 6, blankAccessBits, sizeof(blankAccessBits));
        blockBuffer[9] = 0x69;
        memcpy(blockBuffer + 10, KEY_DEFAULT_KEYAB, sizeof(KEY_DEFAULT_KEYAB));

        // Step 4: Write the trailer block
        if(!(mifareclassic_WriteDataBlock((BLOCK_NUMBER_OF_SECTOR_TRAILER(idx)), blockBuffer)))
        {

        }
    }
    return true;
}
