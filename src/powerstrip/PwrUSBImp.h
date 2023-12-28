
#ifndef PWRUSBIMP_H__
#define PWRUSBIMP_H__

#define HID_API_EXPORT /**< API export macro */
#define HID_API_CALL /**< API call macro */
#define HID_API_EXPORT_CALL HID_API_EXPORT HID_API_CALL /**< API export and call macro*/

#define POWER_USB_MAXNUM 4

#ifdef __cplusplus
extern "C" {
#endif
// Functions for Version 1 and 2 Firmware PowerUSB. Version 3HID_API_EXPORT_CALL  3 ports read and write
///////////////////////////////////////////////////////////
// Main Power Strip functions
HID_API_EXPORT_CALL int InitPowerUSB(int *model);
HID_API_EXPORT_CALL int ClosePowerUSB();
HID_API_EXPORT_CALL int SetCurrentPowerUSB(int count); 
HID_API_EXPORT_CALL int CheckStatusPowerUSB();
HID_API_EXPORT_CALL int SetPortPowerUSB(int port1, int port2, int port3);
HID_API_EXPORT_CALL int SetDefaultStatePowerUSB(int state1, int state2, int port3);


// Functions for Version 2 and later Firmware PowerUSB
///////////////////////////////////////////////////////////
// these functions are available starting with Nov 2010 release of PowerUSB
HID_API_EXPORT_CALL int ReadPortStatePowerUSB(int *state1, int *state2, int *state3);	
HID_API_EXPORT_CALL int ReadDefaultPortStatePowerUSB(int *state1, int *state2, int *state3);	
HID_API_EXPORT_CALL int GetFirmwareVersionPowerUSB();
HID_API_EXPORT_CALL int GetModelPowerUSB();

// Current sensing and Read Functions
////////////////////////////////////
HID_API_EXPORT_CALL int ReadCurrentPowerUSB(int *current);
HID_API_EXPORT_CALL int ReadCurrentCumPowerUSB(int *currentCum);
HID_API_EXPORT_CALL int ResetCurrentCounterPowerUSB();
HID_API_EXPORT_CALL int SetCurrentSensRatioPowerUSB(int currentRatio);
HID_API_EXPORT_CALL int SetOverloadPowerUSB(int overload);
HID_API_EXPORT_CALL int GetOverloadPowerUSB();
HID_API_EXPORT_CALL int ResetBoard();
HID_API_EXPORT_CALL int SetCurrentOffset();

// Digital IO functions. Available only when Digital IO model of PowerUSB is connected
////////////////////////////////////
HID_API_EXPORT_CALL int SetIODirectionPowerUSB(int direction[]);
HID_API_EXPORT_CALL int SetOutputStatePowerUSB(int output[]);
HID_API_EXPORT_CALL int GetInputStatePowerUSB(int input[]);
HID_API_EXPORT_CALL int GenerateClockPowerUSB(int port, int onTime, int offTime);
HID_API_EXPORT_CALL int GetOutputStatePowerUSB(int output[]);

HID_API_EXPORT_CALL int SetInputTriggerPowerUSB(int port, int outlet1, int outlet2, int outlet3, int out1, int out2);

// Watchdog related functions. Available only for Watchdog firmware version of the PowerUSB
//////////////////////////////////////////////////////////////////////////////////////////////
HID_API_EXPORT_CALL int StartWatchdogTimerPowerUSB(int HbTimeSec, int numHbMisses, int resetTimeSec);
HID_API_EXPORT_CALL int StopWatchdogTimerPowerUSB();
HID_API_EXPORT_CALL int GetWatchdogStatusPowerUSB();		// returns 0=IDLE(off), 1=Running, 2=Resetting
HID_API_EXPORT_CALL int SendHeartBeatPowerUSB();
HID_API_EXPORT_CALL int PowerCyclePowerUSB(int resetTimeSec);
#ifdef __cplusplus
}
#endif

#endif
