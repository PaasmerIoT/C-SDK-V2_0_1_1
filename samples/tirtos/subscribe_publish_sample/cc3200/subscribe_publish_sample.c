/*
 * Copyright 2010-2016 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
#include <string.h>


#include <ti/sysbios/knl/Task.h>
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_config.h"


#include "pin_mux_config.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_gpio.h"
#include "pin.h"
#include "gpio.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"

// Driverlib includes
#include "utils.h"

#include "hw_common_reg.h"

#include "hw_adc.h"
#include "hw_ints.h"
#include "hw_gprcm.h"

#include "interrupt.h"
#include "prcm.h"
#include "uart.h"

#include "pin.h"
#include "adc.h"


#include<stdio.h>
#include<stdlib.h>
#include"config.h"

#include<string.h>
typedef struct feed
{
	struct student *pre;
	char feedname[20];
	char feedtype[10];
	char feedbase[15];
	char feedpin[5];
	struct student *next;
}FD;

FD *hptr=NULL;
void feedadd();
void print();
void add(char *feednames,char *feedtypes,char *feedbases,char *feedpins);
void add(char *feednames,char *feedtypes,char *feedbases,char *feedpins)
{
	FD *new,*old;
	new=(FD *)malloc(sizeof(FD));
	strcpy(new->feedname,feednames);
	strcpy(new->feedtype,feedtypes);
	strcpy(new->feedbase,feedbases);
        strcpy(new->feedpin,feedpins);
	if(hptr==NULL)
	{
	new->next=hptr;
	new->pre=hptr;
	hptr=new;
	}
	else
	{
	old=hptr;

	while(old->next!=NULL)
	old=old->next;	

	new->next=old->next;
	old->next=new;
	new->pre=old;
	}
}
void feedadd(){

	int i=0,j=0;
	j = sizeof(feedname)/sizeof(feedname[0]);
	printf("j value is %d\n",j);
	do{
		
		add(feedname[i],feedtype[i],feedbase[i],feedpin[i]);
		IOT_INFO("feedname=%s,feedtype=%s,feedbase=%s,feedpin=%s",feedname[i],feedtype[i],feedbase[i],feedpin[i]);//}
		i++;
		IOT_INFO("%d",i);
	}while(i<j);
	print();
}
void print()
{
	FD *ptr=hptr;
int a[3],msgscount=0;
char cPayload[1024],b[512],c[512],credentials[512];
	while(ptr){
		snprintf(cPayload,sizeof(cPayload),"\{\n\"feed1\":\"%s\",\n\"feed1type\":\"%s\",\n\"feed1base\":\"%s\",\"feed1pin\":\"%s\",\"feed1value\":\"%d\",",ptr->feedname,ptr->feedtype,ptr->feedbase,ptr->feedpin,GPIOPinRead(ptr->feedbase,ptr->feedpin));

		ptr=ptr->next;
		if(ptr)
		{
		snprintf(b,sizeof(b),"\n\"feed2\":\"%s\",\n\"feed2type\":\"%s\",\n\"feed2base\":\"%s\",\"feed2pin\":\"%s\",\"feed2value\":\"%d\",",ptr->feedname,ptr->feedtype,ptr->feedbase,ptr->feedpin,GPIOPinRead(ptr->feedbase,ptr->feedpin));

		strcat(cPayload,b);
	
		ptr=ptr->next;
		}
		else
		{
		snprintf(b,sizeof(b),"\n\"feed2\":\"\",\n\"feed2type\":\"\",\n\"feed2base\":\"\",\n\"feed2pin\":\"\",\n\"feed2value\":\"\",\n");
		strcat(cPayload,b);
		
		}
		if(ptr){
		snprintf(c,sizeof(c),"\n\"feed3\":\"%s\",\n\"feed3type\":\"%s\",\n\"feed3base\":\"%s\",\n\"feed3pin\":\"%s\",\n\"feed3value\":\"%d\"\n\}",ptr->feedname,ptr->feedtype,ptr->feedbase,ptr->feedpin,GPIOPinRead(ptr->feedbase,ptr->feedpin));
	
		strcat(cPayload,c);

		ptr=ptr->next;
		
		}
		else{
		
		snprintf(c,sizeof(c),"\n\"feed3\":\"\",\n\"feed3type\":\"\",\n\"feed3base\":\"\",\n\"feed3pin\":\"\",\n\"feed3value\":\"\"\}");
		strcat(cPayload,c);
		printf("\nEof linkedlist\n");}
		snprintf(credentials,sizeof(credentials),"messagecount = %d",msgscount);
		strcat(cPayload,credentials);
		IOT_INFO("%s",cPayload);
		strcpy(cPayload,"\0");
		msgscount++;
		
		
	}
}



char HostAddress[255] = AWS_IOT_MQTT_HOST;
uint32_t port = AWS_IOT_MQTT_PORT;
uint32_t publishCount = 0;
int sensordetails=1,msgCount=0,subInterrupt=0;
unsigned int  uiChannel;
float pw=0;

float adcread(int ch_no){
if (ch_no == 1)
	uiChannel = ADC_CH_0;
else if (ch_no == 2)
	uiChannel = ADC_CH_1;
else if (ch_no == 3)
	uiChannel = ADC_CH_2;
else if (ch_no == 4)
	uiChannel = ADC_CH_3;

#ifdef CC3200_ES_1_2_1
        //
        // Enable ADC clocks.###IMPORTANT###Need to be removed for PG 1.32
        //
        HWREG(GPRCM_BASE + GPRCM_O_ADC_CLK_CONFIG) = 0x00000043;
        HWREG(ADC_BASE + ADC_O_ADC_CTRL) = 0x00000004;
        HWREG(ADC_BASE + ADC_O_ADC_SPARE0) = 0x00000100;
        HWREG(ADC_BASE + ADC_O_ADC_SPARE1) = 0x0355AA00;
#endif

        //
        // Configure ADC timer which is used to timestamp the ADC data samples
        //
        MAP_ADCTimerConfig(ADC_BASE,2^17);

	//PinTypeADC(PIN_59, 0xFF);

        //
        // Enable ADC timer which is used to timestamp the ADC data samples
        //
        MAP_ADCTimerEnable(ADC_BASE);

        //
        // Enable ADC module
        //
        MAP_ADCEnable(ADC_BASE);

        //
        // Enable ADC channel
        //

        MAP_ADCChannelEnable(ADC_BASE, uiChannel);
	//MAP_UtilsDelay(800000);

	if(MAP_ADCFIFOLvlGet(ADC_BASE, uiChannel)){

	        unsigned long ulSample = MAP_ADCFIFORead(ADC_BASE, uiChannel);
		IOT_INFO("%lu\n",ulSample);
		IOT_INFO("%f...\n",(float)((ulSample >> 2 ) & 0x0FFF));
		IOT_INFO("\n\rVoltage is %f\n\r",(((float)((ulSample >> 2 ) & 0x0FFF))*1.467)/4096);
		pw=(((float)((ulSample >> 2 ) & 0x0FFF))*1.467)/4096;
			
	}
return pw;
}

void NetMACAddressGet(unsigned char *pMACAddress)
{
    unsigned char macAddressLen = SL_MAC_ADDR_LEN;

    //
    // Get the MAC address
    //
    sl_NetCfgGet(SL_MAC_ADDRESS_GET, NULL, &macAddressLen, pMACAddress);
	IOT_INFO("Original MAC id is %s",pMACAddress);
}


void MQTTcallbackHandler(AWS_IoT_Client *pClient, char *topicName,
        uint16_t topicNameLen, IoT_Publish_Message_Params *params, void *pData)
{
    IOT_INFO("Subscribe callback");
    subInterrupt=1;
    IOT_INFO("%.*s\t%.*s",topicNameLen, topicName, (int)params->payloadLen,
            (char *)params->payload);
	

	FD *ptrs=hptr;
	while(ptrs)
	{
		if(!strncmp(params->payload,ptrs->feedname,strlen(ptrs->feedname)))
		{
			if(strstr(params->payload,"on"))
			{
				GPIOPinWrite(ptrs->feedbase,ptrs->feedpin,ptrs->feedpin);
				IOT_INFO("%s turned on",ptrs->feedname);
				
				
			}
			else if(strstr(params->payload,"off"))
			{
				GPIOPinWrite(ptrs->feedbase,ptrs->feedpin,0x0);
				IOT_INFO("%s turned off",ptrs->feedname);
			}
			
			break;

		}


	}
	subInterrupt=0;



}

void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data)
{
    IOT_WARN("MQTT Disconnect");
    IoT_Error_t rc = SUCCESS;

    if (NULL == data) {
        return;
    }

    AWS_IoT_Client *client = (AWS_IoT_Client *)data;
    if (aws_iot_is_autoreconnect_enabled(client)) {
        IOT_INFO("Auto Reconnect is enabled, Reconnecting attempt will start now");
    }
    else {
        IOT_WARN("Auto Reconnect not enabled. Starting manual reconnect...");
        rc = aws_iot_mqtt_attempt_reconnect(client);
        if (NETWORK_RECONNECTED == rc) {
            IOT_WARN("Manual Reconnect Successful");
            IOT_WARN("Manual Reconnect Failed - %d", rc);
        }
        else {
            IOT_WARN("Manual Reconnect Failed - %d", rc);
        }
    }
}

static int livestatus=0;
void runAWSClient(void)
{

     #ifdef AWS_IOT_MY_THING_NAME
    #define AWS_IOT_MY_THING_NAME DeviceName
    #endif

    #ifdef AWS_IOT_MQTT_CLIENT_ID 
    #define AWS_IOT_MQTT_CLIENT_ID DeviceName
    #endif  
    IOT_INFO("aws is starting............");
    IoT_Error_t rc = SUCCESS;
    bool infinitePublishFlag = true;
    char topicName[100] = "sdkTest/sub";
	sprintf(topicName,"%s_%s",UserName,DeviceName);
	IOT_INFO("the topic name is %s\n ",topicName);
    int topicNameLen = strlen(topicName);
	char *pubtopicName = "sdkTest/sub";
    int pubtopicNameLen = strlen(pubtopicName);
    AWS_IoT_Client client;
    IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
    IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

    IoT_Publish_Message_Params paramsQOS0;
    IoT_Publish_Message_Params paramsQOS1;

    IOT_INFO("\nAWS IoT SDK Version %d.%d.%d-%s\n", VERSION_MAJOR, VERSION_MINOR,
            VERSION_PATCH, VERSION_TAG);


    mqttInitParams.enableAutoReconnect = false; // We enable this later below
    mqttInitParams.pHostURL = HostAddress;
    mqttInitParams.port = port;
    /*
     *  The following cert file variables are not used in this release. All
     *  cert files must exist in the "/certs" directory and be named "ca.der",
     *  "cert.der" and "key.der", as shown in the certflasher application. The
     *  ability to change this will be added in a future release.
     */
    mqttInitParams.pRootCALocation = AWS_IOT_ROOT_CA_FILENAME;
    mqttInitParams.pDeviceCertLocation = AWS_IOT_CERTIFICATE_FILENAME;
    mqttInitParams.pDevicePrivateKeyLocation = AWS_IOT_PRIVATE_KEY_FILENAME;
    mqttInitParams.mqttCommandTimeout_ms = 20000;
    mqttInitParams.tlsHandshakeTimeout_ms = 5000;
    mqttInitParams.isSSLHostnameVerify = true;
    mqttInitParams.disconnectHandler = disconnectCallbackHandler;
    mqttInitParams.disconnectHandlerData = (void *)&client;

    rc = aws_iot_mqtt_init(&client, &mqttInitParams);
    if (SUCCESS != rc) {
        IOT_ERROR("aws_iot_mqtt_init returned error : %d ", rc);
    }

    connectParams.keepAliveIntervalInSec = 10;
    connectParams.isCleanSession = true;
    connectParams.MQTTVersion = MQTT_3_1_1;
    connectParams.pClientID = AWS_IOT_MQTT_CLIENT_ID;
    connectParams.clientIDLen = (uint16_t)strlen(AWS_IOT_MQTT_CLIENT_ID);
    connectParams.isWillMsgPresent = false;

	

    IOT_INFO("Connecting...");
    rc = aws_iot_mqtt_connect(&client, &connectParams);
    if (SUCCESS != rc) {
        IOT_ERROR("Error(%d) connecting to %s:%d", rc, mqttInitParams.pHostURL,
                mqttInitParams.port);
    }

    /*
     *  Enable Auto Reconnect functionality. Minimum and Maximum time of
     *  exponential backoff are set in aws_iot_config.h:
     *  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
     *  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
     */
    rc = aws_iot_mqtt_autoreconnect_set_status(&client, true);
    if (SUCCESS != rc) {
        IOT_ERROR("Unable to set Auto Reconnect to true - %d", rc);
    }

    IOT_INFO("Subscribing...");
    rc = aws_iot_mqtt_subscribe(&client, topicName, topicNameLen, QOS0,
            MQTTcallbackHandler, NULL);
    if (SUCCESS != rc) {
        IOT_ERROR("Error subscribing (%d)", rc);
    }

    char cPayload[512];
    
	paramsQOS0.qos = QOS0;
    paramsQOS0.payload = (void *)cPayload;
    paramsQOS0.isRetained = 0;

    paramsQOS1.qos = QOS1;
    paramsQOS1.payload = (void *)cPayload;
    paramsQOS1.isRetained = 0;

    if (publishCount != 0) {
       infinitePublishFlag = false;
    }

    while ((NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc ||
            SUCCESS == rc) && (publishCount > 0 || infinitePublishFlag)) {
        rc = aws_iot_mqtt_yield(&client, 150);

        if (NETWORK_ATTEMPTING_RECONNECT == rc) {
            /* If the client is attempting to reconnect, skip rest of loop */
            continue;
        }

        IOT_INFO("-->sleep");
        Task_sleep(1000);
       /* Recalculate string len to avoid truncation in subscribe callback */
	if(UserName != "" && DeviceName !=""){
		if ((sensordetails==1)){
			if(((++livestatus)%timePeriod) == 0 || livestatus == 1)
			{
				static char mac_id[13];
				mac_id[13]="/0";
				unsigned char macAddressLen = SL_MAC_ADDR_LEN;
				unsigned char pMACAddress[macAddressLen];
	
    				//
    				// Get the MAC address
    				//
				    sl_NetCfgGet(SL_MAC_ADDRESS_GET, NULL, &macAddressLen, pMACAddress);
				IOT_INFO("Original MAC id is %x",pMACAddress);
                
		
				char feed2Details[256],feed3Details[256],Credentials[256];
				FD *ptr=hptr;
				int feed1Value=0,feed2Value=0,feed3Value=0;

				while(ptr && (subInterrupt==0)){
					if(subInterrupt==1)
					{
						break;					
					}
					
					if(GPIOPinRead(ptr->feedbase,ptr->feedpin)){
						feed1Value=1;
					}
					else{
						feed1Value=0;
					}

					snprintf(cPayload,sizeof(cPayload),"\{\n\"feeds\":[\{\"feedname\":\"%s\",\n\"feedtype\":\"%s\",\n\"feedbase\":\"%s\",\"feedpin\":\"%s\",\"feedvalue\":\"%d\"\},",ptr->feedname,ptr->feedtype,ptr->feedbase,ptr->feedpin,feed1Value);

					ptr=ptr->next;
					feed1Value=0;
					if(ptr)
					{
						if(GPIOPinRead(ptr->feedbase,ptr->feedpin)){
							feed2Value=1;
						}
						else{
							feed2Value=0;
						}
						snprintf(feed2Details,sizeof(feed2Details),"\n\{\"feedname\":\"%s\",\n\"feedtype\":\"%s\",\n\"feedbase\":\"%s\",\"feedpin\":\"%s\",\"feedvalue\":\"%d\"\},",ptr->feedname,ptr->feedtype,ptr->feedbase,ptr->feedpin,feed2Value);

						strcat(cPayload,feed2Details);
	
						ptr=ptr->next;
						feed2Value=0;
						strcpy(feed2Details,"\0");
					}
					else
					{
						snprintf(feed2Details,sizeof(feed2Details),"\n\{\"feedname\":\"\",\n\"feedtype\":\"\",\n\"feedbase\":\"\",\n\"feedpin\":\"\",\n\"feedvalue\":\"\"\},\n");
						strcat(cPayload,feed2Details);
						
					}
					if(ptr)
					{
						if(GPIOPinRead(ptr->feedbase,ptr->feedpin)){
							feed3Value=1;
						}
						else{
							feed3Value=0;
						}
						snprintf(feed3Details,sizeof(feed3Details),"\n\{\"feedname\":\"%s\",\n\"feedtype\":\"%s\",\n\"feedbase\":\"%s\",\n\"feedpin\":\"%s\",\n\"feedvalue\":\"%d\"\}],",ptr->feedname,ptr->feedtype,ptr->feedbase,ptr->feedpin,feed3Value);
					
						strcat(cPayload,feed3Details);
				
						ptr=ptr->next;
						feed3Value=0;
						strcpy(feed3Details,"\0");
						
					}
					else{
		
						snprintf(feed3Details,sizeof(feed3Details),"\n\{\"feedname\":\"\",\n\"feedtype\":\"\",\n\"feedbase\":\"\",\n\"feedpin\":\"\",\n\"feedvalue\":\"\"\}],");
						strcat(cPayload,feed3Details);
						printf("\nEof linkedlist\n");
					}
					snprintf(Credentials,sizeof(Credentials),"\"messagecount\": \"%d\",\"paasmerid\":\"%x\",\"username\":\"%s\",\"devicename\":\"%s\",\"devicetype\":\"CC3200\"\}",msgCount,mac_id,UserName,DeviceName);
					strcat(cPayload,Credentials);
					IOT_INFO("%s",cPayload);
					IOT_INFO("%d\n",msgCount);
					
					msgCount++;

		
					sprintf(pubtopicName,"%s","paasmerv2_device_online");
					pubtopicNameLen = strlen(pubtopicName);

					paramsQOS1.payloadLen = strlen(cPayload);
					do 
					{
					    rc = aws_iot_mqtt_publish(&client, pubtopicName, pubtopicNameLen,
						   &paramsQOS1);
					    if (publishCount > 0) 
			

						publishCount--;
					    strcpy(cPayload,"\0");
					}
					    
					while (MQTT_REQUEST_TIMEOUT_ERROR == rc &&
						(publishCount > 0 || infinitePublishFlag));
					
					Task_sleep(1000);	
				}
			}
			}
		
		
				}
		    
    }
			
			
    if (SUCCESS != rc) {
        IOT_ERROR("An error occurred in the loop. Error code = %d\n", rc);
    }
    else {
        IOT_INFO("Publish done\n");
}}
	

