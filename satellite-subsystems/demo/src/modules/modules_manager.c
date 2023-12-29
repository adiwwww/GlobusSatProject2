/*
 * modules_manager.c
 *
 *  Created on: Dec 28, 2023
 *      Author: Muhammad Zahalqa
 */

#include "modules_manager.h"

#include "m_trxvu.h"
#include "m_gomeps.h"

#include <hal/Drivers/I2C.h>
#include <at91/utility/trace.h>


#define I2C_BUS_SPEED_HZ		100000
#define I2C_TIMEOUT				10

static Boolean init_i2c(void)
{
	int retValInt = I2C_start(I2C_BUS_SPEED_HZ, I2C_TIMEOUT);
	if(retValInt != 0) {
		TRACE_FATAL("\n\r I2C_start_Master for demo: %d! \n\r", retValInt);
		return FALSE;
	}
	TRACE_INFO("\t\t\tI2C:     Started.");
	return TRUE;
}

Boolean mm_init()
{
	Boolean result = TRUE;

	result &= init_i2c();
	result &= m_trxvu_init();
	result &= m_gom_eps_init();

	return result;
}
