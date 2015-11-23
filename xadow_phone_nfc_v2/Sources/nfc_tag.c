/*
 * nfc_tag.c
 *
 *  Created on: 2015Äê8ÔÂ14ÈÕ
 *      Author: Wayen
 */



#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "ndef_message.h"
#include "nfc_tag.h"


stru_nfcTag nfcTag_buffer;
char tagType_buffer[16] = {0,};


stru_nfcTag *create_nfc_tag_a()
{
	char tagTpe[7] = {'U','n','k','n','o','w','n'};
	stru_nfcTag *nfcTag = &nfcTag_buffer;
	//nfcTag = malloc(sizeof(stru_nfcTag));

	nfcTag->uid = 0;
	nfcTag->uidLength = 0;

	//nfcTag->tagType = malloc(sizeof(tagTpe));
	nfcTag->tagType = tagType_buffer;
	nfcTag->tagType[sizeof(tagTpe)] = '\0';
	memcpy(nfcTag->tagType, tagTpe, sizeof(tagTpe));

	nfcTag->ndefMessage = (stru_message*)NULL;

	return nfcTag;
}

stru_nfcTag *create_nfc_tag_b(unsigned char *uid, unsigned int uidLength)
{
	char tagTpe[7] = {'U','n','k','n','o','w','n'};
	stru_nfcTag *nfcTag = &nfcTag_buffer;
	//nfcTag = malloc(sizeof(stru_nfcTag));

	nfcTag->uid = uid;
	nfcTag->uidLength = uidLength;

	//nfcTag->tagType = malloc(sizeof(tagTpe));
	nfcTag->tagType = tagType_buffer;
	nfcTag->tagType[sizeof(tagTpe)] = '\0';
	memcpy(nfcTag->tagType, tagTpe, sizeof(tagTpe));

	nfcTag->ndefMessage = (stru_message*)NULL;

	return nfcTag;
}

stru_nfcTag *create_nfc_tag_c(unsigned char *uid, unsigned int uidLength, char *tagType)
{
	stru_nfcTag *nfcTag = &nfcTag_buffer;
	//nfcTag = malloc(sizeof(stru_nfcTag));

	nfcTag->uid = uid;
	nfcTag->uidLength = uidLength;

	//nfcTag->tagType = malloc(strlen(tagType));
	nfcTag->tagType = tagType_buffer;
	nfcTag->tagType[strlen(tagType)] = '\0';
	memcpy(nfcTag->tagType, tagType, strlen(tagType));

	nfcTag->ndefMessage = (stru_message*)NULL;

	return nfcTag;
}

stru_nfcTag *create_nfc_tag_d(unsigned char *uid, unsigned int uidLength, char *tagType, stru_message *ndefMessage)
{
	stru_nfcTag *nfcTag = &nfcTag_buffer;
	//nfcTag = malloc(sizeof(stru_nfcTag));

	nfcTag->uid = uid;
	nfcTag->uidLength = uidLength;

	//nfcTag->tagType = malloc(strlen(tagType));
	nfcTag->tagType = tagType_buffer;
	nfcTag->tagType[strlen(tagType)] = '\0';
	memcpy(nfcTag->tagType, tagType, strlen(tagType));

	nfcTag->ndefMessage = ndefMessage;

	return nfcTag;
}

stru_nfcTag *create_nfc_tag_e(unsigned char *uid, unsigned int uidLength, char *tagType, const unsigned char *ndefData, const int ndefDataLength)
{
	stru_nfcTag *nfcTag = &nfcTag_buffer;
	//nfcTag = malloc(sizeof(stru_nfcTag));

	nfcTag->uid = uid;
	nfcTag->uidLength = uidLength;

	//nfcTag->tagType = malloc(strlen(tagType));
	nfcTag->tagType = tagType_buffer;
	nfcTag->tagType[strlen(tagType)] = '\0';
	memcpy(nfcTag->tagType, tagType, strlen(tagType));

	nfcTag->ndefMessage = create_message_b(ndefData, ndefDataLength);

	return nfcTag;
}

void free_nfc_tag(stru_nfcTag *nfcTag)
{
	for(int i=0;i<nfcTag->ndefMessage->recordCount;i++)
	{
		//free(nfcTag->ndefMessage->records[i]->type);
		//free(nfcTag->ndefMessage->records[i]->payload);
		//free(nfcTag->ndefMessage->records[i]->id);
		//free(nfcTag->ndefMessage->records[i]);
	}
	//free(nfcTag->ndefMessage);
	//free(nfcTag->uid);
	//free(nfcTag->tagType);
	//free(nfcTag);
}

void print_nfc_tag(stru_nfcTag *nfcTag)
{

}

unsigned char get_uid_length(stru_nfcTag *nfcTag)
{
	return nfcTag->uidLength;
}

void get_uid(stru_nfcTag *nfcTag, unsigned char *uid, unsigned int uidLength)
{
	memcpy(uid, nfcTag->uid, nfcTag->uidLength < uidLength ? nfcTag->uidLength : uidLength);
}

char *get_uid_string(stru_nfcTag *nfcTag)
{
	return nfcTag->uid;
}

char *get_tag_type(stru_nfcTag *nfcTag)
{
	return nfcTag->tagType;
}

bool has_ndef_message(stru_nfcTag *nfcTag)
{
	return (nfcTag->ndefMessage != NULL);
}

stru_message *get_ndef_message(stru_nfcTag *nfcTag)
{
	return nfcTag->ndefMessage;
}
