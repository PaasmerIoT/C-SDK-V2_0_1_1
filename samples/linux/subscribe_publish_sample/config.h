#include "../../../deviceName.h"
#define UserName "Email Address" //your user name in website

#define timePeriod 6 //change the time delay as you required for sending actuator values to paasmer cloud

char* feedname[]={"feed1","feed2","feed3","feed4","feed5",.....}; //feed names you use in the website

char* feedtype[]={"actuator","sensor","sensor","actuator","actuator",.....}; //modify with the type of feeds i.e., actuator or sensor

int feedpin[]={3,5,7,11,13,....}; //modify with the pin numbers which you connected devices (actuator or sensor)
