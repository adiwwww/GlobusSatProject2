

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

#define SOREQ_I2C_ADDRESS 0x55
#define READ_PIC32_RESETS 0x66
#define READ_DELAY 200
#define TIMEOUT 4000
#define READ_PIC32_RESETS 0x66
#define PAYLOAD_I2C_ADDRESS 0x55
#define CLEAR_WDT_AND_ALLOW_READ 0x45 // in tau-1

typedef enum {
    PAYLOAD_SUCCESS, PAYLOAD_I2C_Write_Error, PAYLOAD_I2C_Read_Error, PAYLOAD_TIMEOUT
} SoreqResult;
typedef enum
{
	board, radfet, ormad, fpga
} PayloadSubSystem;

int changeIntIndian(int num)
{
	return ((num>>24)&0xff) | // move byte 3 to byte 0
	                    ((num<<8)&0xff0000) | // move byte 1 to byte 2
	                    ((num>>8)&0xff00) | // move byte 2 to byte 1
	                    ((num<<24)&0xff000000); // byte 0 to byte 3
}

SoreqResult payloadRead(int size,unsigned char* buffer)
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
			return PAYLOAD_SUCCESS;
		}
	} while(TIMEOUT/READ_DELAY > i++);
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
	SoreqResult res =  payloadRead(size,buffer);

	return res;
}


SoreqResult payloadPICRead()
{
	char buffer[12];
	int* count = 0;

	SoreqResult res = payloadSendCommand(READ_PIC32_RESETS,12, buffer,0);
	memcpy(count,buffer+4,4);
	if(*count==0)
	{
		memcpy(count,buffer+8,4);
	}
	*count = changeIntIndian(*count);
	printf("\r\n Count number %d \r\n", *count);
}

static Boolean testPayload(void){
	printf("\r\n Starting test of PIC read:\r\n");
	payloadPICRead();
	return TRUE;
}

static Boolean selectAndExecutePayloadDemoTest(void)
{
	int selection = 0;
	Boolean offerMoreTests = TRUE;

	printf( "\n\r Select a test to perform: \n\r");
	printf("\t 1) get PIC resets temp \n\r");
	printf("\t 2) Return to main menu \n\r");

	while(UTIL_DbguGetIntegerMinMax(&selection, 1, 15) == 0);

	switch(selection) {
	case 1:
		offerMoreTests = testPayload();
		break;
	case 2:
		offerMoreTests = FALSE;
		break;

	default:
		break;
	}

	return offerMoreTests;
}

void PayloadDemoLoop(void)
{
	Boolean offerMoreTests = FALSE;

	while(1)
	{
		offerMoreTests = selectAndExecutePayloadDemoTest();	// show the demo command line interface and handle commands

		if(offerMoreTests == FALSE)							// was exit/back selected?
		{
			break;
		}
	}
}

Boolean PayloadDemoInit(){
	return TRUE;
}
Boolean PayloadDemoMain(void)
{
	if(PayloadDemoInit())
	{
		PayloadDemoLoop();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

Boolean Payloadtest(void)
{
	PayloadDemoMain();
	return TRUE;
}
