/*
 * input.h
 *
 *  Created on: 21 aug. 2015
 *      Author: pbot
 */

#ifndef INCLUDE_UTIL_INPUT_H_
#define INCLUDE_UTIL_INPUT_H_

#include <stdint.h>

long INPUT_GetLong_Range(char const* printStr, long min, long max);
unsigned long INPUT_GetULong_Range(char const* printStr, unsigned long min, unsigned long max);

int8_t 	INPUT_GetINT8( char const* printStr );
int16_t INPUT_GetINT16( char const* printStr );
int32_t INPUT_GetINT32( char const* printStr );
uint8_t	INPUT_GetUINT8( char const* printStr );
uint16_t INPUT_GetUINT16( char const* printStr );
uint32_t INPUT_GetUINT32( char const* printStr );
uint8_t	 INPUT_GetHEX8( char const* printStr );
uint16_t INPUT_GetHEX16( char const* printStr );
float  INPUT_GetFLOAT( char const* printStr );
double INPUT_GetDOUBLE( char const* printStr );
void   INPUT_GetSTRING( char* printStr, char * const str, int len );

/**
 * Get an int in the range [min..max]
 * @param printStr	prompt to print
 * @param min		minimal acceptable value
 * @param max		maximal acceptable value
 * @return
 */
int INPUT_GetINT_MinMax(char const* printStr, int min, int max);

#endif /* INCLUDE_UTIL_INPUT_H_ */
