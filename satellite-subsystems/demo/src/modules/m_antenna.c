/*
 * m_antenna.c
 *
 *  Created on: Jan 9, 2024
 *      Author: tryfinally
 */

#include "m_antenna.h"
#include "config/i2c_address.h"

#include <hal/errors.h>
#include <at91/utility/trace.h>

//#include <satellite-subsystems/isis_ants2.h>
//#include <satellite-subsystems/isis_ants2_types.h>


Boolean m_antenna_init(void)
{
	ISISantsI2Caddress i2c_address = {
			.addressSideA = ANTENNA_A_ADDRESS,
			.addressSideB = ANTENNA_B_ADDRESS
	};

	int rv = IsisAntS_initialize(&i2c_address, 1);
	if(rv != E_NO_SS_ERR && rv != E_IS_INITIALIZED) {
		TRACE_ERROR("\n\r IsisAntS_initialize() failed; err=%d! Exiting ... \n\r", rv);
		return FALSE;
	}

	TRACE_INFO("\t\tANTNS:   Started.");
	return TRUE;
}

float antenna_get_temperature(ISISantsSide side)
{
	unsigned short value = 0;
	int rv = IsisAntS_getTemperature(0, side, &value);
	if (rv != 0) {
		TRACE_ERROR("\n\r IsisAntS_getTemperature() failed. returned: %d\n\r", rv);
		return -270;
	}

	float tempC = (value * -0.2922) + 190.65;
	return tempC;
}

unsigned int antenna_get_uptime(ISISantsSide side)
{
	unsigned int value;
	int rv = IsisAntS_getUptime(0, side, &value);
	if (rv != 0) {
		TRACE_ERROR("IsisAntS_getUptime() failed. returned: %d\n\r", rv);
		return 0;
	}

	return value;
}

Boolean antenna_reset(void)
{
	int rv = IsisAntS_reset(0, isisants_sideA);
	if (rv != 0) {
		TRACE_ERROR("IsisAntS_reset(A) failed. returned: %d\n\r", rv);
		return FALSE;
	}

	rv = IsisAntS_reset(0, isisants_sideB);
	if (rv != 0) {
		TRACE_ERROR("IsisAntS_reset(B) failed. returned: %d\n\r", rv);
		return FALSE;
	}

	return TRUE;
}
