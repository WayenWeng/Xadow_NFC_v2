/*
 * pn532_i2c.c
 *
 *  Created on: 2015Äê8ÔÂ10ÈÕ
 *      Author: Wayen
 */

#include <stdint.h>
#include <string.h>
#include "fsl_port_hal.h"
#include "fsl_i2c_master_driver.h"
#include "delay.h"
#include "pn532_i2c.h"


i2c_master_state_t master;
i2c_device_t device =
{
  .address = PN532_I2C_ADDRESS,
  .baudRate_kbps = 400  // 400K Hz
};

uint8_t Buffer[I2C_BUFFER_MAX] = {0};
uint8_t command;


void pn532_i2c_init(void)
{
	PORT_HAL_SetMuxMode(PORTB,3u,kPortMuxAlt2);
	PORT_HAL_SetMuxMode(PORTB,4u,kPortMuxAlt2);
    I2C_DRV_MasterInit(0, &master);
}

void wake_up()
{
	uint8_t address = PN532_I2C_ADDRESS;
	i2c_status_t i2c_status;

	i2c_status = I2C_DRV_MasterSendData(0, &device, 0, 0, &address, 1);
	delay_ms(20);
}

int8_t readAckFrame()
{
	const uint8_t PN532_ACK[] = {0, 0, 0xFF, 0, 0xFF, 0};
	uint8_t ackBuf[7];
	i2c_status_t i2c_status;
    uint16_t time = 0;

    while(1)
    {
    	i2c_status = I2C_DRV_MasterReceiveData(0, &device, 0, 0, ackBuf, 7);
        delay_ms(1);

        if(ackBuf[0] & 1) // check first byte --- status
        {
            break; // PN532 is ready
        }
        time++;
        if (time > PN532_ACK_WAIT_TIME) // Time out when waiting for ACK
        {
            return PN532_TIMEOUT;
        }
    }

	if(memcmp(ackBuf + 1, PN532_ACK, sizeof(PN532_ACK)))
	{
		return PN532_INVALID_ACK;
	}

	return 0;
}

int8_t writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen)
{
	uint8_t i = 0;
	i2c_status_t i2c_status;

	command = header[0];

	memset(Buffer, 0 , sizeof(Buffer));

	Buffer[i++] = PN532_PREAMBLE;
	Buffer[i++] = PN532_STARTCODE1;
	Buffer[i++] = PN532_STARTCODE2;

	uint8_t length = hlen + blen + 1; // length of data field: TFI + DATA
	Buffer[i++] = length;
	Buffer[i++] = ~length + 1; // checksum of length
	Buffer[i++] = PN532_HOSTTOPN532;

    uint8_t sum = PN532_HOSTTOPN532; // sum of TFI + DATA
    for(uint8_t j = 0; j < hlen; j++)
    {
		Buffer[i++] = header[j];
		sum += header[j];
    }
    for(uint8_t j = 0; j < blen; j++)
    {
    	Buffer[i++] = header[j];
    	sum += header[j];
    }

    uint8_t checksum = ~sum + 1; // checksum of TFI + DATA
    Buffer[i++] = checksum;
    Buffer[i++] = PN532_POSTAMBLE;

    i2c_status = I2C_DRV_MasterSendData(0, &device, 0, 0, Buffer, i);
    delay_ms(3);

    return readAckFrame();
}

int8_t readResponse(uint8_t buf[], uint8_t len, uint16_t timeout)
{
    uint8_t i = 0;
	uint16_t time = 0;
	i2c_status_t i2c_status;

	memset(Buffer, 0, sizeof(Buffer));

    while(1)
    {
    	i2c_status = I2C_DRV_MasterReceiveData(0, &device, 0, 0, Buffer, len + 2);
    	delay_ms(3);
        if(Buffer[0] & 1) // check first byte --- status
		{
			break; // PN532 is ready
		}
        time++;
        if((0 != timeout) && (time > timeout))
        {
            return PN532_TIMEOUT;
        }
    }

    i = 1;
    if(0x00 != Buffer[i++]) // PREAMBLE
    {
        return PN532_INVALID_FRAME;
    }
    if(0x00 != Buffer[i++]) // STARTCODE1
	{
		return PN532_INVALID_FRAME;
	}
    if(0xFF != Buffer[i++]) // STARTCODE2
	{
		return PN532_INVALID_FRAME;
	}

    uint8_t length = Buffer[i++];
    if(0 != (uint8_t)(length + Buffer[i++])) // checksum of length
    {
        return PN532_INVALID_FRAME;
    }

    uint8_t cmd = command + 1; // response command
    if (PN532_PN532TOHOST != Buffer[i++] || (cmd) != Buffer[i++])
    {
        return PN532_INVALID_FRAME;
    }

    length -= 2;
    if(length > len)
    {
        return PN532_NO_SPACE; // not enough space
    }

    uint8_t sum = PN532_PN532TOHOST + cmd;
    for(uint8_t j = 0; j < length; j++)
    {
        buf[j] = Buffer[i++];
        sum += buf[j];
    }

    uint8_t checksum = Buffer[i++];
    if (0 != (uint8_t)(sum + checksum))
    {
        return PN532_INVALID_FRAME;
    }

    //Buffer[i]; // POSTAMBLE

    return length;
}





