#include <iostream>
#include <numeric>
#include <stdlib.h>
#include <signal.h>
#include "PwrUSBImp.h"
#include <chrono>
#include <thread>

int main(int argc, char *argv[])
{
    int model;
    int ret = InitPowerUSB(&model);

    std::cout << ret << std::endl;
    SetPortPowerUSB(0,0,0);
    return 0;
}
