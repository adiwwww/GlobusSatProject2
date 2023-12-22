/*
 * SystemC.c
 *
 *  Created on: Dec 22, 2023
 *      Author: Muhammad Zahlqa
 */

#include "SystemC.h"

#include <stdint.h>
#include "utils/menu_selection.h"

static Boolean endian_test(void)
{
	uint32_t data = 0x12345678;
	printf("Data %d.\r\n", data);
	printf("Data %X.\r\n", data);
	uint8_t* p = (uint8_t*) &data;
	printf("Data[0] %u.\r\n", p[0]);
	printf("Data[1] %u.\r\n", p[1]);
	printf(p[0] == 0x12 ? "Big Endian\r\n" : "Little Endian\r\n");

	return TRUE;
}

static MenuAction system_menu[] = {
			{ endian_test, "Platform Endianess test"},
			MENU_ITEM_END
};

Boolean SystemCTest(void)
{
	MenuDisplay(system_menu);
	return TRUE;
}
