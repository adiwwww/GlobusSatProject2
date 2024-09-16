/*
 * beacon_tests.c
 *
 *  Created on: Dec 25, 2023
 *      Author: Muhammad Zahalqa
 */


#include "beacon_tests.h"

#include "utils/menu_selection.h"

Boolean basic_beacon_message_test(void)
{
	return TRUE;
}

Boolean stop_beacon_test(void)
{

	return TRUE;
}

Boolean telemetry_beacon_test(void)
{
	return TRUE;
}

Boolean periodic_beacon_telemetry_test(void)
{

	return TRUE;
}

Boolean stop_periodic_beacon_test(void)
{

	return TRUE;
}

static MenuAction menu[] = {
			{ basic_beacon_message_test, "Send text message as beacon" },
			{ telemetry_beacon_test, "Send Basic telemetry Data as beacon" },
			{ stop_beacon_test, "Stop beacon" },
			{ periodic_beacon_telemetry_test, "Periodic telemetry beacon task" },
			{ stop_periodic_beacon_test, "Stop periodic telemetry beacon" },
			MENU_ITEM_END
};

Boolean BeaconTests(void)
{
	MenuDisplay(menu);
	return TRUE;
}
