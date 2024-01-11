/*
 * antenna_tests.c
 *
 *  Created on: Jan 9, 2024
 *      Author: tryfinally
 */

#include "antenna_tests.h"
#include "utils/menu_selection.h"

#include "modules/m_antenna.h"

#include <stdio.h>


Boolean reset_antenna(void)
{

	return TRUE;
}

Boolean get_ants_temperature(void)
{
	float temp = antenna_get_temperature(isisants_sideA);
	if (temp==-270) {
		printf("Sending temperature failed\n ");
	} else {
		printf("Side A  temperature is : %f\n",temp);
	}

	 temp = antenna_get_temperature(isisants_sideB);
		if (temp==-270) {
			printf("Sending temperature failed\n ");
		} else {
			printf("Side B  temperature is : %f\n",temp);
		}

	return TRUE;
}

Boolean get_ants_uptime(void)
{

	return TRUE;
}

Boolean get_ants_telemetry(void)
{

	return TRUE;
}

static MenuAction menu[] = {
		{ reset_antenna, "Reset antenna subsystem" },
		{ get_ants_temperature, "Get Antenna Temperature" },
		{ get_ants_uptime, "get Antenna Uptime" },
		{ get_ants_telemetry, "Get Antennas Telemetry" },
		MENU_ITEM_END
};

Boolean antenna_tests(void)
{
	MenuDisplay(menu);
	return TRUE;
}
