/*
 * pn532.c
 *
 *  Created on: 2015��8��10��
 *      Author: Wayen
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "delay.h"
#include "pn532_i2c.h"
#include "pn532.h"


stru_pn532 pn532;


void pn532_init(void)
{
	pn532_i2c_init();
}

/**************************************************************************/
/*!
    @brief  Configures the SAM (Secure Access Module)
*/
/**************************************************************************/
bool SAMConfig(void)
{
    pn532.packetbuffer[0] = PN532_COMMAND_SAMCONFIGURATION;
    pn532.packetbuffer[1] = 0x01; // normal mode;
    pn532.packetbuffer[2] = 0x14; // timeout 50ms * 20 = 1 second
    pn532.packetbuffer[3] = 0x01; // use IRQ pin!

    if(writeCommand(pn532.packetbuffer,4,0,0) < 0)return false;

    if(readResponse(pn532.packetbuffer, sizeof(pn532.packetbuffer), 10) < 0)return false;
    else return true;
}

uint32_t getFirmwareVersion(void)
{
	uint32_t response;

    pn532.packetbuffer[0] = PN532_COMMAND_GETFIRMWAREVERSION;

    if(writeCommand(pn532.packetbuffer,1,0,0) < 0)return 0;

    if(readResponse(pn532.packetbuffer, sizeof(pn532.packetbuffer), 10) < 0)return 0;

    response = pn532.packetbuffer[0];
    response <<= 8;
    response |= pn532.packetbuffer[1];
    response <<= 8;
    response |= pn532.packetbuffer[2];
    response <<= 8;
    response |= pn532.packetbuffer[3];

    return response;
}



/**************************************************************************/
/*!
    Writes an 8-bit value that sets the state of the PN532's GPIO pins

    @warning This function is provided exclusively for board testing and
             is dangerous since it will throw an error if any pin other
             than the ones marked "Can be used as GPIO" are modified!  All
             pins that can not be used as GPIO should ALWAYS be left high
             (value = 1) or the system will become unstable and a HW reset
             will be required to recover the PN532.

             pinState[0]  = P30     Can be used as GPIO
             pinState[1]  = P31     Can be used as GPIO
             pinState[2]  = P32     *** RESERVED (Must be 1!) ***
             pinState[3]  = P33     Can be used as GPIO
             pinState[4]  = P34     *** RESERVED (Must be 1!) ***
             pinState[5]  = P35     Can be used as GPIO

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
bool writeGPIO(uint8_t pinstate)
{
    // Make sure pin state does not try to toggle P32 or P34
    pinstate |= (1 << PN532_GPIO_P32) | (1 << PN532_GPIO_P34);

    // Fill command buffer
    pn532.packetbuffer[0] = PN532_COMMAND_WRITEGPIO;
    pn532.packetbuffer[1] = PN532_GPIO_VALIDATIONBIT | pinstate;  // P3 Pins
    pn532.packetbuffer[2] = 0x00;    // P7 GPIO Pins (not used ... taken by I2C)

    // Send the WRITEGPIO command (0x0E)
    if(writeCommand(pn532.packetbuffer,3,0,0) < 0)return false;

	if(readResponse(pn532.packetbuffer, sizeof(pn532.packetbuffer), 10) < 0)return false;
	else return true;
}

/**************************************************************************/
/*!
    Reads the state of the PN532's GPIO pins

    @returns An 8-bit value containing the pin state where:

             pinState[0]  = P30
             pinState[1]  = P31
             pinState[2]  = P32
             pinState[3]  = P33
             pinState[4]  = P34
             pinState[5]  = P35
*/
/**************************************************************************/
uint8_t readGPIO(void)
{
	int8_t status;

    pn532.packetbuffer[0] = PN532_COMMAND_READGPIO;

    // Send the READGPIO command (0x0C)
    status = writeCommand(pn532.packetbuffer,1,0,0);
    if(status < 0)return false;

    status = readResponse(pn532.packetbuffer, sizeof(pn532.packetbuffer), 10);
    if(status < 0)return false;

    /* READGPIO response without prefix and suffix should be in the following format:

      byte            Description
      -------------   ------------------------------------------
      b0              P3 GPIO Pins
      b1              P7 GPIO Pins (not used ... taken by I2C)
      b2              Interface Mode Pins (not used ... bus select pins)
    */
    return pn532.packetbuffer[0];
}

