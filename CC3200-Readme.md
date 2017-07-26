# PAASMER-CC3200
**Paasmer IoT SDK** for Single Board Computers Running Linux and CC3200 

<a name="RequiredHW"></a>
#### Required Hardware
The following device is required for this SDK:
- [CC3200 Launchpad](http://www.ti.com/tool/cc3200-launchxl)

Please ensure that your device has been updated with the latest firmware and or service pack.

<a name="TI-SW"></a>
#### TI Software Installation
The following TI software products must be installed in order to build and run the sample applications. While not strictly required, we recommend that you install these products into a common directory and that you use directory names without any whitespace. This documentation assumes that you haved installed everything in a directory named `C:/ti`.

- Install [Code Composer Studio v6.1.2](http://www.ti.com/tool/ccstudio?keyMatch=code%20composer%20studio) or higher.

- Install [TI-RTOS for CC32xx 2.16.00.08](http://downloads.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/tirtos/index.html) or higher.


## Installation

* Download the SDK or clone it using the command below.
```
$ git clone https://github.com/PaasmerIoT/C-SDK-V2_0_1_1.git
$ cd C-SDK-V2_0_1_1
```

* To connect the device to Paasmer IoT Platfrom, the following steps need to be performed.


```
$ sudo ./install.sh
```
This will install all required softwares.
* To register the device to the Paasmer IoT Platform, the following command need to be executed.

```
$ sudo ./paasmerDeviceRegistration.sh
```
This will ask for the device name. Give a unique device name for your device and that must be alphanumeric[a-z A-Z 0-9].

* Upon successful completion of the above command, the following commands need to be executed.
```
echo "-->  1) sudo su "
echo "-->  2) source ~/.bashrc "
echo "-->  3) PAASMER_THING "
echo "-->  4) PAASMER_POLICY "
echo "-->  5) sed -i 's/alias PAASMER/#alias PAASMER/g' ~/.bashrc "
echo "-->  6) exit "
$ exit
```

* To fetch the certificates, Go to the directory below and run the following command
```
$ cd samples/tirtos/certs/cc3200

$ sudo ./fetchCerts.sh

$ cd ../../../../
```


<a name="Enviro-Setup"></a>
## Build Environment Set Up
### Updating Paths To Product Dependencies
1. Edit the `products.mak` file in `<AWS_INSTALL_DIR>/` using your favorite text editor.
2. Update the variables `XDC_INSTALL_DIR and TIRTOS_INSTALL_DIR to point to the locations where you installed these products.
3. The variable `TI_ARM_CODEGEN_INSTALL_DIR` should point to the installation location of the TI ARM compiler in your CCS installation.
4. After modification, these variable definitions should look similar to the following if you are working in Windows. (Windows users: note the use of "/" in the path).
    ```
    XDC_INSTALL_DIR = home\logname\ti\xdctools_3_32_00_06_core
    TIRTOS_INSTALL_DIR = home\logname\ti\tirtos_cc32xx_2_16_00_08
    TI_ARM_CODEGEN_INSTALL_DIR = home\logname\ti\ccsv6\tools\compiler\ti-cgt-arm_5.2.5
    ```

* Go to the diectory below.
```
$ cd samples/tirtos/subscribe_publish_sample/cc3200
```

* Edit the config.h file to include the user name(Email), device name, feed names and GPIO pin details.

```c
#define UserName "Email Address" //your user name in website

#define timePeriod 6 //change the time delay as you required for sending actuator values to paasmer cloud

char* feedname[]={"feed1","feed2","feed3","feed4","feed5",.....}; //feed names you use in the website

char* feedtype[]={"actuator","sensor","sensor","actuator","actuator",.....}; //modify with the type of feeds i.e., actuator or sensor

char* feedbase[]={"GPIOA1_BASE","GPIOA2_BASE","GPIOA3_BASE","GPIOA2_BASE","GPIOA2_BASE",.....}; //modify with the GPIO Bases which you connected the devices (actuator or sensor)

char* feedpin[]={"0x10","0x2","0x30","0x40","0x50",.....}; //modify with GPIO Hexnumbers for the selected pin
```
* Edit the wificonfig.h to include the SSID and Password of wifi network.
```c
#define SSID "SSID"
#define SECURITY_KEY "Password"
#define SECURITY_TYPE SL_SEC_TYPE_WPA
```

* After editing config.h and wificonfig.h go to the `<PAASMER-SDK_INSTALL_DIR>/`.
      
* Compile the code and generate output file.
```
$ sudo chmod 777 ./*
$ sudo ./cc3200_Build.sh 
```
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

1. Select Run menu -> Load -> Load Program..., and browse to the file `certflasher.out` in `<CC3200-SDK_INSTALL_DIR>/<YOUR DEVICE NAME FOLDER>`. Hit OK. This will load the program onto the board.

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

2. Select Run menu -> Load -> Load Program..., and browse to the file `subscribe_publish_sample.out` in `<CC3200-SDK_INSTALL_DIR>/<YOUR DEVICE NAME FOLDER>`. Hit OK. This will load the program onto the board. (The same procedure applies to other samples by substituting `subscribe_publish_sample`)

3. Run the application by pressing F8.

* The device would now be connected to the Paasmer IoT Platfrom and publishing sensor values atx specified intervals.

## Support

The support forum is hosted on the GitHub, issues can be identified by users and the Team from Paasmer would be taking up requstes and resolving them. You could also send a mail to support@paasmer.co with the issue details for quick resolution.

## Note

The Paasmer IoT C-SDK-V2_0_1_1 utilizes the features provided by AWS-IOT-SDK for C.
