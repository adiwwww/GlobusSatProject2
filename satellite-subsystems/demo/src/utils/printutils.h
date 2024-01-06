/*
 * printutils.h
 *
 *  Created on: Jan 6, 2024
 *      Author: Muhammad Zahalqa
 */

#ifndef UTILS_PRINTUTILS_H_
#define UTILS_PRINTUTILS_H_

/**
 * Print a buffer as hexadecimal bytes
 * @param buffer pointer to data
 * @param length length of data
 */
void print_hex_array(unsigned char const* buffer, unsigned int length);

#endif /* UTILS_PRINTUTILS_H_ */
