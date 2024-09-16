/*
 * m_solar_panels.c
 *
 *  Created on: Jan 9, 2024
 *      Author: Muhammad Zahalqa
 */

#include "m_solar_panels.h"
#include "utils/timeutils.h"

#include "config/i2c_address.h"
#include <at91/utility/trace.h>

#include <hal/Drivers/SPI.h>
#include <hal/errors.h>

Boolean m_solar_panels_init(void)
{
	int rv = IsisSolarPanelv2_initialize(slave0_spi);
	if (rv != ISIS_SOLAR_PANEL_ERR_NONE) {
		TRACE_ERROR("\n\r IsisSolarPanelv2_initialize() failed. returned: %d\n\r", rv);
		return FALSE;
	}

	rv = IsisSolarPanelv2_sleep();
	if (rv != 0) {
		TRACE_ERROR("\n\r IsisSolarPanelv2_sleep failed() failed. returned: %d\n\r", rv);
	}

	return TRUE;
}

Boolean solar_panels_is_awake(void)
{
	return IsisSolarPanelv2_getState() == ISIS_SOLAR_PANEL_STATE_AWAKE;
}

Boolean solar_panels_wakeup(void)
{
	int rv = IsisSolarPanelv2_wakeup();
	if (rv != 0) {
		TRACE_ERROR("\n\r IsisSolarPanelv2_wakeup failed() failed. returned: %d\n\r", rv);
		return FALSE;
	}

	for (int i = 0; i < 8; ++i) {
		if (solar_panels_is_awake()) {
			return TRUE;
		}
		delay_ms(100);
	}

	TRACE_ERROR("\n\r solar_panels_wake() failed.\r\n");
	return FALSE;
}

Boolean solar_panels_sleep(void)
{
	int rv = IsisSolarPanelv2_sleep();
	if (rv != 0) {
		TRACE_ERROR("\n\r IsisSolarPanelv2_sleep() failed() failed. returned: %d\n\r", rv);
		return FALSE;
	}

	delay_ms(1);
	return TRUE;
}

float* solar_panels_get_temperature(void)
{
	static float panel_temperatures[ISIS_SOLAR_PANEL_COUNT];

	solar_panels_wakeup();
	for (int i = 0; i < ISIS_SOLAR_PANEL_COUNT; ++i) {
		int32_t value;
		uint8_t status;
		int rv = IsisSolarPanelv2_getTemperature(i, &value, &status);
		if (rv != 0) {
			TRACE_ERROR("\n\r IsisSolarPanelv2_getTemperature(%d) failed. returned: %d\n\r", i, rv);
			// panel_temperatures[i] = -270.0;
			continue;
		}

		panel_temperatures[i] = value * ISIS_SOLAR_PANEL_CONV;
	}

	solar_panels_sleep();

	return &panel_temperatures[0];
}
