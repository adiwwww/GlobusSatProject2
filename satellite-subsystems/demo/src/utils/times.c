/*
 * util/times.c
 *
 *  Created on: Dec 29, 2023
 *      Author: Muhammad Zahalqa
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void delay_ms(int milliseconds)
{
	vTaskDelay(milliseconds / portTICK_RATE_MS);
}