/**************************************************************************/
/*!
    Sets the MxRtyPassiveActivation uint8_t of the RFConfiguration register

    @param  maxRetries    0xFF to wait forever, 0x00..0xFE to timeout
                          after mxRetries

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
bool setPassiveActivationRetries(uint8_t maxRetries)
{
    pn532.packetbuffer[0] = PN532_COMMAND_RFCONFIGURATION;
    pn532.packetbuffer[1] = 5;    // Config item 5 (MaxRetries)
    pn532.packetbuffer[2] = 0xFF; // MxRtyATR (default = 0xFF)
    pn532.packetbuffer[3] = 0x01; // MxRtyPSL (default = 0x01)
    pn532.packetbuffer[4] = maxRetries;

    if(writeCommand(pn532.packetbuffer,5,0,0) < 0)return false; // no ACK

    if(readResponse(pn532.packetbuffer, sizeof(pn532.packetbuffer), 10) < 0)return false;
    else return true;
}

/**
 * Peer to Peer
 */
int8_t tgInitAsTarget(uint16_t timeout)
{
    const uint8_t command[] =
    {
        PN532_COMMAND_TGINITASTARGET,
        0,
        0x00, 0x00,         //SENS_RES
        0x00, 0x00, 0x00,   //NFCID1
        0x40,               //SEL_RES

        0x01, 0xFE, 0x0F, 0xBB, 0xBA, 0xA6, 0xC9, 0x89, // POL_RES
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0xFF, 0xFF,

        0x01, 0xFE, 0x0F, 0xBB, 0xBA, 0xA6, 0xC9, 0x89, 0x00, 0x00, //NFCID3t: Change this to desired value

        0x06, 0x46,  0x66, 0x6D, 0x01, 0x01, 0x10, 0x00 // LLCP magic number and version parameter
    };
    return tgInitAsTargetP(command, sizeof(command), timeout);
}

int8_t tgInitAsTargetP(const uint8_t* command, const uint8_t len, const uint16_t timeout)
{
	int8_t status;

	status = writeCommand(command,len,0,0);

	if(status < 0)
	{
		return -1;
	}

	status = readResponse(pn532.packetbuffer, sizeof(pn532.packetbuffer), timeout);
	if(status > 0)
	{
		return 1;
	}
	else if(PN532_TIMEOUT == status)
	{
		return 0;
	}
	else
	{
		return -2;
	}
}

int16_t tgGetData(uint8_t *buf, uint8_t len)
{
    buf[0] = PN532_COMMAND_TGGETDATA;

    if(writeCommand(buf, 1, 0, 0) < 0)
    {
        return -1;
    }

    int8_t status = readResponse(buf, len, 3000);
    if(0 >= status)
    {
        return status;
    }

    uint16_t length = status - 1;


    if(buf[0] != 0)
    {
        return -5;
    }

    for(uint8_t i = 0; i < length; i++)
    {
        buf[i] = buf[i + 1];
    }

    return length;
}

bool tgSetData(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen)
{
    if(hlen > (sizeof(pn532.packetbuffer) - 1))
    {
        if ((body != 0) || (header == pn532.packetbuffer))
        {
            return false;
        }

        pn532.packetbuffer[0] = PN532_COMMAND_TGSETDATA;
        if(writeCommand(pn532.packetbuffer, 1, header, hlen) < 0)
        {
            return false;
        }
    }
    else
    {
        for(int8_t i = hlen - 1; i >= 0; i--)
        {
            pn532.packetbuffer[i + 1] = header[i];
        }
        pn532.packetbuffer[0] = PN532_COMMAND_TGSETDATA;

        if(writeCommand(pn532.packetbuffer, hlen + 1, body, blen) < 0)
        {
            return false;
        }
    }

    if(readResponse(pn532.packetbuffer, sizeof(pn532.packetbuffer), 3000) < 0)
    {
        return false;
    }

    if(0 != pn532.packetbuffer[0])
    {
        return false;
    }

    return true;
}

int8_t inRelease(const uint8_t relevantTarget)
{
    pn532.packetbuffer[0] = PN532_COMMAND_INRELEASE;
    pn532.packetbuffer[1] = relevantTarget;

    if(writeCommand(pn532.packetbuffer,2,0,0) < 0)
    {
        return 0;
    }

    // read data packet
    return readResponse(pn532.packetbuffer, sizeof(pn532.packetbuffer), 10);
}

