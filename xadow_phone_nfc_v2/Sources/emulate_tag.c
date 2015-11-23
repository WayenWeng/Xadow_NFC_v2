/*
 * emulate_tag.c
 *
 *  Created on: 2015Äê8ÔÂ17ÈÕ
 *      Author: Wayen
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "pn532_i2c.h"
#include "emulate_tag.h"


stru_EmulateTag emulateTag =
{
	.uidPtr = (unsigned char*)NULL,
	.tagWrittenByInitiator = false,
	.tagWriteable = true,
	.updateNdefCallback = 0
};


bool emulate_tag_init()
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

void get_content(unsigned char** buf, unsigned int* length)
{
  *buf = emulateTag.ndef_file + 2; // first 2 bytes = length
  *length = (emulateTag.ndef_file[0] << 8) + emulateTag.ndef_file[1];
}

bool write_occured()
{
  return emulateTag.tagWrittenByInitiator;
}

void set_tag_writeable(bool setWriteable)
{
	emulateTag.tagWriteable = setWriteable;
}

unsigned char* get_ndef_file_ptr()
{
  return emulateTag.ndef_file;
}

unsigned char get_ndef_max_length()
{
  return NDEF_MAX_LENGTH;
}

void attach(void (*func)(unsigned char *buf, unsigned int length))
{
	emulateTag.updateNdefCallback = func;
};

bool emulate_tag(const unsigned int tgInitAsTargetTimeout)
{
	unsigned char command[] =
	{
		PN532_COMMAND_TGINITASTARGET,
		2,                  // MODE

		0x00, 0x08,         // SENS_RES
		0x00, 0x00, 0x00,   // NFCID1
		0x40,               // SEL_RES

		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,   // FeliCaParams
		0,0,

		0,0,0,0,0,0,0,0,0,0, // NFCID3t

		14, // length of general bytes
		0x01,0x01,0x0a,0x54,0x02,0x65,0x6E,0x58,0x61,0x64,0x6F,0x77,0x20,0x36,
		14,  // length of historical bytes
		0x01,0x01,0x0a,0x54,0x02,0x65,0x6E,0x58,0x61,0x64,0x6F,0x77,0x20,0x36
	};

	if(emulateTag.uidPtr != 0)
	{  // if uid is set copy 3 bytes to nfcid1
		memcpy(command + 4, emulateTag.uidPtr, 3);
	}

	if(1 != tgInitAsTargetP(command,sizeof(command), tgInitAsTargetTimeout))
	{
		return false;
	}

	unsigned char compatibility_container[] =
	{
		0, 0x0F,
		0x20,
		0, 0x54,
		0, 0xFF,
		0x04,       // T
		0x06,       // L
		0xE1, 0x04, // File identifier
		((NDEF_MAX_LENGTH & 0xFF00) >> 8), (NDEF_MAX_LENGTH & 0xFF), // maximum NDEF file size
		0x00,       // read access 0x0 = granted
		0x00        // write access 0x0 = granted | 0xFF = deny
	};

	if(emulateTag.tagWriteable == false)
	{
		compatibility_container[14] = 0xFF;
	}

	emulateTag.tagWrittenByInitiator = false;

	unsigned char rwbuf[128];
	unsigned char sendlen;
	int status;
	tag_file currentFile = NONE;
	unsigned int cc_size = sizeof(compatibility_container);
	bool runLoop = true;

	while(runLoop)
	{
		status = tgGetData(rwbuf, sizeof(rwbuf));
		if(status < 0)
		{
			inRelease(0);
			return true;
		}

		unsigned char p1 = rwbuf[C_APDU_P1];
		unsigned char p2 = rwbuf[C_APDU_P2];
		unsigned char lc = rwbuf[C_APDU_LC];
		unsigned int p1p2_length = ((int) p1 << 8) + p2;
		const unsigned char ndef_tag_application_name_v2[] = {0, 0x7, 0xD2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01 };

		switch(rwbuf[C_APDU_INS])
		{
			case ISO7816_SELECT_FILE:
			switch(p1)
			{
				case C_APDU_P1_SELECT_BY_ID:
				if(p2 != 0x0c)
				{
					set_response(COMMAND_COMPLETE, rwbuf, &sendlen, 0);
				}
				else if(lc == 2 && rwbuf[C_APDU_DATA] == 0xE1 && (rwbuf[C_APDU_DATA+1] == 0x03 || rwbuf[C_APDU_DATA+1] == 0x04))
				{
					set_response(COMMAND_COMPLETE, rwbuf, &sendlen, 0);
					if(rwbuf[C_APDU_DATA+1] == 0x03)
					{
						currentFile = CC;
					}
					else if(rwbuf[C_APDU_DATA+1] == 0x04)
					{
						currentFile = NDEF;
					}
				}
				else
				{
					set_response(TAG_NOT_FOUND, rwbuf, &sendlen, 0);
				}
				break;
				case C_APDU_P1_SELECT_BY_NAME:
				if(0 == memcmp(ndef_tag_application_name_v2, rwbuf + C_APDU_P2, sizeof(ndef_tag_application_name_v2)))
				{
					set_response(COMMAND_COMPLETE, rwbuf, &sendlen, 0);
				}
				else
				{
					set_response(FUNCTION_NOT_SUPPORTED, rwbuf, &sendlen, 0);
				}
				break;
			}
			break;
			case ISO7816_READ_BINARY:
			switch(currentFile)
			{
				case NONE:
				set_response(TAG_NOT_FOUND, rwbuf, &sendlen, 0);
				break;
				case CC:
				if( p1p2_length > NDEF_MAX_LENGTH)
				{
					set_response(END_OF_FILE_BEFORE_REACHED_LE_BYTES, rwbuf, &sendlen, 0);
				}
				else
				{
					memcpy(rwbuf,compatibility_container + p1p2_length, lc);
					set_response(COMMAND_COMPLETE, rwbuf + lc, &sendlen, lc);
				}
				break;
				case NDEF:
				if( p1p2_length > NDEF_MAX_LENGTH)
				{
					set_response(END_OF_FILE_BEFORE_REACHED_LE_BYTES, rwbuf, &sendlen, 0);
				}
				else
				{
					memcpy(rwbuf, emulateTag.ndef_file + p1p2_length, lc);
					set_response(COMMAND_COMPLETE, rwbuf + lc, &sendlen, lc);
				}
				break;
			}
			break;
			case ISO7816_UPDATE_BINARY:
			if(!emulateTag.tagWriteable)
			{
				set_response(FUNCTION_NOT_SUPPORTED, rwbuf, &sendlen, 0);
			}
			else
			{
				if(p1p2_length > NDEF_MAX_LENGTH)
				{
					set_response(MEMORY_FAILURE, rwbuf, &sendlen, 0);
				}
				else
				{
					memcpy(emulateTag.ndef_file + p1p2_length, rwbuf + C_APDU_DATA, lc);
					set_response(COMMAND_COMPLETE, rwbuf, &sendlen, 0);
					emulateTag.tagWrittenByInitiator = true;

					unsigned int ndef_length = (emulateTag.ndef_file[0] << 8) + emulateTag.ndef_file[1];
					if((ndef_length > 0) && (emulateTag.updateNdefCallback != 0))
					{
						emulateTag.updateNdefCallback(emulateTag.ndef_file + 2, ndef_length);
					}
				}
			}
			break;
			default:
			set_response(FUNCTION_NOT_SUPPORTED, rwbuf, &sendlen, 0);
		}
		status = tgSetData(rwbuf, sendlen, 0, 0);
		if(status < 0)
		{
			inRelease(0);
			return true;
		}
	}

	inRelease(0);
	return true;
}

void set_uid(unsigned char* uid)
{
	emulateTag.uidPtr = uid;
}

void set_ndef_file(const unsigned char* ndef, const int ndefLength)
{
	if(ndefLength > (NDEF_MAX_LENGTH - 2))
	{
		return;
	}

	emulateTag.ndef_file[0] = ndefLength >> 8;
	emulateTag.ndef_file[1] = ndefLength & 0xFF;
	memcpy(emulateTag.ndef_file+2, ndef, ndefLength);
}

void set_response(responseCommand cmd, unsigned char* buf, unsigned char* sendlen, unsigned char sendlenOffset)
{
	switch(cmd)
	{
		case COMMAND_COMPLETE:
		buf[0] = R_APDU_SW1_COMMAND_COMPLETE;
		buf[1] = R_APDU_SW2_COMMAND_COMPLETE;
		*sendlen = 2 + sendlenOffset;
		break;
		case TAG_NOT_FOUND:
		buf[0] = R_APDU_SW1_NDEF_TAG_NOT_FOUND;
		buf[1] = R_APDU_SW2_NDEF_TAG_NOT_FOUND;
		*sendlen = 2;
		break;
		case FUNCTION_NOT_SUPPORTED:
		buf[0] = R_APDU_SW1_FUNCTION_NOT_SUPPORTED;
		buf[1] = R_APDU_SW2_FUNCTION_NOT_SUPPORTED;
		*sendlen = 2;
		break;
		case MEMORY_FAILURE:
		buf[0] = R_APDU_SW1_MEMORY_FAILURE;
		buf[1] = R_APDU_SW2_MEMORY_FAILURE;
		*sendlen = 2;
		break;
		case END_OF_FILE_BEFORE_REACHED_LE_BYTES:
		buf[0] = R_APDU_SW1_END_OF_FILE_BEFORE_REACHED_LE_BYTES;
		buf[1] = R_APDU_SW2_END_OF_FILE_BEFORE_REACHED_LE_BYTES;
		*sendlen= 2;
		break;
	}
}
