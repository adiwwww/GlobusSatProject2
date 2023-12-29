/*
 * menu_selection.c
 *
 *  Created on: Dec 1, 2023
 *      Author: tryfinally
 */

#include "menu_selection.h"

#include "utils/input.h"

#include <stdio.h>

#include <hal/Utility/util.h>

Boolean quit_menu() { return FALSE; }

Boolean MenuSelectAndExecute(MenuAction menu[])
{
	int index = 0;
	MenuAction* pmenu = menu;

	printf("\r\n");
	while (pmenu->action) {
		printf("\t %-2d) %s\r\n", ++index, pmenu->menuSelection);
		++pmenu;
	}

	int selection = INPUT_GetINT_MinMax("Select a test to perform: ", 1, index);
	if (0 <= selection && selection <= index) {
		return menu[selection-1].action();
	}

	return TRUE;
}

Boolean MenuDisplay(MenuAction menu[])
{
	do {
	} while(MenuSelectAndExecute(menu));

	return FALSE;
}

unsigned int GetNumberInRange(int min, int max)
{
	int selection;
	while(UTIL_DbguGetIntegerMinMax(&selection, min, max) == 0){
			;
	}
	return selection;
}

void GetHexBuffer(unsigned char* buffer, unsigned int size)
{
	unsigned int input;
	for (unsigned int i = 0; i < size; ++i) {
		if ( 0 == UTIL_DbguGetHexa32(&input) ) {
			buffer[i] = 0;
			break;
		}
		buffer[i] = input;
	}
}







