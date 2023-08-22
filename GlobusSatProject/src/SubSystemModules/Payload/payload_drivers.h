

#ifndef PAYLOAD_DRIVERS_H_
#define PAYLOAD_DRIVERS_H_

#include <hal/boolean.h>
#include "payload_telemetry.h"
#define PAYLOAD_I2C_ADDRESS 0x55
#define NUMBER_OF_SMPA_SAMPLES 5500
#define SMPA_SAMPLE_SIZE (sizeof(int))
#define SMPA_DATA_SIZE ((NUMBER_OF_SMPA_SAMPLES)*(SMPA_SAMPLE_SIZE))
#define SMPA_NUM_OF_SAMPLES_PER_PACKET (25)
#define SMPA_PACKET_ALL_SAMPLES_SIZE (SMPA_NUM_OF_SAMPLES_PER_PACKET*SMPA_SAMPLE_SIZE)
#define SIZE_OF_SMPA_PACKET (SMPA_PACKET_ALL_SAMPLES_SIZE+1)
#define NUMBER_OF_SMPA_PACKETS ((SMPA_DATA_SIZE)/SMPA_PACKET_ALL_SAMPLES_SIZE)

typedef enum
{
	PAYLOAD_SUCCESS,
	PAYLOAD_I2C_Write_Error,
	PAYLOAD_I2C_Read_Error,
	PAYLOAD_TIMEOUT,
	PAYLOAD_NOT_DONE,
	PAYLOAD_FRAM_ERROR,
	payload_Semaphore_Failed
}SoreqResult;

typedef struct __attribute__ ((__packed__))
{
	unsigned int last_ack_time;
	char payload_fatal_error;
	int sat_payload_turn_on_num;
	int sat_payload_turn_off_num;
	unsigned int ORMADD_Readout_Time;
	unsigned int PIC_Readout_Time;
	unsigned int FPGA_Readout_Time;
	char current_ON_FPGAs;
	char last_ON_FPGAs;
	char last_FPGA_Config;
	char overall_FPGA_Errors;
	char anomaly_Region;
	unsigned int RADFET_Readout_Time;
	char RADFET_4FPGA_FailCount;
	char RADFET_3FPGA_FailCount;
	char RADFET_2FPGA_FailCount;
	char SMPA_Stat;
}PayloadState;
typedef struct
{
	int device1_0_to_1;
	int device1_1_to_0;
	int device2_0_to_1;
	int device2_1_to_0;
	int device3_0_to_1;
	int device3_1_to_0;
	int device4_0_to_1;
	int device4_1_to_0;
}SpartanData;
typedef struct
{
	int volt1;
	int volt2;
}RadfetVoltages;
typedef struct
{
	int ormadd1;
	int ormadd2;
	int ormadd3;
	//cspace_adcs_cmdangles_t angles;
	//cspace_adcs_fine_estang_t angle_rat;
	int z_temp;
}OrmadData;
PayloadState payloadState;


SoreqResult payloadSofatReset(Boolean is_blocking);
SoreqResult payloadSendCommand(char opcode,Boolean is_blocking,int size,unsigned char* buffer,int delay);
SoreqResult payloadRaed(int size,unsigned char* buffer,Boolean is_blocking);
SoreqResult payloadReadGetTotalPIC32Resets(Boolean is_blocking,int* count);
SoreqResult payloadGetPIC32_LatchUpCounter(Boolean is_blocking,int* count);
SoreqResult payloadSetNumberOfFPGA(Boolean is_blocking, int number_of_FPGA);
SoreqResult payloadReadNumberOfFPGA(Boolean is_blocking, int* number_of_FPGA);
SoreqResult payloadReadRadfetVoltages(Boolean is_blocking, RadfetVoltages* radfetVoltages);
SoreqResult payloadInit(Boolean first_activation);
SoreqResult payloadOpenSMPA(Boolean is_blocking);
SoreqResult payloadStopSMPA(Boolean is_blocking);
SoreqResult payloadRead_SMPA(Boolean is_blocking, char* buffer);
SoreqResult payloadTurnOn_FPGA(Boolean is_blocking);
void payloadTask(void* args);
void payloadFPGA_startTask(void* param);
SoreqResult payloadGetPIC32_Readback(Boolean is_blocking,char* arr);
SoreqResult payloadMEASUER_SMPA(Boolean is_blocking, int* res);
SoreqResult payloadOrmadMeasure(Boolean is_blocking, int* ormadd1, int* ormadd2,int* ormadd3);
SoreqResult payloadSpartan(Boolean is_blocking, SpartanData* spartan_data);
SoreqResult payloadTurnOff_FPGA(Boolean is_blocking);

//SoreqResult payloadRaed(int size,char* buffer);
//SoreqResult payloadRaed(int size,char* buffer);


#endif /* PAYLOAD_DRIVERS_H_ */