/**************************************************************************/
/*!
    @brief  'InLists' a passive target. PN532 acting as reader/initiator,
            peer acting as card/responder.
*/
/**************************************************************************/
bool inListPassiveTarget()
{
    pn532.packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
    pn532.packetbuffer[1] = 1;
    pn532.packetbuffer[2] = 0;

    if(writeCommand(pn532.packetbuffer,3,0,0) < 0)
    {
        return false;
    }

    int16_t status = readResponse(pn532.packetbuffer, sizeof(pn532.packetbuffer), 30000);
    if(status < 0)
    {
        return false;
    }

    if(pn532.packetbuffer[0] != 1)
    {
        return false;
    }

    pn532.inListedTag = pn532.packetbuffer[1];

    return true;
}

/***** ISO14443A Commands ******/

/**************************************************************************/
/*!
    Waits for an ISO14443A target to enter the field

    @param  cardBaudRate  Baud rate of the card
    @param  uid           Pointer to the array that will be populated
                          with the card's UID (up to 7 bytes)
    @param  uidLength     Pointer to the variable that will hold the
                          length of the card's UID.
    @param  timeout       The number of tries before timing out
    @param  inlist        If set to true, the card will be inlisted

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
bool readPassiveTargetID(uint8_t cardbaudrate, uint8_t *uid, uint8_t *uidLength, uint16_t timeout, bool inlist)
{
	if(timeout == 0)timeout = 1000;
	if(inlist == 0)inlist = false;
    pn532.packetbuffer[0] = PN532_COMMAND_INLISTPASSIVETARGET;
    pn532.packetbuffer[1] = 1;  // max 1 cards at once (we can set this to 2 later)
    pn532.packetbuffer[2] = cardbaudrate;

    if(writeCommand(pn532.packetbuffer, 3, 0 ,0) < 0)
    {
        return false;  // command failed
    }

    // read data packet
    if(readResponse(pn532.packetbuffer, sizeof(pn532.packetbuffer), timeout) < 0)
    {
        return false;
    }

    // check some basic stuff
    /* ISO14443A card response should be in the following format:

      byte            Description
      -------------   ------------------------------------------
      b0              Tags Found
      b1              Tag Number (only one used in this example)
      b2..3           SENS_RES
      b4              SEL_RES
      b5              NFCID Length
      b6..NFCIDLen    NFCID
    */

    if(pn532.packetbuffer[0] != 1)return 0;

    uint16_t sens_res = pn532.packetbuffer[2];
    sens_res <<= 8;
    sens_res |= pn532.packetbuffer[3];

    /* Card appears to be Mifare Classic */
    *uidLength = pn532.packetbuffer[5];

    for(uint8_t i = 0; i < pn532.packetbuffer[5]; i++)
    {
        uid[i] = pn532.packetbuffer[6 + i];
    }

    if(inlist)
    {
        pn532.inListedTag = pn532.packetbuffer[1];
    }

    return 1;
}

/**************************************************************************/
/*!
    @brief  Exchanges an APDU with the currently inlisted peer

    @param  send            Pointer to data to send
    @param  sendLength      Length of the data to send
    @param  response        Pointer to response data
    @param  responseLength  Pointer to the response data length
*/
/**************************************************************************/
bool inDataExchange(uint8_t *send, uint8_t sendLength, uint8_t *response, uint8_t *responseLength)
{
    uint8_t i;

    pn532.packetbuffer[0] = 0x40; // PN532_COMMAND_INDATAEXCHANGE;
    pn532.packetbuffer[1] = pn532.inListedTag;

    if(writeCommand(pn532.packetbuffer, 2, send, sendLength) < 0)
    {
        return false;
    }

    int16_t status = readResponse(response, *responseLength, 1000);
    if(status < 0)
    {
        return false;
    }

    if((response[0] & 0x3f) != 0)
    {
        return false;
    }

    uint8_t length = status;
    length -= 1;

    if(length > *responseLength)
    {
        length = *responseLength; // silent truncation...
    }

    for(uint8_t i = 0; i < length; i++)
    {
        response[i] = response[i + 1];
    }
    *responseLength = length;

    return true;
}

