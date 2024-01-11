/*
 * solar_panel_tests.c
 *
 *  Created on: Jan 9, 2024
 *      Author: Muhammad Zahalqa
 */

#include "solar_panel_tests.h"
#include "utils/menu_selection.h"

#include "modules/m_solar_panels.h"

Boolean get_solar_panels_sensor_state(void)
{
	if (solar_panels_is_awake()) {
		printf("The sensors are currently active.");
	} else {
		printf("The sensors aren't active.");
	}
	return TRUE;
}

Boolean activate_solar_panels_sensor(void)
{
	return TRUE;
}

Boolean shutdown_solar_panels_sensor(void)
{
	return TRUE;
}

Boolean get_solar_panels_temperature(void)
{
	// we have ISIS_SOLAR_PANEL_COUNT solar panels

	return TRUE;
}

static MenuAction menu[] = {
			{ get_solar_panels_sensor_state, "Get Solar  panels temperature sensor state" },
			{ activate_solar_panels_sensor, "Activate Solar  panels temperature sensor" },
			{ shutdown_solar_panels_sensor, "Shutdown Solar  panels temperature sensor" },
			{ get_solar_panels_temperature, "Get Solar  panels temperature" },
			MENU_ITEM_END
};

Boolean solar_panels_tests(void)
{
	MenuDisplay(menu);
	return TRUE;
}
