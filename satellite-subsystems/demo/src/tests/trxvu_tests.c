/*
 * trxvu_tests.c
 *
 *  Created on: Dec 28, 2023
 *      Author: Muhammad Zahalqa
 */

#include "trxvu_tests.h"
#include "modules/m_gomeps.h"

#include "utils/input.h"
#include "utils/time.h"
#include "utils/error_report.h"
#include "utils/menu_selection.h"

#include <satellite-subsystems/IsisTRXVU.h>

#include <hal/errors.h>

#include <stdio.h>


static Boolean transmit_fixed_message_test(void)
{
	unsigned char buffer[] = "HELLO WORLD FROM CUBE-SAT KQ";
	printf("Message to be sent: [%s]\r\n", buffer);
	int repeats = INPUT_GetINT8("How many times do you want to send this: ");

	unsigned char avalFrames = 0;
	for (int i = 0; i < repeats; ++i) {
		int r = IsisTrxvu_tcSendAX25DefClSign(0, buffer, ARRAY_SIZE(buffer), &avalFrames);

		if (r == E_NO_SS_ERR) {
			printf("OK Transmitting [%d]: %s\r\n", i, buffer);
		} else {
			print_error(r);
		}

		delay_ms(200);
	}

	return TRUE;
}

static Boolean transmit_user_message_test(void)
{
	unsigned char buffer[80] = {0};
	INPUT_GetSTRING("What message do you want to send: ", (char*)buffer, 80);
	int repeats = INPUT_GetINT8("How many times do you want to send it: ");
	printf("Will send: %s \r\n", buffer);
	printf("  repeats: %d \r\n", repeats);

	unsigned char avalFrames = 0;

	for (int i = 0; i < repeats; ++i) {
		int r = IsisTrxvu_tcSendAX25DefClSign(0, buffer, ARRAY_SIZE(buffer), &avalFrames);
		print_error(r);
		if (r == E_NO_SS_ERR) {
			printf("OK Transmitting [%d]: %s\r\n", i, buffer);
		}

		delay_ms(200);
	}

	return TRUE;
}

static MenuAction menu[] = {
			{ transmit_fixed_message_test, "Transmit fixed text message" },
			{ transmit_user_message_test, "Transmit message defined by user" },
			MENU_ITEM_END
};

Boolean trxvu_tests(void)
{
	MenuDisplay(menu);
	return TRUE;
}