/***** Mifare Classic Functions ******/

/**************************************************************************/
/*!
      Indicates whether the specified block number is the first block
      in the sector (block 0 relative to the current sector)
*/
/**************************************************************************/
bool mifareclassic_IsFirstBlock(uint32_t uiBlock)
{
    // Test if we are in the small or big sectors
    if(uiBlock < 128)return((uiBlock) % 4 == 0);
    else return ((uiBlock) % 16 == 0);
}

/**************************************************************************/
/*!
      Indicates whether the specified block number is the sector trailer
*/
/**************************************************************************/
bool mifareclassic_IsTrailerBlock(uint32_t uiBlock)
{
    // Test if we are in the small or big sectors
    if (uiBlock < 128)return((uiBlock + 1) % 4 == 0);
    else return((uiBlock + 1) % 16 == 0);
}

/**************************************************************************/
/*!
    Tries to authenticate a block of memory on a MIFARE card using the
    INDATAEXCHANGE command.  See section 7.3.8 of the PN532 User Manual
    for more information on sending MIFARE and other commands.

    @param  uid           Pointer to a byte array containing the card UID
    @param  uidLen        The length (in bytes) of the card's UID (Should
                          be 4 for MIFARE Classic)
    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  keyNumber     Which key type to use during authentication
                          (0 = MIFARE_CMD_AUTH_A, 1 = MIFARE_CMD_AUTH_B)
    @param  keyData       Pointer to a byte array containing the 6 bytes
                          key value

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t mifareclassic_AuthenticateBlock(uint8_t *uid, uint8_t uidLen, uint32_t blockNumber, uint8_t keyNumber, uint8_t *keyData)
{
    uint8_t i;

    // Hang on to the key and uid data
    memcpy(pn532.key, keyData, 6);
    memcpy(pn532.uid, uid, uidLen);
    pn532.uidLen = uidLen;

    // Prepare the authentication command //
    pn532.packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;   /* Data Exchange Header */
    pn532.packetbuffer[1] = 1;                              /* Max card numbers */
    pn532.packetbuffer[2] = (keyNumber) ? MIFARE_CMD_AUTH_B : MIFARE_CMD_AUTH_A;
    pn532.packetbuffer[3] = blockNumber;                    /* Block Number (1K = 0..63, 4K = 0..255 */

    memcpy(pn532.packetbuffer + 4, pn532.key, 6);

    for(i = 0; i < pn532.uidLen; i++)
    {
        pn532.packetbuffer[10 + i] = pn532.uid[i];              /* 4 bytes card ID */
    }

    if(writeCommand(pn532.packetbuffer, 10 + pn532.uidLen, 0, 0) < 0)return 0;

    // Read the response packet
    readResponse(pn532.packetbuffer, sizeof(pn532.packetbuffer), 10);

    // Check if the response is valid and we are authenticated???
    // for an auth success it should be bytes 5-7: 0xD5 0x41 0x00
    // Mifare auth error is technically byte 7: 0x14 but anything other and 0x00 is not good
    if(pn532.packetbuffer[0] != 0x00)
    {
        return 0;
    }

    return 1;
}

/**************************************************************************/
/*!
    Tries to read an entire 16-bytes data block at the specified block
    address.

    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  data          Pointer to the byte array that will hold the
                          retrieved data (if any)

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t mifareclassic_ReadDataBlock(uint8_t blockNumber, uint8_t *data)
{
    /* Prepare the command */
    pn532.packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
    pn532.packetbuffer[1] = 1;                      /* Card number */
    pn532.packetbuffer[2] = MIFARE_CMD_READ;        /* Mifare Read command = 0x30 */
    pn532.packetbuffer[3] = blockNumber;            /* Block Number (0..63 for 1K, 0..255 for 4K) */

    /* Send the command */
    if(writeCommand(pn532.packetbuffer, 4, 0, 0) < 0)
    {
        return 0;
    }

    /* Read the response packet */
    readResponse(pn532.packetbuffer, sizeof(pn532.packetbuffer), 10);

    /* If byte 8 isn't 0x00 we probably have an error */
    if(pn532.packetbuffer[0] != 0x00)
    {
        return 0;
    }

    /* Copy the 16 data bytes to the output buffer        */
    /* Block content starts at byte 9 of a valid response */
    memcpy(data, pn532.packetbuffer + 1, 16);

    return 1;
}

