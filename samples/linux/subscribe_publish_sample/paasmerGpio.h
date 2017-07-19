#include "device.h"
#include <stdio.h>
char command[100];

void gpioSetup()
{
	if((!strcmp(deviceType,"raspberrypi")) || (!strcmp(deviceType,"bananapi")) || (!strcmp(deviceType,"orangepi")) || (!strcmp(deviceType,"odroidxu4")))
	{
		
	}
	else;
}

void gpioModesetup(int pinNum,char *mode)
{
	if((!strcmp(deviceType,"raspberrypi")) || (!strcmp(deviceType,"bananapi")) || (!strcmp(deviceType,"orangepi")) || (!strcmp(deviceType,"odroidxu4")))
	{
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
	if((!strcmp(deviceType,"raspberrypi")) || (!strcmp(deviceType,"bananapi")) || (!strcmp(deviceType,"orangepi")) || (!strcmp(deviceType,"odroidxu4")))
	{
		sprintf(command,"gpio -1 write %d %d",pinNum,state);
		system(command);
		strcpy(command,"\0");
	}
	else;
}

int gpioRead(int pinNum)
{
	int res=0;
	if((!strcmp(deviceType,"raspberrypi")) || (!strcmp(deviceType,"bananapi")) || (!strcmp(deviceType,"orangepi")) || (!strcmp(deviceType,"odroidxu4")))
	{
		sprintf(command,"gpio -1 read %d",pinNum);
		FILE *fp = popen(command,"r");
		char data[10];
		fgets(data,10,fp);
		pclose(fp);
		strcpy(command,"\0");
		res = atoi(data);
	}
	else;
	return res;
}
