
#include <hal/Drivers/I2C.h>
#include <hal/boolean.h>
#include <hal/errors.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <hal/Timing/Time.h>
#include "hal/Drivers/I2C.h"
#include <hal/Utility/util.h>
#include <satellite-subsystems/payload_demo.h>

#define SOREQ_I2C_ADDRESS 0x55
#define READ_PIC32_RESETS 0x66
#define READ_Radiation 0x33
#define READ_DELAY 200
#define TIMEOUT 4000
#define READ_PIC32_RESETS 0x66
#define PAYLOAD_I2C_ADDRESS 0x55
#define SOREQ_DEBUGGING 0x32
#define CLEARWATCHDOG 0x3F
#define SOFTRESET 0xF8
#define OPCODE_MASTER_READ_FROM_PAYLOAD 0x45 // in tau-1

#define RADIATION_DELAY 2500
#define Temperature_DELAY 1690
#define PIC32SEL_DELAY 20
#define PIC32SEU_DELAY 200
#define CLEARWATCHDOG_DELAY 10; //TODO: What is the correct delay?
#define SOFTRESET_DELAY 10; //TODO: What is the correct delay?

typedef enum {
    PAYLOAD_SUCCESS, PAYLOAD_I2C_Write_Error, PAYLOAD_I2C_Read_Error, PAYLOAD_TIMEOUT
} SoreqResult;

typedef enum {
    ADC_VALID // When Bit 29 is 1 and Bit 30 is 0 and Bit 31 is 0
    , STILL_PROCESSING // When Bit 31 is 1 TODO: Check if we need to address this issue with a rerun or different indication.
    , ADC_INVALID // When Bit 29 is 0 and not 1 OR Bit 30 is 1 and not 0
} ADCResult;

typedef enum
{
    board, radfet, ormad, fpga
} PayloadSubSystem;


Boolean checkStatus(unsigned char* status, char* buffer)
{
    *status = (0x0000FF0000000000000000 & buffer) >> 40;
    if (status == 0)
    {
        return True;
    }
    return False;
}

void extractOpcode(unsigned char* opcode, char* buffer)
{
    *opcode = (0xFFFF00000000000000000000 & buffer) >> 48;
}

int changeIntIndian(int num)
{
    return ((num>>24)&0xff) | // move byte 3 to byte 0
                        ((num<<8)&0xff0000) | // move byte 1 to byte 2
                        ((num>>8)&0xff00) | // move byte 2 to byte 1
                        ((num<<24)&0xff000000); // byte 0 to byte 3
}

