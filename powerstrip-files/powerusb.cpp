#include"powerusb.h"

int (*InitPowerUSB_Address)(int *);
int (*CheckStatusPowerUSB_Address)(void);
int (*ReadPortStatePowerUSB_Address)(int *, int *, int *);
int (*ReadDefaultPortStatePowerUSB_Address)(int *, int *, int *);
int (*SetPortPowerUSB_Address)(int, int, int);
int (*SetDefaultStatePowerUSB_Address)(int, int, int);

int port11 = 1, port12 = 0, port13 = 0;
int port21 = 0, port22 = 1, port23 = 0;
int port31 = 0, port32 = 0, port33 = 1;

PowerUSB :: PowerUSB(int argc, char* argv[])
{
	loadSharedLibrary();
	initPowerUSB();
	checkStatusPowerUSB();
	handleRequest(argc, argv);
}

void PowerUSB :: loadSharedLibrary()
{
	sharedLibraryHandle = dlopen ("./lib/64bit/libpowerusb.so", RTLD_LAZY);
	if (!sharedLibraryHandle) 
	{
            fputs (dlerror(), stderr);
	    sharedLibraryHandle = NULL;
            exit(1);
        }
}

void PowerUSB :: initPowerUSB()
{
	int i = 1;
	InitPowerUSB_Address = (int (*)(int *))dlsym(sharedLibraryHandle, "InitPowerUSB");
	(*InitPowerUSB_Address)(&i);
}

void PowerUSB :: checkStatusPowerUSB()
{
	CheckStatusPowerUSB_Address = (int (*)(void))dlsym(sharedLibraryHandle, "CheckStatusPowerUSB");
	int status = (*CheckStatusPowerUSB_Address)();
	if(!status)
	{
		printf("PowerUSB Not Connected\n");
		exit(1);
	}
}

