/*
 * IsisTRXVUdemo.c
 *
 *  Created on: 6 feb. 2015
 *      Author: malv
 */

#include "common.h"
#include "modules/m_gomeps.h"
#include "modules/m_trxvu.h"

#include "input.h"
#include "utils/menu_selection.h"
#include "trxvu_frame_ready.h"
#include "config/i2c_address.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <at91/utility/exithandler.h>
#include <at91/commons.h>
#include <at91/utility/trace.h>
#include <at91/peripherals/cp15/cp15.h>

#include <hal/Utility/util.h>
#include <hal/Timing/WatchDogTimer.h>
#include <hal/Drivers/I2C.h>
#include <hal/Drivers/LED.h>
#include <hal/boolean.h>
#include <hal/errors.h>

#include <satellite-subsystems/IsisTRXVU.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if USING_GOM_EPS == 1
#include <SatelliteSubsystems/GomEPS.h>
#endif

////General Variables
#define TX_UPBOUND				30
#define TIMEOUT_UPBOUND			10

#define SIZE_RXFRAME	30
#define SIZE_TXFRAME	235

static xSemaphoreHandle trxvuInterruptTrigger = NULL;

// Test Function
static Boolean softResetVUTest(void)
{
	printf("\r\n Soft Reset of both receiver and transmitter microcontrollers \r\n");
	print_error(IsisTrxvu_componentSoftReset(0, trxvu_rc));
	vTaskDelay(1 / portTICK_RATE_MS);
	print_error(IsisTrxvu_componentSoftReset(0, trxvu_tc));

	return TRUE;
}

static Boolean hardResetVUTest(void)
{
	printf("\r\n Hard Reset of both receiver and transmitter microcontrollers \r\n");
	print_error(IsisTrxvu_componentHardReset(0, trxvu_rc));
	vTaskDelay(1 / portTICK_RATE_MS);
	print_error(IsisTrxvu_componentHardReset(0, trxvu_tc));

	return TRUE;
}

static Boolean vutc_sendDefClSignTest(void)
{
	//Buffers and variables definition
	unsigned char testBuffer1[10]  = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x40};
	unsigned char txCounter = 0;
	unsigned char avalFrames = 0;
	unsigned int timeoutCounter = 0;

	while(txCounter < 5 && timeoutCounter < 5)
	{
		printf("\r\n Transmission of single buffers with default callsign. AX25 Format. \r\n");
		print_error(IsisTrxvu_tcSendAX25DefClSign(0, testBuffer1, 10, &avalFrames));

		if ((avalFrames != 0)&&(avalFrames != 255))
		{
			printf("\r\n Number of frames in the buffer: %d  \r\n", avalFrames);
			txCounter++;
		}
		else
		{
			vTaskDelay(100 / portTICK_RATE_MS);
			timeoutCounter++;
		}
	}

	return TRUE;
}

static Boolean vutc_sendInputTest(void)
{
	//Buffers and variables definition
	unsigned char buffer[16]  = {0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x40,' ', ' ', '!', '!', 0};
	unsigned char txCounter = 0;
	unsigned char avalFrames = 0;
	unsigned int timeoutCounter = 0;

	printf("\r\n Transmission of user input. \r\n");
	printf("Input a up to 10 hex numbers: \r\n");
	GetHexBuffer(buffer, ARRAY_SIZE(buffer)-1);

	while(txCounter < 5 && timeoutCounter < 5)
	{
		printf("Transmitting [%d]: %s\r\n", txCounter, buffer);
		int error_result = IsisTrxvu_tcSendAX25DefClSign(0, buffer, ARRAY_SIZE(buffer)*sizeof(int), &avalFrames);
		print_error(error_result);

		if ((avalFrames != 0)&&(avalFrames != 255))
		{
			printf("\r\n Number of frames in the buffer: %d  \r\n", avalFrames);
			++txCounter;
		}
		else
		{
			vTaskDelay(200 / portTICK_RATE_MS);
			++timeoutCounter;
		}
		vTaskDelay(100 / portTICK_RATE_MS);
	}

	return TRUE;
}

