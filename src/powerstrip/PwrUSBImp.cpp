#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <locale.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <pthread.h>

//#include <dlfcn.h>

#include "PwrUSBHid.h"
#include "PwrUSBImp.h"

#define MY_DEVICE_ID  "Vid_04d8&Pid_003F"			// This is the Vendor ID and Product ID for PowerUSB microcontroller

#define BUF_LEN 256
#define BUF_WRT 65

// Commands to write to PowerUSB
#define ON_PORT1			'A'
#define OFF_PORT1			'B'
#define ON_PORT2			'C'
#define OFF_PORT2			'D'
#define ON_PORT3			'E'
#define OFF_PORT3			'P'

#define DEFON_PORT1			'N'
#define DEFOFF_PORT1			'F'
#define DEFON_PORT2			'G'
#define DEFOFF_PORT2			'Q'
#define DEFON_PORT3			'O'
#define DEFOFF_PORT3			'H'

#define	READ_P1				0xa1
#define	READ_P2				0xa2
#define	READ_P3				0xac

#define	READ_P1_PWRUP			0xa3
#define	READ_P2_PWRUP			0xa4
#define	READ_P3_PWRUP			0xad
	
#define	READ_FIRMWARE_VER		0xa7
#define READ_MODEL			0xaa

#define READ_CURRENT			0xb1
#define READ_CURRENT_CUM		0xb2
#define RESET_CURRENT_COUNT		0xb3
#define	WRITE_OVERLOAD			0xb4
#define READ_OVERLOAD			0xb5
#define SET_CURRENT_RATIO		0xb6
#define RESET_BOARD			0xc1
#define SET_CURRENT_OFFSET		0xc2

#define	ALL_PORT_ON			0xa5
#define	ALL_PORT_OFF			0xa6
#define	SET_MODE			0xa8
#define	READ_MODE			0xa9 

// Digital IO
#define	SET_IO_DIRECTION	0xd1
#define	SET_IO_OUTPUT		0xd3
#define	GET_IO_INPUT		0xd4
#define	SET_IO_CLOCK		0xd5
#define GET_IO_OUTPUT		0xd6
#define SET_IO_TRIGGER		0xd7

#define TRUE  1
#define FALSE 0

// Local Functions
int CheckPowerUSBConnected();
int WriteData(int len);
int USBRead();

// GLOBAL Variables
hid_device *AttachedDeviceHandles[POWER_USB_MAXNUM];
int AttachedDeviceCount, CurrentDevice;
int AttachedState;
unsigned char OUTBuffer[65];	//Allocate a memory buffer equal to the OUT endpoint size + 1
unsigned char INBuffer[65];		//Allocate a memory buffer equal to the IN endpoint size + 1

void *sharedLibraryHandle;	//Pointer to Shared Library

