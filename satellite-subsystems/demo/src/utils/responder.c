/*
 * responder.c
 *
 *  Created on: Dec 22, 2023
 *      Author: Muhammad Zahalqa
 */

#include "responder.h"

#include <hal/Drivers/I2C.h>
#include "config/i2c_address.h"

#define SET_TRANSMITTER_MODE			0x38	///> Sets the transmitter current running mode
#define NOMINAL_MODE		0x1
#define RESPONDER_MODE	0x2

Boolean trxvu_activate_responder(void)
{
	unsigned char cmd[] = {SET_TRANSMITTER_MODE, NOMINAL_MODE | RESPONDER_MODE};
	return 0 == I2C_write(TRXVU_TC_ADDRESS, cmd, sizeof(cmd));
}


Boolean trxvu_deactivate_responder(void)
{
	unsigned char cmd[] = {SET_TRANSMITTER_MODE, NOMINAL_MODE};
	return 0 == I2C_write(TRXVU_TC_ADDRESS, cmd, sizeof(cmd));
}
