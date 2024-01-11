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

	return TRUE;
}

Boolean get_ants_uptime(void)
{
	int time = antenna_get_uptime(isisants_sideA);
		if (time==0) {
			printf("Sending uptime failed\n ");
		} else {
			printf("Side A uptime is : %d\n", time);
		}

		time = antenna_get_uptime(isisants_sideB);
		if (time==0) {
			printf("Sending uptime failed\n ");
		} else {
			printf("Side B  uptime is : %d\n", time);
		}

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
