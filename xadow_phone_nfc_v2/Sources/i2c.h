
#ifndef SOURCES_I2C_H_
#define SOURCES_I2C_H_


#define I2C_DEVICE_ADDR    0x06
#define DATA_LENGTH    	   128

/*
	Data format : ID, Length (n bytes, form 1 to n), Data1, Data2, ... ,Data n
*/
#define NFC_SCAN_ID					0 //

#define NFC_ADAPTER_INIT_ID			1 //
#define NFC_ADAPTER_GET_UID_ID		2 //
#define NFC_ADAPTER_READ_ID			3 //
#define NFC_ADAPTER_WRITE_ID		4 //
#define NFC_ADAPTER_ERASE_ID		5 //

#define NFC_EMULATE_INIT_ID			6 //

#define NFC_MAX_ID					6 //

#define GPIO_TEST_CMD		0x54 // 'T'



typedef struct
{
    uint8_t *pTxBuff;   // pointer points to transmit buffer
    uint32_t txSize;    // Size of transmit buffer
    uint8_t *pRxBuff;   // pointer points to receive buffer
    uint32_t rxSize;    // Size of receive buffer
} call_param_t;


void i2c_init(void);


#endif /* SOURCES_I2C_H_ */
