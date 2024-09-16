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


typedef struct AntennaTelemetry {
	struct __attribute__ ((__packed__)) {
		float temperature;
		unsigned int uptime;
		ISISantsStatus status;
	} side[2];
} AntennaTelemetry;


Boolean m_antenna_init(void);

float antenna_get_temperature(ISISantsSide side);

unsigned int antenna_get_uptime(ISISantsSide side);

Boolean antenna_reset(void);

AntennaTelemetry* antenna_get_telemetry(void);

#endif /* MODULES_M_ANTENNA_H_ */
