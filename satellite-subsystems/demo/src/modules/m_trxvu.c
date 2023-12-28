/*
 * m_trxvu.c
 *
 *  Created on: Dec 28, 2023
 *      Author: Muhammad Zahalqa
 */

#include "m_trxvu.h"

#include "config/i2c_address.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

//#include <at91/utility/exithandler.h>
//#include <at91/commons.h>
#include <at91/utility/trace.h>
//#include <at91/peripherals/cp15/cp15.h>

#include <hal/Timing/WatchDogTimer.h>
#include <hal/Drivers/I2C.h>
#include <hal/boolean.h>
#include <hal/errors.h>

#include <satellite-subsystems/IsisTRXVU.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


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

	return TRUE;
}


