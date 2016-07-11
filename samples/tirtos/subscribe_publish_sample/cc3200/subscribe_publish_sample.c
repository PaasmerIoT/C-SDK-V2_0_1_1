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

char HostAddress[255] = AWS_IOT_MQTT_HOST;
uint32_t port = AWS_IOT_MQTT_PORT;
uint32_t publishCount = 0;

void MQTTcallbackHandler(AWS_IoT_Client *pClient, char *topicName,
        uint16_t topicNameLen, IoT_Publish_Message_Params *params, void *pData)
{
    IOT_INFO("Subscribe callback");
    IOT_INFO("%.*s\t%.*s",topicNameLen, topicName, (int)params->payloadLen,
            (char *)params->payload);
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

void runAWSClient(void)
{
    IoT_Error_t rc = SUCCESS;
    int32_t i = 0;
    bool infinitePublishFlag = true;
    char *topicName = "sdkTest/sub";
    int topicNameLen = strlen(topicName);

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

    char cPayload[100];
    sprintf(cPayload, "%s : %d ", "hello from SDK", i);

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
        rc = aws_iot_mqtt_yield(&client, 100);

        if (NETWORK_ATTEMPTING_RECONNECT == rc) {
            /* If the client is attempting to reconnect, skip rest of loop */
            continue;
        }

        IOT_INFO("-->sleep");
        Task_sleep(1000);
        sprintf(cPayload, "%s : %d ", "hello from SDK QOS0", i++);
        /* Recalculate string len to avoid truncation in subscribe callback */
        paramsQOS0.payloadLen = strlen(cPayload);
        rc = aws_iot_mqtt_publish(&client, topicName, topicNameLen,
                &paramsQOS0);
        if (publishCount > 0) {
           publishCount--;
        }

        sprintf(cPayload, "%s : %d ", "hello from SDK QOS1", i++);
        /* Recalculate string len to avoid truncation in subscribe callback */
        paramsQOS1.payloadLen = strlen(cPayload);
        do {
            rc = aws_iot_mqtt_publish(&client, topicName, topicNameLen,
                    &paramsQOS1);
            if (publishCount > 0) {
                publishCount--;
            }
        } while (MQTT_REQUEST_TIMEOUT_ERROR == rc &&
                (publishCount > 0 || infinitePublishFlag));
    }

    if (SUCCESS != rc) {
        IOT_ERROR("An error occurred in the loop. Error code = %d\n", rc);
    }
    else {
        IOT_INFO("Publish done\n");
    }
}
