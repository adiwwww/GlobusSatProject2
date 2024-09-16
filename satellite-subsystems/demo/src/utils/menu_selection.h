/*
 * menu_selection.h
 *
 *  Created on: Dec 1, 2023
 *      Author: tryfinally
 */

#ifndef DEMOS_MENU_SELECTION_H_
#define DEMOS_MENU_SELECTION_H_

#include <hal/boolean.h>

Boolean quit_menu();
typedef Boolean (*MenuActionPtr)(void);

typedef struct MenuAction {
	MenuActionPtr action;
	char const*  menuSelection;
} MenuAction;

#define MENU_ITEM_END		{quit_menu, "Return to previous menu"}, {0, 0}
#define END_OF_MENU					{NULL, NULL}
#define ARRAY_SIZE(a) 	(sizeof(a)/sizeof((a)[0]))

/**
 * Display menu and execute action selected
 * @param tests array of menu item {function, description }
 * @param size  number of elements in the array
 * @return
 */
Boolean MenuSelectAndExecute(MenuAction menu[]);

Boolean MenuDisplay(MenuAction menu[]);

/**
 * Get an integer in the range [min, max]
 * @param min lower bound
 * @param max upper bound
 * @return user input integer
 */
unsigned int GetNumberInRange(int min, int max);

void GetHexBuffer(unsigned char* buffer, unsigned int size);

#endif /* DEMOS_MENU_SELECTION_H_ */
