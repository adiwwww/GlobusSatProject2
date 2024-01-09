/*
 * m_antenna.h
 *
 *  Created on: Jan 9, 2024
 *      Author: tryfinally
 */

#ifndef MODULES_M_ANTENNA_H_
#define MODULES_M_ANTENNA_H_

#include <hal/boolean.h>
#include <satellite-subsystems/IsisAntS.h>

//#include <satellite-subsystems/Isis.h>

Boolean m_antenna_init(void);

float antenna_get_temperature(ISISantsSide side);

unsigned int antenna_get_uptime(ISISantsSide side);

Boolean antenna_reset(void);

#endif /* MODULES_M_ANTENNA_H_ */
