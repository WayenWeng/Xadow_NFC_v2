/*
 * ndef_record.c
 *
 *  Created on: 2015Äê8ÔÂ14ÈÕ
 *      Author: Wayen
 */


#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ndef_record.h"


stru_record record_buffer;
unsigned char type_buffer[32] = {0,};
unsigned char payload_buffer[128] = {0,};
unsigned char id_buffer[32] = {0,};


stru_record *create_record()
{
	stru_record *record = &record_buffer;

	//record = (stru_record *)malloc(sizeof(stru_record));

	//if(record)
	{
		record->tnf = 0;
		record->typeLength = 0;
		record->payloadLength = 0;
		record->idLength = 0;
		record->type = (unsigned char *)NULL;
		record->payload = (unsigned char *)NULL;
		record->id = (unsigned char *)NULL;
		return record;
	}
	//else return NULL;
}

void free_record(stru_record *record)
{
	//free(record->type);
	//free(record->payload);
	//free(record->id);
	//free(record);
}

int get_encoded_record_size(stru_record *record)
{
	int size = 2; // tnf + typeLength

	if(record->payloadLength > 0xFF)
	{
		size += 4;
	}
	else
	{
		size += 1;
	}

	if(record->idLength)
	{
		size += 1;
	}

	size += (record->typeLength + record->payloadLength + record->idLength);

	return size;
}

void encode_record(stru_record *record, unsigned char *data, bool firstRecord, bool lastRecord)
{
    // assert data > getEncodedSize()
    unsigned char *data_ptr = &data[0];

    *data_ptr = get_tnf_byte(record, firstRecord, lastRecord);
    data_ptr += 1;

    *data_ptr = record->typeLength;
    data_ptr += 1;

    if (record->payloadLength <= 0xFF)  // short record
    {
        *data_ptr = record->payloadLength;
        data_ptr += 1;
    }
    else // long format
    {
        // 4 bytes but we store length as an int
        data_ptr[0] = 0x0; // (_payloadLength >> 24) & 0xFF;
        data_ptr[1] = 0x0; // (_payloadLength >> 16) & 0xFF;
        data_ptr[2] = (record->payloadLength >> 8) & 0xFF;
        data_ptr[3] = record->payloadLength & 0xFF;
        data_ptr += 4;
    }

    if (record->idLength)
    {
        *data_ptr = record->idLength;
        data_ptr += 1;
    }

    memcpy(data_ptr, record->type, record->typeLength);
    data_ptr += record->typeLength;

    memcpy(data_ptr, record->payload, record->payloadLength);
    data_ptr += record->payloadLength;

    if (record->idLength)
    {
        memcpy(data_ptr, record->id, record->idLength);
        data_ptr += record->idLength;
    }
}

unsigned char get_tnf_byte(stru_record *record ,bool firstRecord, bool lastRecord)
{
	int value = record->tnf;

	if(firstRecord) // mb
	{
		value = value | 0x80;
	}

	if(lastRecord) //
	{
		value = value | 0x40;
	}

	if(record->payloadLength <= 0xFF)
	{
		value = value | 0x10;
	}

	if(record->idLength)
	{
		value = value | 0x8;
	}

	return value;
}

unsigned char get_tnf(stru_record *record)
{
	return record->tnf;
}

unsigned int get_type_length(stru_record *record)
{
	return record->typeLength;
}

int get_payload_length(stru_record *record)
{
	return record->payloadLength;
}

unsigned int get_id_length(stru_record *record)
{
	return record->idLength;
}

unsigned char *get_type(stru_record *record)
{
	return record->type;
}

unsigned char *get_payload(stru_record *record)
{
	return record->payload;
}

unsigned char *get_id(stru_record *record)
{
	return record->id;
}

void set_tnf(stru_record *record, unsigned char tnf)
{
	record->tnf = tnf;
}

void set_type(stru_record *record, const unsigned char *type, const unsigned int numBytes)
{
	/*
	if(record->typeLength)
	{
		free(record->type);
	}
	*/

	//record->type = (unsigned char*)malloc(numBytes);
	record->type = type_buffer;
	memcpy(record->type, type, numBytes);

	record->type[numBytes] = '\0';

	record->typeLength = numBytes;
}

void set_payload(stru_record *record ,const unsigned char *payload, const int numBytes)
{
	/*
    if(record->payloadLength)
    {
        free(record->payload);
    }
    */

    //record->payload = (unsigned char*)malloc(numBytes);
    record->payload = payload_buffer;
    memcpy(record->payload, payload, numBytes);

    record->payload[numBytes] = '\0';

    record->payloadLength = numBytes;
}

void set_id(stru_record *record ,const unsigned char *id, const unsigned int numBytes)
{
	/*
    if(record->idLength)
    {
        free(record->id);
    }
    */

    //record->id = (unsigned char*)malloc(numBytes);
    record->id = id_buffer;
    memcpy(record->id, id, numBytes);

    record->id[numBytes] = '\0';

    record->idLength = numBytes;
}

void print_record(stru_record *record)
{

}