static Boolean vutc_toggleIdleStateTest(void)
{
	static Boolean toggle_flag = 0;

	if(toggle_flag)
	{
	    print_error(IsisTrxvu_tcSetIdlestate(0, trxvu_idle_state_off));
		toggle_flag = FALSE;
	}
	else
	{
	    print_error(IsisTrxvu_tcSetIdlestate(0, trxvu_idle_state_on));
		toggle_flag = TRUE;
	}

	return TRUE;
}

static Boolean vutc_setTxBitrate9600Test(void)
{
    print_error(IsisTrxvu_tcSetAx25Bitrate(0, trxvu_bitrate_9600));

	return TRUE;
}

static Boolean vutc_setTxBitrate1200Test(void)
{
    print_error(IsisTrxvu_tcSetAx25Bitrate(0, trxvu_bitrate_1200));

	return TRUE;
}

static Boolean vurc_getFrameCountTest(void)
{
	unsigned short RxCounter = 0;
	unsigned int timeoutCounter = 0;

	while(timeoutCounter < 4*TIMEOUT_UPBOUND)
	{
	    print_error(IsisTrxvu_rcGetFrameCount(0, &RxCounter));

		timeoutCounter++;

		vTaskDelay(10 / portTICK_RATE_MS);
	}
	printf("\r\n There are currently %d frames waiting in the RX buffer \r\n", RxCounter);

	return TRUE;
}

static Boolean vurc_getFrameCmdTest(void)
{
	unsigned short RxCounter = 0;
	unsigned int i = 0;
	unsigned char rxframebuffer[SIZE_RXFRAME] = {0};
	ISIStrxvuRxFrame rxFrameCmd = {0,0,0, rxframebuffer};

	print_error(IsisTrxvu_rcGetFrameCount(0, &RxCounter));

	printf("\r\nThere are currently %d frames waiting in the RX buffer\r\n", RxCounter);

	while(RxCounter > 0)
	{
		print_error(IsisTrxvu_rcGetCommandFrame(0, &rxFrameCmd));

		printf("Size of the frame is = %d \r\n", rxFrameCmd.rx_length);

		printf("Frequency offset (Doppler) for received frame: %.2f Hz\r\n", ((double)rxFrameCmd.rx_doppler) * 13.352 - 22300.0); // Only valid for rev. B & C boards
		printf("Received signal strength (RSSI) for received frame: %.2f dBm\r\n", ((double)rxFrameCmd.rx_rssi) * 0.03 - 152);

		printf("The received frame content is = ");

		for(i = 0; i < rxFrameCmd.rx_length; i++)
		{
			printf("%02x ", rxFrameCmd.rx_framedata[i]);
		}
		printf("\r\n");

		print_error(IsisTrxvu_rcGetFrameCount(0, &RxCounter));

		vTaskDelay(10 / portTICK_RATE_MS);
	}

	return TRUE;
}

static Boolean vurc_getFrameCmdAndTxTest(void)
{
	unsigned short RxCounter = 0;
	unsigned int i = 0;
	unsigned char rxframebuffer[SIZE_RXFRAME] = {0};
	unsigned char trxvuBuffer = 0;
	ISIStrxvuRxFrame rxFrameCmd = {0,0,0, rxframebuffer};

	print_error(IsisTrxvu_rcGetFrameCount(0, &RxCounter));

	printf("\r\nThere are currently %d frames waiting in the RX buffer\r\n", RxCounter);

	while(RxCounter > 0)
	{
		print_error(IsisTrxvu_rcGetCommandFrame(0, &rxFrameCmd));

		printf("Size of the frame is = %d \r\n", rxFrameCmd.rx_length);

		printf("Frequency offset (Doppler) for received frame: %.2f Hz\r\n", ((double)rxFrameCmd.rx_doppler) * 13.352 - 22300.0); // Only valid for rev. B & C boards
		printf("Received signal strength (RSSI) for received frame: %.2f dBm\r\n", ((double)rxFrameCmd.rx_rssi) * 0.03 - 152);

		rxframebuffer[26] = '-';
		rxframebuffer[27] = 'O';
		rxframebuffer[28] = 'B';
		rxframebuffer[29] = 'C';

		IsisTrxvu_tcSendAX25DefClSign(0, rxframebuffer, SIZE_RXFRAME, &trxvuBuffer);

		printf("The received frame content is = ");

		for(i = 0; i < rxFrameCmd.rx_length; i++)
		{
			printf("%02x ", rxFrameCmd.rx_framedata[i]);
		}
		printf("\r\n");

		print_error(IsisTrxvu_rcGetFrameCount(0, &RxCounter));

		vTaskDelay(10 / portTICK_RATE_MS);
	}

	return TRUE;
}

