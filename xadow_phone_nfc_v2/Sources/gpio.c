
#include "gpio.h"
#include "fsl_gpio_hal.h"


void gpio_init(void)
{
	GPIO_HAL_SetPinDir(PTA, 6, kGpioDigitalOutput); // INT1 pin
	GPIO_HAL_SetPinDir(PTA, 7, kGpioDigitalInput); // INT2 pin
	GPIO_HAL_SetPinDir(PTB, 1, kGpioDigitalOutput); // RSTPD_N pin
	GPIO_HAL_SetPinDir(PTB, 2, kGpioDigitalInput); // RSTOUT_N pin
	GPIO_HAL_SetPinDir(PTB, 5, kGpioDigitalInput); // IRQ pin

	GPIO_HAL_SetPinOutput(PTA, 6); // INT1 set high
	GPIO_HAL_SetPinOutput(PTB, 1); // RSTPD_N pin set high
}