//Pointers to functions in Shared Library
/*struct hid_device_info* (*EnumeratePowerUSB)(int, int);
void (*FreeEnumerationPowerUSB)(struct hid_device_info *);
hid_device* (*OpenPathPowerUSB)(const char *);
int (*SetNonBlockingPowerUSB)(hid_device *, int);
int (*ReadFromPowerUSB)(hid_device *, unsigned char *, int);
int (*WriteToPowerUSB)(hid_device *, unsigned char *, int);
*/
#ifdef __cplusplus
extern "C" {
#endif


HID_API_EXPORT_CALL int InitPowerUSB(int *model)
{
	int i, r;
	AttachedState = 0;
	AttachedDeviceCount = 0;
	CurrentDevice = 0;
	*model = 0;

	// Load the linux libhid.so which supports HID class functions
	/*sharedLibraryHandle = dlopen ("./lib/libhid.so", RTLD_LAZY);
	if (!sharedLibraryHandle) 
	{
            fputs (dlerror(), stderr);
            exit(1);
        }
	*/
	if ((AttachedDeviceCount = CheckPowerUSBConnected()) > 0)		
	{
		// For each of the connected devices, check to see whether it can read and write.
		for (i = 0; i < AttachedDeviceCount; i++)
		{
			if(AttachedDeviceHandles[i] != NULL)
			{
				AttachedState = TRUE;		//Let the rest of the PC application know the USB device is connected, and it is safe to read/write to it
			}
			else //for some reason the device was physically plugged in, but one or both of the read/write handles didn't open successfully...
			{
				AttachedState = FALSE;		//Let the rest of this application known not to read/write to the device.
				r = -1;
			}
		}
	}
	else	//Device must not be connected (or not programmed with correct firmware)
	{
		AttachedState = FALSE;
		r = -1;
	}
	CurrentDevice = AttachedDeviceCount - 1;
	if (r > 0)
	{
		*model = GetModelPowerUSB();
		return AttachedDeviceCount;
	}
	return r;
}

HID_API_EXPORT_CALL int ClosePowerUSB()
{
	if (AttachedState)
	{
		AttachedState = FALSE;
		for (int i = 0; i < AttachedDeviceCount; i++)
		{
			free(AttachedDeviceHandles[i]);
			AttachedDeviceHandles[i] = NULL;
		}
	}
	return 0;
}

HID_API_EXPORT_CALL int SetCurrentPowerUSB(int count)
{
	if (count >= AttachedDeviceCount)
		count = AttachedDeviceCount - 1;
	CurrentDevice = count;
	return count;
}

// Checks to see if PowerUSB device is connected to computer
/////////////////////////////////////////////////////////////////
HID_API_EXPORT_CALL int CheckStatusPowerUSB()
{
	if (CheckPowerUSBConnected() > 0)
		return 1;
	return 0;
}

// Sets the state of the outlet 1 and 2
// A=Outlet1 On. B=Outlet1 Off
// C=Outlet2 On  D=Outlet2 Off
////////////////////////////////////////
HID_API_EXPORT_CALL int SetPortPowerUSB(int port1, int port2, int port3)
{
	int r;

	if(AttachedState != TRUE)
		return -1;

	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	if (port1 >= 0)
	{
		if (port1)
			OUTBuffer[1]= ON_PORT1;
		else
			OUTBuffer[1] = OFF_PORT1;
		r = WriteData(2);
		usleep(20*1000);
	}

	if (port2 >= 0)
	{
		if (port2)
			OUTBuffer[1] = ON_PORT2;
		else
			OUTBuffer[1] = OFF_PORT2;
		r = WriteData(2);
		usleep(20*1000);	
	}

	if (port3 >= 0 && port3 <= 1)
	{
		if (port3)
			OUTBuffer[1] = ON_PORT3;
		else
			OUTBuffer[1] = OFF_PORT3;
		r = WriteData(2);	
	}

	USBRead();			// read and clear the input buffer
	return r;
}

HID_API_EXPORT_CALL int SetDefaultStatePowerUSB(int state1, int state2, int state3)
{
	int r;

	if(AttachedState != TRUE)
		return -1;

	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	if (state1 >= 0)
	{
		if (state1)
			OUTBuffer[1]= DEFON_PORT1;
		else
			OUTBuffer[1] = DEFOFF_PORT1;
		r = WriteData(2);
		usleep(20*1000);
	}

	if (state2 >= 0)
	{
		if (state2)
			OUTBuffer[1] = DEFON_PORT2;
		else
			OUTBuffer[1] = DEFOFF_PORT2;
		r = WriteData(2);
		usleep(20*1000);	
	}

	if (state3 >= 0 && state3 <= 1)
	{
		if (state3)
			OUTBuffer[1] = DEFON_PORT3;
		else
			OUTBuffer[1] = DEFOFF_PORT3;
		r = WriteData(2);	
	}
	USBRead();			// read and clear the input buffer
	return r;
}

HID_API_EXPORT_CALL int ReadPortStatePowerUSB(int *port1, int *port2, int *port3)
{
	int r;

	if(AttachedState != TRUE)
		return -1;
	
	if (*port1 > 0)
	{
		OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
		OUTBuffer[1]= READ_P1;
		WriteData(2);
		usleep(20*1000);
		USBRead();
		r = (INBuffer[0]==0 ? 0:1);
		usleep(30*1000);
	}
	if (*port2 > 0)
	{
		OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
		OUTBuffer[1]= READ_P2;
		WriteData(2);
		usleep(20*1000);
		USBRead();
		r = (INBuffer[0]==0 ? 0:1);
		usleep(30*1000);
	}
	if (*port3 > 0)
	{
		OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
		OUTBuffer[1]= READ_P3;
		WriteData(2);
		usleep(20*1000);
		USBRead();
		r = (INBuffer[0]==0 ? 0:1);
		usleep(30*1000);
	}
	return r;
}

HID_API_EXPORT_CALL int ReadDefaultPortStatePowerUSB(int *port1, int *port2, int *port3)
{
	int r;

	if(AttachedState != TRUE)
		return -1;

	if (*port1 > 0)
	{
		OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
		OUTBuffer[1]= READ_P1_PWRUP;
		WriteData(2);
		usleep(20*1000);
		USBRead();
		r = (INBuffer[0]==0 ? 0:1);
		usleep(30*1000);
	}
	if (*port2 > 0)
	{
		OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
		OUTBuffer[1]= READ_P2_PWRUP;
		WriteData(2);
		usleep(20*1000);
		USBRead();
		r = (INBuffer[0]==0 ? 0:1);
		usleep(30*1000);
	}
	if (*port3 > 0)
	{
		OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
		OUTBuffer[1]= READ_P3_PWRUP;
		WriteData(2);
		usleep(20*1000);
		USBRead();
		r = (INBuffer[0]==0 ? 0:1);
		usleep(30*1000);
	}
	return r;
}

// Reads the current version of firmware
// As of Nov 1st 2010. This function is not implemented in firmware
///////////////////////////////////////////////////////////////////////
HID_API_EXPORT_CALL int GetFirmwareVersionPowerUSB()
{
	int r;
	if(AttachedState != TRUE)
		return -1;
	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	OUTBuffer[1]= READ_FIRMWARE_VER;
	r = WriteData(2);
	USBRead();
	r = INBuffer[0];
	usleep(20*1000);
	return r;
}

// Returns
//0: Not connected or unknown
//1: Basic model. Computer companion
//2: Digital IO model. 
//3: Computer Watchdog Model
//4: Smart Pro Model
HID_API_EXPORT_CALL int GetModelPowerUSB()
{
	int r;
	if(AttachedState != TRUE)
		return -1;
	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	OUTBuffer[1]= READ_MODEL;
	r = WriteData(2);
	usleep(50*1000);
	USBRead();
	r = INBuffer[0];
	usleep(20*1000);
	return r;
}

// Current Sensing Related Functions
////////////////////////////////////////////////////
HID_API_EXPORT_CALL int ReadCurrentPowerUSB(int *current)
{
	int r;
	if(AttachedState != TRUE)
		return -1;
	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	OUTBuffer[1]= READ_CURRENT;
	r = WriteData(2);
	usleep(20*1000);
	r = USBRead();
	if (r >= 0)
		*current = INBuffer[0]<<8 | INBuffer[1];
	else
		*current = 0;
	return r;
}

HID_API_EXPORT_CALL int ReadCurrentCumPowerUSB(int *currentCum)
{
	int r;
	if(AttachedState != TRUE)
		return -1;
	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	OUTBuffer[1]= READ_CURRENT_CUM;
	r = WriteData(2);
	usleep(20*1000);
	r = USBRead();
	if (r >= 0)
		*currentCum = INBuffer[0]<<24 | INBuffer[1]<<16 | INBuffer[2]<<8 | INBuffer[3];
	else
		*currentCum = 0;
	return r;
}

HID_API_EXPORT_CALL int ResetCurrentCounterPowerUSB()
{
	int r;
	if(AttachedState != TRUE)
		return -1;
	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	OUTBuffer[1]= RESET_CURRENT_COUNT;
	r = WriteData(2);
	usleep(20*1000);
	USBRead();			// read and clear the input buffer
	return r;
}

HID_API_EXPORT_CALL int SetCurrentSensRatioPowerUSB(int currentRatio)
{
	int r;
	if(AttachedState != TRUE)
		return -1;
	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	OUTBuffer[1]= SET_CURRENT_RATIO;
	OUTBuffer[2]= currentRatio;
	r = WriteData(3);
	r = USBRead();
	if (r >= 0)
		return INBuffer[0];
	return -1;
}

HID_API_EXPORT_CALL int SetOverloadPowerUSB(int overload)
{
	int r;
	if(AttachedState != TRUE)
		return -1;
	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	OUTBuffer[1]= WRITE_OVERLOAD;
	OUTBuffer[2] = overload; 
	r = WriteData(2);
	usleep(20*1000);
	USBRead();			// read and clear the input buffer
	return 0;
}

HID_API_EXPORT_CALL int GetOverloadPowerUSB()
{
	int r;
	if(AttachedState != TRUE)
		return -1;
	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	OUTBuffer[1]= READ_OVERLOAD;
	r = WriteData(2);
	r = USBRead();
	if (r >= 0)
		return INBuffer[0];
	return -1;
}

HID_API_EXPORT_CALL int ResetBoard()
{
	int r;
	if(AttachedState != TRUE)
		return -1;
	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	OUTBuffer[1]= RESET_BOARD;
	r = WriteData(2);
	usleep(20*1000);
	USBRead();			// read and clear the input buffer
	return r;
}

HID_API_EXPORT_CALL int SetCurrentOffset()
{
	int r;
	if(AttachedState != TRUE)
		return -1;
	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	OUTBuffer[1]= SET_CURRENT_OFFSET;
	r = WriteData(2);
	usleep(20*1000);
	USBRead();			// read and clear the input buffer
	return r;
}


///////////////////////////////////////
// Digital IO related functions
/////////////////////////////////////
HID_API_EXPORT_CALL int SetIODirectionPowerUSB(int direction[])
{
	int r, i;
	unsigned char outFlag;

	if(AttachedState != TRUE)
		return -1;
	outFlag = 0;
	for (i = 0; i < 7; i++)			// 7 IO ports. put 7 bytes as bits in one byte
		if (direction[i])
			outFlag = (outFlag | (0x01 << i));
	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	OUTBuffer[1]= SET_IO_DIRECTION;
	OUTBuffer[2] = outFlag;
	r = WriteData(3);
	usleep(20*1000);
	USBRead();			// read and clear the input buffer
	return r;
}



HID_API_EXPORT_CALL int SetOutputStatePowerUSB(int output[])
{
	int r, i;
	unsigned char outFlag;

	if(AttachedState != TRUE)
		return -1;
	outFlag = 0;
	for (i = 0; i < 7; i++)			// 7 IO ports. put 7 bytes as bits in one byte
		if (output[i])
			outFlag = (outFlag | (0x01 << i));
	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	OUTBuffer[1]= SET_IO_OUTPUT;
	OUTBuffer[2] = outFlag;
	r = WriteData(3);
	usleep(20*1000);
	USBRead();			// read and clear the input buffer
	return r;
}




HID_API_EXPORT_CALL int GetInputStatePowerUSB(int input[])
{
	int r, i;
	unsigned char ch;

	if(AttachedState != TRUE)
		return -1;
	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	OUTBuffer[1]= GET_IO_INPUT;
	r = WriteData(2);
	usleep(30*1000);
	r = USBRead();			// read and clear the input buffer
	ch = INBuffer[0];
	if (r >= 0)
	{
		for (i = 0; i < 7; i++)
			input[i] = (ch >> i) & 0x01;
	}
	return r;
}


HID_API_EXPORT_CALL int GenerateClockPowerUSB(int port, int onTime, int offTime)
{
	int r;

	if(AttachedState != TRUE)
		return -1;
	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	OUTBuffer[1]= SET_IO_CLOCK;
	OUTBuffer[2] = port;
	OUTBuffer[3] = onTime;
	OUTBuffer[4] = offTime;
	r = WriteData(5);
	usleep(20*1000);
	USBRead();			// read and clear the input buffer
	return r;
}

HID_API_EXPORT_CALL int GetOutputStatePowerUSB(int output[])
{
	int r, i;
	unsigned char ch;

	if(AttachedState != TRUE)
		return -1;
	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	OUTBuffer[1]= GET_IO_OUTPUT;
	r = WriteData(2);
	usleep(30*1000);
	r = USBRead();			// read and clear the input buffer
	ch = INBuffer[0];
	if (r >= 0)
	{
		for (i = 0; i < 7; i++)
			output[i] = (ch >> i) & 0x01;
	}
	return r;
}

HID_API_EXPORT_CALL int SetInputTriggerPowerUSB(int port, int outlet1, int outlet2, int outlet3, int out1, int out2)
{
	int r;

	if(AttachedState != TRUE)
		return -1;
	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	OUTBuffer[1]= SET_IO_TRIGGER;
	OUTBuffer[2] = port;
	OUTBuffer[3] = (outlet1 >> 8) | 0x00ff;
	OUTBuffer[4] = (outlet1 | 0x00ff);
	OUTBuffer[5] = (outlet2 >> 8) | 0x00ff;
	OUTBuffer[6] = (outlet2 | 0x00ff);
	OUTBuffer[7] = (outlet3 >> 8) | 0x00ff;
	OUTBuffer[8] = (outlet3 | 0x00ff);
	OUTBuffer[9] = (out1 >> 8) | 0x00ff;
	OUTBuffer[10] = (out1 | 0x00ff);
	OUTBuffer[11] = (out2 >> 8) | 0x00ff;
	OUTBuffer[12] = (out2 | 0x00ff);
	r = WriteData(5);
	usleep(20*1000);
	USBRead();			// read and clear the input buffer
	return r;
}


// Watchdog related functions
/////////////////////////////////////

#define	START_WDT			0x90
#define	STOP_WDT			0x91		
#define	POWER_CYCLE			0x92
#define	READ_WDT 			0x93	//-> return the all status.
#define	HEART_BEAT			0x94

// Starts watchdog in the PowerUSB. 
// HbTimeSec: expected time for heartbeat
// numHbMisses: Number of accepted consicutive misses in heartbeat
// resetTimeSec: Amount of time to switch off the computer outlet
/////////////////////////////////////////////////////////////////////////

HID_API_EXPORT_CALL int StartWatchdogTimerPowerUSB(int HbTimeSec, int numHbMisses, int resetTimeSec)
{
	int r=0;
	if(AttachedState != TRUE)
		return -1;

	OUTBuffer[0] = 0;					
	OUTBuffer[1] = START_WDT;			// start watchdog code
	OUTBuffer[2] = 0;					// 
	OUTBuffer[3] = HbTimeSec;				// heartbeat time (all in seconds)
	OUTBuffer[4] = numHbMisses;			// hb times
	OUTBuffer[5] = resetTimeSec;			// reset time  
	r = WriteData(6);
	return r;
}

// Stops the watchdog timer in PowerUSB
/////////////////////////////////////////////
HID_API_EXPORT_CALL int StopWatchdogTimerPowerUSB()
{
	int r=0;
	if(AttachedState != TRUE)
		return -1;
	OUTBuffer[0] = 0;			
	OUTBuffer[1] = STOP_WDT;			// stop watchdog code
	r = WriteData(2);
	usleep(100*1000);
	USBRead();
	usleep(50*1000);
	return r;
}

// Get the current state of Watchdog in PowerUSB
// Return 0: watchdog is not running, 1: Watchdog is active, 2: In PowerCycle phase
///////////////////////////////////////////////////////////////////////////////////////
HID_API_EXPORT_CALL int GetWatchdogStatusPowerUSB()
{
	int r;
	if(AttachedState != TRUE)
		return -1;
	OUTBuffer[0] = 0;			//The first byte is the "Report ID" and does not get sent over the USB bus.  Always set = 0.
	OUTBuffer[1]= READ_WDT;
	r = WriteData(2);
	usleep(100*1000);
	USBRead();
	r = INBuffer[0];
	usleep(20*1000);
	return r;
}

// Sends the Heartbeat to PowerUSB
///////////////////////////////////
HID_API_EXPORT_CALL int SendHeartBeatPowerUSB()
{
	int r=0;
	if(AttachedState != TRUE)
		return -1;
	OUTBuffer[0] = 0;			
	OUTBuffer[1] = HEART_BEAT;			// send heart beat
	r = WriteData(2);
	return r;
}

// Switch off the computer outlet and switch it back on after resetTimeSec
///////////////////////////////////////////////////////////////////////////////
HID_API_EXPORT_CALL int PowerCyclePowerUSB(int resetTimeSec)
{
	int r=0;
	if(AttachedState != TRUE)
		return -1;
	OUTBuffer[0] = 0;			
	OUTBuffer[1] = POWER_CYCLE;			// send heart beat
	OUTBuffer[2] = resetTimeSec;
	r = WriteData(3);
	usleep(100*1000);
	USBRead();
	usleep(50*1000);
	return r;
}

#ifdef __cplusplus
}
#endif

