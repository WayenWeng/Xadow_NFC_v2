/* ###################################################################
**     Filename    : main.c
**     Project     : xadow_phone_nfc_v2
**     Processor   : MKL02Z32VFM4
**     Version     : Driver 01.01
**     Compiler    : Keil ARM C/C++ Compiler
**     Date/Time   : 2015-08-10, 10:27, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/
/*!
**  @addtogroup main_module main module documentation
**  @{
*/
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "clockMan1.h"
#include "pin_mux.h"
#include "osa1.h"
#include "delay.h"
#include "gpio.h"
#include "i2c.h"
#include "pn532.h"
#include "nfc_adapter.h"
#include "emulate_tag.h"


#define NFC_IDLE		0
#define NFC_ADAPTER		1
#define NFC_EMULATE		2


uint8_t nfc_status;
stru_message *message;
stru_nfcTag *tag;
stru_NfcAdapter adapter;

uint8_t nfc_cmd = 0;

unsigned char uid[3] = { 0x11, 0x22, 0x33 };

extern uint8_t RxBuff[DATA_LENGTH];
extern uint8_t i2c_comand_flag;

uint8_t status;

unsigned char sys_gpio_test = 0;


void GPIO_Test_for_TE(void);


void main(void)
{
	PE_low_level_init();
	delay_init();
	gpio_init();
	i2c_init();
	pn532_init();
	/*
	adapter_init();
	while(1)
	{
		if(tag_present(&adapter, 30))
		tag = adapter_read(&adapter);
		delay_ms(100);
	}
	*/
	while(1)
	{
		if(nfc_cmd == NFC_ADAPTER_INIT_ID)
		{
			nfc_cmd = 0;
			if(nfc_status != NFC_ADAPTER)
			{
				GPIO_HAL_ClearPinOutput(PTB, 1); // RSTPD_N pin set low
				delay_ms(100);
				GPIO_HAL_SetPinOutput(PTB, 1); // RSTPD_N pin set high
				delay_ms(100);

				if(adapter_init())
				{
					nfc_status = NFC_ADAPTER;
				}
				else
				{
					nfc_status = NFC_IDLE;
					nfc_cmd = NFC_ADAPTER_INIT_ID;
				}
			}
		}

		else if(nfc_cmd == NFC_EMULATE_INIT_ID)
		{
			nfc_cmd = 0;
			if(nfc_status != NFC_EMULATE)
			{
				GPIO_HAL_ClearPinOutput(PTB, 1); // RSTPD_N pin set low
				delay_ms(100);
				GPIO_HAL_SetPinOutput(PTB, 1); // RSTPD_N pin set high
				delay_ms(100);

				uid[0] = RxBuff[2];
				uid[1] = RxBuff[3];
				uid[2] = RxBuff[4];
				set_uid(uid);

				if(emulate_tag_init())
				{
					//emulate_tag(30);
					nfc_status = NFC_EMULATE;
					adapter.uidLength = 0;
				}
				else
				{
					nfc_status = NFC_IDLE;
					nfc_cmd = NFC_EMULATE_INIT_ID;
				}
			}
		}

		if(nfc_status == NFC_ADAPTER)
		{
			switch(nfc_cmd)
			{
				case NFC_ADAPTER_GET_UID_ID:
					nfc_cmd = 0;
					tag_present(&adapter, 30);
				break;

				case NFC_ADAPTER_READ_ID:
					nfc_cmd = 0;
					if(tag_present(&adapter, 30))
					{
						//delay_ms(100);
						if(tag_present(&adapter, 30))
						{
							if(i2c_comand_flag == 0)
							{
								tag = adapter_read(&adapter);
								free_nfc_tag(tag);
							}
						}
					}

				break;

				case NFC_ADAPTER_WRITE_ID:
					nfc_cmd = 0;
					if(tag_present(&adapter, 30))
					{
						//delay_ms(100);
						if(tag_present(&adapter, 30))
						{
							message = create_message();
							add_text_record(message, &RxBuff[2]);
							status = adapter_write(&adapter, message);
							free_message(message);
						}
					}

				break;

				case NFC_ADAPTER_ERASE_ID:
					nfc_cmd = 0;
					if(tag_present(&adapter, 30))
					{
						//delay_ms(100);
						if(tag_present(&adapter, 30))
						{
							adapter_erase(&adapter);
						}
					}
				break;

				default:
				break;
			}
		}

		GPIO_Test_for_TE();

		delay_ms(100);
	}
}

void GPIO_Test_for_TE(void)
{
	if(sys_gpio_test)
	{
		sys_gpio_test = 0;

		GPIO_HAL_SetPinDir(PTA, 6, kGpioDigitalOutput); // INT1 pin
		GPIO_HAL_SetPinDir(PTA, 7, kGpioDigitalInput); // INT2 pin

		PORT_HAL_SetMuxMode(PORTA,3u,kPortMuxAsGpio);
		PORT_HAL_SetMuxMode(PORTA,4u,kPortMuxAsGpio);
		GPIO_HAL_SetPinDir(PTA, 3, kGpioDigitalOutput); // SCL pin
		GPIO_HAL_SetPinDir(PTA, 4, kGpioDigitalOutput); // SDA pin

		while(1)
		{
			if(GPIO_HAL_ReadPinInput(PTA, 7)) // Read the INT2
			{
				GPIO_HAL_ClearPinOutput(PTA, 6); // INT1 set low
				GPIO_HAL_SetPinOutput(PTA, 4); // SCL set high
				GPIO_HAL_ClearPinOutput(PTA, 3); // SDA set low
			}
			else
			{
				GPIO_HAL_SetPinOutput(PTA, 6); // INT1 set high
				GPIO_HAL_ClearPinOutput(PTA, 4); // SCL set low
				GPIO_HAL_SetPinOutput(PTA, 3); // SDA set high
			}

			delay_ms(100);
		}
	}
}
