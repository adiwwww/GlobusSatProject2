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

Boolean telemetry_beacon_test(void)
{
	return TRUE;
}

Boolean periodic_telemetry_beacon_test(void)
{

	return TRUE;
}

static MenuAction menu[] = {
			{ basic_beacon_message_test, "Send text message as beacon"},
			{ telemetry_beacon_test, "Send Basic telemetry Data as beacon"},
			{ periodic_telemetry_beacon_test, "Periodically Collect and send telemetry Data"},
			MENU_ITEM_END
};

Boolean BeaconTests(void)
{
	MenuDisplay(menu);
	return TRUE;
}
