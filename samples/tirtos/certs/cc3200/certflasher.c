/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
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
/*
 *  ======== certflasher.c ========
 */
#include <stdlib.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/WiFi.h>

#include <simplelink.h>

#include "Board.h"
#include "configure.h"

#if defined(MSP430WARE) || defined(MSP432WARE)
#define SPI_BIT_RATE    2000000
#elif defined(CC32XXWARE)
#define SPI_BIT_RATE    20000000
#else
#define SPI_BIT_RATE    14000000
#endif

/*
 * USER STEP: Copy the lines in the root CA certificate file between
 *
 *            -----BEGIN CERTIFICATE-----
 *            ...
 *            -----END CERTIFICATE-----
 *
 *            Paste the lines in string format for the following array:
 */
/*const char root_ca_pem[] = 
"MIIE0zCCA7ugAwIBAgIQGNrRniZ96LtKIVjNzGs7SjANBgkqhkiG9w0BAQUFADCB"
"yjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL"
"ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJp"
"U2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxW"
"ZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0"
"aG9yaXR5IC0gRzUwHhcNMDYxMTA4MDAwMDAwWhcNMzYwNzE2MjM1OTU5WjCByjEL"
"MAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZW"
"ZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJpU2ln"
"biwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxWZXJp"
"U2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0aG9y"
"aXR5IC0gRzUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCvJAgIKXo1"
"nmAMqudLO07cfLw8RRy7K+D+KQL5VwijZIUVJ/XxrcgxiV0i6CqqpkKzj/i5Vbex"
"t0uz/o9+B1fs70PbZmIVYc9gDaTY3vjgw2IIPVQT60nKWVSFJuUrjxuf6/WhkcIz"
"SdhDY2pSS9KP6HBRTdGJaXvHcPaz3BJ023tdS1bTlr8Vd6Gw9KIl8q8ckmcY5fQG"
"BO+QueQA5N06tRn/Arr0PO7gi+s3i+z016zy9vA9r911kTMZHRxAy3QkGSGT2RT+"
"rCpSx4/VBEnkjWNHiDxpg8v+R70rfk/Fla4OndTRQ8Bnc+MUCH7lP59zuDMKz10/"
"NIeWiu5T6CUVAgMBAAGjgbIwga8wDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8E"
"BAMCAQYwbQYIKwYBBQUHAQwEYTBfoV2gWzBZMFcwVRYJaW1hZ2UvZ2lmMCEwHzAH"
"BgUrDgMCGgQUj+XTGoasjY5rw8+AatRIGCx7GS4wJRYjaHR0cDovL2xvZ28udmVy"
"aXNpZ24uY29tL3ZzbG9nby5naWYwHQYDVR0OBBYEFH/TZafC3ey78DAJ80M5+gKv"
"MzEzMA0GCSqGSIb3DQEBBQUAA4IBAQCTJEowX2LP2BqYLz3q3JktvXf2pXkiOOzE"
"p6B4Eq1iDkVwZMXnl2YtmAl+X6/WzChl8gGqCBpH3vn5fJJaCGkgDdk+bW48DW7Y"
"5gaRQBi5+MHt39tBquCWIMnNZBU4gcmU7qKEKQsTb47bDN0lAtukixlE0kF6BWlK"
"WE9gyn6CagsCqiUXObXbf+eEZSqVir2G3l6BFoMtEMze/aiCKm0oHw0LxOXnGiYZ"
"4fQRbxC1lfznQgUy286dUV4otp6F01vvpX1FQHKOtw5rDgb7MzVIcbidJ4vEZV8N"
"hnacRHr2lVz2XTIIM6RUthg/aFzyQkqFOFSDX9HoLPKsEdao7WNq";*/

/*
 * USER STEP: Copy the lines in the cert.pem certificate file between
 *
 *            -----BEGIN CERTIFICATE-----
 *            ...
 *            -----END CERTIFICATE-----
 *
 *            Paste the lines in string format for the following array:
 */
