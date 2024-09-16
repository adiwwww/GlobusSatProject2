/*
 * GOM_EPS.c
 *
 *  Created on: Dec 23, 2023
 *      Author: Muhammad Zahalqa
 */

#include "m_gomeps.h"

#include "Demos/common.h"

#include "config/i2c_address.h"
#include <at91/utility/trace.h>

#include <hal/errors.h>
#include <hal/boolean.h>
#include <satellite-subsystems/GomEPS.h>

Boolean m_gom_eps_init(void)
{
	unsigned char i2c_address = GOM_EPS_ADDRESS;
	int rv = GomEpsInitialize(&i2c_address, 1);
	if (rv != E_NO_SS_ERR && rv != E_IS_INITIALIZED) {
		TRACE_ERROR("\n\r GomEpsInitialize() failed; err=%d! Exiting ... \n\r", rv);
		return FALSE;
	}

	TRACE_INFO("\t\tGOMEPS:  Started.");
	return TRUE;
}

Boolean gom_eps_get_basic_HK_data(gom_eps_hk_basic_t* telemetry)
{
	int rv = GomEpsGetHkData_basic(0, telemetry);
	return rv == E_NO_SS_ERR;
}

Boolean gom_eps_get_HK(EPS_HK_Data* eps_hk)
{
	if (!eps_hk){
		return FALSE;
	}

	gom_eps_hkparam_t hk_params;
	print_error(GomEpsGetHkData_param(0, &hk_params));
	eps_hk->photoVoltaic1 = hk_params.fields.photoVoltaic1;
	eps_hk->photoVoltaic2 = hk_params.fields.photoVoltaic2;
	eps_hk->photoVoltaic3 = hk_params.fields.photoVoltaic3;
	eps_hk->pc = hk_params.fields.pc;
	eps_hk->sc = hk_params.fields.sc;
	eps_hk->bv = hk_params.fields.bv;
	eps_hk->tempBattery = hk_params.fields.tempBattery;
	eps_hk->bootCount = hk_params.fields.bootCount;
	eps_hk->swErrors = hk_params.fields.swErrors;

	return TRUE;
}
