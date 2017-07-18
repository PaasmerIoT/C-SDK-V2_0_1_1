/*
 * Copyright (c) 2014-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdbool.h>

#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/WiFi.h>

#include "Board.h"
#include "wificonfig.h"

#include <simplelink.h>
#include <osi.h>



#include "pin_mux_config.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_gpio.h"
#include "pin.h"
#include "gpio.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"


#define WLAN_DEL_ALL_PROFILES   0xFF

#if defined(MSP430WARE) || defined(MSP432WARE)
#define SPI_BIT_RATE    2000000
#elif defined(CC32XXWARE)
#define SPI_BIT_RATE    20000000
#else
#define SPI_BIT_RATE    14000000
#endif

volatile unsigned long  g_ulStatus1 = 0;//SimpleLink Status
static uint32_t deviceConnected = false;
static uint32_t ipAcquired = false;
static uint32_t currButton, prevButton;

extern void startNTP(void);
int smarttime=0;
/*
 *  ======== SimpleLinkWlanEventHandler ========
 *  SimpleLink Host Driver callback for handling WLAN connection or
 *  disconnection events.
 */
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pArgs)
{
    switch (pArgs->Event) {
        case SL_WLAN_CONNECT_EVENT:
            deviceConnected = true;
            break;

        case SL_WLAN_DISCONNECT_EVENT:
            deviceConnected = false;
            break;

        default:
            break;
    }
}

/*
 *  ======== SimpleLinkNetAppEventHandler ========
 *  SimpleLink Host Driver callback for asynchoronous IP address events.
 */
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pArgs)
{
    switch (pArgs->Event) {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
            ipAcquired = true;
            break;

        default:
            break;
    }
}

/*
 *  ======== SimpleLinkSockEventHandler ========
 */
void SimpleLinkSockEventHandler(SlSockEvent_t *pArgs)
{

}

/*
 *  ======== SimpleLinkGeneralEventHandler ========
 */
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *sockEvent)
{

}

/*
 *  ======== SimpleLinkHttpServerCallback ========
 *  SimpleLink Host Driver callback for HTTP server events.
 */
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpServerEvent,
        SlHttpServerResponse_t *pHttpServerResponse)
{
}

/*
 *  ======== smartConfigFxn ========
 */
void smartConfigFxn()
{
/*  .... Starting
System_printf("SMART CONFIG Function Called...\n");
  uint8_t policyVal;

  // Set auto connect policy 
  sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1,0,0, 0, 0),
            &policyVal, sizeof(policyVal));

  // Start SmartConfig using unsecured method. 
  sl_WlanSmartConfigStart(0, SMART_CONFIG_CIPHER_NONE, 0, 0, 0,
            NULL, NULL, NULL);
*/

unsigned char policyVal;
    long lRetVal = -1;

    // Clear all profiles 
    // This is of course not a must, it is used in this example to make sure
    // we will connect to the new profile added by SmartConfig
    //
//   if(!(smarttime == 1200))
    lRetVal = sl_WlanProfileDel(WLAN_DEL_ALL_PROFILES);
    //ASSERT_ON_ERROR(lRetVal);

    //set AUTO policy
    lRetVal = sl_WlanPolicySet(  SL_POLICY_CONNECTION,
                      SL_CONNECTION_POLICY(1,0,0,0,1),
                      &policyVal,
                      1 /*PolicyValLen*/);
    //ASSERT_ON_ERROR(lRetVal);

    // Start SmartConfig
    // This example uses the unsecured SmartConfig method
    //
    lRetVal = sl_WlanSmartConfigStart(0,                /*groupIdBitmask*/
                           SMART_CONFIG_CIPHER_NONE,    /*cipher*/
                           0,                           /*publicKeyLen*/
                           0,                           /*group1KeyLen*/
                           0,                           /*group2KeyLen */
                           NULL,                        /*publicKey */
                           NULL,                        /*group1Key */
                           NULL);                       /*group2Key*/
    //ASSERT_ON_ERROR(lRetVal);

    // Wait for WLAN Event
while ((deviceConnected != true) || (ipAcquired != true))
  //  while((!IS_CONNECTED(g_ulStatus1)) || (!IS_IP_ACQUIRED(g_ulStatus1)))
    {
	System_printf("RUNNING IN LOOP...\n");
       // _SlNonOsMainLoopTask();
    }
     //
     // Turn ON the RED LED to indicate connection success
     //
	GPIO_write(Board_LED2, Board_LED_ON);
//     GPIO_IF_LedOn(MCU_RED_LED_GPIO);
     //wait for few moments
     //MAP_UtilsDelay(80000000);
     //reset to default AUTO policy
     lRetVal = sl_WlanPolicySet(  SL_POLICY_CONNECTION,
                           SL_CONNECTION_POLICY(1,0,0,0,0),
                           &policyVal,
                           1 /*PolicyValLen*/);
     //ASSERT_ON_ERROR(lRetVal);

//     return SUCCESS;
}

