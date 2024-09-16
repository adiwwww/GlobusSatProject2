/*
 * m_solar_panels.h
 *
 *  Created on: Jan 9, 2024
 *      Author: Muhammad Zahalqa
 */

#ifndef MODULES_M_SOLAR_PANELS_H_
#define MODULES_M_SOLAR_PANELS_H_

#include <hal/boolean.h>
#include <satellite-subsystems/IsisSolarPanelv2.h>

Boolean m_solar_panels_init(void);

Boolean solar_panels_is_awake(void);

Boolean solar_panels_wakeup(void);

Boolean solar_panels_sleep(void);

float* solar_panels_get_temperature(void);

#endif /* MODULES_M_SOLAR_PANELS_H_ */