/*const char client_cert_pem[] = 
"MIIDWjCCAkKgAwIBAgIVAIEokE5Cc2yM6xJA9yydxP1LixEKMA0GCSqGSIb3DQEB"
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t"
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0xNjEwMDMxMjA0"
"NTBaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh"
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC1hEbeThcQb8Vsncl6"
"fVl3f3uQg/qaTJMcTzJBDbs81h75iF9iXJPdS1sgqRlzkQOJ6CpuBvKf4srtX+vc"
"AtGneWh4JteljdPGJVzXh49FgL9XEc14HnCYf8QT669aaLtSrztEyb26DwSi8daL"
"jZqjTklntAgxvV3SbHPz7xG5258FpTwjt2iewO5H4D7oaquGOgKmOj6Ne9/adI8n"
"Wjcn4HjWzPI4F2JytQxu+Vqow9zrsf2wu+fkd7UH1wNHD5MEL3gA3gNfMpkZWk8a"
"uucK0mIk3ZlpocI4mHzVRkN9B3o1+5QDiBFfMX9rWTS3LdkmNY/kdqA5mD2nNPzF"
"N3EjAgMBAAGjYDBeMB8GA1UdIwQYMBaAFBm5QDoGKUKbHYUgYY1BSyRUvFs5MB0G"
"A1UdDgQWBBQwCJm3VeRQ3NWuQqBF/uWzCJAkwTAMBgNVHRMBAf8EAjAAMA4GA1Ud"
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAllglaITox4VW0l/y55+Gc4GU"
"RjghzJszsfxgktTqn2hJ2vRxVSlyOgF7AythXclPLpLao56nsFpjsFc+6hhnrfP9"
"uhlOovtdC26H8lcnn4ix7oAc4A5PwLv6Lm+x/EvPITyxvzIMYOT4FSV17KI/rh3D"
"rNBBeKJ880IoWCfgBUPMBahFmymV6VO2eDpZBWaSK1fn269JGiLy08LQRetKC72T"
"fXmcjgHMNysskXk6njfLQyUEthiPpd7w4ZvB95IPCr5ldD6ObGWnp2uzTsGKrgSO"
"JWSRdBLitPgzEFo7QY4tP19Fhe+ERH+sk5+nwn58tU8Gl1lv5/zobZsnFip2mg==";*/

/*
 * USER STEP: Copy the lines in the privateKey.pem certificate file between
 *
 *            -----BEGIN CERTIFICATE-----
 *            ...
 *            -----END CERTIFICATE-----
 *
 *            Paste the lines in string format for the following array:
 */
/*const char client_private_key_pem[] =
"MIIEpQIBAAKCAQEAtYRG3k4XEG/FbJ3Jen1Zd397kIP6mkyTHE8yQQ27PNYe+Yhf"
"YlyT3UtbIKkZc5EDiegqbgbyn+LK7V/r3ALRp3loeCbXpY3TxiVc14ePRYC/VxHN"
"eB5wmH/EE+uvWmi7Uq87RMm9ug8EovHWi42ao05JZ7QIMb1d0mxz8+8RudufBaU8"
"I7donsDuR+A+6GqrhjoCpjo+jXvf2nSPJ1o3J+B41szyOBdicrUMbvlaqMPc67H9"
"sLvn5He1B9cDRw+TBC94AN4DXzKZGVpPGrrnCtJiJN2ZaaHCOJh81UZDfQd6NfuU"
"A4gRXzF/a1k0ty3ZJjWP5HagOZg9pzT8xTdxIwIDAQABAoIBAQCTugDXQeSheuXI"
"/S2RY9zhVT5iIhddZn6/pnC24w8EdxskaGmAqumoS4Dxts3A8T9ZCDsn6r2hhDk1"
"MUDKgPzR+Qwd5TbSRPgapiPGQUxH+309xn5biSKH6kvRDppa0SvLIlPxKuMfPsO+"
"PWYnk2pMCBXTsYCNwplN/Pftih76C7IXVSFUENvb/fYzReLY4uZV9GOagTeMD6wP"
"IWQD2zHBsm7/6dCHdY01EbvVa1Fcvt6kTqvDx4VcDsVvgnfYUZXbHosAEH7Wit9C"
"uNUNXtchTZEsraatJpaeYB26TsWG1wquAm40wLngwXc8UzxIoRJKzfnKZO0aAP+q"
"+xWRI38xAoGBAPhzaE8jgUnZ8qdCLLZQBYVuDebDEcyP8seT9Qc3ZBejq4OQGqvw"
"1iDDIbeugqWfkRYleLpGibx3HthssrfrYIm9AcXDA7+zDqf4KpW0NwC1OFGNJFHy"
"oMjput7LPpe3cHV3pFFJTwBfcGFLcJPhX5uq0FuuLFwqa/nk7mKh96M7AoGBALsI"
"N7XNvfJIBlnelCrKqJGWqgQYi6sppy23ab1j3ONn6De3YQ6jpe5nXptuVzLJd5tO"
"wiZLBoo7dDP+BS5jazJl8oBYm76LNhSwAn990DeGYKTh0SzNu4nDj7QsWF+ZH3/c"
"D1TuhAFjYSS9VesrFtmWSpi16O5T8S1yAKO1CLs5AoGBAJN2XCjsl+Yvd/iSLFuh"
"L0W8xn96tGvH3H9ccAhydfQZ7sjNlScMcOtNJgwRhfWrub99SiS1jTbRNPubuI+1"
"Ir1d81lfoC/9pOtJPm1QY30GEoFE1pPVAF1/bKyj0WjXNeZitqKQclV0UA1Pa2g6"
"UjbgsX5Y6HdjrmbJ3DuqioLVAoGBALUKuVKEe08hJPStHWNmlFGJM72PPPNL+IWq"
"OvKWzeRaWlBEc/DFvALPkKaH4mRI3Dw7undyvDvmXQ9O3hkPJ167jEdsfiWtf4X6"
"8hAS6xA0RSyVYfMqmlQ6NspC+rBe+jy7+SNPVfw6BHiZxadEKHQqpL65p2W7X4OM"
"DM5qmK8RAoGAdg/W0sBFmEm97TEXt5ZsGA7yOL+o6UNwchoNzSpIrNdNjz6D8zF+"
"cIpRGEDGPYKPsQXqcdwvIv5GDjkdTQFOjlq/Nv4O4kFu2OCRjd0DjS+11ux/pK8m"
"i2XiPZ9LSNhfy+j5qEwCsmc5SKy7zF8hw+1UhIeso0FLFr62XHeqBVY=";*/