/**************************************************************************/
/*!
    Tries to write an entire 16-bytes data block at the specified block
    address.

    @param  blockNumber   The block number to authenticate.  (0..63 for
                          1KB cards, and 0..255 for 4KB cards).
    @param  data          The byte array that contains the data to write.

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t mifareclassic_WriteDataBlock(uint8_t blockNumber, uint8_t *data)
{
	int8_t status;

    /* Prepare the first command */
    pn532.packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
    pn532.packetbuffer[1] = 1;                      /* Card number */
    pn532.packetbuffer[2] = MIFARE_CMD_WRITE;       /* Mifare Write command = 0xA0 */
    pn532.packetbuffer[3] = blockNumber;            /* Block Number (0..63 for 1K, 0..255 for 4K) */
    memcpy (pn532.packetbuffer + 4, data, 16);        /* Data Payload */

    /* Send the command */
    if(writeCommand(pn532.packetbuffer, 20, 0, 0) < 0)
    {
        return 0;
    }

    /* Read the response packet */
    status = readResponse(pn532.packetbuffer, sizeof(pn532.packetbuffer), 10);
    if(status < 0)return false;
    else return true;
}

/**************************************************************************/
/*!
    Formats a Mifare Classic card to store NDEF Records

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t mifareclassic_FormatNDEF(void)
{
    uint8_t sectorbuffer1[16] = {0x14, 0x01, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1};
    uint8_t sectorbuffer2[16] = {0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1, 0x03, 0xE1};
    uint8_t sectorbuffer3[16] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0x78, 0x77, 0x88, 0xC1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    // Note 0xA0 0xA1 0xA2 0xA3 0xA4 0xA5 must be used for key A
    // for the MAD sector in NDEF records (sector 0)

    // Write block 1 and 2 to the card
    if (!(mifareclassic_WriteDataBlock(1, sectorbuffer1)))return 0;

    if (!(mifareclassic_WriteDataBlock(2, sectorbuffer2)))return 0;

    // Write key A and access rights card
    if (!(mifareclassic_WriteDataBlock(3, sectorbuffer3)))return 0;

    // Seems that everything was OK (?!)
    return 1;
}

/**************************************************************************/
/*!
    Writes an NDEF URI Record to the specified sector (1..15)

    Note that this function assumes that the Mifare Classic card is
    already formatted to work as an "NFC Forum Tag" and uses a MAD1
    file system.  You can use the NXP TagWriter app on Android to
    properly format cards for this.

    @param  sectorNumber  The sector that the URI record should be written
                          to (can be 1..15 for a 1K card)
    @param  uriIdentifier The uri identifier code (0 = none, 0x01 =
                          "http://www.", etc.)
    @param  url           The uri text to write (max 38 characters).

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t mifareclassic_WriteNDEFURI(uint8_t sectorNumber, uint8_t uriIdentifier, const char *url)
{
    // Figure out how long the string is
    uint8_t len = strlen(url);

    // Make sure we're within a 1K limit for the sector number
    if((sectorNumber < 1) || (sectorNumber > 15))return 0;

    // Make sure the URI payload is between 1 and 38 chars
    if((len < 1) || (len > 38))return 0;

    // Note 0xD3 0xF7 0xD3 0xF7 0xD3 0xF7 must be used for key A
    // in NDEF records

    // Setup the sector buffer (w/pre-formatted TLV wrapper and NDEF message)
    uint8_t sectorbuffer1[16] = {0x00, 0x00, 0x03, len + 5, 0xD1, 0x01, len + 1, 0x55, uriIdentifier, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t sectorbuffer2[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t sectorbuffer3[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t sectorbuffer4[16] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7, 0x7F, 0x07, 0x88, 0x40, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    if(len <= 6)
    {
        // Unlikely we'll get a url this short, but why not ...
        memcpy(sectorbuffer1 + 9, url, len);
        sectorbuffer1[len + 9] = 0xFE;
    }
    else if(len == 7)
    {
        // 0xFE needs to be wrapped around to next block
        memcpy(sectorbuffer1 + 9, url, len);
        sectorbuffer2[0] = 0xFE;
    }
    else if((len > 7) || (len <= 22))
    {
        // Url fits in two blocks
        memcpy(sectorbuffer1 + 9, url, 7);
        memcpy(sectorbuffer2, url + 7, len - 7);
        sectorbuffer2[len - 7] = 0xFE;
    }
    else if(len == 23)
    {
        // 0xFE needs to be wrapped around to final block
        memcpy(sectorbuffer1 + 9, url, 7);
        memcpy(sectorbuffer2, url + 7, len - 7);
        sectorbuffer3[0] = 0xFE;
    }
    else
    {
        // Url fits in three blocks
        memcpy(sectorbuffer1 + 9, url, 7);
        memcpy(sectorbuffer2, url + 7, 16);
        memcpy(sectorbuffer3, url + 23, len - 24);
        sectorbuffer3[len - 22] = 0xFE;
    }

    // Now write all three blocks back to the card
    if (!(mifareclassic_WriteDataBlock (sectorNumber * 4, sectorbuffer1)))return 0;
    if (!(mifareclassic_WriteDataBlock ((sectorNumber * 4) + 1, sectorbuffer2)))return 0;
    if (!(mifareclassic_WriteDataBlock ((sectorNumber * 4) + 2, sectorbuffer3)))return 0;
    if (!(mifareclassic_WriteDataBlock ((sectorNumber * 4) + 3, sectorbuffer4)))return 0;

    // Seems that everything was OK (?!)
    return 1;
}

/***** Mifare Ultralight Functions ******/

