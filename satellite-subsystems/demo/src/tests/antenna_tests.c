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
	printf("Reseting Side A and Side B... ");
	if (antenna_reset()) {
		printf("Done!\n");
	} else {
		printf("Failed.\n");
	}

	return TRUE;
}

Boolean get_ants_temperature(void)
{
	printf("Antenna temperature:\n");
	float a = antenna_get_temperature(isisants_sideA);
	float b = antenna_get_temperature(isisants_sideB);
	printf("Side A: %f C\n", a);
	printf("Side B: %f C\n", b);

	return TRUE;
}

Boolean get_ants_uptime(void)
{
	printf("Antenna uptime:\n");
	unsigned int a = antenna_get_uptime(isisants_sideA);
	unsigned int b = antenna_get_uptime(isisants_sideB);
	printf("Side A: %u sec\n", a);
	printf("Side B: %u sec\n", b);

	return TRUE;
}

Boolean get_ants_telemetry(void)
{
	AntennaTelemetry* telem = antenna_get_telemetry();
	for (int i = 0; i < 2; ++i) {
		printf("Side %c: Status[%d]  Uptime:%u\n", 'A'+i, telem->side[i].status.fields.ant1Undeployed, telem->side[0].uptime);
	}

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
