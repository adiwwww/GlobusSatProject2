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
	unsigned char cmd[] = {SET_TRANSMITTER_MODE, RESPONDER_MODE};
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

Boolean trxvu_set_ax25_bitrate(ISIStrxvuBitrate bitrate)
{
	int r = IsisTrxvu_tcSetAx25Bitrate(0, bitrate);
	return r == E_NO_SS_ERR;
}

Boolean trxvu_send_message(unsigned char const* data, unsigned short length)
{
	unsigned char availableFrames;
	int r = IsisTrxvu_tcSendAX25DefClSign(0, (unsigned char*)data, length, &availableFrames);
	return r == E_NO_SS_ERR;
}

int trxvu_count_incoming_frames(void)
{
	unsigned short count = 0;
	int r = IsisTrxvu_rcGetFrameCount(0, &count);
	return (r == E_NO_SS_ERR) ? count : 0;
}

TrxvuRxFrame* trxvu_get_frame()
{
	static unsigned char rxframebuffer[M_TRXVU_RX_FRAME_SIZE] = {0};
	static ISIStrxvuRxFrame rxFrameCmd = {0,0,0, rxframebuffer};
	static TrxvuRxFrame rxframe;

	int r = IsisTrxvu_rcGetCommandFrame(0, &rxFrameCmd);
	if (r == E_NO_SS_ERR) {
		rxframe.doppler = ((double)rxFrameCmd.rx_doppler) * 13.352 - 22300.0;
		rxframe.rssi = ((double)rxFrameCmd.rx_rssi) * 0.03 - 152;
		rxframe.length = rxFrameCmd.rx_length;
		rxframe.framedata = rxframebuffer;
		return &rxframe;
	}

	return NULL;
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

static unsigned char from_callsign[] = "TVLKQ1";
static unsigned char to_callsign[] = "KQ-GND";
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

	IsisTrxvu_tcSetDefToClSign(0, to_callsign);
	IsisTrxvu_tcSetDefFromClSign(0, from_callsign);

	// Start watch dog kick task
	xTaskCreate(_WatchDogKickTask,(signed char*)"TRXVU-WDT", 2048, NULL, tskIDLE_PRIORITY, &watchdogKickTaskHandle );

	TRACE_INFO("\t\tTRXVU:   Started.");
	return TRUE;
}


