#include "payload_drivers.h"
#include "hal/Drivers/I2C.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <stdlib.h>
#include <stdio.h>
#include <hal/Storage/FRAM.h>
#include <string.h>
#include <hal/Timing/Time.h>
//#include <satellite-subsystems/cspaceADCS_types.h>
//#include <satellite-subsystems/cspaceADCS.h>
#include "satellite-subsystems/IsisSolarPanelv2.h"
#include "../Global/GlobalParam.h"
#include "../Global/OnlineTM.h"
#include "../TRXVU.h"
#include "../Global/FRAMadress.h"

#define SOFT_RESET 0xF8
#define TURN_OFF_FPGA_POWER 0x55
#define PREFORM_READBACK 0x1C
#define ERASE_FPGA 0x32
#define CLEAR_WDT_AND_ALLOW_READ 0x3F
#define READ_ORMADD 0xE3
#define SOREQ_I2C_ADDRESS 0x55
#define READ_PIC32_RESETS 0x66
#define READ_SMP_SAMLE_DATA 0x44
#define MEASUER_SMPA 0xCC
#define OPEN_SMPA 0xAA
#define STOP_SMPA_OPENING 0xBB
#define READ_RADFET_VOLTAGES 0x33
#define NUMBER_OF_TESTED_FPGA_2 0x65
#define NUMBER_OF_TESTED_FPGA_3 0x76
#define NUMBER_OF_TESTED_FPGA_4 0x88
#define READ_TOTAL_TESTES_FPGAA 0x89
#define TURN_OFF_FPGA 0x55
#define TURN_ON_FPGA_POWER 0x07
#define PREFORM_READBACK 0x1C
#define TIMEOUT 4000
#define READ_DELAY 200


#define PAYLOAD_DATA_SIZE (4)
void payloadTask(void* args);
SoreqResult payloadTurnOff_FPGA(Boolean is_blocking);
int changeIntIndian(int num)
{
	return ((num>>24)&0xff) | // move byte 3 to byte 0
	                    ((num<<8)&0xff0000) | // move byte 1 to byte 2
	                    ((num>>8)&0xff00) | // move byte 2 to byte 1
	                    ((num<<24)&0xff000000); // byte 0 to byte 3
}

xSemaphoreHandle xSemaphore;
Boolean8bit override_reboot =FALSE_8BIT;
typedef enum
{
	board,
	radfet,
	ormad,
	fpga

} PayloadSubSystem;
SoreqResult updatePayloadStruct()
{
	if(FRAM_write(&payloadState,PAYLOAD_DATA_ADD,sizeof(payloadState))!=0)
	{
		return PAYLOAD_FRAM_ERROR;
	}
	return PAYLOAD_SUCCESS;
}
#define CURRENT_SAVE_PIRIOD (5)
#define SMPA_MUTE_TIME (40)
SoreqResult payloadOpenSMPA(Boolean is_blocking)
{
	int res;

	delete_onlineTM_param_from_offline(TM_ORMAD);
	delete_onlineTM_param_from_offline(TM_RATFET);
	delete_onlineTM_param_from_offline(TM_PIC32);
	delete_onlineTM_param_from_offline(TM_ORMAD);
	delete_onlineTM_param_from_offline(TM_ORMAD_PIC_SPARTAN);
	add_onlineTM_param_to_save_list(TM_CURRENT,CURRENT_SAVE_PIRIOD,(unsigned int)-1);
	set_mute_time(SMPA_MUTE_TIME);
	payloadTurnOff_FPGA(FALSE);
	vTaskDelay(500);
	payloadSendCommand(OPEN_SMPA,is_blocking,4,&res,0);

	payloadState.SMPA_Stat=1;
	updatePayloadStruct();
	xTaskCreate(payloadTask, (const signed char*)"payload",8192*2,NULL,(unsigned portBASE_TYPE)TASK_DEFAULT_PRIORITIES,NULL);
	return PAYLOAD_SUCCESS;
}
SoreqResult payloadMEASUER_SMPA(Boolean is_blocking, int* res)
{
	char buffer[8];
	payloadSendCommand(MEASUER_SMPA,is_blocking,8,buffer,0);
	memcpy(res,buffer+4,4);
	changeIntIndian(res);
	return PAYLOAD_SUCCESS;
}
SoreqResult payloadStopSMPA(Boolean is_blocking)
{
	int res;
	payloadSendCommand(STOP_SMPA_OPENING,is_blocking,4,&res,0);
	payloadState.SMPA_Stat = 0;
	updatePayloadStruct();
	return PAYLOAD_SUCCESS;
}
void setCurrAckTime(PayloadSubSystem payloadSubSystem)
{
	unsigned int time;
	Time_getUnixEpoch(&time);
	switch(payloadSubSystem)
	{
		case board:
			payloadState.last_ack_time = time;
			break;
		case radfet:
			payloadState.RADFET_Readout_Time = time;
			break;
		case ormad:
			payloadState.ORMADD_Readout_Time = time;
			break;
		case fpga:
			payloadState.PIC_Readout_Time = time;
			break;
	}

}
SoreqResult payloadInit(Boolean first_activation)
{
	int err = 0;
	vSemaphoreCreateBinary(xSemaphore);
	payloadTurnOn_FPGA(FALSE);
	if(first_activation)
	{
		memset((void*)&payloadState,0,sizeof(PayloadState));
		payloadState.current_ON_FPGAs=4;
		payloadState.last_ON_FPGAs=4;
		err = FRAM_write(&payloadState,PAYLOAD_DATA_ADD,sizeof(PayloadState));
		err = FRAM_write(&override_reboot,PAYLOAD_OVERRIDE_REBOOT_FPGA_ADDR,1);

	}
	else
	{
		err = FRAM_read(&payloadState,PAYLOAD_DATA_ADD,sizeof(PayloadState));
		err = FRAM_read(&override_reboot,PAYLOAD_OVERRIDE_REBOOT_FPGA_ADDR,1);

	}
	initPayloadTelemetry(first_activation);
	return PAYLOAD_SUCCESS;
}
#define FPGA_DELAY 10000
void payloadFPGA_startTask(void* param)
{
	vTaskDelay(FPGA_DELAY);
	payloadSetNumberOfFPGA(TRUE,payloadState.current_ON_FPGAs);
	vTaskDelete(NULL);
}

