/*
 * SystemC.c
 *
 *  Created on: Dec 22, 2023
 *      Author: Muhammad Zahlqa
 */

#include "SystemC.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>		// printf format codes
#include "utils/menu_selection.h"

static Boolean endian_test(void)
{
	uint32_t data = 0x12345678;
	printf("Data %" PRIu32 ".\r\n", data);
	uint8_t* p = (uint8_t*) &data;
	printf("Data[0] %x.\r\n", p[0]);
	printf("Data[1] %x.\r\n", p[1]);
	printf(p[0] == 0x12 ? "Big Endian\r\n" : "Little Endian\r\n");
	return TRUE;
}

static Boolean data_types_sizes_test(void)
{
	print("size of   short: %d\r\n", sizeof(short));
	print("size of     int: %d\r\n", sizeof(int));
	print("size of    long: %d\r\n", sizeof(long));
	print("size of   float: %d\r\n", sizeof(float));
	print("size of  double: %d\r\n", sizeof(double));
	print("size of int32_t: %d\r\n", sizeof(int32_t));

	return TRUE;
}

static MenuAction system_menu[] = {
			{ endian_test, "Platform Endianess test"},
			{ data_types_sizes_test, "Data types size test"},
			MENU_ITEM_END
};

Boolean SystemCTest(void)
{
	MenuDisplay(system_menu);
	return TRUE;
}
