/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

/**
 * @file subscribe_publish_sample.c
 * @brief simple MQTT publish and subscribe on the same topic
 *
 * This example takes the parameters from the aws_iot_config.h file and establishes a connection to the AWS IoT MQTT Platform.
 * It subscribes and publishes to the same topic - "sdkTest/sub"
 *
 * If all the certs are correct, you should see the messages received by the application in a loop.
 *
 * The application takes in the certificate path, host name , port and the number of times the publish should happen.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <jsmn.h>


#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"

#include "config.h"
#include "mac.h"
#include "paasmerGpio.h"



int sensordetails=1,msgCount=0;
/////added for new architecture
typedef struct feed
{
	struct feed *pre;
	char feedname[20];
	char feedtype[10];
	int feedpin;
	struct feed *next;
}FD;

FD *hptr=NULL;
void feedadd();
void print();
void add(char *feednames,char *feedtypes,int feedpins);
void add(char *feednames,char *feedtypes,int feedpins)
{
	FD *new,*old;
	new=(FD *)malloc(sizeof(FD));
	strcpy(new->feedname,feednames);
	strcpy(new->feedtype,feedtypes);
        new->feedpin=feedpins;
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
		//if(feednamess[i]){
		add(feedname[i],feedtype[i],feedpin[i]);
		IOT_INFO("feedname=%s,feedtype=%s,feedpin=%d",feedname[i],feedtype[i],feedpin[i]);//}
		if(!strcmp(feedtype[i],"actuator"))
			gpioModesetup(feedpin[i],"OUT");
		else
			gpioModesetup(feedpin[i],"IN");
		i++;
		IOT_INFO("%d",i);
	}while(i<j);
	print();
	#ifdef AWS_IOT_MY_THING_NAME
	    #define AWS_IOT_MY_THING_NAME DeviceName
	#endif

	#ifdef AWS_IOT_MQTT_CLIENT_ID 
	    #define AWS_IOT_MQTT_CLIENT_ID DeviceName
	#endif
}
void print()
{
	FD* ptr=hptr;
//	struct feed* ptr=hptr;
int a[3],msgscount=0;
char cPayload[1024],b[512],c[512],credentials[512];
	while(ptr){
		IOT_INFO("this is in print function");
		snprintf(cPayload,sizeof(cPayload),"{\n\"feed1\":\"%s\",\n\"feed1type\":\"%s\",\n\"feed1pin\":\"%d\",\"feed1value\":\"%d\",",ptr->feedname,ptr->feedtype,ptr->feedpin,ptr->feedpin);
//		a[0]=1;
		printf("%s\n",cPayload);
		ptr=ptr->next;
		if(ptr)
		{
		IOT_INFO("this is in second node");
		snprintf(b,sizeof(b),"\n\"feed2\":\"%s\",\n\"feed2type\":\"%s\",\n\"feed2pin\":\"%d\",\"feed2value\":\"%d\",",ptr->feedname,ptr->feedtype,ptr->feedpin,ptr->feedpin);

		strcat(cPayload,b);
		printf("%s",cPayload);
		ptr=ptr->next;
		}
		else
		{
		snprintf(b,sizeof(b),"\n\"feed2\":\"\",\n\"feed2type\":\"\",\n\"feed2pin\":\"\",\n\"feed2value\":\"\",\n");
		strcat(cPayload,b);
		
		}
		if(ptr){
		snprintf(c,sizeof(c),"\n\"feed3\":\"%s\",\n\"feed3type\":\"%s\",\n\"feed3pin\":\"%d\",\n\"feed3value\":\"%d\"\n}",ptr->feedname,ptr->feedtype,ptr->feedpin,ptr->feedpin);
	
		strcat(cPayload,c);
		printf("%s",cPayload);
		//printf("%s",c);
		ptr=ptr->next;
		
		}
		else{
		
		snprintf(c,sizeof(c),"\n\"feed3\":\"\",\n\"feed3type\":\"\",\n\"feed3pin\":\"\",\n\"feed3value\":\"\"}");
		strcat(cPayload,c);
		printf("\nEof linkedlist\n");}
		snprintf(credentials,sizeof(credentials),"messagecount = %d",msgscount);
		strcat(cPayload,credentials);
		IOT_INFO("%s",cPayload);
		strcpy(cPayload,"\0");
		msgscount++;
		
		
	}
}



/**
 * @brief Default cert location
 */