SoreqResult payloadRaed(int size,unsigned char* buffer,Boolean is_blocking)
{
	unsigned char wtd_and_read[] = {CLEAR_WDT_AND_ALLOW_READ};
	int err=0;

	int i=0;
	do
	{
		err = I2C_write(PAYLOAD_I2C_ADDRESS,wtd_and_read,1);
		if(err!=0)
		{
			return PAYLOAD_I2C_Write_Error;
		}
		vTaskDelay(READ_DELAY);
		err = I2C_read(SOREQ_I2C_ADDRESS,buffer,size);
		if(err!=0)
		{
			return PAYLOAD_I2C_Read_Error;
		}
		if(buffer[3] == 0)
		{
			setCurrAckTime(board);
			return PAYLOAD_SUCCESS;
		}
	} while(is_blocking == TRUE && TIMEOUT/READ_DELAY > i++);
	if(is_blocking == FALSE)
	{
		return PAYLOAD_NOT_DONE;
	}
	return PAYLOAD_TIMEOUT;
}
SoreqResult payloadSendCommand(char opcode,Boolean is_blocking,int size,unsigned char* buffer,int delay)
{
	if( xSemaphoreTake( xSemaphore, ( portTickType ) 10000 ) == pdTRUE )
	{

		vTaskDelay(50);
		int err = I2C_write(PAYLOAD_I2C_ADDRESS,&opcode,1);
		if(err!=0)
		{
			xSemaphoreGive(xSemaphore);
			return PAYLOAD_I2C_Write_Error;
		}
		if(delay>0)
		{
			vTaskDelay(delay);
		}
		SoreqResult res =  payloadRaed(size,buffer,is_blocking);
		xSemaphoreGive(xSemaphore);
		return res;
	}
	return payload_Semaphore_Failed;
}
SoreqResult payloadSofatReset(Boolean is_blocking)
{
	int res;
	return payloadSendCommand(SOFT_RESET,is_blocking,sizeof(int),&res,0);
}

SoreqResult payloadGetPIC32_LatchUpCounter(Boolean is_blocking,int* count)
{
	char buffer[12];
	SoreqResult res = payloadSendCommand(READ_PIC32_RESETS, is_blocking,12, buffer,0);
	memcpy(count,buffer+4,4);
	if(*count==0)
	{
		memcpy(count,buffer+8,4);
	}
	*count = changeIntIndian(*count);
	setCurrAckTime(fpga);
	updatePayloadStruct();
	return res;
}