static void vurc_revDInterruptCallback()
{
    portBASE_TYPE higherPriorityTaskWoken = pdFALSE;
    //Because the callback is from an interrupt context, we need to use FromISR
    xSemaphoreGiveFromISR(trxvuInterruptTrigger, &higherPriorityTaskWoken);
    //This forces a context switch to a now woken task
    // This should improve the response time for incoming packets
    if (higherPriorityTaskWoken == pdTRUE)
    {
        portYIELD_FROM_ISR();
    }
}

static Boolean vurc_getFrameCmdInterruptTest(void)
{
    //Using a binary semaphore for syncronization between the interrupt and this task
    vSemaphoreCreateBinary(trxvuInterruptTrigger);
    //Create will do a give by itself. We need to counteract that
    xSemaphoreTake(trxvuInterruptTrigger, 0);
    //Enable the interrupt by giving it the callback we want to use
    TRXVU_FR_Enable(vurc_revDInterruptCallback);

    unsigned short RxCounter = 0;
    unsigned int i = 0;
    unsigned char rxframebuffer[SIZE_RXFRAME] = {0};
    ISIStrxvuRxFrame rxFrameCmd = {0,0,0, rxframebuffer};
    Boolean continueRunning = TRUE;

    //Using a do-while loop here to ensure we empty the TRXVU of frames before
    // we wait for interrupts
    do
    {
        printf("\r\nFrame ready pin = %d\r\n", TRXVU_FR_IsReady());

        print_error(IsisTrxvu_rcGetFrameCount(0, &RxCounter));

        printf("There are currently %d frames waiting in the RX buffer\r\n", RxCounter);

        for (; RxCounter > 0; RxCounter--)
        {
            print_error(IsisTrxvu_rcGetCommandFrame(0, &rxFrameCmd));

            printf("Size of the frame is = %d \r\n", rxFrameCmd.rx_length);

            printf("Frequency offset (Doppler) for received frame: %.2f Hz\r\n", ((double)rxFrameCmd.rx_doppler) * 13.352 - 22300.0); // Only valid for rev. B & C boards
            printf("Received signal strength (RSSI) for received frame: %.2f dBm\r\n", ((double)rxFrameCmd.rx_rssi) * 0.03 - 152);

            printf("The received frame content is = ");

            for(i = 0; i < rxFrameCmd.rx_length; i++)
            {
                printf("%02x ", rxFrameCmd.rx_framedata[i]);
            }
            printf("\r\n");
        }
        //Ensure some time is passing after last frame retrieve to ensure
        // the frame ready pin has sufficient time to go low
        // The wait requirement is about 0.5ms. To ensure that we actually need
        // to use 2ms as the argument here. That will result in 1-2ms wait
        vTaskDelay(2 / portTICK_RATE_MS);

        //In some cases, new frames arriving while processing the existing ones
        // This will essentially emulate the interrupt occurring again
        if(TRXVU_FR_IsReady())
        {
            xSemaphoreGive(trxvuInterruptTrigger);
        }

        //Check for a new interrupt, and letting it timeout every 10th second.
        // Blocking using portMAX_DELAY is possible, but it is not recommended
        // to rely solely on the pin and interrupts. For demo purposes we do
        // however rely on the pin and the interrupts, and instead check for
        // user input in order to be able to escape the demo
        printf("Press ESC if you want to stop the loop (you may have to wait 10 second)\r\n");
        do
        {
            if (DBGU_IsRxReady() && DBGU_GetChar() == 0x1B)
            {
                continueRunning = FALSE;
                printf("Exiting the demo\r\n");
            }
        }
        while (continueRunning
                && xSemaphoreTake(trxvuInterruptTrigger, 10000 / portTICK_RATE_MS) == pdFALSE);
    }
    while(continueRunning);

    TRXVU_FR_Disable();
    vSemaphoreDelete(trxvuInterruptTrigger);

    return TRUE;
}

