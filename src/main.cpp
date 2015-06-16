/*
===============================================================================
 Name        : main.c
 Author      : uCXpresso
 Version     : v1.0.0
 Copyright   : www.ucxpresso.net
 License	 : MIT
 Description : A BLE UAET to Debug Console (Serial I/F) example
===============================================================================
 	 	 	 	 	 	 	 	 History
 ---------+---------+--------------------------------------------+-------------
 DATE     |	VERSION |	DESCRIPTIONS							 |	By
 ---------+---------+--------------------------------------------+-------------
 2014/10/14 v1.0.0	First Edition.									LEO
 ===============================================================================
 */

#include <uCXpresso.h>

#ifdef DEBUG
#include <debug.h>
#include <class/serial.h>
#define DBG dbg_printf
#else
#define DBG(...)
#endif

// TODO: insert other include files here
#include <class/ble/ble_device.h>
#include <class/ble/ble_service.h>
#include <class/pin.h>
#include <class/ble/ble_rssi.h>
#include <class/ble/ble_service_uart.h>
#include <class/ble/ble_conn_params.h>
#include <class/timeout.h>
#include "at_command.h"
#include "RssiFilter.h"

#define BOARD_NANO51822_UDK
#include <config/board.h>

// TODO: insert other definitions and declarations here
#define DEVICE_NAME                          "BleMeter"            /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME                    "uCXpresso.NRF"        /**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL                     1000                    /**< The advertising interval (in ms). */
#define APP_COMPANY_IDENTIFIER           	 0x0059					/**< Company identifier for Nordic Semi. as per www.bluetooth.org. */


//Declare ble device as globale
bleDevice ble;	// use internal 32.768KHz for SoftDevice
float connection_interval_min = 10;
float connection_interval_max = 50;
float rssiOutput;
bool  isRunning=false, isTesting=false;
int   runDelay=500;
CTimeout tmRun,tmTest;

//
// AT command list
//
static LPCTSTR const at_cmd_list[] {
	"&RESET",		///< 0,	    AT+RESET
	"&CONMIN=",	    ///< 1,		AT&CONMIN=xxxx
	"&CONMIN?",	    ///< 2, 	AT&CONMIN?
	"&CONMAX=",	    ///< 3,		AT&CONMIN=xxxx
	"&CONMAX?",	    ///< 4, 	AT&CONMIN?
	"&RSSI?",		///< 5,	    AT&RSSI?
	"&RUNDELAY=",	///< 6,		AT&CONMIN=xxxx
	"&RUNDELAY?",	///< 7, 	AT&CONMIN?
	"&RUN",         ///< 8,	    AT&RUN
	"&STOP",        ///< 9,     AT&STOP
	"&TEST",        ///< 10,     AT&STOP
};

#define ATCMD_COUNT	(sizeof(at_cmd_list)/sizeof(LPCTSTR))

//
// AT command handle
//
void at_command_handle(CStream *p_serial, CString &str) {
	str.makeUpper();

	CString response;

	// finds command with command list. and return the index of the command list, if found.
	switch( str.find(at_cmd_list, ATCMD_COUNT) ) {

	case 0:// reset
		system_reset();
		break;

	case 1: // set connection_interval_min
		connection_interval_min = str.parseFloat(8);
		ble.m_gap.settings(DEVICE_NAME,connection_interval_min,connection_interval_max);	// set Device Name on GAP
		response = "OK\n";
		break;

	case 2: // get connection_interval_min
		response.printf("%f\n", connection_interval_min);
		break;

	case 3: // set connection_interval_max
		connection_interval_max = str.parseFloat(8);
		ble.m_gap.settings(DEVICE_NAME,connection_interval_min,connection_interval_max);	// set Device Name on GAP
		response = "OK\n";
		break;

	case 4:// set connection_interval_max
		response.printf("%f\n", connection_interval_max);
		break;

	case 5:// get measured rssi
		response.printf("%0.2f\n", rssiOutput);
		break;

	case 6:// set runDelay
		runDelay = str.parseInt(10);
		response = "OK\n";
		break;

	case 7: // get runDelay
		response.printf("%d\n", runDelay);
		break;

	case 8:// Run simulation
		isRunning = true;
		tmRun.reset();
		break;

	case 9:// Stop Simluation
		isRunning = false;
		break;

	case 10:// Begin test
		isTesting = true;
		isRunning=false; //stop the running
		response = "WAIT\n";
		break;

	default:// command not found
		response = "ERROR\n";
		break;
	}

	// response to stream
	if (response.length()>0) {
		p_serial->write(response.getBuffer(), response.length());
	}
}

