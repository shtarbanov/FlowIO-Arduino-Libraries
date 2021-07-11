/* Module16Inputs.h - a library for the 16 input module

I can implement this as part of the FlowIO main library and then invoke the functions here
on the flowio instance.

Alternatively, I can invoke them as flowio.Module16Inputs.functionName()

I can also have my FlowIO library inherit this library. See https://www.tutorialspoint.com/cplusplus/cpp_inheritance.htm
*/

#ifndef Module16Inputs_h
#define Module16Inputs_h

#include "Arduino.h"

class Module16Inputs{
private:
	uint8_t _enPin=2;
	uint8_t x=26;
	uint8_t s0=25;
	uint8_t s1=24;

	uint8_t _A4=A4;
	uint8_t _A5=A5;
	uint8_t settleTime=1;

	//The following arrays are for the moving average calculations.
	//The arrays must be initialized, otherwise I get garbage initially.
	//TODO: initialize the arrays with the first value read rather than 0 if possible.
	uint16_t lastSamples[10] = {0}; //This array will hold the last samples (up to 50) for cases where only 1 channel is used.
	uint16_t lastSamplesAllChannels[10][16] = {0}; //this will hold the last 50 samples for all 16 channels.
	uint8_t sampleNumber=0; //index for the moving arverage array.

public:
	Module16Inputs(); //Constructor
	void setMuxesChannel(uint8_t n);
	void enableModule();
	void disableModule();
	uint16_t readChannel(uint8_t n); //rename to readInput
	void read16ChannelsInto(uint16_t a[]); //argument is the array that will store the values.

	//Moving average functions
	uint16_t readChannelMovAvg(uint8_t n, uint8_t samples);
	void read16ChannelsMovAvgInto(uint16_t a[], uint8_t samples);
};

#endif
