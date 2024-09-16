/*
 * gom_eps.c
 *
 *  Created on: Dec 28, 2023
 *      Author: Muhammad Zahalqa
 */

#include "gom_eps_tests.h"
#include "modules/m_gomeps.h"
#include "utils/menu_selection.h"
#include "utils/error_report.h"

#include <satellite-subsystems/IsisTRXVU.h>

#include <stdio.h>

static Boolean basic_eps_telemetry(void)
{
	gom_eps_hk_basic_t telemetry;
	gom_eps_get_basic_HK_data(&telemetry);

	printf("Bat Temperature %d C.\r\n", telemetry.fields.temp[4]);
	printf("Bat Temperature %d C.\r\n", telemetry.fields.temp[5]);
	printf("Battery Mode = %d\r\n", telemetry.fields.battmode);

	return TRUE;
}

static Boolean extended_eps_telemetry(void)
{
	EPS_HK_Data data;
	if (gom_eps_get_HK(&data)) {
		printf("vBat       : %d\r\n", data.bv);
		printf("Current    : %d\r\n", data.pc);
		printf("Temperature: %d\r\n", data.tempBattery);

		int r = IsisTrxvu_tcSetAx25BeaconDefClSign(0, (unsigned char*)&data, sizeof(data), 30);
		if (r) {
			printf("Beacon was started");
		} else {
			print_error(r);
		}
	}

	return TRUE;
}

static MenuAction menu[] = {
			{ basic_eps_telemetry, "GOMSapce EPS Basic Telemetry Data"},
			{ extended_eps_telemetry, "GOMSapce EPS Extended Telemetry Data"},
			MENU_ITEM_END
};

Boolean gom_eps_test(void)
{
	MenuDisplay(menu);
	return TRUE;
}