static Boolean vurc_getRxTelemTest_revD(void)
{
	unsigned short telemetryValue;
	float eng_value = 0.0;
	ISIStrxvuRxTelemetry telemetry;
	int rv;

	// Telemetry values are presented as raw values
	printf("\r\nGet all Telemetry at once in raw values \r\n\r\n");
	rv = IsisTrxvu_rcGetTelemetryAll(0, &telemetry);
	if(rv)
	{
		printf("Subsystem call failed. rv = %d", rv);
		return TRUE;
	}

	telemetryValue = telemetry.fields.rx_doppler;
	eng_value = ((float)telemetryValue) * 13.352 - 22300;
	printf("Receiver doppler = %f Hz\r\n", eng_value);

	telemetryValue = telemetry.fields.rx_rssi;
	eng_value = ((float)telemetryValue) * 0.03 - 152;
	printf("Receiver RSSI = %f dBm\r\n", eng_value);

	telemetryValue = telemetry.fields.bus_volt;
	eng_value = ((float)telemetryValue) * 0.00488;
	printf("Bus voltage = %f V\r\n", eng_value);

	telemetryValue = telemetry.fields.vutotal_curr;
	eng_value = ((float)telemetryValue) * 0.16643964;
	printf("Total current = %f mA\r\n", eng_value);

	telemetryValue = telemetry.fields.vutx_curr;
	eng_value = ((float)telemetryValue) * 0.16643964;
	printf("Transmitter current = %f mA\r\n", eng_value);

	telemetryValue = telemetry.fields.vurx_curr;
	eng_value = ((float)telemetryValue) * 0.16643964;
	printf("Receiver current = %f mA\r\n", eng_value);

	telemetryValue = telemetry.fields.vupa_curr;
	eng_value = ((float)telemetryValue) * 0.16643964;
	printf("PA current = %f mA\r\n", eng_value);

	telemetryValue = telemetry.fields.pa_temp;
	eng_value = ((float)telemetryValue) * -0.07669 + 195.6037;
	printf("PA temperature = %f degC\r\n", eng_value);

	telemetryValue = telemetry.fields.board_temp;
	eng_value = ((float)telemetryValue) * -0.07669 + 195.6037;
	printf("Local oscillator temperature = %f degC\r\n", eng_value);

	return TRUE;
}

static Boolean vutc_getTxTelemTest_revD(void)
{
	unsigned short telemetryValue;
	float eng_value = 0.0;
	ISIStrxvuTxTelemetry telemetry;
	int rv;

	// Telemetry values are presented as raw values
	printf("\r\nGet all Telemetry at once in raw values \r\n\r\n");
	rv = IsisTrxvu_tcGetTelemetryAll(0, &telemetry);
	if(rv)
	{
		printf("Subsystem call failed. rv = %d", rv);
		return TRUE;
	}

	telemetryValue = telemetry.fields.tx_reflpwr;
	printf("RF reflected power ADC: %d.\r\n", telemetryValue);
	eng_value = ((float)(telemetryValue * telemetryValue)) * 5.887E-5;
	printf("RF reflected power = %f mW\r\n", eng_value);

	telemetryValue = telemetry.fields.tx_fwrdpwr;
	printf("RF forward power ADC: %d.\r\n", telemetryValue);
	eng_value = ((float)(telemetryValue * telemetryValue)) * 5.887E-5;
	printf("RF forward power = %f mW\r\n", eng_value);

	telemetryValue = telemetry.fields.bus_volt;
	eng_value = ((float)telemetryValue) * 0.00488;
	printf("Bus voltage = %f V\r\n", eng_value);

	telemetryValue = telemetry.fields.vutotal_curr;
	eng_value = ((float)telemetryValue) * 0.16643964;
	printf("Total current = %f mA\r\n", eng_value);

	telemetryValue = telemetry.fields.vutx_curr;
	eng_value = ((float)telemetryValue) * 0.16643964;
	printf("Transmitter current = %f mA\r\n", eng_value);

	telemetryValue = telemetry.fields.vurx_curr;
	eng_value = ((float)telemetryValue) * 0.16643964;
	printf("Receiver current = %f mA\r\n", eng_value);

	telemetryValue = telemetry.fields.vupa_curr;
	eng_value = ((float)telemetryValue) * 0.16643964;
	printf("PA current = %f mA\r\n", eng_value);

	telemetryValue = telemetry.fields.pa_temp;
	eng_value = ((float)telemetryValue) * -0.07669 + 195.6037;
	printf("PA temperature = %f degC\r\n", eng_value);

	telemetryValue = telemetry.fields.board_temp;
	eng_value = ((float)telemetryValue) * -0.07669 + 195.6037;
	printf("Local oscillator temperature = %f degC\r\n", eng_value);

	return TRUE;
}