/**************************************************************************/
/*!
    Tries to read an entire 4-bytes page at the specified address.

    @param  page        The page number (0..63 in most cases)
    @param  buffer      Pointer to the byte array that will hold the
                        retrieved data (if any)
*/
/**************************************************************************/
uint8_t mifareultralight_ReadPage(uint8_t page, uint8_t *buffer)
{
    if(page >= 64)
    {
        return 0;
    }

    /* Prepare the command */
    pn532.packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
    pn532.packetbuffer[1] = 1;                   /* Card number */
    pn532.packetbuffer[2] = MIFARE_CMD_READ;     /* Mifare Read command = 0x30 */
    pn532.packetbuffer[3] = page;                /* Page Number (0..63 in most cases) */

    /* Send the command */
    if(writeCommand(pn532.packetbuffer, 4, 0, 0) < 0)
    {
        return 0;
    }

    /* Read the response packet */
    readResponse(pn532.packetbuffer, sizeof(pn532.packetbuffer), 10);

    /* If byte 8 isn't 0x00 we probably have an error */
    if(pn532.packetbuffer[0] == 0x00)
    {
        /* Copy the 4 data bytes to the output buffer         */
        /* Block content starts at byte 9 of a valid response */
        /* Note that the command actually reads 16 bytes or 4  */
        /* pages at a time ... we simply discard the last 12  */
        /* bytes                                              */
        memcpy(buffer, pn532.packetbuffer + 1, 4);
    }
    else
    {
        return 0;
    }

    // Return OK signal
    return 1;
}

/**************************************************************************/
/*!
    Tries to write an entire 4-bytes data buffer at the specified page
    address.

    @param  page     The page number to write into.  (0..63).
    @param  buffer   The byte array that contains the data to write.

    @returns 1 if everything executed properly, 0 for an error
*/
/**************************************************************************/
uint8_t mifareultralight_WritePage(uint8_t page, uint8_t *buffer)
{
    /* Prepare the first command */
    pn532.packetbuffer[0] = PN532_COMMAND_INDATAEXCHANGE;
    pn532.packetbuffer[1] = 1;                           /* Card number */
    pn532.packetbuffer[2] = MIFARE_CMD_WRITE_ULTRALIGHT; /* Mifare UL Write cmd = 0xA2 */
    pn532.packetbuffer[3] = page;                        /* page Number (0..63) */
    memcpy (pn532.packetbuffer + 4, buffer, 4);          /* Data Payload */

    /* Send the command */
    if(writeCommand(pn532.packetbuffer, 8, 0, 0) < 0)
    {
        return 0;
    }

    /* Read the response packet */
    return (0 < readResponse(pn532.packetbuffer, sizeof(pn532.packetbuffer), 10));
}

uint8_t *getBuffer(uint8_t *len)
{
	*len = sizeof(pn532.packetbuffer) - 4;
	return pn532.packetbuffer;
};