SoreqResult payloadSetNumberOfFPGA(Boolean is_blocking, int number_of_FPGA)
{
	char buffer[8];
	char opcode;
	switch(number_of_FPGA)
	{
	case 0:
		opcode = TURN_OFF_FPGA;
		break;
	case 2:
		opcode = NUMBER_OF_TESTED_FPGA_2;
		break;
	case 3:
		opcode = NUMBER_OF_TESTED_FPGA_3;

		break;
	case 4:
		opcode = NUMBER_OF_TESTED_FPGA_4;
		break;
	}
	payloadState.last_ON_FPGAs = payloadState.current_ON_FPGAs;
	payloadState.current_ON_FPGAs =number_of_FPGA;
	vTaskDelay(100);
	updatePayloadStruct();
	SoreqResult res = payloadSendCommand(opcode, is_blocking,8, buffer,0);
	if(res!=PAYLOAD_SUCCESS)
	{
		return res;
	}

	if(number_of_FPGA>0)
	{
		res = payloadSendCommand(TURN_ON_FPGA_POWER, is_blocking,8, buffer,0);
	}

	return res;
}
SoreqResult payloadReadNumberOfFPGA(Boolean is_blocking, int* number_of_FPGA)
{
	char buffer[8];
	SoreqResult res = payloadSendCommand(READ_TOTAL_TESTES_FPGAA, is_blocking,8, buffer,0);
	memcpy(number_of_FPGA,buffer+4,4);
	*number_of_FPGA = changeIntIndian(*number_of_FPGA);
	updatePayloadStruct();
	return res;
}
SoreqResult payloadReadRadfetVoltages(Boolean is_blocking, RadfetVoltages* radfetVoltages)
{
	char buffer[12];
	SoreqResult res = payloadSendCommand(READ_RADFET_VOLTAGES, is_blocking,12, buffer,6000);
	memcpy(&(radfetVoltages->volt1),buffer+4,4);
	memcpy(&(radfetVoltages->volt2),buffer+8,4);
	radfetVoltages->volt1 = changeIntIndian(radfetVoltages->volt1);
	radfetVoltages->volt2 = changeIntIndian(radfetVoltages->volt2);
	setCurrAckTime(radfet);
	updatePayloadStruct();
	return res;
}
#define NUM_OF_ITERATION_BEFORE_RESET (60*90/30)
SoreqResult payloadSpartan(Boolean is_blocking, SpartanData* spartan_data)
{

	char buffer[40];
	static int iter=1;
	if(((iter++)%NUM_OF_ITERATION_BEFORE_RESET==0)&&(override_reboot==FALSE_8BIT))
	{
		payloadSetNumberOfFPGA(payloadState.current_ON_FPGAs,TRUE);
	}
	SoreqResult res = payloadSendCommand(PREFORM_READBACK, is_blocking,40, buffer,10000);
	memcpy(spartan_data,buffer+4,36);
	setCurrAckTime(fpga);
	updatePayloadStruct();
	return res;
}
SoreqResult payloadOrmadMeasure(Boolean is_blocking, int* ormadd1, int* ormadd2,int* ormadd3)
{

	char buffer[16];
	SoreqResult res = payloadSendCommand(READ_ORMADD, is_blocking,16, buffer,2000);
	memcpy(ormadd1,buffer+4,4);
	memcpy(ormadd2,buffer+8,4);
	memcpy(ormadd3,buffer+12,4);
	*ormadd1 = changeIntIndian(*ormadd1);
	*ormadd2 = changeIntIndian(*ormadd2);
	*ormadd3 = changeIntIndian(*ormadd3);
	setCurrAckTime(ormad);
	updatePayloadStruct();
	return res;
}
SoreqResult payloadRead_SMPA(Boolean is_blocking, char* buffer_SMPA)
{

	SoreqResult res = payloadSendCommand(READ_SMP_SAMLE_DATA, is_blocking,SMPA_DATA_SIZE, buffer_SMPA,5500*4+4);
	return res;
}
SoreqResult payloadTurnOn_FPGA(Boolean is_blocking)
{
	char buffer[16];
	SoreqResult res = payloadSendCommand(TURN_ON_FPGA_POWER, is_blocking,4, buffer,0);
	return res;
}
SoreqResult payloadTurnOff_FPGA(Boolean is_blocking)
{
	char buffer[16];
	SoreqResult res = payloadSendCommand(TURN_OFF_FPGA, is_blocking,4, buffer,0);
	return res;
}
#define PAYLOAD_TASK_DELAY 5000
char smpa_arr_TEMP[5500*4+sizeof(int)*2];
void payloadTask(void* args)
{
	unsigned int res=0;
	f_enterFS();
	unsigned char* res_pointer = (unsigned char*)&res;
	do
	{
		vTaskDelay(PAYLOAD_TASK_DELAY);
		payloadRaed(sizeof(int),res_pointer,TRUE);
	}while(res_pointer[3]!=0);
	set_mute_time(0);
	unmute_Tx();
	delete_onlineTM_param_from_offline(TM_CURRENT);
	payloadState.SMPA_Stat=0;
	updatePayloadStruct();
	payloadRaed(SMPA_DATA_SIZE,smpa_arr_TEMP,TRUE);
	F_FILE* file =f_open("smpa","w");
	f_write(smpa_arr_TEMP+sizeof(int)*2,1,SMPA_DATA_SIZE,file);
	f_close(file);
	f_releaseFS();
	vTaskDelete(NULL);
}
