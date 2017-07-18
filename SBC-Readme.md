# PAASMER-SBC
**Paasmer IoT SDK** for Single Board Computers Running Linux and cc3200

# Installation

* Download the SDK or clone it using the command below.
```
$ git clone github.com/PaasmerIoT/SBC-CC3200-SDK.git
$ cd SBC-CC3200-SDK
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

* Go to the diectory below.
```
$ cd samples/linux/subscribe_publish_sample/
```

* Edit the config.h file to include the user name(Email), device name, feed names and GPIO pin details.

```c
#define UserName "Email Address" //your user name in website

#define timePeriod 6 //change the time delay as you required for sending actuator values to paasmer cloud

char* feedname[]={"feed1","feed2","feed3","feed4","feed5",.....}; //feed names you use in the website

char* feedtype[]={"actuator","sensor","sensor","actuator","actuator",.....}; //modify with the type of feeds i.e., actuator or sensor

int feedpin[]={3,5,7,11,13,....}; //modify with the pin numbers which you connected devices (actuator or sensor)
```
      
* Compile the code and generate output file.
```
$ sudo make
```

* Run the code using the command below.
```
$ sudo ./subscribe_publish_sample
```

* The device would now be connected to the Paasmer IoT Platfrom and publishing sensor values are specified intervals.

## Support

The support forum is hosted on the GitHub, issues can be identified by users and the Team from Paasmer would be taking up requstes and resolving them. You could also send a mail to support@paasmer.co with the issue details for quick resolution.

## Note:

The Paasmer IoT SBC-CC3200-SDK utilizes the features provided by AWS-IOT-SDK for C.
