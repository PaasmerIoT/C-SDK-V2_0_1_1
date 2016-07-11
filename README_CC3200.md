# CC3200 Set Up

This file contains the following information:
- How to build the AWS SDK and the samples for the following devices:
    - TI CC3200 Launchpad
- How to run the samples.

## Table of Contents
- [Required Hardware](#RequiredHW)
- [TI Software Installation](#TI-SW)
- [Build Environment Set Up](#Enviro-Setup)
- [AWS IoT Installation And Set Up](#AWS-Install)
- [Obtain The Certificate Files](#Obtain-CERTS)
- [Building The Certificate Flasher Tool](#Build-TOOL)
- [Building The Sample Applications](#Build-SAMPLE)
- [Setting Up Code Composer Studio Before Running The Samples](#Setup-CCS)
- [Running The Certificate Flasher Tool](#Run-TOOL)
- [Running A Sample](#Run-SAMPLE)

<a name="RequiredHW"></a>
## Required Hardware
The following device is required for this SDK:
- [CC3200 Launchpad](http://www.ti.com/tool/cc3200-launchxl)

Please ensure that your device has been updated with the latest firmware and or service pack.

<a name="TI-SW"></a>
## TI Software Installation
The following TI software products must be installed in order to build and run the sample applications. While not strictly required, we recommend that you install these products into a common directory and that you use directory names without any whitespace. This documentation assumes that you haved installed everything in a directory named `C:/ti`.

- Install [Code Composer Studio v6.1.2](http://www.ti.com/tool/ccstudio?keyMatch=code%20composer%20studio) or higher.

- Install [TI-RTOS for CC32xx 2.16.00.08](http://downloads.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/tirtos/index.html) or higher.

<a name="Enviro-Setup"></a>
## Build Environment Set Up
### Updating Paths To Product Dependencies
1. Edit the `products.mak` file in `<AWS_INSTALL_DIR>/` using your favorite text editor.
2. Update the variables `XDC_INSTALL_DIR and TIRTOS_INSTALL_DIR to point to the locations where you installed these products.
3. The variable `TI_ARM_CODEGEN_INSTALL_DIR` should point to the installation location of the TI ARM compiler in your CCS installation.
4. After modification, these variable definitions should look similar to the following if you are working in Windows. (Windows users: note the use of "/" in the path).
    ```
    XDC_INSTALL_DIR = C:/ti/xdctools_3_32_00_06_core
    TIRTOS_INSTALL_DIR = C:/ti/tirtos_cc32xx_2_16_00_08
    TI_ARM_CODEGEN_INSTALL_DIR = C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5
    ```

### Notes On The gmake Utiltity
When building an application for the first time, the TI ARM compiler first builds the C runtime library, which requires that ```gmake``` is on the environment path. It is therefore recommended that the ```gmake``` tool be installed on your machine and added to the environment path.

Alternatively, the XDCtools product (```C:/ti/xdctools_3_32_00_06_core```) contains ```gmake```, which can be utilized by adding the install location of XDCtools to the environment path. But note, it is recommended that the XDCtools is removed from the path after the inital step of building the C runtime libraries is complete. This is to avoid conflicts when you use newer versions of XDCtools.

For detailed information about building TI ARM C runtime library, please read the [Mklib wiki](http://processors.wiki.ti.com/index.php/Mklib).

<a name="AWS-Install"></a>
## AWS IoT Installation And Set Up
### AWS IoT SDK Installation
This procedure assumes you have already installed the AWS SDK on your development machine. These instructions refer to the folder that contains the AWS SDK on your local machine as `<AWS_INSTALL_DIR>`.

### AWS IoT Developer Set Up
In order to run the sample applications, it is necessary to complete the steps of the [AWS IoT Developer Guide](http://docs.aws.amazon.com/iot/latest/developerguide/iot-gs.html).  In particular, the following steps of the guide must be completed:
1. [Installing the AWS Command Line Interface](http://docs.aws.amazon.com/cli/latest/userguide/installing.html)
2. [Configuring the AWS Command Line Interface](http://docs.aws.amazon.com/cli/latest/userguide/cli-chap-getting-started.html)
3. [Sign in to the AWS IoT Console](http://docs.aws.amazon.com/iot/latest/developerguide/iot-console-signin.html)
4. [Create a Device in the Thing Registry](http://docs.aws.amazon.com/iot/latest/developerguide/create-device.html)
5. [Create and Activate a Device Certificate](http://docs.aws.amazon.com/iot/latest/developerguide/create-device-certificate.html)
    - Make sure you choose to download the certificate files, as these will be needed in the [Building The Certificate Flasher Tool](#Build-TOOL) steps.
    - Note: you will also need a root CA certificate.  You can download the root CA from [here](http://docs.aws.amazon.com/iot/latest/developerguide/identity-in-iot.html).  Just search for the link titled "VeriSign root CA certificate"
6. [Create an AWS IoT Policy](http://docs.aws.amazon.com/iot/latest/developerguide/create-iot-policy.html)
7. [Attach an AWS IoT Policy to a Device Certificate](http://docs.aws.amazon.com/iot/latest/developerguide/attach-policy-to-certificate.html)
8. [Attach a Thing to a Certificate](http://docs.aws.amazon.com/iot/latest/developerguide/attach-cert-thing.html)

Note that Amazon has recently tailored the [AWS IoT Developer Guide](http://docs.aws.amazon.com/iot/latest/developerguide/iot-gs.html) instructions specifically for the "AWS IoT Button."  You can ignore the mentions of the button as your TI embedded device will be used in place of this.  For more details on AWS IoT, please refer to the [What Is AWS IoT](http://docs.aws.amazon.com/iot/latest/developerguide/what-is-aws-iot.html) page.

<a name="Obtain-CERTS"></a>
## Obtain The Certificate Files
Certificate files used by the samples need to be obtained from the AWS CLI tool and the AWS IoT website directly. If you have not already done so, follow the steps in the previous section to set up your thing, certificates and policy.

<a name="Build-TOOL"></a>
## Building The Certificate Flasher Tool

1. Open the file `certflasher.c` from the directory `<AWS_INSTALL_DIR>/samples/tirtos/certs/cc3200`.

2. Search for "USER STEP" and update the CA root certificate string, the client certificate string, and the client (private) key string. These should be extracted from certificate (.pem) files downloaded from AWS (remember to remove the newline '\n' characters in the JSON data). A typical string would be of this format:

    ```
    const char root_ca_pem[] =
    "JQQGEwJVUzEQMA4GA1UECAwHTW9udGFuYTEQMA4GA1UEBwwHQm96ZW1hbjERMA8G"
    "A1UECgwIU2F3dG9vdGgxEzARBgNVBAsMCkNvbnN1bHRpbmcxGDAWBlNVBAMMD3d3"
    "dy53b2xmc3NsLmNvbTEfMB0GCSqGSIb3DQEJARYQaW5mb0B3b2xmc3NsLmNvbTAe"
    "Fw0xNTA1MDcxODIxMDFaFw0xOGAxMzExODIxMDFaMIGUMQswCQYDVQQGEwJVUzEQ"
    "MA4GA1UECAwHTW9udGFuYTEQMA4GA1UEBwwHQm96ZW1hbjERMA8GA1UECgwIU2F3"
    "dG9vdGgxEzARBgNVBAsMCkNvbnN1bHRpbmcxGDAWBgNVBAMMD3d3dy53b2xmc3Ns"
    "LmNvbTEfMB0GCSqGSIb3DQEJARYQaW5mb0B3b2xmy3NsLmNvbTCCASIwDQYJKoZI"
    "hvcNAQEBBQADggEPADCCAQoCggEBAL8Myi0Ush6EQlvNOB9K8k11EPG2NZ/fyn0D"
    "mNOs3gNm7irx2LB9bgdUCxCYIU2AyxIg58xP3kV9yXJ3MurKkLtpUhADL6jzlcXx";
    ```
Note that certificate files must be name "ca.der", "cert.der" and "key.der", and written to the "/cert" directory (as shown in the certflasher application source file).  The ability to change these names and their location will be added in a future release.

On the command line, enter the following commands to build the application:

    cd <AWS_INSTALL_DIR>/samples/tirtos/certs/cc3200
    C:/ti/xdctools_3_32_00_06_core/gmake all

<a name="Build-SAMPLE"></a>
## Building The Sample Applications

Before building a specific application, complete the following steps (we are referring to the `subscribe_publish_sample` application here, but the same procedure applies to other samples as well):

1. If you are using Windows, open a Windows command prompt. If you are using Linux, open a terminal window.
2. Use the following AWS CLI command to to retrieve your AWS account-specific AWS IoT endpoint:
```aws iot describe-endpoint```
3. Open the `aws_iot_config.h` file from the directory `<AWS_INSTALL_DIR>/samples/tirtos/subscribe_publish_sample/cc3200` in a text editor
4. Update the value of the macro "AWS_IOT_MQTT_HOST" with string output from this command (for example, it will have a format similar to the following: `<random-string>.iot.us-east-1.amazonaws.com`)
5. Update the value of the macro "AWS_IOT_MQTT_CLIENT_ID" to a unique name for your device
6. Update the value of the macro "AWS_IOT_MY_THING_NAME" with the name of the thing you created during the steps on the [Create a Device in the Thing Registry](http://docs.aws.amazon.com/iot/latest/developerguide/create-device.html) web page

7. The certificate file variables "AWS_IOT_ROOT_CA_FILENAME", "AWS_IOT_CERTIFICATE_FILENAME", and "AWS_IOT_PRIVATE_KEY_FILENAME" are not used in this release and should be left as is here.  Certificate file placement is handled by the certificate flasher tool in the next section.

8. Open the file `wificonfig.h` from the directory `<AWS_INSTALL_DIR>/samples/tirtos/subscribe_publish_sample/cc3200`. Search for "USER STEP" and update the WIFI SSID and SECURITY_KEY macros.

On the command line, enter the following commands to build the application:

    cd <AWS_INSTALL_DIR>/samples/tirtos/subscribe_publish_sample/cc3200
    C:/ti/xdctools_3_32_00_06_core/gmake all

<a name="Setup-CCS"></a>
## Setting Up Code Composer Studio Before Running The Samples
1. Plug the CC3200 Launchpad into a USB port on your PC

2. Open a serial session to the appropriate COM port with the following settings:

    ```
    Baudrate:     9600
    Data bits:       8
    Stop bits:       1
    Parity:       None
    Flow Control: None
    ```

3. Open Code Composer Studio.

4. In Code Composer Studio, open the CCS Debug Perspective - Windows menu -> Open Perspective -> CCS Debug

5. Open the Target Configurations View - Windows menu -> Show View -> Target Configurations

6. Right-click on User Defined. Select New Target Configuration.

7. Use `CC3200.ccxml` as "File name". Hit Finish.

8. In the Basic window, select "Stellaris In-Circuit Debug Interface" as the "Connection", and check the box next to "CC3200" in "Board or Device". Hit Save.

9. Right-click "CC3200.ccxml" in the Target Configurations View. Hit Launch Selected Configuration.

10. Under the Debug View, right-click on "Stellaris In-Circuit Debug Interface_0/Cortex_M4_0". Select "Connect Target".

<a name="Run-TOOL"></a>
## Running The Certificate Flasher Tool
All samples rely on a set of certificates from AWS. As a result, the certificates need to be stored once into flash memory prior to running the samples. To flash the certificates, simply run the flasher tool you have previously [built](#Build-TOOL) using this procedure:

1. Select Run menu -> Load -> Load Program..., and browse to the file `certflasher.out` in `<AWS_INSTALL_DIR>/samples/tirtos/certs/cc3200`. Hit OK. This will load the program onto the board.

2. Run the application by pressing F8. The output in the CCS Console looks as follows:

    ```
    Flashing ca certificate file ...
    Flashing client certificate file ...
    Flashing client key file ...
    done.
    ```

3. Hit Alt-F8 (Suspend) to halt the CPU.

<a name="Run-SAMPLE"></a>
## Running A Sample
1. Disconnect and reconnect the CC3200's USB cable to power cycle the hardware, and then reconnect in CCS.  For best results, it is recommended to do this before loading and running an application every time, in order to reset the CC3200's network processor.

2. Select Run menu -> Load -> Load Program..., and browse to the file `subscribe_publish_sample.out` in `<AWS_INSTALL_DIR>/samples/tirtos/subscribe_publish_sample/cc3200`. Hit OK. This will load the program onto the board. (The same procedure applies to other samples by substituting `subscribe_publish_sample`)

3. Run the application by pressing F8. The output will appear in your serial terminal session:

    ```
    CC3200 has connected to AP and acquired an IP address.
    IP Address: 192.168.1.130

    AWS IoT SDK Version 2.1.0-

    Connecting...
    Subscribing...
    -->sleep
    Subscribe callback
    sdkTest/sub     hello from SDK QOS0 : 0
    Subscribe callback
    sdkTest/sub     hello from SDK QOS1 : 1
    -->sleep
    ```
