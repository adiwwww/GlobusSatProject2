/*
 * printutils.c
 *
 *  Created on: Jan 6, 2024
 *      Author: Muhammad Zahalqa
 */

#include "printutils.h"
#include <stdio.h>

void print_hex_array(unsigned char const* buffer, unsigned int length)
{
	if (!buffer || !length){
		return;
	}

	for (unsigned int i = 0; i < length; ++i) {
		printf("%02x ", buffer[i]);
	}

	printf("\r\n");
}