/*
 *  ======== setStationMode ========
 *  Sets the SimpleLink Wi-Fi in station mode and enables DHCP client
 */
void setStationMode(void)
{
   System_printf("Station ...mode has Called from Net. WIFI Function...\n");
    int           mode;
    int           response;
    unsigned char param;
System_printf("1\n");
    mode = sl_Start(0, 0, 0);
System_printf("2\n");
    if (mode < 0) {
System_printf("3\n");
	System_printf("SL_START_ABORTED..\n");
        System_abort("Could not initialize SimpleLink Wi-Fi..");
    }
System_printf("4\n");
    /* Change network processor to station mode */
    if (mode != ROLE_STA) {
	System_printf("SL_wlanSETMODE..\n");
        sl_WlanSetMode(ROLE_STA);
System_printf("5\n");
        /* Restart network processor */
        sl_Stop(0);
        mode = sl_Start(0, 0, 0);
        if (mode < 0) {
	System_printf("SL_FAILED TO SET SIMPLE LINK WIFI...\n");
            System_abort("Failed to set SimpleLink Wi-Fi to Station mode");
        }
System_printf("6\n");
    }
	System_printf("WLAN DISCONNECTED.....\n");
    sl_WlanDisconnect();
    /* Set auto connect policy */
System_printf("7\n");
    response = sl_WlanPolicySet(SL_POLICY_CONNECTION,
            SL_CONNECTION_POLICY(1, 0, 0, 0, 0), NULL, 0);
    if (response < 0) {
System_printf("8\n");
        System_abort("Failed to set connection policy to auto");
    }
System_printf("9\n");
    /* Enable DHCP client */
    param = 1;
    response = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE, 1, 1, &param);
    if(response < 0) {
System_printf("10\n");
       System_abort("Could not enable DHCP client");
    }
System_printf("11\n");
    sl_Stop(0);

    /* Set connection variables to initial values */
    deviceConnected = false;
    ipAcquired = false;
}

/*
 *  ======== wlanConnect =======
 *  Secure connection parameters
 */
static int wlanConnect()
{
System_printf("WLAN CONNECTION FUNCTION CALLED. with ..\n");
System_printf("WLAN CONNECTION FUNCTION CALLED. with %s. %s.\n",(signed char*)SSID,(signed char *)SECURITY_KEY);
    SlSecParams_t secParams = {0};
    int ret = 0;

    secParams.Key = (signed char *)SECURITY_KEY;
    secParams.KeyLen = strlen((const char *)secParams.Key);
    secParams.Type = SECURITY_TYPE;

    ret = sl_WlanConnect((signed char*)SSID, strlen((const char*)SSID),
            NULL, &secParams, NULL);

    return (ret);
}

/*
 *  ======== NetWifi_init =======
 *  Initialize Wifi
 */
