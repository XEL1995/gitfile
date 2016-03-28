/*
 *  Copyright (C) 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

/*
 * This is a board specific configuration file for
 * the RD-88MW300 RD and AB-88MW300 baseboard
 * based on schematic dated 19th June 2014.
 *
 * Details:
 * By default the board file configures the CPU frequncy to 200MHz
 * The dev board has:
 * MAINXTAL 38.4MHz
 * 32k XTAL
 * Four push buttons being used as wakeup0(GPIO22) and wakeup1(GPIO23)
 * PUSH_SW0(GPIO26)
 */

#include <wmtypes.h>
#include <wmerrno.h>
#include <wm_os.h>
#include <board.h>
#include <lowlevel_drivers.h>

int board_console_uart_id(void)
{
	return UART0_ID;
}

int board_main_xtal()
{
	/* MAINXTAL: 38.4MHZ */
	return 38400000;
}

int board_main_osc()
{
	return -WM_FAIL;
}

int board_cpu_freq()
{
	return 200000000;
}

int board_32k_xtal()
{
	return false;
}

int board_32k_osc()
{
       //Modified by James 20150407
	return false;
}

int board_rc32k_calib()
{
	return false; 
}

void board_sdio_pdn()
{
/*
 * This cannot be done so easily
 * as MCU XTAL clock is tied
 * to WLAN
 * PMU_PowerDownWLAN();
 */
}

void board_sdio_pwr()
{
/*	PMU_PowerOnWLAN();*/
}

void board_sdio_reset()
{
	board_sdio_pdn();
	_os_delay(20);
	board_sdio_pwr();
	_os_delay(20);
}

int board_sdio_pdn_support()
{
	return true;
}

int board_button_pressed(int pin)
{
	if (pin < 0)
		return false;

	GPIO_SetPinDir(pin, GPIO_INPUT);
	if (GPIO_ReadPinLevel(pin) == GPIO_IO_LOW)
		return true;

	return false;
}

void board_gpio_power_on()
{
	/* RF_CTRL pins
	Commented by James 20150407 as Changhong only needs single antenna, 
	diversity is not needed,  */
	//GPIO_PinMuxFun(GPIO_44, PINMUX_FUNCTION_7);
	//GPIO_PinMuxFun(GPIO_45, PINMUX_FUNCTION_7);
	/* Wakeup push buttons are active low */
	PMU_ConfigWakeupPin(PMU_GPIO22_INT, PMU_WAKEUP_LEVEL_LOW);
	PMU_ConfigWakeupPin(PMU_GPIO23_INT, PMU_WAKEUP_LEVEL_LOW);
}

void board_uart_pin_config(int id)
{
	switch (id) {
	case UART0_ID:
		GPIO_PinMuxFun(GPIO_2, GPIO2_UART0_TXD);
		GPIO_PinMuxFun(GPIO_3, GPIO3_UART0_RXD);
		break;
	case UART1_ID:
		//3¡èo?UART
		GPIO_PinMuxFun(GPIO_44, GPIO44_UART1_TXD);
		GPIO_PinMuxFun(GPIO_45, GPIO45_UART1_RXD);

//		GPIO_PinMuxFun(GPIO_13, GPIO13_UART1_TXD);
//		GPIO_PinMuxFun(GPIO_14, GPIO14_UART1_RXD);
                break;
	case UART2_ID:
		/* Not implemented yet */
		GPIO_PinMuxFun(GPIO_48, GPIO48_UART2_TXD);
                GPIO_PinMuxFun(GPIO_49, GPIO49_UART2_RXD);
		break;
	}
}

bool board_supports_5GHz(void)
{
	return false;
}

void board_i2c_pin_config(int id)
{
	switch (id) {
	case I2C0_PORT:
		GPIO_PinMuxFun(GPIO_4, GPIO4_I2C0_SDA);
		GPIO_PinMuxFun(GPIO_5, GPIO5_I2C0_SCL);
		break;
	case I2C1_PORT:
	//ALex
		GPIO_PinMuxFun(GPIO_26, GPIO26_I2C1_SCL);
		GPIO_PinMuxFun(GPIO_25, GPIO25_I2C1_SDA);
		break;
	}
}

void board_usb_pin_config()
{
	//not needed for mw300, James 20150407
}

