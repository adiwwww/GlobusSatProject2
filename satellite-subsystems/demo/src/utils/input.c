/*
 * util.c
 *
 *  Created on: 21 aug. 2015
 *      Author: pbot
 */

#include <hal/Timing/WatchDogTimer.h>

#include <at91/peripherals/dbgu/dbgu.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define _MAX_INPUT_BUFFER_LENGTH 64

static char _input[_MAX_INPUT_BUFFER_LENGTH];

static int _GetInput( char* input, int len );

static char* get_input(char const* message)
{
	printf("%s", message);
	fflush(stdout);

	for (;;) {
		if (_GetInput( _input, _MAX_INPUT_BUFFER_LENGTH) ) {
			break;
		}
		printf("Buffer overrun!\n\r");
	}
	return _input;
}

long INPUT_GetLong_Range(char const* printStr, long min, long max)
{
	long value = 0;
	char *pEnd = NULL;

	for (;;) {
		char* buffer = get_input(printStr);
		value = strtol(buffer, &pEnd, 0);
		if( value > max ) {
			printf("Value too large!\n\r");
		} else if(value < min) {
			printf("Value too small!\n\r");
		} else if (buffer == pEnd) {
			printf("Not a number!\n\r");
		} else {
			break;
		}
	}

	return value;
}

unsigned long INPUT_GetULong_RangeWithBase(char const* printStr, unsigned long min, unsigned long max, int base)
{
	unsigned long value = 0;
	char *pEnd = NULL;

	for (;;) {
		char* buffer = get_input(printStr);
		value = strtoul(buffer, &pEnd, base);
		if( value > max ) {
			printf("Value too large!\n\r");
		} else if(value < min) {
			printf("Value too small!\n\r");
		} else if (buffer == pEnd) {
			printf("Not a number!\n\r");
		} else {
			break;
		}
	}

	return value;
}

unsigned long INPUT_GetULong_Range(char const* printStr, unsigned long min, unsigned long max)
{
	return INPUT_GetULong_RangeWithBase(printStr, min, max, 0);
}

int8_t INPUT_GetINT8( char const* printStr )
{
	return (int8_t)INPUT_GetLong_Range(printStr, INT8_MIN, INT8_MAX);
}

int16_t INPUT_GetINT16( char const* printStr )
{
	return (int16_t)INPUT_GetLong_Range(printStr, INT16_MIN, INT16_MAX);;
}

int32_t INPUT_GetINT32( char const* printStr )
{
	return (uint32_t)INPUT_GetLong_Range(printStr, INT32_MIN, INT32_MAX);
}

uint8_t INPUT_GetUINT8( char const* printStr )
{
	return (uint8_t)INPUT_GetULong_Range(printStr, 0, UINT8_MAX);
}

uint16_t INPUT_GetUINT16( char const* printStr )
{
	return (uint16_t)INPUT_GetULong_Range(printStr, 0, UINT16_MAX);
}

uint32_t INPUT_GetUINT32( char const* printStr )
{
	return (uint32_t)INPUT_GetULong_Range(printStr, 0, UINT32_MAX);
}

uint8_t INPUT_GetHEX8( char const* printStr )
{
	return (uint8_t)INPUT_GetULong_RangeWithBase(printStr, 0, UINT8_MAX, 16);
}

uint16_t INPUT_GetHEX16( char const* printStr )
{
	return (uint16_t)INPUT_GetULong_RangeWithBase(printStr, 0, UINT16_MAX, 16);
}

float INPUT_GetFLOAT( char const* printStr )
{
	float value = 0;
	char *pEnd = NULL;
	for(;;){
		char* buffer = get_input(printStr);
		value = strtof(buffer, &pEnd);
		if (buffer != pEnd) {
			break;
		}
		printf("Not a floating point number!\r\n");
	}
	return value;
}

double INPUT_GetDOUBLE( char const* printStr )
{
	double value = 0;
	char *pEnd = NULL;
	for(;;){
		char* buffer = get_input(printStr);
		value = strtod(buffer, &pEnd);
		if (buffer != pEnd) {
			break;
		}
		printf("Not a floating point number!\r\n");
	}
	return value;
}

void INPUT_GetSTRING( char const* printStr, char * const str, int len )
{
	printf("%s", printStr);
	fflush(stdout);
	_GetInput( str, len );
}

int INPUT_GetINT_MinMax(char const* printStr, int min, int max)
{
	for(;;) {
		int result = INPUT_GetINT32(printStr);
		if ( min <= result && result <= max){
			return result;
		}
	}
	return 0;
}

static int _GetInput( char* input, int len )
{
	uint8_t key;
	int index, inputOK;

	index = 0;
	inputOK = 0;

	do
	{
		do
		{
			WDT_forceKickEveryNms(10);
			vTaskDelay(1);
			/* TODO: timeout ? */
		}
		while( !DBGU_IsRxReady() );

		key = DBGU_GetChar();
		DBGU_PutChar( key );

		if( key == 0x0A || key == 0x0D )
		{
			printf("\n\r");
			input[index] = 0; /* terminate string */
			inputOK = 1;
			break;
		}
		else if( key == 0x08 || key == 0x7F )
		{
			index--; /* backspace */
		}
		else
		{
			input[index++] = key;
		}
	}
	while( index < len );

	return inputOK;
}
