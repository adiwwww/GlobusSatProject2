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
	printf("Get Solar Temperature Sensor State\n");
	Boolean awake = solar_panels_is_awake();
	printf("Solar Panel Sensor is %s awake.\n", awake ? "" : "not");
	return TRUE;

}

Boolean activate_solar_panels_sensor(void)
{
	printf("Activate Solar Temperature Sensor\n");
	solar_panels_wakeup();
	return TRUE;

}

Boolean shutdown_solar_panels_sensor(void)
{
	printf("Shutdown Solar Temperature Sensor\n");
	solar_panels_sleep();
	return TRUE;
}

Boolean get_solar_panels_temperature(void)
{
	printf("Solar Panels Temperatures:\n");
	float* values = solar_panels_get_temperature();

	for (int i = 0; i < ISIS_SOLAR_PANEL_COUNT; ++i) {
		printf("Panel[%d] temperature= %f\n", i, values[i]);
	}
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
