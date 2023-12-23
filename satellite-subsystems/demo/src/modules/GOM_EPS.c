/*
 * GOM_EPS.c
 *
 *  Created on: Dec 23, 2023
 *      Author: tryfinally
 */

#include "GOM_EPS.h"
#include "config/i2c_address.h"

#include <at91/utility/trace.h>

#include <hal/errors.h>
#include <hal/boolean.h>
#include <satellite-subsystems/GomEPS.h>

Boolean gom_eps_init(void)
{
	unsigned char i2c_address = GOM_EPS_ADDRESS;
	int rv = GomEpsInitialize(&i2c_address, 1);
	if (rv != E_NO_SS_ERR && rv != E_IS_INITIALIZED)
	{
		TRACE_ERROR("\n\r GomEpsInitialize() failed; err=%d! Exiting ... \n\r", rv);
		return FALSE;
	}

	return TRUE;
}

Boolean gom_eps_get_basic_HK_data(gom_eps_hk_basic_t* telemetry)
{
	int rv = GomEpsGetHkData_basic(0, telemetry);
	return rv == E_NO_SS_ERR;
}

