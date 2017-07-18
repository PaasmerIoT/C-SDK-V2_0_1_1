#include "../../../../deviceName.h"
#define UserName "Email Address" //your user name in website

#define timePeriod 6 //change the time delay as you required for sending actuator values to paasmer cloud

char* feedname[]={"feed1","feed2","feed3","feed4","feed5",.....}; //feed names you use in the website

char* feedtype[]={"actuator","sensor","sensor","actuator","actuator",.....}; //modify with the type of feeds i.e., actuator or sensor

char* feedbase[]={"GPIOA1_BASE","GPIOA2_BASE","GPIOA3_BASE","GPIOA2_BASE","GPIOA2_BASE",.....}; //modify with the GPIO Bases which you connected the devices (actuator or sensor)

char* feedpin[]={"0x10","0x2","0x30","0x40","0x50",.....}; //modify with GPIO Hexnumbers for the selected pin
