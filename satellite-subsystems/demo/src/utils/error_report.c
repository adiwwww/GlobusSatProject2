/*
 * error_report.c
 *
 *  Created on: Dec 29, 2023
 *      Author: Muhammad Zahalqa
 */

#include "error_report.h"

#include <hal/errors.h>
#include <at91/utility/trace.h>

void print_error(int error)
{
	if(error != E_NO_SS_ERR) {
	    TRACE_ERROR("ERROR %d encountered while executing the request\n\r", error);
	}
}