int PowerUSB :: handleRequest(int argc, char* argv[])
{
	if(argc == 1)
	{
		printf("%s: missing file operand\n", argv[0]);
		printf("Try `%s --help` for more information.\n", argv[0]);
	}
	else if(argc == 2)
	{
		if(strcmp(argv[1], "--help") == 0)
		{
			printf("Usage: 1) powerusb portnumber state       ---Sets the state of the respective port---\n");
			printf("          portnumber = 1-3, state = on/off\n");			
			printf("          Ex: powerusb 2 on               ---Switches on Port 2---\n");
			printf("       2) powerusb -d portnumber state    ---Sets the default state of the respective port---\n");
			printf("          portnumber = 1-3, state = on/off\n");
			printf("          Ex: powerusb -d 2 on            ---Sets the default state of Port 2 to on---\n");			
			printf("       3) powerusb portnumber             ---Reads the state of the respective port---\n");
			printf("          portnumber = 1-3\n");
			printf("          Ex: powerusb 2                  ---Returns the state of Port 2---\n");
			printf("       4) powerusb -d portnumber          ---Reads the default state of the respective port---\n");
			printf("          portnumber = 1-3\n");
			printf("          Ex: powerusb -d 2               ---Returns the default state of Port 2---\n");
		}
		else if(strcmp(argv[1], "--help") != 0)
		{
			if(strcmp(argv[1], "1") == 0)
			{	
				ReadPortStatePowerUSB_Address = (int (*)(int *, int *, int *))dlsym(sharedLibraryHandle, "ReadPortStatePowerUSB");
				readPortStatus((*ReadPortStatePowerUSB_Address)(&port11, &port12, &port13), 1);
			}
			else if(strcmp(argv[1], "2") == 0)
			{
				ReadPortStatePowerUSB_Address = (int (*)(int *, int *, int *))dlsym(sharedLibraryHandle, "ReadPortStatePowerUSB");
				readPortStatus((*ReadPortStatePowerUSB_Address)(&port21, &port22, &port23), 2);
			}
			else if(strcmp(argv[1], "3") == 0)
			{	
				ReadPortStatePowerUSB_Address = (int (*)(int *, int *, int *))dlsym(sharedLibraryHandle, "ReadPortStatePowerUSB");
				readPortStatus((*ReadPortStatePowerUSB_Address)(&port31, &port32, &port33), 3);
			}
			else
			{
				printf("Invalid Argument: Try `%s --help` for more information.\n", argv[0]);
			}
		}
	}
	else if(argc == 3)
	{
		if(strcmp(argv[1], "-d") == 0)
		{
			if(strcmp(argv[2], "1") == 0)
			{
				ReadDefaultPortStatePowerUSB_Address = (int (*)(int *, int *, int *))dlsym(sharedLibraryHandle, "ReadDefaultPortStatePowerUSB");
				readDefaultPortStatus((*ReadDefaultPortStatePowerUSB_Address)(&port11, &port12, &port13), 1);
			}
			else if(strcmp(argv[2], "2") == 0)
			{
				ReadDefaultPortStatePowerUSB_Address = (int (*)(int *, int *, int *))dlsym(sharedLibraryHandle, "ReadDefaultPortStatePowerUSB");
				readDefaultPortStatus((*ReadDefaultPortStatePowerUSB_Address)(&port21, &port22, &port23), 2);
			}
			else if(strcmp(argv[2], "3") == 0)
			{
				ReadDefaultPortStatePowerUSB_Address = (int (*)(int *, int *, int *))dlsym(sharedLibraryHandle, "ReadDefaultPortStatePowerUSB");
				readDefaultPortStatus((*ReadDefaultPortStatePowerUSB_Address)(&port31, &port32, &port33), 3);
			}
			else
			{
				printf("Invalid Argument(s): Try `%s --help` for more information.\n", argv[0]);
			}
		}
		else if(strcmp(argv[1], "-d") != 0)
		{
			if(strcmp(argv[1], "1") == 0 && strcmp(argv[2], "on") == 0)
			{
				SetPortPowerUSB_Address = (int (*)(int, int, int))dlsym(sharedLibraryHandle, "SetPortPowerUSB");
				(*SetPortPowerUSB_Address)(1, -1, -1);
			}
			else if(strcmp(argv[1], "1") == 0 && strcmp(argv[2], "off") == 0)
			{
				SetPortPowerUSB_Address = (int (*)(int, int, int))dlsym(sharedLibraryHandle, "SetPortPowerUSB");
				(*SetPortPowerUSB_Address)(0, -1, -1);
			}
			else if(strcmp(argv[1], "2") == 0 && strcmp(argv[2], "on") == 0)
			{
				SetPortPowerUSB_Address = (int (*)(int, int, int))dlsym(sharedLibraryHandle, "SetPortPowerUSB");
				(*SetPortPowerUSB_Address)(-1, 1, -1);
			}
			else if(strcmp(argv[1], "2") == 0 && strcmp(argv[2], "off") == 0)
			{
				SetPortPowerUSB_Address = (int (*)(int, int, int))dlsym(sharedLibraryHandle, "SetPortPowerUSB");
				(*SetPortPowerUSB_Address)(-1, 0, -1);
			}
			else if(strcmp(argv[1], "3") == 0 && strcmp(argv[2], "on") == 0)
			{
				SetPortPowerUSB_Address = (int (*)(int, int, int))dlsym(sharedLibraryHandle, "SetPortPowerUSB");
				(*SetPortPowerUSB_Address)(-1, -1, 1);
			}
			else if(strcmp(argv[1], "3") == 0 && strcmp(argv[2], "off") == 0)
			{
				SetPortPowerUSB_Address = (int (*)(int, int, int))dlsym(sharedLibraryHandle, "SetPortPowerUSB");
				(*SetPortPowerUSB_Address)(-1, -1, 0);
			}
			else
			{
				printf("Invalid Argument(s): Try `%s --help` for more information.\n", argv[0]);	
			}
		}
		else
		{
			printf("Invalid Argument(s): Try `%s --help` for more information.\n", argv[0]);
		}
	}
	else if(argc == 4)
	{
		if(strcmp(argv[1], "-d") == 0)
		{
			if(strcmp(argv[2], "1") == 0 && strcmp(argv[3], "on") == 0)
			{
				SetDefaultStatePowerUSB_Address = (int (*)(int, int, int))dlsym(sharedLibraryHandle, "SetDefaultStatePowerUSB");
				(*SetDefaultStatePowerUSB_Address)(1, -1, -1);
			}
			else if(strcmp(argv[2], "1") == 0 && strcmp(argv[3], "off") == 0)
			{
				SetDefaultStatePowerUSB_Address = (int (*)(int, int, int))dlsym(sharedLibraryHandle, "SetDefaultStatePowerUSB");
				(*SetDefaultStatePowerUSB_Address)(0, -1, -1);
			}
			else if(strcmp(argv[2], "2") == 0 && strcmp(argv[3], "on") == 0)
			{
				SetDefaultStatePowerUSB_Address = (int (*)(int, int, int))dlsym(sharedLibraryHandle, "SetDefaultStatePowerUSB");
				(*SetDefaultStatePowerUSB_Address)(-1, 1, -1);
			}
			else if(strcmp(argv[2], "2") == 0 && strcmp(argv[3], "off") == 0)
			{
				SetDefaultStatePowerUSB_Address = (int (*)(int, int, int))dlsym(sharedLibraryHandle, "SetDefaultStatePowerUSB");
				(*SetDefaultStatePowerUSB_Address)(-1, 0, -1);
			}
			else if(strcmp(argv[2], "3") == 0 && strcmp(argv[3], "on") == 0)
			{
				SetDefaultStatePowerUSB_Address = (int (*)(int, int, int))dlsym(sharedLibraryHandle, "SetDefaultStatePowerUSB");
				(*SetDefaultStatePowerUSB_Address)(-1, -1, 1);
			}
			else if(strcmp(argv[2], "3") == 0 && strcmp(argv[3], "off") == 0)
			{
				SetDefaultStatePowerUSB_Address = (int (*)(int, int, int))dlsym(sharedLibraryHandle, "SetDefaultStatePowerUSB");
				(*SetDefaultStatePowerUSB_Address)(-1, -1, 0);
			}
			else
			{
				printf("Invalid Argument(s): Try `%s --help` for more information.\n", argv[0]);	
			}
		}
		else
		{
			printf("Invalid Argument(s): Try `%s --help` for more information.\n", argv[0]);	
		}
	}
	else
	{
		printf("Incorrect Usage: Try `%s --help` for more information.\n", argv[0]);
	}
	return 1;
}

void PowerUSB :: readPortStatus(int status, int port)
{
	if(status == 0)printf("Port %d = Off\n", port);
	if(status == 1)printf("Port %d = On\n", port);
}

void PowerUSB :: readDefaultPortStatus(int status, int port)
{
	if(status == 0)printf("Default Port %d State = Off\n", port);
	if(status == 1)printf("Default Port %d State = On\n", port);
}

int main(int argc, char* argv[])
{
	new PowerUSB(argc, argv);
	return 1;
}
