/*
 * pn532_i2c.h
 *
 *  Created on: 2015Äê8ÔÂ10ÈÕ
 *      Author: Wayen
 */

#ifndef SOURCES_PN532_I2C_H_
#define SOURCES_PN532_I2C_H_


#define PN532_I2C_ADDRESS       (0x48 >> 1)

#define PN532_PREAMBLE                (0x00)
#define PN532_STARTCODE1              (0x00)
#define PN532_STARTCODE2              (0xFF)
#define PN532_POSTAMBLE               (0x00)

#define PN532_HOSTTOPN532             (0xD4)
#define PN532_PN532TOHOST             (0xD5)

#define PN532_ACK_WAIT_TIME           (10)  // ms, timeout of waiting for ACK

#define PN532_INVALID_ACK             (-1)
#define PN532_TIMEOUT                 (-2)
#define PN532_INVALID_FRAME           (-3)
#define PN532_NO_SPACE                (-4)

#define I2C_BUFFER_MAX	(128 + 16)


void pn532_i2c_init(void);
void wake_up();
int8_t readAckFrame();
int8_t writeCommand(const uint8_t *header, uint8_t hlen, const uint8_t *body, uint8_t blen);
int8_t readResponse(uint8_t buf[], uint8_t len, uint16_t timeout);


#endif /* SOURCES_PN532_I2C_H_ */