static Boolean SendTextMessage(void)
{
	unsigned char buffer[32] = {'?', 0};
	printf("Enter a message(30): \r\n");
	INPUT_GetSTRING("Enter message: ", (char*)buffer, ARRAY_SIZE(buffer)-1);
	unsigned int repeats = INPUT_GetUINT8("How many times do you want to send this: ");
	printf("Will send: %s \r\n", buffer);
	printf("    Times: %d \r\n", repeats);

	unsigned char txCounter = 0;
	unsigned char avalFrames = 0;
	unsigned int timeoutCounter = 0;

	while(txCounter < repeats && timeoutCounter < repeats)
	{
		printf("Transmitting [%d]: %s\r\n", txCounter, buffer);
		int error_result = IsisTrxvu_tcSendAX25DefClSign(0, buffer, ARRAY_SIZE(buffer)*sizeof(int), &avalFrames);
		print_error(error_result);

		if ((avalFrames != 0)&&(avalFrames != 255))
		{
			printf("\r\n Number of frames in the buffer: %d  \r\n", avalFrames);
			++txCounter;
		}
		else
		{
			vTaskDelay(200 / portTICK_RATE_MS);
			++timeoutCounter;
		}
		vTaskDelay(100 / portTICK_RATE_MS);
	}

	return TRUE;
}

static Boolean activateResponderTest(void)
{
	printf("Activate the responder \r\n");
	if (trxvu_activate_responder()){
		printf("Responder is ON.\r\n");
	} else {
		printf("Could not turn responder ON.\r\n");
	}

	return TRUE;
}

static Boolean deActivateResponderTest(void)
{
	printf("Deactivate the responder \r\n");
	if (trxvu_deactivate_responder()){
		printf("Responder is OFF.\r\n");
	} else {
		printf("Could not turn responder OFF.\r\n");
	}
	return TRUE;
}

static void ShutdownResponderTask(void *parameters)
{
	int milli_seconds = *(unsigned int*) parameters;
	vTaskDelay(milli_seconds / portTICK_RATE_MS);
	if (trxvu_deactivate_responder()){
		printf("Responder deactivated by task.\r\n");
	} else {
		printf("Task could not turn responder OFF.\r\n");
	}
	vTaskDelete(NULL);
}

static Boolean activateResponderAutoTest(void)
{
	// DOC: ISIS.TrxVU.ICD.001_v1.6 - TRXVU Interface Control Document_revD
	// DOC: ISIS-TRXVU-ICD-00001A-ANNEX_A_TRXVU_transponder_mode-1_0

	static unsigned int time_active;
	static xTaskHandle deactivateResponderTaskHandle = NULL;

	printf("Activate the responder \r\n");
	time_active = INPUT_GetUINT32("Enter period in minutes to activate the responder: ");
	time_active *= 60 * 1000;
	printf("Activating the responder for %d milliseconds.\r\n", time_active);
	// activate responder
	if (trxvu_activate_responder()){
		printf("Responder is ON.\r\n");
		xTaskCreate(ShutdownResponderTask,(signed char*)"Responder Shutdown", 512, &time_active, tskIDLE_PRIORITY, &deactivateResponderTaskHandle );
	} else {
		printf("Could not turn responder ON.\r\n");
	}

	return TRUE;
}

