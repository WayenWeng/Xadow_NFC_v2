
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "board.h"
#include "i2c.h"
#include "fsl_port_hal.h"
#include "fsl_i2c_slave_driver.h"
#include "nfc_adapter.h"


uint8_t RegScanData[4] = {0, 0, 0, I2C_DEVICE_ADDR};
uint8_t TxBuff[DATA_LENGTH] = {0};
uint8_t RxBuff[DATA_LENGTH] = {0}; // First byte is the buffer of i2c receive data

i2c_slave_state_t slave;
call_param_t callParam;
i2c_slave_user_config_t userConfig;

uint8_t i2c_send_count = 0;
uint8_t i2c_receive_count = 0;
uint8_t i2c_send_length = 0;
uint8_t i2c_receive_length = 0;
uint8_t i2c_comand = 0;
uint8_t i2c_comand_flag = 0;

extern uint8_t sys_busy;
extern stru_message *message;
extern stru_nfcTag *tag;
extern stru_NfcAdapter adapter;
extern uint8_t nfc_cmd;

extern unsigned char sys_gpio_test;


static void i2c_data_copy(uint8_t* strDes, uint8_t* strSrc, uint8_t num)
{
	uint8_t i;
	for(i=0;i<num;i++)strDes[i] = strSrc[i];
}

void i2c_slave_callback(uint8_t instance,i2c_slave_event_t i2cEvent,void *param)
{
    i2c_slave_state_t * slaveState = I2C_DRV_SlaveGetHandler(instance);
    call_param_t *userData = (call_param_t*)param;

    switch(i2cEvent)
    {
        // Transmit request
        case kI2CSlaveTxReq:
            // Update information for transmit process
            slaveState->txSize = userData->txSize;
            slaveState->txBuff = userData->pTxBuff + i2c_send_count++;
            slaveState->isTxBusy = true;

            if(i2c_send_count == i2c_send_length)
			{
				i2c_send_count = 0;
				i2c_comand_flag = 0;
			}
        break;

        // Receive request
        case kI2CSlaveRxReq:
            // Update information for received process
            slaveState->rxSize = userData->rxSize;
            slaveState->rxBuff = userData->pRxBuff;
            slaveState->isRxBusy = true;
        break;

        // Transmit buffer is empty
        case kI2CSlaveTxEmpty:
            slaveState->isTxBusy = false;
        break;

        // Receive buffer is full
        case kI2CSlaveRxFull:
        	if(i2c_comand_flag == 0)
			{
        		i2c_comand_flag = 1;
        		i2c_comand = userData->pRxBuff[0];
        		i2c_receive_count = 1;

        		if(i2c_comand == GPIO_TEST_CMD)sys_gpio_test = 1;

        		if(i2c_comand > NFC_MAX_ID)
        		{
        			i2c_comand = 0;
        			i2c_comand_flag = 0;
        			i2c_receive_count = 0;
        		}
			}
        	else
        	{
        		if(i2c_receive_count == 1)
        		{
        			i2c_receive_length = userData->pRxBuff[0];
        			i2c_receive_count = 2;

        			switch(i2c_comand)
					{
						case NFC_SCAN_ID:
							nfc_cmd = NFC_SCAN_ID;

							TxBuff[0] = NFC_SCAN_ID;
							TxBuff[1] = 4;
							i2c_data_copy(TxBuff + 2, RegScanData, TxBuff[1]);

							i2c_send_count = 0;
							i2c_send_length = TxBuff[1] + 2;
						break;

						case NFC_ADAPTER_INIT_ID:
							nfc_cmd = NFC_ADAPTER_INIT_ID;
							i2c_comand_flag = 0;
						break;

						case NFC_ADAPTER_GET_UID_ID:
							nfc_cmd = NFC_ADAPTER_GET_UID_ID;

							TxBuff[0] = NFC_ADAPTER_GET_UID_ID;
							TxBuff[1] = adapter.uidLength;
							i2c_data_copy(TxBuff + 2, adapter.uid, TxBuff[1]);

							i2c_send_count = 0;
							i2c_send_length = TxBuff[1] + 2;
						break;

						case NFC_ADAPTER_READ_ID:
							nfc_cmd = NFC_ADAPTER_READ_ID;

							TxBuff[0] = NFC_ADAPTER_READ_ID;

							if(adapter.uidLength == 4 || adapter.uidLength == 7)
							{
								if(tag->uidLength == 4 || tag->uidLength == 7)
								{
									if(tag->ndefMessage->recordCount > 0)
									{
										if(tag->ndefMessage->records[0]->payloadLength >= 3)
										{
											TxBuff[1] = tag->ndefMessage->records[0]->payloadLength - 3;

											if(TxBuff[1] >= DATA_LENGTH)TxBuff[1] = DATA_LENGTH;

											i2c_data_copy(TxBuff + 2, tag->ndefMessage->records[0]->payload + 3, TxBuff[1]);
										}
										else{TxBuff[1] = 0;}
									}
									else{TxBuff[1] = 0;}
								}
								else{TxBuff[1] = 0;}
							}
							else{TxBuff[1] = 0;}

							i2c_send_count = 0;
							i2c_send_length = TxBuff[1] + 2;
						break;

						case NFC_ADAPTER_ERASE_ID:
							nfc_cmd = NFC_ADAPTER_ERASE_ID;
							i2c_comand_flag = 0;
						break;

						default:
						break;
					}
        		}
        		else
        		{
        			RxBuff[i2c_receive_count] = userData->pRxBuff[0];
        			i2c_receive_count ++;
        			if(i2c_receive_count >= (i2c_receive_length + 2))
        			{
						i2c_comand_flag = 0;
        				i2c_receive_count = 0;
        				RxBuff[i2c_receive_length + 2] = '\0';

        				switch(i2c_comand)
        				{
							case NFC_ADAPTER_WRITE_ID:
								nfc_cmd = NFC_ADAPTER_WRITE_ID;
							break;

							case NFC_EMULATE_INIT_ID:
								nfc_cmd = NFC_EMULATE_INIT_ID;
							break;

							default:
							break;
        				}
        			}
        		}
        	}
            slaveState->isRxBusy = false;
        break;

        default:
        break;
    }
}

void i2c_init(void)
{
	callParam.pTxBuff = TxBuff;
	callParam.pRxBuff = RxBuff;
	callParam.txSize = 1;
	callParam.rxSize = 1;

	userConfig.address        = I2C_DEVICE_ADDR;
	userConfig.slaveCallback  = i2c_slave_callback;
	userConfig.callbackParam  = &callParam;
	userConfig.slaveListening = true;
	userConfig.startStopDetect = true;

	PORT_HAL_SetMuxMode(PORTA,3u,kPortMuxAlt3);
	PORT_HAL_SetMuxMode(PORTA,4u,kPortMuxAlt3);

    I2C_DRV_SlaveInit(1, &userConfig, &slave);
}