// Writes the data to current open device
/////////////////////////////////////////////////////////////////////////////////////////////////
int WriteData(int len)
{
	int i, r=0;

	if(AttachedState != TRUE)
		return -1;

	for (i = len; i < BUF_WRT; i++)		//This loop is not strictly necessary.  Simply initializes unused bytes to//
		OUTBuffer[i] = 0xFF;		//0xFF for lower EMI and power consumption when driving the USB cable.
	//Now send the packet to the USB firmware on the microcontroller
	
	int status = hid_set_nonblocking(AttachedDeviceHandles[CurrentDevice], 1);		//Blocking function, unless an "overlapped" structure is used
        /*SetNonBlockingPowerUSB= (int (*)(hid_device *,int))dlsym(sharedLibraryHandle, "hid_set_nonblocking");
        int status = (*SetNonBlockingPowerUSB)(AttachedDeviceHandles[CurrentDevice], 1);*/
	
	if(status != -1)
	{
		/*WriteToPowerUSB = (int (*)(hid_device *, unsigned char *, int))dlsym(sharedLibraryHandle, "hid_write");
		r = (*WriteToPowerUSB)(AttachedDeviceHandles[CurrentDevice], OUTBuffer, BUF_WRT);*/
		r = hid_write(AttachedDeviceHandles[CurrentDevice], OUTBuffer, BUF_WRT);
	}
	return r;
}