SoreqResult masterRead(int size,unsigned char* buffer)
{
    unsigned char *opcode_read = OPCODE_MASTER_READ_FROM_PAYLOAD;
    int err=0;
    int i=0;
    
    do
    {
        err = I2C_write(PAYLOAD_I2C_ADDRESS,opcode_read,1);
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
            return PAYLOAD_SUCCESS;
        }
        vTaskDelay(READ_DELAY);
    }while(i++<TIMEOUT);
    return PAYLOAD_TIMEOUT;
}
SoreqResult payloadSendCommand(char opcode, int size, unsigned char* buffer,int delay)
{
	vTaskDelay(50);
	int err = I2C_write(PAYLOAD_I2C_ADDRESS, &opcode, 1);
	if(err!=0)
	{
		return PAYLOAD_I2C_Write_Error;
	}
	if(delay>0)
	{
		vTaskDelay(delay);
	}
	SoreqResult res =  masterRead(size,buffer);

	return res;
}
ADCResult retrieveADCData(unsigned char *sample[4])
{
    if ((sample & 0x00000001 == 0) && (sample & 0x00000002 == 0) && (sample & 0x00000004 == 1))
    {
        return ADC_VALID;
    }
    else if (sample & 0x00000001 == 1)
    {
        return STILL_PROCESSING;
    }
    return ADC_INVALID;
}
Boolean clearWatchdog(ActionData* action){
    unsigned char* buffer = malloc(sizeof(unsigned char)*3);
    if(buffer == NULL){
        printf("Memory allocation failed.\n")
        return FALSE;
    }
    SoreqResult res = payloadSendCommand(CLEARWATCHDOG, 3, buffer, CLEARWATCHDOG_DELAY); //TODO: What is the correct delay?
    if (res != PAYLOAD_SUCCESS)
    {
        printf("Error when trying to clear the watchdog.\n");
        free(buffer);
        return FALSE;
    }
    if(!checkStatus(&action->STATUS, buffer))
    {
        printf("Data is not ready.\n");
        free(buffer);
        return FALSE;
    }
    extractOpcode(&action->OPCODE, buffer);
    free(buffer);
    return TRUE;

}
Boolean softReset(ActionData* action){
    unsigned char* buffer = malloc(sizeof(unsigned char)*3);
    if(buffer == NULL){
        printf("Memory allocation failed.\n")
        return FALSE;
    }
    SoreqResult res = payloadSendCommand(SOFTRESET, 3, buffer, SOFTRESET_DELAY); //TODO: What is the correct delay?
    if (res != PAYLOAD_SUCCESS)
    {
        printf("Error when trying to reset the payload.\n");
        free(buffer);
        return FALSE;
    }
    if(!checkStatus(&action->STATUS, buffer))
    {
        printf("Data is not ready.\n");
        free(buffer);
        return FALSE;
    }
    extractOpcode(&action->OPCODE, buffer);
    free(buffer);
    return TRUE;
}
Boolean masterPIC32SELWrite(PIC32SELData *pic32sel)
{
    unsigned char* buffer = malloc(sizeof(unsigned char)*12);
    if(buffer == NULL){
        printf("Memory allocation failed.\n")
        return FALSE;
    }
    SoreqResult res = payloadSendCommand(READ_PIC32_RESETS, 12, buffer, PIC32SEL_DELAY);
    if (res != PAYLOAD_SUCCESS)
    {
        printf("Error when trying to read PIC32SEL data.\n");
        free(buffer);
        return FALSE;
    }
    if(!checkStatus(&pic32sel->STATUS, buffer))
    {
        printf("Data is not ready.\n");
        free(buffer);
        return FALSE;
    }
    extractOpcode(&pic32sel->OPCODE, buffer);
    pic32sel->PIC32SEL = (0x00000000FFFFFFFF00000000 & buffer) >> 16;
    pic32sel->PIC32SELBACKUP = (0x0000000000000000FFFFFFFF00 & buffer) >> 8;
    memcpy(pic32sel->COUNTPIC32SEL,pic32sel->PIC32SEL,4);
    changeIntIndian(pic32sel->COUNTPIC32SEL);
    memcpy(pic32sel->COUNTPIC32SELBACKUP,pic32sel->PIC32SELBACKUP,4);
    changeIntIndian(pic32sel->COUNTPIC32SELBACKUP);
    free(buffer);
    return TRUE;
    
}
Boolean masterPIC32SEUWrite(PIC32SEUData *pic32seu)
{
    unsigned char* buffer = malloc(sizeof(unsigned char)*3);
    if(buffer == NULL){
        printf("Memory allocation failed.\n")
        return FALSE;
    }
    SoreqResult res = payloadSendCommand(READ_PIC32_RESETS, 3, buffer, PIC32SEU_DELAY);
    if (res != PAYLOAD_SUCCESS)
    {
        printf("Error when trying to read PIC32SEU data.\n");
        free(buffer);
        return FALSE;
    }
    if(!checkStatus(&pic32seu->STATUS, buffer))
    {
        printf("Data is not ready.\n");
        free(buffer);
        return FALSE;
    }
    extractOpcode(&pic32seu->OPCODE, buffer);
    pic32seu->PIC32SEU = (0x00000000FFFFFFFF00000000 & buffer) >> 16;
    free(buffer);
    return TRUE;
}
Boolean masterRadiationWrite(RadiationData* rad)
{
    unsigned char* buffer = malloc(sizeof(unsigned char)*12); //TODO: check if we need 12 or lower? (11 Bytes).
    if(buffer == NULL){
        printf("Memory allocation failed.\n")
        return FALSE;
    }
    SoreqResult res = payloadSendCommand(READ_Radiation, 12, buffer, RADIATION_DELAY);
    if (res != PAYLOAD_SUCCESS)
    {
        printf("Error when trying to read radiation data.\n");
        free(buffer);
        return FALSE;
    }
    if(!checkStatus(&rad->STATUS, buffer))
    {
        printf("Data is not ready.\n");
        free(buffer);
        return FALSE;
    }
    
    extractOpcode(&rad->OPCODE, buffer);
    rad->RADFET1 = (0x000000FFFFFFFF0000000000 & buffer) >> 24;
    if (retrieveADCData(rad->RADFET1) != ADC_VALID) //TODO: Should we address PROCESSING?
    {
        printf("ADC data 1 is invalid.\n");
        free(buffer);
        return FALSE;
    }
    rad->RADFET2 = (0x0000000000000000FFFFFFFF00 & buffer) >> 8;
    if(retrieveADCData(rad->RADFET2) != ADC_VALID) //TODO: Should we address PROCESSING?
    {
        printf("ADC data 2 is invalid.\n");
        free(buffer);
        return FALSE;
    }
    free(buffer);
    return TRUE;
}
Boolean MasterTemperatureWrite(TemperatureData * temprature)
{
    unsigned char* buffer = malloc(sizeof(unsigned char)*12); //TODO: check if we need 12 or lower? (11 Bytes).
    if(buffer == NULL){
        printf("Memory allocation failed.\n")
        return FALSE;
    }

    SoreqResult res = payloadSendCommand(READ_Radiation, 12, buffer, 200);
    if (res != PAYLOAD_SUCCESS)
    {
        free(buffer);
        return FALSE;
    }
    if(!checkStatus(&temprature->STATUS, buffer))
    {
        free(buffer);
        return FALSE;
    }
    extractOpcode(&temprature->OPCODE, buffer);
    temprature->TEMP = (0x000000FFFFFFFF0000000000 & buffer) >> 24;
    if (retrieveADCData(temprature->TEMP) != ADC_VALID) //TODO: Should we address PROCESSING?
    {
        free(buffer);
        return FALSE;
    }
    free(buffer);
    return TRUE;
}
Boolean soreqDebugging(char buffer[12])
{
    SoreqResult res = payloadSendCommand(SOREQ_DEBUGGING, 12, buffer, 0);
    if (res != PAYLOAD_SUCCESS)
    {
        return FALSE;
    }
    unsigned char *status=malloc(sizeof(char));
    if(status == NULL){
        printf("Memory allocation failed.\n")
        return FALSE;
    }
    if(!checkStatus(status, buffer))
    {
        return FALSE;
    }
    Boolean first_check = (*buffer >> 32) & 0xFFFF == 0x3800;
    Boolean second_check = (*buffer >> 16) & 0xFFFF == 0x3200;
    Boolean third_check = (*buffer) & 0xFFFF == 0x3500;
    if(first_check && second_check && third_check)
    {
        return TRUE;
    }
    return FALSE;
}