char certDirectory[PATH_MAX + 1] = "../../../certs";

/**
 * @brief Default MQTT HOST URL is pulled from the aws_iot_config.h
 */
char HostAddress[255] = AWS_IOT_MQTT_HOST;

/**
 * @brief Default MQTT port is pulled from the aws_iot_config.h
 */
uint32_t port = AWS_IOT_MQTT_PORT;

/**
 * @brief This parameter will avoid infinite loop of publish and exit the program after certain number of publishes
 */
uint32_t publishCount = 0;

void iot_subscribe_callback_handler(AWS_IoT_Client *pClient, char *topicName, uint16_t topicNameLen,
									IoT_Publish_Message_Params *params, void *pData) {
	IOT_UNUSED(pData);
	IOT_UNUSED(pClient);
	IOT_INFO("Subscribe callback");
	IOT_INFO("%.*s\t%.*s", topicNameLen, topicName, (int) params->payloadLen, params->payload);

////added for new architecture to control controlFeeds
	FD *ptrs=hptr;
	while(ptrs)
	{
		if(!strncmp(params->payload,ptrs->feedname,strlen(ptrs->feedname)))
		{
			if(strstr(params->payload,"on"))
			{
				gpioWrite(ptrs->feedpin,1);
				IOT_INFO("%s turned on",ptrs->feedname);
				//break;
				
			}
			else if(strstr(params->payload,"off"))
			{
				gpioWrite(ptrs->feedpin,0);
				IOT_INFO("%s turned off",ptrs->feedname);
			}
			//else;
			break;

		}
		else
		{
			ptrs=ptrs->next;
		}


	}

////////////
}

void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data) {
	IOT_WARN("MQTT Disconnect");
	IoT_Error_t rc = FAILURE;

	if(NULL == pClient) {
		return;
	}

	IOT_UNUSED(data);

	if(aws_iot_is_autoreconnect_enabled(pClient)) {
		IOT_INFO("Auto Reconnect is enabled, Reconnecting attempt will start now");
	} else {
		IOT_WARN("Auto Reconnect not enabled. Starting manual reconnect...");
		rc = aws_iot_mqtt_attempt_reconnect(pClient);
		if(NETWORK_RECONNECTED == rc) {
			IOT_WARN("Manual Reconnect Successful");
		} else {
			IOT_WARN("Manual Reconnect Failed - %d", rc);
		}
	}
}

void parseInputArgsForConnectParams(int argc, char **argv) {
	int opt;

	while(-1 != (opt = getopt(argc, argv, "h:p:c:x:"))) {
		switch(opt) {
			case 'h':
				strcpy(HostAddress, optarg);
				IOT_DEBUG("Host %s", optarg);
				break;
			case 'p':
				port = atoi(optarg);
				IOT_DEBUG("arg %s", optarg);
				break;
			case 'c':
				strcpy(certDirectory, optarg);
				IOT_DEBUG("cert root directory %s", optarg);
				break;
			case 'x':
				publishCount = atoi(optarg);
				IOT_DEBUG("publish %s times\n", optarg);
				break;
			case '?':
				if(optopt == 'c') {
					IOT_ERROR("Option -%c requires an argument.", optopt);
				} else if(isprint(optopt)) {
					IOT_WARN("Unknown option `-%c'.", optopt);
				} else {
					IOT_WARN("Unknown option character `\\x%x'.", optopt);
				}
				break;
			default:
				IOT_ERROR("Error in command line argument parsing");
				break;
		}
	}

}

