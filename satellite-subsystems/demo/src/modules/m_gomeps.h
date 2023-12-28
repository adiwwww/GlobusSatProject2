/*
 * GOM_EPS.h
 *
 *  Created on: Dec 23, 2023
 *      Author: tryfinally
 */

#ifndef MODULES_M_GOMEPS_H_
#define MODULES_M_GOMEPS_H_

#include <hal/boolean.h>
#include <satellite-subsystems/GomEPS.h>

typedef struct __attribute__ ((__packed__)) EPS_HK_Data {
	unsigned short photoVoltaic3; //!< Voltage value at photovoltaic 3 [mV]
	unsigned short photoVoltaic2; //!< Voltage value at photovoltaic 2 [mV]
	unsigned short photoVoltaic1; //!< Voltage value at photovoltaic 1 [mV]
	unsigned short pc; //!< Total photo current [mA]
	unsigned short bv; //!< Battery voltage [mV]
	unsigned short sc; //!< Total system current [mA]

	short tempBattery; //!< Temperature of EPS battery [degC]
	unsigned short bootCount; //!< Number of EPS reboots
	unsigned short swErrors; //!< Number of errors in the eps software
} EPS_HK_Data;

Boolean m_gom_eps_init(void);

Boolean gom_eps_get_basic_HK_data(gom_eps_hk_basic_t* telemetry);

Boolean gom_eps_get_HK(EPS_HK_Data* eps_hk);

#endif /* MODULES_M_GOMEPS_H_ */