void NetWiFi_init()
{

System_printf("CC3200 has Called from main.c...\n");
    WiFi_Params wifiParams;
    WiFi_Handle handle;

    SlNetCfgIpV4Args_t ipV4;
    uint8_t           len = sizeof(ipV4);
    uint8_t           dhcpIsOn;

    /* Turn LED OFF. It will be used as a connection indicator */
    GPIO_write(Board_LED0, Board_LED_OFF);
    GPIO_write(Board_LED1, Board_LED_OFF);
    GPIO_write(Board_LED2, Board_LED_OFF);
    //GPIOPinWrite(GPIOA3_BASE, 0x40, 0x40);

    /* Open WiFi driver */
    WiFi_Params_init(&wifiParams);
    wifiParams.bitRate = SPI_BIT_RATE;
    handle = WiFi_open(Board_WIFI, Board_WIFI_SPI, NULL, &wifiParams);
    if (handle == NULL) {
	System_printf("WIFI DRIVER PROBLEM...\n");
        System_abort("WiFi driver failed to open.");
    }

    setStationMode();

    System_printf("Seting Station Mode Over...\n");

    /* Host driver starts the network processor */
    if (sl_Start(NULL, NULL, NULL) < 0) {
        System_abort("Could not initialize WiFi");
    }
System_printf("12 -> Conecting to Wifi Router.. and gettinf IP...\n");

    if (wlanConnect() < 0) {
        //System_abort("Could not connect to WiFi AP");
System_printf("13\n");
    }
System_printf("14\n");
    /*
     * Wait for the WiFi to connect to an AP. If a profile for the AP in
     * use has not been stored yet, press Board_BUTTON0 to start SmartConfig.
     */
    int wificount =1;
    while ((deviceConnected != true) || (ipAcquired != true)) {

 
        /*
         *  Start SmartConfig if a button is pressed. This could be done with
         *  GPIO interrupts, but for simplicity polling is used to check the
         *  button.
         */
System_printf("15\n");
        currButton = GPIO_read(Board_BUTTON0);
        //currButton = GPIOPinRead(GPIOA2_BASE, 0x2); 
    //    if (((currButton == 0) && (prevButton != 0)) || ++smarttime == 1200) {
        if ((currButton == 0) && (prevButton != 0)) {
		System_printf("16\n");
                GPIOPinWrite(GPIOA3_BASE, 0x40, 0x0);
	        Task_sleep(50);
		GPIOPinWrite(GPIOA3_BASE, 0x40, 0x40);
	        Task_sleep(50);
		GPIOPinWrite(GPIOA3_BASE, 0x40, 0x0);
	        Task_sleep(50);
		GPIOPinWrite(GPIOA3_BASE, 0x40, 0x40);
            	smartConfigFxn();
        }
        prevButton = currButton;
        
        Task_sleep(50);
System_printf("17\n");
wificount++;
if(wificount > 3000){
		System_printf("Taking too much time to connect to wifi network. please check your wifi credentials or wifi network speed\n");
		Task_sleep(1000);
    }

    /* Retrieve & print the IP address */
    sl_NetCfgGet(SL_IPV4_STA_P2P_CL_GET_INFO, &dhcpIsOn, &len,
            (unsigned char *)&ipV4);
    System_printf("CC3200 has ..connected to AP and acquired an IP address.\n");
    System_printf("IP Address: %d.%d.%d.%d\n", SL_IPV4_BYTE(ipV4.ipV4,3),
            SL_IPV4_BYTE(ipV4.ipV4,2), SL_IPV4_BYTE(ipV4.ipV4,1),
            SL_IPV4_BYTE(ipV4.ipV4,0));
    System_flush();

    /* Use NTP to get the current time, as needed for SSL authentication */
    startNTP();
System_printf("18\n");

    //GPIO_write(Board_LED0, Board_LED_ON);
    //GPIOPinWrite(GPIOA3_BASE, 0x40, 0x0);
}

/*
 *  ======== NetWifi_isConnected =======
 */
uint32_t NetWiFi_isConnected()
{
    return (deviceConnected);
}

/*
 *  ======== NetWifi_exit =======
 */
void NetWiFi_exit()
{
}
