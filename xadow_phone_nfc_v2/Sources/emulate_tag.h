/*
 * emulate_tag.h
 *
 *  Created on: 2015Äê8ÔÂ17ÈÕ
 *      Author: Wayen
 */

#ifndef SOURCES_EMULATE_TAG_H_
#define SOURCES_EMULATE_TAG_H_


#include "pn532.h"


#define NDEF_MAX_LENGTH 128  // altough ndef can handle up to 0xfffe in size

// Command APDU
#define C_APDU_CLA   0
#define C_APDU_INS   1 // instruction
#define C_APDU_P1    2 // parameter 1
#define C_APDU_P2    3 // parameter 2
#define C_APDU_LC    4 // length command
#define C_APDU_DATA  5 // data

#define C_APDU_P1_SELECT_BY_ID   0x00
#define C_APDU_P1_SELECT_BY_NAME 0x04

// Response APDU
#define R_APDU_SW1_COMMAND_COMPLETE 0x90
#define R_APDU_SW2_COMMAND_COMPLETE 0x00

#define R_APDU_SW1_NDEF_TAG_NOT_FOUND 0x6a
#define R_APDU_SW2_NDEF_TAG_NOT_FOUND 0x82

#define R_APDU_SW1_FUNCTION_NOT_SUPPORTED 0x6A
#define R_APDU_SW2_FUNCTION_NOT_SUPPORTED 0x81

#define R_APDU_SW1_MEMORY_FAILURE 0x65
#define R_APDU_SW2_MEMORY_FAILURE 0x81

#define R_APDU_SW1_END_OF_FILE_BEFORE_REACHED_LE_BYTES 0x62
#define R_APDU_SW2_END_OF_FILE_BEFORE_REACHED_LE_BYTES 0x82

// ISO7816-4 commands
#define ISO7816_SELECT_FILE 0xA4
#define ISO7816_READ_BINARY 0xB0
#define ISO7816_UPDATE_BINARY 0xD6


typedef enum
{
	NONE,
	CC,
	NDEF
}tag_file;   // CC ... Compatibility Container

typedef enum
{
	COMMAND_COMPLETE,
	TAG_NOT_FOUND,
	FUNCTION_NOT_SUPPORTED,
	MEMORY_FAILURE,
	END_OF_FILE_BEFORE_REACHED_LE_BYTES
}responseCommand;

typedef struct
{
	unsigned char ndef_file[NDEF_MAX_LENGTH];
	unsigned char* uidPtr;
	bool tagWrittenByInitiator;
	bool tagWriteable;
	void (*updateNdefCallback)(unsigned char *ndef, unsigned int length);
}stru_EmulateTag;


bool emulate_tag_init();
bool emulate_tag(const unsigned int tgInitAsTargetTimeout);
/*
* @param uid pointer to byte array of length 3 (uid is 4 bytes - first byte is fixed) or zero for uid
*/
void set_uid(unsigned char* uid);
void set_ndef_file(const unsigned char* ndef, const int ndefLength);
void set_response(responseCommand cmd, unsigned char* buf, unsigned char* sendlen, unsigned char sendlenOffset);

void get_content(unsigned char** buf, unsigned int* length);
bool write_occured();
void set_tag_writeable(bool setWriteable);
unsigned char* get_ndef_file_ptr();
unsigned char get_ndef_max_length();
void attach(void (*func)(unsigned char *buf, unsigned int length));


#endif /* SOURCES_EMULATE_TAG_H_ */