void board_ssp_pin_config(int id, bool cs)
{

//commented by James Yang, SSP is not needed for changhong
#if 0 
	/* To do */
	switch (id) {
	case SSP0_ID:
		GPIO_PinMuxFun(GPIO_0, GPIO0_SSP0_CLK);
		if (cs)
			GPIO_PinMuxFun(GPIO_1, GPIO1_SSP0_FRM);
		GPIO_PinMuxFun(GPIO_2, GPIO2_SSP0_TXD);
		GPIO_PinMuxFun(GPIO_3, GPIO3_SSP0_RXD);
		break;
	case SSP1_ID:
		GPIO_PinMuxFun(GPIO_11, GPIO11_SSP1_CLK);
		if (cs)
			GPIO_PinMuxFun(GPIO_12, GPIO12_SSP1_FRM);
		GPIO_PinMuxFun(GPIO_13, GPIO13_SSP1_TXD);
		GPIO_PinMuxFun(GPIO_14, GPIO14_SSP1_RXD);
		break;
	case SSP2_ID:
		break;
	}
#endif
}

output_gpio_cfg_t board_led_1()
{
	output_gpio_cfg_t gcfg = {
		.gpio = GPIO_40,
		.type = GPIO_ACTIVE_LOW,
	};

	return gcfg;
}

output_gpio_cfg_t board_led_2()
{
	output_gpio_cfg_t gcfg = {
		.gpio = GPIO_41,
		.type = GPIO_ACTIVE_LOW,
	};

	return gcfg;
}

output_gpio_cfg_t board_led_3()
{
	output_gpio_cfg_t gcfg = {
		.gpio = -1,
	};

	return gcfg;
}

output_gpio_cfg_t board_led_4()
{
	output_gpio_cfg_t gcfg = {
		.gpio = -1,
	};

	return gcfg;
}

void board_led_on(int pin)
{
	if (pin < 0)
		return;

	/* Set the direction of GPIO pin
	 * Note : All GPIOs are in INPUT by default
	 */
	//GPIO_SetPinDir(pin, GPIO_OUTPUT);
	//ALex
	//GPIO_WritePinOutput(pin, GPIO_IO_LOW);
	GPIO_WritePinOutput(pin, GPIO_IO_HIGH);

}

void board_led_off(int pin)
{
	if (pin < 0)
		return;

	//GPIO_SetPinDir(pin, GPIO_OUTPUT);
	//ALex
	//GPIO_WritePinOutput(pin, GPIO_IO_HIGH);
	GPIO_WritePinOutput(pin, GPIO_IO_LOW);
}

int board_button_1()
{
	GPIO_PinMuxFun(GPIO_0, GPIO0_GPIO0);
	GPIO_PinMuxFun(GPIO_1, GPIO1_GPIO1);
	GPIO_SetPinDir(GPIO_0, GPIO_OUTPUT);
	GPIO_SetPinDir(GPIO_1, GPIO_OUTPUT);


	GPIO_PinMuxFun(GPIO_42, GPIO42_GPIO42);
	GPIO_PinMuxFun(GPIO_43, GPIO43_GPIO43);
	GPIO_PinMuxFun(GPIO_46, GPIO46_GPIO46);
	GPIO_PinMuxFun(GPIO_47, GPIO47_GPIO47);

	GPIO_SetPinDir(GPIO_42, GPIO_OUTPUT);
	GPIO_SetPinDir(GPIO_43, GPIO_OUTPUT);
	GPIO_SetPinDir(GPIO_46, GPIO_OUTPUT);
	GPIO_SetPinDir(GPIO_47, GPIO_OUTPUT);
	return 0;
}

int board_button_2()
{
	GPIO_SetPinDir(GPIO_18, GPIO_OUTPUT);
	//GPIO_PinMuxFun(GPIO_24, GPIO24_GPIO24);
	return 0;
}

int board_button_3()
{
	GPIO_SetPinDir(GPIO_18, GPIO_INPUT);
	return 0;
}

int board_wifi_host_wakeup()
{
	return 16;
}

int board_wakeup0_functional()
{
	return true;
}

int board_wakeup1_functional()
{
	return true;
}
int led_on_off()
{
	GPIO_PinMuxFun(GPIO_0, GPIO0_GPIO0);
	GPIO_SetPinDir(GPIO_0, GPIO_OUTPUT);

	GPIO_PinMuxFun(GPIO_1, GPIO1_GPIO1);
	GPIO_SetPinDir(GPIO_1, GPIO_OUTPUT);

	GPIO_PinMuxFun(GPIO_16, GPIO16_GPIO16);
	GPIO_SetPinDir(GPIO_16, GPIO_OUTPUT);

	GPIO_PinMuxFun(GPIO_40, GPIO40_GPIO40);
	GPIO_SetPinDir(GPIO_40, GPIO_OUTPUT);

//	GPIO_PinMuxFun(GPIO_48, GPIO48_GPIO48);
//	GPIO_SetPinDir(GPIO_48, GPIO_OUTPUT);
	return true;
}