//
// Main Routine
//
int main(void) {
#ifdef DEBUG
	CSerial ser;		// declare a Serial object
	ser.enable();
	CDebug dbg(ser);	// Debug stream use the UART object
	dbg.start();
#endif

	//
	// Your Application setup code here
	//

	CPin led1(LED1);	// led1 flashes while main loop is running
	CPin led2(LED2);	// led2 on if nus is connected
	CPin led3(LED3);	// led3 on if nus/dbg is transmitting/receiving
	CPin led4(LED4);	// led4 on in Testing mode
	led1.output();	// set led1 as an output pin
	led2.output();	// set led2 as an output pin
	led3.output();	// set led3 as an output pin
	led4.output();	// set led4 as an output pin

	//
	// SoftDevice
	//
	ble.enable();	// enable BLE SoftDevice stack

	// GAP
	ble.m_gap.settings(DEVICE_NAME,connection_interval_min,connection_interval_max);	// set Device Name on GAP
	ble.m_gap.tx_power(BLE_TX_0dBm);	// set Output power

	//
	// Add RSSI service
	//
	bleRSSI rssi(ble);
	//
	// Add BLE UART Service
	//
	bleServiceUART nus(ble);			// declare a BLE "Nordic UART Service" (NUS) object

	//
	// Add "connection parameters update" negotiation. (optional)
	//
	bleConnParams conn(ble);

	//
	// BLE Advertising
	//
	ble.m_advertising.interval(APP_ADV_INTERVAL);					// set advertising interval
	ble.m_advertising.commpany_identifier(APP_COMPANY_IDENTIFIER);	// add company identifier

	ble.m_advertising.update();										// update advertising data

	// Start advertising
	ble.m_advertising.start();


	//
	// AT Command Parse
	//
	atCommand at(nus);					// set the nus stream to atCommand object.
	at.attachHandle(at_command_handle);	// set the AT command handle function.

	//
	// Filter
	//
	CRssiFilter filter;
	CTimeout	tmMain,tmUart;
	uint8_t		ch;

	//
    // Enter main loop.
	//
    while(1) {
    	at.parse(); // AT Command parse


    	if (nus.isAvailable() ) {	// check BLE NUS service
    		led2= LED_ON;

    		if ( nus.readable() ) {
    			tmUart.reset();
    			led3 = LED_ON;	// Led on
    			/* disabled if AT command is used
    			ch = nus.read();
    			dbg.putc(ch);		// echo to Debug Console
    			nus.write(ch);		// echo to BLE NUS service
    			*/
    		}
    		if ( dbg.available() ) {
    			tmUart.reset();
    			led3 = LED_ON;	// Led on
    			/* disabled if AT command is used
    			ch = dbg.read();
    			nus.write(ch);		// echo to BLE NUS service
    			dbg.putc(ch);		// echo to Debug Console
    			*/
    		}

    	} else {
    		led2 = LED_OFF;
    	}


    	//
    	// blink led
    	//
    	if ( tmMain.isExpired(500) ) {
    		tmMain.reset();
    		led1 = !led1;	// blink led

        	if ( rssi.isAvailable() ) {
        		int rssiSample=rssi.read();
        		rssiOutput = filter.Sampling(rssiSample);
        		//DBG("RSSI Sample=%d, Output=%0.2f\n", rssiSample, rssiOutput);
        	}
    	}

    	//
    	// Uart led
    	//
    	if ( tmUart.isExpired(500) ) {
    		tmUart.reset();
    		led3 = LED_OFF;	// blink led
    	}

    	// running
    	if (isRunning && tmRun.isExpired(runDelay))
    	{
    		tmRun.reset();
    		nus.write("$123456789",10,1);
    	}

    	//test
    	if (isTesting) {
    		int rxCount=0, rxTime=0;
    		CTimeout tmTest, tmWait;
			led4 = LED_ON;	// Led on
			dbg.printf("Performance Measure\n");
			while (!nus.readable()); //wait for 1st byte
    		tmTest.reset();
    		tmWait.reset();
    		while (!tmWait.isExpired(1000)) {
    			if (nus.readable()) {
    				tmWait.reset();
    				ch = nus.read();
    				rxCount++;
    				rxTime = tmTest.elapsed(); //remember the latest time
    			}
    		}
			led4 = LED_OFF;	// Led off
			CString measure;
			measure.printf("bytes=%d,time=%d, throughput=%dBytes/s\n", rxCount, rxTime, 1000*rxCount/rxTime);
			dbg.printf("%s", measure.getBuffer());
			nus.write(measure.getBuffer(),measure.length()); //echo to terminal
			isTesting = false;
    	}

    	// Negotiate the "Connect Parameters Update"
    	conn.negotiate();
    }
}
