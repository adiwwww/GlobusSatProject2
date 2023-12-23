/*
 * GOM_EPS.h
 *
 *  Created on: Dec 23, 2023
 *      Author: tryfinally
 */

#ifndef MODULES_GOM_EPS_H_
#define MODULES_GOM_EPS_H_

#include <hal/boolean.h>
#include <satellite-subsystems/GomEPS.h>

Boolean gom_eps_init(void);
Boolean gom_eps_get_basic_HK_data(gom_eps_hk_basic_t* telemetry);

#endif /* MODULES_GOM_EPS_H_ */