/*
 *  In this release, the cert files must exist as shown below, as the
 *  TLS_create() function expects them to be named as show, and be
 *  placed in a directory "/cert". The ability to change this will be
 *  added in a future release.
 */
const char ca_cert_path[] = "/cert/ca.der";
const char client_cert_path[] = "/cert/cert.der";
const char client_key_path[] = "/cert/key.der";

void SimpleLinkWlanEventHandler(SlWlanEvent_t *pArgs)
{
}

void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pArgs)
{
}

/*
 *  ======== SimpleLinkGeneralEventHandler ========
 */
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *sockEvent)
{

}

void SimpleLinkSockEventHandler(SlSockEvent_t *pArgs)
{
}

void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpServerEvent,
        SlHttpServerResponse_t *pHttpServerResponse)
{
}

/*
 *  ======== base64Decode ========
 *  Returns the decoded value
 */
unsigned char base64Decode(unsigned char ch)
{
    unsigned char ret = 0;
    if (ch >= 'A' && ch <= 'Z') {
        ret = ch - 'A';
    }
    else if (ch >= 'a' && ch <= 'z') {
        ret = ch - 'a' + 26;
    }
    else if (ch >= '0' && ch <= '9') {
        ret = ch - '0' + 52;
    }
    else if (ch == '+') {
        ret = 62;
    }
    else if (ch == '/') {
        ret = 63;
    }

    return (ret);
}

/*
 *  ======== convertPemToDer ========
 *  Returns the length of the der output buffer or -1 on failure
 */
int convertPemToDer(const char *pem, int plen, char** der)
{
    int value = 0;
    int i;
    int j;
    int dlen = 0;
    int padZero = 0;
    char *derPtr = NULL;

    for (i = plen; pem[i - 1] == '='; i--) {
        padZero++;
    }

    /* Base64 decode: 4 characters to 3 bytes */
    dlen = (plen * 3) / 4;
    derPtr = (char *) malloc (sizeof(char) * dlen);
    if (derPtr == NULL) {
        return (-1);
    }

    for (i = 0, j = 0; (i + 3) < plen && (j + 2) < dlen; i += 4, j += 3) {
        value = (base64Decode(pem[i]) << 18)
                + (base64Decode(pem[i + 1]) <<  12)
                + (base64Decode(pem[i + 2]) << 6)
                + base64Decode(pem[i + 3]);

        derPtr[j]     = (value >> 16) & 0xFF;
        derPtr[j + 1] = (value >> 8) & 0xFF;
        derPtr[j + 2] = value & 0xFF;
    }

    *der = derPtr;

    return (dlen - padZero);
}

