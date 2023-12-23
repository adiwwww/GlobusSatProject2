/*
 * GOM_EPS.h
 *
 *  Created on: Dec 23, 2023
 *      Author: tryfinally
 */

#ifndef MODULES_GOM_EPS_H_
#define MODULES_GOM_EPS_H_

#include "config/i2c_address.h"
#include "GomEPSdemo.h"

#include <hal/errors.h>
#include <hal/boolean.h>

inline Boolean gom_eps_init(void)
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

inline Boolean gom_eps_get_basic_HK_data(gom_eps_hk_basic_t* telemetry)
{
	int rv = GomEpsGetHkData_basic(0, telemetry);
	return rv == E_NO_SS_ERR;
}

#endif /* MODULES_GOM_EPS_H_ */
