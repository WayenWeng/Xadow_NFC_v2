/*
 * ndef_record.h
 *
 *  Created on: 2015Äê8ÔÂ14ÈÕ
 *      Author: Wayen
 */

#ifndef SOURCES_NDEF_RECORD_H_
#define SOURCES_NDEF_RECORD_H_


#define TNF_EMPTY 0x0
#define TNF_WELL_KNOWN 0x01
#define TNF_MIME_MEDIA 0x02
#define TNF_ABSOLUTE_URI 0x03
#define TNF_EXTERNAL_TYPE 0x04
#define TNF_UNKNOWN 0x05
#define TNF_UNCHANGED 0x06
#define TNF_RESERVED 0x07


typedef struct
{
    unsigned char tnf; // 3 bit
    unsigned int typeLength;
    int payloadLength;
    unsigned int idLength;
    unsigned char *type;
    unsigned char *payload;
    unsigned char *id;
}stru_record;


int get_encoded_record_size(stru_record *record);
void encode_record(stru_record *record, unsigned char *data, bool firstRecord, bool lastRecord);

unsigned char get_tnf_byte(stru_record *record ,bool firstRecord, bool lastRecord);

unsigned char get_tnf(stru_record *record);
unsigned int get_type_length(stru_record *record);
int get_payload_length(stru_record *record);
unsigned int get_id_length(stru_record *record);

unsigned char *get_type(stru_record *record);
unsigned char *get_payload(stru_record *record);
unsigned char *get_id(stru_record *record);

void set_tnf(stru_record *record, unsigned char tnf);
void set_type(stru_record *record, const unsigned char *type, const unsigned int numBytes);
void set_payload(stru_record *record ,const unsigned char *payload, const int numBytes);
void set_id(stru_record *record ,const unsigned char *id, const unsigned int numBytes);

stru_record *create_record();
void free_record(stru_record *record);
void print_record(stru_record *record);


#endif /* SOURCES_NDEF_RECORD_H_ */
