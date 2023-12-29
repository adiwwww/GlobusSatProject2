/*
 * m_trxvu.c
 *
 *  Created on: Dec 28, 2023
 *      Author: Muhammad Zahalqa
 */

#include "m_trxvu.h"

#include "config/i2c_address.h"

#include <satellite-subsystems/IsisTRXVU.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <at91/utility/trace.h>

#include <hal/Timing/WatchDogTimer.h>
#include <hal/Drivers/I2C.h>
#include <hal/boolean.h>
#include <hal/errors.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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


static xTaskHandle watchdogKickTaskHandle = NULL;

static void _WatchDogKickTask(void *parameters)
{
	(void) parameters;
	// Kick radio I2C watch dog by requesting uptime every 10 seconds
	portTickType xLastWakeTime = xTaskGetTickCount();
	for(;;) {
		unsigned int uptime;
		(void)IsisTrxvu_tcGetUptime(0, &uptime);
		vTaskDelayUntil(&xLastWakeTime, 10000);
	}
}

Boolean m_trxvu_init(void)
{
    // Definition of I2C and TRXUV
	ISIStrxvuI2CAddress myTRXVUAddress[1];
	ISIStrxvuFrameLengths myTRXVUBuffers[1];
	ISIStrxvuBitrate myTRXVUBitrates[1];

	//I2C addresses defined
	myTRXVUAddress[0].addressVu_rc = TRXVU_RC_ADDRESS;
	myTRXVUAddress[0].addressVu_tc = TRXVU_TC_ADDRESS;

	//Buffer definition
	myTRXVUBuffers[0].maxAX25frameLengthTX = M_TRXVU_TX_FRAME_SIZE;
	myTRXVUBuffers[0].maxAX25frameLengthRX = M_TRXVU_RX_FRAME_SIZE;

	//Bitrate definition
	myTRXVUBitrates[0] = trxvu_bitrate_1200;

	//Initialize the trxvu subsystem
	int rv = IsisTrxvu_initialize(myTRXVUAddress, myTRXVUBuffers, myTRXVUBitrates, 1);
	if(rv != E_NO_SS_ERR && rv != E_IS_INITIALIZED) {
		TRACE_ERROR("\n\r IsisTrxvu_initialize() failed; err=%d! Exiting ... \n\r", rv);
		return FALSE;
	}

	// Start watch dog kick task
	xTaskCreate(_WatchDogKickTask,(signed char*)"TRXVU-WDT", 2048, NULL, tskIDLE_PRIORITY, &watchdogKickTaskHandle );

	TRACE_INFO("\t\tTRXVU:   Started.");
	return TRUE;
}


