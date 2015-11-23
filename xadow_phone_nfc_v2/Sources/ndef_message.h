/*
 * ndef_message.h
 *
 *  Created on: 2015Äê8ÔÂ14ÈÕ
 *      Author: Wayen
 */

#ifndef NDEF_MESSAGE_H_
#define NDEF_MESSAGE_H_

#include "ndef_record.h"


#define MAX_NDEF_RECORDS 4


typedef struct
{
	stru_record *records[MAX_NDEF_RECORDS];
	unsigned int recordCount;
}stru_message;


int get_encoded_message_size(stru_message *message); // need so we can pass array to encode
void encode_message(stru_message *message, unsigned char *data);

bool add_record(stru_message *message, stru_record *record);
//void add_mime_media_record(stru_message *message, char *mimeType, unsigned char *payload);
void add_text_record(stru_message *message, char *text);
//void add_uri_record(stru_message *message, char *uri);
void add_empty_record(stru_message *message);

unsigned int get_record_count(stru_message *message);
stru_record *get_record(stru_message *message, int index);

stru_message *create_message();
stru_message *create_message_b(const unsigned char* data, const int numBytes);
void free_message(stru_message *message);
void print_message(stru_message *message);


#endif /* NDEF_MESSAGE_H_ */
