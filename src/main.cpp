#include <unistd.h>
#include <libusb-1.0/libusb.h>
#include <iostream>
#include <sysexits.h>
#include <dlfcn.h>

void *sharedLibraryHandle = NULL;

int (*InitPowerUSB_Address)(int *);
int (*CheckStatusPowerUSB_Address)(void);
int (*ReadPortStatePowerUSB_Address)(int *, int *, int *);
int (*ReadDefaultPortStatePowerUSB_Address)(int *, int *, int *);
int (*SetPortPowerUSB_Address)(int, int, int);
int (*SetDefaultStatePowerUSB_Address)(int, int, int);

int port11 = 1, port12 = 0, port13 = 0;
int port21 = 0, port22 = 1, port23 = 0;
int port31 = 0, port32 = 0, port33 = 1;

int main(int argc, char *argv[]) 
{
    sharedLibraryHandle = dlopen ("libpowerusb.so", RTLD_LAZY);
				if (!sharedLibraryHandle) 
				{
				    fputs (dlerror(), stderr);
				    sharedLibraryHandle = NULL;
                }
    std::cout << "Hello world!" << std::endl;

    int i = 1;
    InitPowerUSB_Address = (int (*)(int *))dlsym(sharedLibraryHandle, "InitPowerUSB");
    (*InitPowerUSB_Address)(&i);

    CheckStatusPowerUSB_Address = (int (*)(void))dlsym(sharedLibraryHandle, "CheckStatusPowerUSB");
    int status = (*CheckStatusPowerUSB_Address)();
    if(!status)
    {
        printf("PowerUSB Not Connected\n");
        exit(1);
    }


    return 0;
}