// Reads the current open device
/////////////////////////////////////////////////////////////////////////////////////////////////
int USBRead()
{
	int r = -1; 

	if(AttachedState != TRUE)
		return -1;
	
	int status = hid_set_nonblocking(AttachedDeviceHandles[CurrentDevice], 1); 	//Blocking function, unless an "overlapped" structure is used
	/*SetNonBlockingPowerUSB= (int (*)(hid_device *,int))dlsym(sharedLibraryHandle, "hid_set_nonblocking");
	int status = (*SetNonBlockingPowerUSB)(AttachedDeviceHandles[CurrentDevice], 1);*/

	if(status != -1)
	{
		/*ReadFromPowerUSB = (int (*)(hid_device *, unsigned char *, int))dlsym(sharedLibraryHandle, "hid_read");
		r = (*ReadFromPowerUSB)(AttachedDeviceHandles[CurrentDevice], INBuffer, BUF_WRT);*/
		r = hid_read(AttachedDeviceHandles[CurrentDevice], INBuffer, BUF_WRT);
	}
	return r;
}

// Searches all the devices in the computer and selects the devices that match our PID and VID
/////////////////////////////////////////////////////////////////////////////////////////////////
int CheckPowerUSBConnected()
{
	int FoundIndex = 0;
	hid_device *DeviceHandle;
	struct hid_device_info *devs, *cur_dev;
	devs = hid_enumerate(0x4d8, 0x3f);
	/*EnumeratePowerUSB = (struct hid_device_info* (*)(int, int))dlsym(sharedLibraryHandle, "hid_enumerate");
	devs = (*EnumeratePowerUSB)(0x4d8, 0x3f);*/
	cur_dev = devs;	
	while (cur_dev) 
	{
		if(FoundIndex < POWER_USB_MAXNUM)
		{
			DeviceHandle = hid_open_path(cur_dev->path);
			/*OpenPathPowerUSB = (hid_device* (*)(const char *))dlsym(sharedLibraryHandle, "hid_open_path");
			DeviceHandle = (*OpenPathPowerUSB)(cur_dev->path);*/
			//if(DeviceHandle != NULL)
			//{
				AttachedDeviceHandles[FoundIndex] = DeviceHandle;
				FoundIndex = FoundIndex + 1;
			//}
		}
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);
	/*FreeEnumerationPowerUSB = (void (*)(struct hid_device_info *))dlsym(sharedLibraryHandle, "hid_free_enumeration");
	(*FreeEnumerationPowerUSB)(devs);*/
	return FoundIndex;
}
