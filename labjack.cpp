#include "labjack.h"
#include <stdio.h> // For printf

extern "C" {
	#include <LabJackM.h> // For the LabJackM Library
    #include "LJM_Utilities.h" // For LabJackM helper functions
}

labjack :: labjack()
{
	handle = 0;
	errorAddress = INITIAL_ERR_ADDRESS;
}

labjack :: ~labjack() {}

int labjack :: lj_init() // return handle
{
	//errorAddress = INITIAL_ERR_ADDRESS;

    int err;
	const int INTERVAL_HANDLE = 1;

	// Set up for configuring the AINs
    // AIN0, AIN1, and AIN2:
	//   Negative channel = single ended (199)
	//   Range: +/-10.0 V (10.0). T4 note: Only AIN0-AIN3 can support +/-10 V range.
	//   Resolution index = Default (0)
	//   Settling, in microseconds = Auto (0)
    enum { NUM_FRAMES_CONFIG = 12 };
	const char * aNamesConfig[NUM_FRAMES_CONFIG] = \
		{"AIN0_NEGATIVE_CH", "AIN0_RANGE", "AIN0_RESOLUTION_INDEX", "AIN0_SETTLING_US",
         "AIN1_NEGATIVE_CH", "AIN1_RANGE", "AIN1_RESOLUTION_INDEX", "AIN1_SETTLING_US",
         "AIN2_NEGATIVE_CH", "AIN2_RANGE", "AIN2_RESOLUTION_INDEX", "AIN2_SETTLING_US"};
	const double aValuesConfig[NUM_FRAMES_CONFIG] = {199, 10, 0, 0,
                                                     199, 10, 0, 0,
                                                     199, 10, 0, 0};

	int msDelay = 1000;

	// Open first found LabJack
    try { handle = OpenOrDie(LJM_dtANY, LJM_ctANY, "LJM_idANY"); }
    catch (...)
    {
        //printf("ERROR could not open labjack\n");
        return -1;
    }

	// handle = OpenSOrDie("LJM_dtANY", "LJM_ctANY", "LJM_idANY");
	if(handle < 0)
    {
        //printf("ERROR could not open labjack\n");
        return -1;
    }

	//PrintDeviceInfoFromHandle(handle);

	// Setup and call eWriteNames to configure AINs on the LabJack.
	err = LJM_eWriteNames(handle, NUM_FRAMES_CONFIG, aNamesConfig, aValuesConfig,
		&errorAddress);
	//ErrorCheckWithAddress(err, errorAddress, "LJM_eWriteNames");
	if(err != 0)
    {
        //printf("ERROR writing configuration to labjack\n");
        return -2;
    }
/*
	printf("\nSet configuration:\n");
    for (int i=0; i<NUM_FRAMES_CONFIG; i++) {
		printf("    %s : %f\n", aNamesConfig[i], aValuesConfig[i]);
	}
*/
	err = LJM_StartInterval(INTERVAL_HANDLE, msDelay * 100); // previously 1000, now set to 100ms
	//ErrorCheck(err, "LJM_StartInterval");
	if(err != 0)
    {
        //printf("ERROR writing configuration to labjack\n");
        return -3;
    }

	return handle;
}

int labjack :: lj_close()
{
	const int INTERVAL_HANDLE = 1;
	int err = LJM_CleanInterval(INTERVAL_HANDLE);
	//PrintErrorIfError(err, "LJM_CleanInterval");
	if(err != 0)
	{
		printf("ERROR occured while cleaning interval\n");
        return -1;
	}

	CloseOrDie(handle);
    return 0;
}

double * labjack :: lj_read()
{
	int err, SkippedIntervals;
	const int INTERVAL_HANDLE = 1;
	//int errorAddress = INITIAL_ERR_ADDRESS;

	// Set up for reading AIN values
    enum { NUM_FRAMES_AIN = 3 };
    double aValuesAIN[NUM_FRAMES_AIN] = {0};
    double* vals = (double*)malloc(3 * sizeof(double));
    const char * aNamesAIN[NUM_FRAMES_AIN] = {"AIN0", "AIN1", "AIN2"};

	//printf("reading labjack...\n");

	// Read AIN from the LabJack
	err = LJM_eReadNames(handle, NUM_FRAMES_AIN, aNamesAIN, aValuesAIN, &errorAddress);
	//ErrorCheckWithAddress(err, errorAddress, "LJM_eReadNames");
	if(err != 0)
	{
		printf("ERROR could not read from labjack\n");
        //return NULL;
	}

	//printf("%s : %f V, %s : %f V\n", aNamesAIN[0], aValuesAIN[0], aNamesAIN[1], aValuesAIN[1]);

	err = LJM_WaitForNextInterval(INTERVAL_HANDLE, &SkippedIntervals);
	//ErrorCheck(err, "LJM_WaitForNextInterval");
	if(err != 0)
	{
		printf("ERROR occured while waiting for next interval\n");
        //return NULL;
	}

	if (SkippedIntervals > 0) {
        //printf("skipped intervals: %d\n", SkippedIntervals);
    }

    vals[0] = aValuesAIN[0];
    vals[1] = aValuesAIN[1];
    vals[2] = aValuesAIN[2];
    pnt = aValuesAIN;
    return vals;
}
