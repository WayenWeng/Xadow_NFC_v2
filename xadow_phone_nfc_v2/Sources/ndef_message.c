/*
 * ndef_message.c
 *
 *  Created on: 2015Äê8ÔÂ14ÈÕ
 *      Author: Wayen
 */



#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ndef_message.h"


stru_message message_buffer;


stru_message *create_message()
{
	stru_message *message = &message_buffer;

	//message = (stru_message *)malloc(sizeof(stru_message));

	message->recordCount = 0;

	return message;
}

stru_message *create_message_b(const unsigned char* data, const int numBytes)
{
	stru_message *message = &message_buffer;

	//message = (stru_message *)malloc(sizeof(stru_message));

	int index = 0;
	message->recordCount = 0;

    while(index <= numBytes)
    {
        // decode tnf - first byte is tnf with bit flags
        // see the NFDEF spec for more info
        unsigned char tnf_byte = data[index];
        bool mb = (tnf_byte & 0x80) != 0;
        bool me = (tnf_byte & 0x40) != 0;
        bool cf = (tnf_byte & 0x20) != 0;
        bool sr = (tnf_byte & 0x10) != 0;
        bool il = (tnf_byte & 0x8) != 0;
        unsigned char tnf = (tnf_byte & 0x7);

        stru_record *record = create_record();
        set_tnf(record, tnf);

        index++;
        int typeLength = data[index];

        int payloadLength = 0;
        if(sr)
        {
            index++;
            payloadLength = data[index];
        }
        else
        {
            payloadLength =((0xFF & data[++index]) << 24) | ((0xFF & data[++index]) << 16) | ((0xFF & data[++index]) << 8) | (0xFF & data[++index]);
        }

        int idLength = 0;
        if(il)
        {
            index++;
            idLength = data[index];
        }

        index++;
        set_type(record, &data[index], typeLength);
        index += typeLength;

        if(il)
        {
            set_id(record, &data[index], idLength);
            index += idLength;
        }

        set_payload(record, &data[index], payloadLength);
        index += payloadLength;

        add_record(message, record);

        if(me)break; // last message
    }
    return message;
}

void free_message(stru_message *message)
{
	for(int i=0;i<message->recordCount;i++)
	{
		//free(message->records[i]->type);
		//free(message->records[i]->payload);
		//free(message->records[i]->id);
		//free(message->records[i]);
	}
	//free(message);
}

int get_encoded_message_size(stru_message *message)
{
	int size = 0;
	for (int i = 0; i < message->recordCount; i++)
	{
		size += get_encoded_record_size(message->records[i]);
	}
	return size;
}

void encode_message(stru_message *message, unsigned char *data)
{
    // assert sizeof(data) >= getEncodedSize()
    unsigned char* data_ptr = &data[0];

    for(int i = 0; i < message->recordCount; i++)
    {
    	encode_record(message->records[i], data_ptr, i == 0, (i + 1) == message->recordCount);

        // can NdefRecord.encode return the record size?
    	data_ptr += get_encoded_record_size(message->records[i]);
    }
}

bool add_record(stru_message *message, stru_record *record)
{
    if(message->recordCount < MAX_NDEF_RECORDS)
    {
    	message->records[message->recordCount] = record;
    	message->recordCount++;
        return true;
    }
    else
    {
        return false;
    }
}
/*
void add_mime_media_record(stru_message *message, char *mimeType, unsigned char *payload)
{
	unsigned char payloadBytes[strlen(payload) + 1];

	memcpy(payloadBytes, payload,strlen(payload));

	stru_record *record = create_record();
	set_tnf(record, TNF_MIME_MEDIA);

	unsigned char type[strlen(mimeType) + 1];
	memcpy(type, mimeType, strlen(mimeType));
	set_type(record, type, strlen(mimeType));

	set_payload(record, payloadBytes, strlen(payload));

	add_record(message, record);
}
*/
void add_text_record(stru_message *message, char *text)
{
	stru_record *record = create_record();
	set_tnf(record, TNF_WELL_KNOWN);

	unsigned char RTD_TEXT[1] = { 0x54 }; // this should be a constant or preprocessor
	set_type(record, RTD_TEXT, sizeof(RTD_TEXT));

	// X is a placeholder for encoding length
	// is it more efficient to build w/o string concatenation?
	unsigned char payload[strlen(text) + 4];

	payload[strlen(text) + 3] = '\0';
	memcpy(payload, "Xen", 3);
	memcpy(payload + 3, text, strlen(text));
	// replace X with the real encoding length
	payload[0] = 2;
	set_payload(record, payload, strlen(text) + 3);

	add_record(message, record);
}
/*
void add_uri_record(stru_message *message, char *uri)
{
	stru_record *record = create_record();
	set_tnf(record, TNF_WELL_KNOWN);

	unsigned char RTD_URI[1] = { 0x55 }; // this should be a constant or preprocessor
	set_type(record, RTD_URI, sizeof(RTD_URI));

	// X is a placeholder for identifier code
	unsigned char payload[strlen(uri) + 2];
	memcpy(payload, "X", 1);
	memcpy(payload + 1, uri, strlen(uri));

	// add identifier code 0x0, meaning no prefix substitution
	payload[0] = 0x0;

	set_payload(record, payload, strlen(uri) + 1);

	add_record(message, record);
}
*/
void add_empty_record(stru_message *message)
{
	stru_record *record = create_record();
    set_tnf(record, TNF_EMPTY);
    add_record(message, record);
}

unsigned int get_record_count(stru_message *message)
{
	return message->recordCount;
}

stru_record *get_record(stru_message *message, int index)
{
	return message->records[index];
}

void print_message(stru_message *message)
{

}

