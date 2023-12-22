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

#define SET_RSSI_THRESHOLD			0x52	///> Set RSSI Transponder Threshold

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

Boolean trxvu_set_responder_rssi_threshold(uint16_t rssi)
{
	unsigned char cmd[] = {SET_RSSI_THRESHOLD, rssi>>8, 0, 0};
	cmd[1] = rssi >> 8;
	cmd[2] = rssi & 0xFF;
	return 0 == I2C_write(TRXVU_TC_ADDRESS, cmd, sizeof(cmd));
}
