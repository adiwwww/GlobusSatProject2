/*
 * trxvu_tests.c
 *
 *  Created on: Dec 28, 2023
 *      Author: Muhammad Zahalqa
 */

#include "trxvu_tests.h"
#include "modules/m_gomeps.h"
#include "modules/m_trxvu.h"

#include "utils/input.h"
#include "utils/timeutils.h"
#include "utils/error_report.h"
#include "utils/menu_selection.h"

#include <hal/errors.h>

#include <stdio.h>


static Boolean transmit_fixed_message_test(void)
{
	unsigned char buffer[] = "There's a starman waiting in the sky\n"
							 "He'd like to come and meet us\n"
							 "But he thinks he'd blow our minds\n"
							 "CUBESAT: K.Qari3.";
	printf("Message to be sent:\n%s\r\n", buffer);
	int repeats = INPUT_GetINT8("How many times do you want to send this: ");

	for (int i = 0; i < repeats; ++i) {
		if (trxvu_send_message(buffer, ARRAY_SIZE(buffer))) {
			printf("OK Transmit [%d]: %s\r\n", i, buffer);
		} else {
			printf("FAIL Transmit [%d]: %s\r\n", i, buffer);
		}

		delay_ms(200);
	}

	return TRUE;
}

static Boolean transmit_user_message_test(void)
{
	unsigned char buffer[81] = {0};
	INPUT_GetSTRING("What message do you want to send: ", (char*)buffer, 80);
	int repeats = INPUT_GetINT8("How many times do you want to send it: ");
	printf("Will send: %s \r\n", buffer);
	printf("  repeats: %d \r\n", repeats);

	for (int i = 0; i < repeats; ++i) {
		if (trxvu_send_message(buffer, ARRAY_SIZE(buffer))) {
			printf("OK Transmit [%d]: %s\r\n", i, buffer);
		} else {
			printf("FAIL Transmit [%d]: %s\r\n", i, buffer);
		}

		delay_ms(200);
	}

	return TRUE;
}

static Boolean set_ax25_bitrate_test(void)
{
	return TRUE;
}

static Boolean activate_responder_test(void)//
{
	if(trxvu_activate_responder())
		printf("it is Work");
	else
		printf("Falid work");
	return TRUE;
}

static Boolean deactivate_responder_test(void)
{
	return TRUE;
}

static Boolean automatic_responder_stop_test(void)
{
	return TRUE;
}

static Boolean set_responder_rssi(void)
{
	return TRUE;
}

static Boolean get_incoming_frames_test(void)
{

	return TRUE;
}

static MenuAction menu[] = {
			{ transmit_fixed_message_test, "Transmit fixed text message" },
			{ transmit_user_message_test, "Transmit message defined by user" },
			{ set_ax25_bitrate_test, "Set AX.25 bit rate" },
			{ activate_responder_test, "Activate responder" },
			{ deactivate_responder_test, "Deactivate responder" },
			{ automatic_responder_stop_test, "Activate responder for a time interval" },
			{ set_responder_rssi, "Set Responder RSSI threshold test" },
			{ get_incoming_frames_test, "Get incoming RX frames test" },
			MENU_ITEM_END
};

Boolean trxvu_tests(void)
{
	MenuDisplay(menu);
	return TRUE;
}
