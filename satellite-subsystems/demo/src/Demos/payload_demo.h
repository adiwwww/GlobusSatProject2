/*
 * IsisTRXVUdemo.h
 *
 *  Created on: 6 feb. 2015
 *      Author: malv
 */

#ifndef ISIS_SOREQ_PAYLOAD_H_
#define ISIS_SOREQ_PAYLOAD_H_

#include <hal/boolean.h>

/***
 * Starts demo.
 * Calls Init and Menu in sequence.
 * Returns FALSE on failure to initialize.
 */

typedef struct {
    unsigned char STATUS[1];
    unsigned char RADFET1[4];
    unsigned char RADFET2[4];
    unsigned char OPCODE[2];
} RadiationData;

typedef struct {
    unsigned char STATUS[1];
    unsigned char TEMP[4];
    unsigned char OPCODE[2];
} TemperatureData;

typedef struct {
    unsigned char STATUS[1];
    unsigned char OPCODE[2];
    unsigned char PIC32SEL[4];
    int* COUNTPIC32SEL;
    unsigned char PIC32SELBACKUP[4];
    int* COUNTPIC32SELBACKUP;
} PIC32SELData;

typedef struct {
    unsigned char STATUS;
    unsigned char OPCODE;
    unsigned char PIC32SEU;
} PIC32SEUData;

typedef struct {
    unsigned char STATUS[1];
    unsigned char OPCODE[2];
} ActionData;


Boolean masterRadiationWrite(RadiationData* data);
/***
 * This function reads the voltages for RADFET1 and RADFET2 from the Soreq payload.
 * OPCODE sent to the payload is 0x33.
 * Returns FALSE on failure.
 */

Boolean masterTemperatureWrite(TemperatureData* data);
/***
 * This function sends a command to the Soreq payload to start the radiation experiment.
 * OPCODE sent to the payload is 0x77.
 * Returns FALSE on failure.
 */

Boolean masterPIC32SELWrite(PIC32SELData *data);
/***
 * This function sends a command to the Soreq payload to start the radiation experiment.
 * OPCODE sent to the payload is 0x66.
 * Returns FALSE on failure.
 */

Boolean masterPIC32SEUWrite(PIC32SEUData *data);
/***
 * This function sends a command to the Soreq payload to start the radiation experiment.
 * OPCODE sent to the payload is 0x47.
 * Returns FALSE on failure.
 */


Boolean clearWatchdog (ActionData *action);
/***
 * This function sends a command to the Soreq payload to clear the watchdog.
 * OPCODE sent to the payload is 0x3F.
 * Returns FALSE on failure.
 */

Boolean softReset (ActionData *action);
/***
 * This function sends a command to the Soreq payload to perform a soft reset.
 * OPCODE sent to the payload is 0xF8.
 * Returns FALSE on failure.
 */

Boolean soreqDebugging(void);
/***
 * This function sends a command to the Soreq payload to start the radiation experiment.
 * OPCODE sent to the payload is 0x32.
 * Returns FALSE on failure.
 */

Boolean soreqPayloadInit(void);

/***
 * Loop producing an interactive
 * text menu for invoking subsystem functions
 * note:
 * Depends on an initialized TRXVU subsystem driver.
 */

#endif /* ISIS_SOREQ_PAYLOAD_H_ */
