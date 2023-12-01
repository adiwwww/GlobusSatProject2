/*
 * menu_selection.c
 *
 *  Created on: Dec 1, 2023
 *      Author: tryfinally
 */

#include "menu_selection.h"

#include <stdio.h>

#include <hal/Utility/util.h>

Boolean quit_menu() { return FALSE; }

Boolean MenuSelectAndExecute(MenuAction tests[], unsigned int size)
{
	int selection = 0;
	printf( "\n\r Select a test to perform: \n\r");
	for (unsigned int i = 0; i < size; ++i) {
		printf("\t %-2d) %s\r\n", i+1, tests[i].menuSelection);
	}

	while(UTIL_DbguGetIntegerMinMax(&selection, 1, size) == 0){
		;
	}

	return tests[selection-1].action();
}

Boolean MenuDisplay(MenuAction tests[], unsigned int size)
{
	do {
	} while(MenuSelectAndExecute(tests, size));

	return FALSE;
}