int main(int argc, char **argv) {

gpioSetup();


feedadd();

 

  
	bool infinitePublishFlag = true;
	int timedelay=1;
	char rootCA[PATH_MAX + 1];
	char clientCRT[PATH_MAX + 1];
	char clientKey[PATH_MAX + 1];
	char CurrentWD[PATH_MAX + 1];
	char cPayload[512];

	int32_t i = 0;

	IoT_Error_t rc = FAILURE;

	AWS_IoT_Client client;
	IoT_Client_Init_Params mqttInitParams = iotClientInitParamsDefault;
	IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

	IoT_Publish_Message_Params paramsQOS0;
	IoT_Publish_Message_Params paramsQOS1;

	parseInputArgsForConnectParams(argc, argv);

	IOT_INFO("\nPAASMER IoT SDK Version %d.%d.%d-%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);

	getcwd(CurrentWD, sizeof(CurrentWD));
	snprintf(rootCA, PATH_MAX + 1, "%s/%s/%s", CurrentWD, certDirectory, AWS_IOT_ROOT_CA_FILENAME);
	snprintf(clientCRT, PATH_MAX + 1, "%s/%s/%s%s", CurrentWD, certDirectory,DeviceName, AWS_IOT_CERTIFICATE_FILENAME);
	snprintf(clientKey, PATH_MAX + 1, "%s/%s/%s%s", CurrentWD, certDirectory, DeviceName,AWS_IOT_PRIVATE_KEY_FILENAME);

	IOT_DEBUG("rootCA %s", rootCA);
	IOT_DEBUG("clientCRT %s", clientCRT);
	IOT_DEBUG("clientKey %s", clientKey);
	mqttInitParams.enableAutoReconnect = false; // We enable this later below
	mqttInitParams.pHostURL = HostAddress;
	mqttInitParams.port = port;
	mqttInitParams.pRootCALocation = rootCA;
	mqttInitParams.pDeviceCertLocation = clientCRT;
	mqttInitParams.pDevicePrivateKeyLocation = clientKey;
	mqttInitParams.mqttCommandTimeout_ms = 20000;
	mqttInitParams.tlsHandshakeTimeout_ms = 5000;
	mqttInitParams.isSSLHostnameVerify = true;
	mqttInitParams.disconnectHandler = disconnectCallbackHandler;
	mqttInitParams.disconnectHandlerData = NULL;

	rc = aws_iot_mqtt_init(&client, &mqttInitParams);
	if(SUCCESS != rc) {
		IOT_ERROR("paasmer_iot_mqtt_init returned error : %d ", rc);
		return rc;
	}

	connectParams.keepAliveIntervalInSec = 10;
	connectParams.isCleanSession = true;
	connectParams.MQTTVersion = MQTT_3_1_1;
	connectParams.pClientID = AWS_IOT_MQTT_CLIENT_ID;
	connectParams.clientIDLen = (uint16_t) strlen(AWS_IOT_MQTT_CLIENT_ID);
	connectParams.isWillMsgPresent = false;

	IOT_INFO("Connecting...");
	rc = aws_iot_mqtt_connect(&client, &connectParams);
	if(SUCCESS != rc) {
		IOT_ERROR("Error(%d) connecting to %s:%d", rc, mqttInitParams.pHostURL, mqttInitParams.port);
		return rc;
	}
	/*
	 * Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
	 *  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
	 *  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
	 */
	rc = aws_iot_mqtt_autoreconnect_set_status(&client, true);
	if(SUCCESS != rc) {
		IOT_ERROR("Unable to set Auto Reconnect to true - %d", rc);
		return rc;
	}

	IOT_INFO("Subscribing...");
	char sub_topic[100];
	sprintf(sub_topic,"%s_%s",UserName,DeviceName);
	rc = aws_iot_mqtt_subscribe(&client,sub_topic, strlen(sub_topic), QOS0, iot_subscribe_callback_handler, NULL);
	if(SUCCESS != rc) {
		IOT_ERROR("Error subscribing : %d ", rc);
		return rc;
	}



	paramsQOS0.qos = QOS0;
	paramsQOS0.payload = (void *) cPayload;
	paramsQOS0.isRetained = 0;

	paramsQOS1.qos = QOS1;
	paramsQOS1.payload = (void *) cPayload;
	paramsQOS1.isRetained = 0;

	if(publishCount != 0) {
		infinitePublishFlag = false;
	}

	while((NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc)
		  && (publishCount > 0 || infinitePublishFlag)) {

		//Max time the yield function will wait for read messages
		rc = aws_iot_mqtt_yield(&client, 100);
		if(NETWORK_ATTEMPTING_RECONNECT == rc) {
			// If the client is attempting to reconnect we will skip the rest of the loop.
			continue;
		}

		IOT_INFO("-->sleep!");
		sleep(1);
		

if(UserName != "" && DeviceName !=""){
if (sensordetails==1){
if(((timedelay++)%timePeriod)==0){



	////added for new architecture
	char feed2Details[256],feed3Details[256],Credentials[256];
				FD *ptr=hptr;
				int feed1Value=0,feed2Value=0,feed3Value=0;

				while(ptr){
					
					feed1Value = gpioRead(ptr->feedpin);
					snprintf(cPayload,sizeof(cPayload),"{\n\"feeds\":[{\"feedname\":\"%s\",\n\"feedtype\":\"%s\",\n\"feedpin\":\"%d\",\"feedvalue\":\"%d\"},",ptr->feedname,ptr->feedtype,ptr->feedpin,feed1Value);

					ptr=ptr->next;
					feed1Value=0;
					if(ptr)
					{
						
						feed2Value = gpioRead(ptr->feedpin);
						snprintf(feed2Details,sizeof(feed2Details),"\n{\"feedname\":\"%s\",\n\"feedtype\":\"%s\",\n\"feedpin\":\"%d\",\"feedvalue\":\"%d\"},",ptr->feedname,ptr->feedtype,ptr->feedpin,feed2Value);

						strcat(cPayload,feed2Details);
	
						ptr=ptr->next;
						feed2Value=0;
						strcpy(feed2Details,"\0");
					}
					else
					{
						snprintf(feed2Details,sizeof(feed2Details),"\n{\"feedname\":\"\",\n\"feedtype\":\"\",\n\"feedpin\":\"\",\n\"feedvalue\":\"\"},\n");
						strcat(cPayload,feed2Details);
						
					}
					if(ptr)
					{
						
						feed3Value = gpioRead(ptr->feedpin);
						snprintf(feed3Details,sizeof(feed3Details),"\n{\"feedname\":\"%s\",\n\"feedtype\":\"%s\",\n\"feedpin\":\"%d\",\n\"feedvalue\":\"%d\"}],",ptr->feedname,ptr->feedtype,ptr->feedpin,feed3Value);
					
						strcat(cPayload,feed3Details);
				
						ptr=ptr->next;
						feed3Value=0;
						strcpy(feed3Details,"\0");
						
					}
					else{
		
						snprintf(feed3Details,sizeof(feed3Details),"\n{\"feedname\":\"\",\n\"feedtype\":\"\",\n\"feedpin\":\"\",\n\"feedvalue\":\"\"}],");
						strcat(cPayload,feed3Details);
						printf("\nEof linkedlist\n");
					}
					snprintf(Credentials,sizeof(Credentials),"\"messagecount\": \"%d\",\"paasmerid\":\"%s\",\"username\":\"%s\",\"devicename\":\"%s\",\"devicetype\":\"SBC\"}",msgCount,MAC,UserName,DeviceName);
					strcat(cPayload,Credentials);
					IOT_INFO("%s",cPayload);
					IOT_INFO("%d\n",msgCount);
					
					msgCount++;

		
					
					paramsQOS1.payloadLen = strlen(cPayload);
                rc = aws_iot_mqtt_publish(&client, "paasmerv2_device_online",23, &paramsQOS1);
                if (rc == MQTT_REQUEST_TIMEOUT_ERROR) {
                        IOT_WARN("QOS1 publish ack not received.\n");
                        rc = SUCCESS;
                }
                if(publishCount > 0) {
                        publishCount--;
                }
					
					sleep(1);	
				}

	//////

	
}
}}

	}

	if(SUCCESS != rc) {
		IOT_ERROR("An error occurred in the loop.\n");
	} else {
		IOT_INFO("Publish done\n");
	}


	return rc;
}
