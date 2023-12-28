#include <iostream>
#include <numeric>
#include <stdlib.h>
#include <signal.h>
#include "PwrUSBImp.h"
#include <chrono>
#include <thread>

int main(int argc, char *argv[])
{
    long wait_time = 10;
    
    if (argc > 1) {
        wait_time = std::strtol(argv[1], NULL, 10);
    }

    int model;
    int ret = InitPowerUSB(&model);

    std::cout << "Start! Will power cycle every " << wait_time << " minutes..." << std::endl;
    std::cout << ret << std::endl;
    SetPortPowerUSB(0,0,0);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    SetPortPowerUSB(1,1,1);
    
    while (true) {
        std::cout << "Waiting for " << wait_time << " minutes..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(60 * wait_time));
        std::cout << "Power cycling strip, waiting 30 sec before powering on..." << std::endl;
        SetPortPowerUSB(0,0,0);
        std::this_thread::sleep_for(std::chrono::seconds(10));
        SetPortPowerUSB(1,1,1);
    }
    return 0;
}
