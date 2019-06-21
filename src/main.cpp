#include <unistd.h>
#include <libusb-1.0/libusb.h>
#include <iostream>
#include <sysexits.h>
#include <dlfcn.h>
#include "PwrUSBImp.h"

int main(int argc, char *argv[]) 
{
    std::cout << "Hello world!" << std::endl;
        int model;
        if(InitPowerUSB(&model) > 0)
        {
            SetPortPowerUSB(1,1,1);
        }


    return 0;
}