static Boolean responder_set_rssi(void)
{
	uint16_t rssi = INPUT_GetUINT16("Enter an RSSI value between 0 and 4095: ");
	trxvu_set_responder_rssi_threshold(rssi);
	return TRUE;
}

static Boolean beacon_test(void)
{
	static unsigned char fromCallSign[7] = {'I', 'K', 'Q', 'H', 'S', '1', 0};
	static unsigned char toCallSign[7] = {'I', 'K', 'Q', 'G', 'S', '2', 0};
	static unsigned char data[82] = "KQ Cube Sat Lost in Space, abducted by grumpy old green aliens!";
	printf("TRXVU Beacon test.\r\n");
	INPUT_GetSTRING("Beacon Message (max 80 chars): ", (char*)data, sizeof(data));
	unsigned short interval = INPUT_GetUINT16("Becon interval in seconds: ");
	int r = IsisTrxvu_tcSetAx25BeaconOvrClSign(0, fromCallSign, toCallSign, data, sizeof(data), interval);
	print_error(r);
	return TRUE;
}

static Boolean clear_beacon_test(void)
{
	int r = IsisTrxvu_tcClearBeacon(0);
	print_error(r);
	return TRUE;
}

static Boolean eps_basic_hk_tests(void)
{
	gom_eps_hk_basic_t telemetry;
	gom_eps_get_basic_HK_data(&telemetry);

	printf("Bat Temperature %d C.\r\n", telemetry.fields.temp[4]);
	printf("Bat Temperature %d C.\r\n", telemetry.fields.temp[5]);
	printf("Battery Mode = %d\r\n", telemetry.fields.battmode);

	return TRUE;
}

Boolean eps_beacon_hk_tests(void)
{
	EPS_HK_Data data;
	if (gom_eps_get_HK(&data)) {
		printf("vBat: %d\r\n", data.bv);
		int r = IsisTrxvu_tcSetAx25BeaconDefClSign(0, (unsigned char*)&data, sizeof(data), 30);
		if (r) {
			printf("Beacon was started");
		} else {
			print_error(r);
		}
	}

	return TRUE;
}

static MenuAction trxvu_menu[] = {
			{ softResetVUTest, "Soft Reset TRXVU both microcontrollers"},
			{ hardResetVUTest, "Hard Reset TRXVU both microcontrollers"},
			{ vutc_sendDefClSignTest, "Default Callsign Send Tes"},
			{ vutc_toggleIdleStateTest, "Toggle Idle state"},
			{ vutc_setTxBitrate9600Test, "Change transmission bitrate to 9600 "},
			{ vutc_setTxBitrate1200Test, "Change transmission bitrate to 1200"},
			{ vurc_getFrameCountTest, "Get frame count"},
			{ vurc_getFrameCmdTest, "Get command frame"},
			{ vurc_getFrameCmdAndTxTest, "Get command frame and retransmit"},
			{ vurc_getFrameCmdInterruptTest, "(revD) Get command frame by interrupt"},
			{ vurc_getRxTelemTest_revD, "(revD) Get receiver telemetry"},
			{ vutc_getTxTelemTest_revD, "(revD) Get transmitter telemetry"},
			{ vutc_sendInputTest, "Transmit Hex message written by user"},
			{ SendTextMessage, "Transmit text message written by user" },
			{ activateResponderTest, "Activate responder" },
			{ deActivateResponderTest, "Deactivate responder" },
			{ activateResponderAutoTest, "Activate responder for time interval" },
			{ responder_set_rssi, "Set Responder RSSI Threshold test"},
			{ beacon_test, "Beacon Test"},
			{ clear_beacon_test, "Clear the beacon"},
			{ eps_basic_hk_tests, "Get EPS Basic HK Data - GOMSpace"},
			{ eps_beacon_hk_tests, "Send Beacon with HK Data - GOMSpace"},
			MENU_ITEM_END
};

Boolean TRXVUtest(void)
{
	// Assume I2C and IsisTRXVU subsystem drivers already initialized
	MenuDisplay(trxvu_menu);
	return TRUE;
}
