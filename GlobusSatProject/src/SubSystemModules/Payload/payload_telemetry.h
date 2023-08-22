/*
 * payload_telemetry.h
 *
 *  Created on: Jun 23, 2020
 *      Author: Owner
 */

#ifndef PAYLOAD_TELEMETRY_H_
#define PAYLOAD_TELEMETRY_H_

int HK_collect_Radfet_Voltages(unsigned char index, void* param);
int HK_collect_ormad_Voltages(unsigned char index, void* param);
int HK_collect_PIC32(unsigned char index, void* param);
int HK_collect_PayloadState(unsigned char index, void* param);
int HK_collect_PayloadSpartan(unsigned char index, void* param);
int HK_collect_SMPA(unsigned char index, void* param);
int HK_collect_Current(unsigned char index, void* param);
int HK_collect_ormad_fpga_spartan(unsigned char index, void* param);
void initPayloadTelemetry(Boolean first_activetion);
int HK_collect_fpga_spartan(unsigned char index, void* param);


#endif /* PAYLOAD_TELEMETRY_H_ */
