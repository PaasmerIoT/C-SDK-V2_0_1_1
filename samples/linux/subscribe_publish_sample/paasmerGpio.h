#include "device.h"
#include <stdio.h>
//#include <../../../../wiringPi/wiringPi/wiringPi.h>


//#define OUT 2
//#define IN 4
//#define HIGH 2
//#define LOW 4
//#define deviceType
char command[100];

void gpioSetup()
{
	if((strcmp(deviceType,"raspberrypi")) || (strcmp(deviceType,"bananapi")) || (strcmp(deviceType,"orangepi")) || (strcmp(deviceType,"odroidxu4")))
	//if(deviceType == "Raspberry")
	{
		//wiringPiSetup();
	}
	else;
}

void gpioModesetup(int pinNum,char *mode)
{
	//if(strcmp(deviceType,"raspberry"))
	if((strcmp(deviceType,"raspberrypi")) || (strcmp(deviceType,"bananapi")) || (strcmp(deviceType,"orangepi")) || (strcmp(deviceType,"odroidxu4")))
	{
		//pinMode(pinNum,mode);
		if(mode == "OUT")
		{
			sprintf(command,"gpio -1 mode %d out",pinNum);
			system(command);
			strcpy(command,"\0");
		}
		else if(mode == "IN")
		{
			sprintf(command,"gpio -1 mode %d in",pinNum);
                        system(command);
                        strcpy(command,"\0");

		}
	}
	else;
}

void gpioWrite(int pinNum,int state)
{
	//if(strcmp(deviceType,"raspberry"))
	if((strcmp(deviceType,"raspberrypi")) || (strcmp(deviceType,"bananapi")) || (strcmp(deviceType,"orangepi")) || (strcmp(deviceType,"odroidxu4")))
	{
		//digitalWrite(pinNum,state);
		sprintf(command,"gpio -1 write %d %d",pinNum,state);
		system(command);
		strcpy(command,"\0");
	}
	else;
}

int gpioRead(int pinNum)
{
	int res=0;
	if(strcmp(deviceType,"Raspberry"))
        {
		//res = digitalRead(pinNum);
		sprintf(command,"gpio -1 read %d",pinNum);
		//res = system(command);
		FILE *fp = popen(command,"r");
		char data[10];
		fgets(data,10,fp);
		pclose(fp);
		strcpy(command,"\0");
		res = atoi(data);
		//printf("%d",res);
	}
	else;
	return res;
}