/*
 *  ======== flashFile ========
 *  Returns 0 on success or  -1 on failure
 */
int flashFile(const char *path, const unsigned char *buffer, int len)
{
    long ret;
    long fileHandle;

    if (!path || !buffer) {
        System_printf("flashfile: Error invalid input params\n");
        ret = -1;
        goto flashdone;
    }

    ret = sl_FsOpen((unsigned char *)path,
            FS_MODE_OPEN_CREATE(len,  _FS_FILE_PUBLIC_WRITE), NULL,
            &fileHandle);
    if (ret < 0) {
        System_printf("flashfile: Error couldn't open the file %s"
                ", error code = %d\n", path, ret);
        ret = -1;
        goto flashdone;
    }

    ret = sl_FsWrite(fileHandle, 0, (unsigned char *)buffer, len);
    if (ret < 0) {
        sl_FsClose(fileHandle, NULL, NULL, 0);

        System_printf("flashfile: Error couldn't write to the file %s"
                ", error code = %d\n", path, ret);
        ret = -1;
        goto flashdone;
    }

    ret = sl_FsClose(fileHandle, NULL, NULL, 0);
    if (ret < 0) {
        System_printf("flashfile: Error couldn't close to the file %s"
                ", error code = %d\n", path, ret);
        ret = -1;
        goto flashdone;
    }

    ret = 0;

flashdone:
    return (ret);
}

/*
 *  ======== cc3200CertFlashTask ========
 */
void cc3200CertFlashTask(unsigned int arg0, unsigned int arg1)
{
    char *der;
    int dlen;
    WiFi_Params wifiParams;
    WiFi_Handle wifiHandle;

    /* Open the WiFi driver */
    WiFi_Params_init(&wifiParams);
    wifiParams.bitRate = SPI_BIT_RATE;
    wifiHandle = WiFi_open(Board_WIFI, Board_WIFI_SPI, NULL, &wifiParams);
    if (!wifiHandle) {
        System_abort("WiFi driver failed to open.");
    }

    if (sl_Start(0, 0, 0) < 0) {
        System_abort("cc3200CertFlashTask: Error initializing SimpleLink Wi-Fi"
                " stack\n");
    }

    System_printf("Flashing ca certificate file ...\n");

    der = NULL;
    dlen = convertPemToDer(root_ca_pem, strlen(root_ca_pem), &der);
    if ((dlen < 0)
            || (flashFile(ca_cert_path, (unsigned char *)der, dlen) == -1)) {
        System_abort("cc3200CertFlashTask: Error flashing ca certificate "
                "file\n");
    }
    free(der);

    System_printf("Flashing client certificate file ...\n");

    der = NULL;
    dlen = convertPemToDer(client_cert_pem, strlen(client_cert_pem), &der);
    if ((dlen < 0)
            || (flashFile(client_cert_path, (unsigned char *)der, dlen)
            == -1)) {
        System_abort("cc3200CertFlashTask: Error flashing client certificate"
                " file\n");
    }
    free(der);

    System_printf("Flashing client key file ...\n");

    der = NULL;
    dlen = convertPemToDer(client_private_key_pem, strlen(client_private_key_pem), &der);
    if ((dlen < 0)
            || (flashFile(client_key_path, (unsigned char *)der, dlen) == -1)) {
        System_abort("cc3200CertFlashTask: Error flashing client key file\n");
    }
    free(der);

    System_printf("done.\n");
    System_flush();
}

/*
 *  ======== main ========
 */
int main(int argc, char *argv[])
{
    Task_Handle taskHandle;
    Task_Params taskParams;

    Board_initGeneral();
    Board_initGPIO();

    Board_initWiFi();

    GPIO_write(Board_LED0, Board_LED_ON);

    Task_Params_init(&taskParams);
    taskParams.stackSize = 4096;

    taskHandle = Task_create((Task_FuncPtr)cc3200CertFlashTask, &taskParams,
            NULL);
    if (taskHandle == NULL) {
        System_abort("main: failed to create a Task!\n");
    }

    BIOS_start();
}
