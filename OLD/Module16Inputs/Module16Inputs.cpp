/*
There is a slew rate or propagation delay problem, thus we must have a short delay after each
digitalWrite mux change that is preceding an analogRead.Even 1ms is perfectly sufficient.

The above fixes the issue with having 0 values when nothing is connected to the inputs. However, it
does not fix the other issue of cross-contamination between channels. To fix that one we must have
a dummy read followed by a short delay before each real read. The effect is even better if we have
2 dummy reads with a selay after each one.
*/

#include "Arduino.h"
#include "Module16Inputs.h"

//Constructor
Module16Inputs::Module16Inputs(){
	enableModule();
	pinMode(x, OUTPUT);
	pinMode(s1, OUTPUT);
	pinMode(s0, OUTPUT);
	pinMode(_A4,INPUT);
	pinMode(_A5,INPUT);
}

//Methods
void Module16Inputs::enableModule(){
	digitalWrite(_enPin,OUTPUT);
	pinMode(_enPin, HIGH);
}
void Module16Inputs::disableModule(){
	pinMode(_enPin, LOW);
}
void Module16Inputs::setMuxesChannel(uint8_t n){
	bool xVal = (n%2==1);
	bool s1Val = !(n<9);
	bool s0Val = (n==5 || n==6 || n==7 || n==8 || n==13 || n==14 || n==15 || n==16);
	digitalWrite(x, xVal);
	digitalWrite(s1, s1Val);
	digitalWrite(s0, s0Val);
	delay(settleTime); //required for the MCU signal to rise after switching.
}
uint16_t Module16Inputs::readChannel(uint8_t n){
	if(n<1 || n>16) return -1;

	uint8_t analogPin = (n==1 || n==2 || n==5 || n==6 || n==9 || n==10 || n==13 || n==14) ? A5 : A4;
	setMuxesChannel(n); //this one has a 1ms delay built-in
  analogRead(analogPin);
  delay(settleTime);
	return analogRead(analogPin);
}
uint16_t Module16Inputs::readChannelMovAvg(uint8_t n, uint8_t samples){
	if(samples==1) return readChannel(n);

	//we can either shift the array item by 1 before each new read,
	//or we can just store each new value into a new index which would be far more efficient.
	//To use the second approach, I will have the array index go up only to <samples and then back to 0.
	//I must use an array size that will always be greater than samples. I will assume up to 10 samples.
	sampleNumber++; //this is the array index
	if(sampleNumber==samples) sampleNumber=0;
	lastSamples[sampleNumber] = readChannel(n);
	float avg=0;
	for(uint8_t i=0; i<samples; i++){
		avg += lastSamples[i];
	}
	return (uint16_t) avg/samples;
}

void Module16Inputs::read16ChannelsMovAvgInto(uint16_t a[], uint8_t samples){ //argument is the array provided by user containing the averaged results only.
	if(samples==1){
		read16ChannelsInto(a);
		return;
	}
	sampleNumber++; //this is the array index
	if(sampleNumber==samples) sampleNumber=0;
	read16ChannelsInto(lastSamplesAllChannels[sampleNumber]); //first index is the sample number (0 to 49). Second index is channel number. We don't write the second index in the argument.
	//Now we want to average over the first index for all 16 channels.
	float avgCh[16]={0};
	for(uint8_t i=0; i<samples; i++){
		//Keep these not in a loop for clarity.
		avgCh[0] += lastSamplesAllChannels[i][0];
		avgCh[1] += lastSamplesAllChannels[i][1];
		avgCh[2] += lastSamplesAllChannels[i][2];
		avgCh[3] += lastSamplesAllChannels[i][3];
		avgCh[4] += lastSamplesAllChannels[i][4];
		avgCh[5] += lastSamplesAllChannels[i][5];
		avgCh[6] += lastSamplesAllChannels[i][6];
		avgCh[7] += lastSamplesAllChannels[i][7];
		avgCh[8] += lastSamplesAllChannels[i][8];
		avgCh[9] += lastSamplesAllChannels[i][9];
		avgCh[10] += lastSamplesAllChannels[i][10];
		avgCh[11] += lastSamplesAllChannels[i][11];
		avgCh[12] += lastSamplesAllChannels[i][12];
		avgCh[13] += lastSamplesAllChannels[i][13];
		avgCh[14] += lastSamplesAllChannels[i][14];
		avgCh[15] += lastSamplesAllChannels[i][15];
	}
	for(uint8_t i=0; i<samples; i++){
		a[i] = (uint16_t) avgCh[i]/samples;
	}
}
//It is strange that I cannot pass an array
void Module16Inputs::read16ChannelsInto(uint16_t a[]){ //The results are stored into the array data16Channels
	//Read all 16 values and store them into the array passed in the argument. Index 0 is sensor 1.
	setMuxesChannel(1);
	analogRead(A5);
  delay(settleTime);
  analogRead(A5);
  delay(settleTime);
	a[0] = analogRead(A5);

	setMuxesChannel(5);
	analogRead(A5);
	delay(settleTime);
	analogRead(A5);
	delay(settleTime);
	a[4] = analogRead(A5);

	setMuxesChannel(9);
	analogRead(A5);
  delay(settleTime);
  analogRead(A5);
  delay(settleTime);
	a[8] = analogRead(A5);

	setMuxesChannel(13);
	analogRead(A5);
  delay(settleTime);
  analogRead(A5);
  delay(settleTime);
	a[12] = analogRead(A5);

	setMuxesChannel(2);
	analogRead(A5);
	delay(settleTime);
	analogRead(A5);
	delay(settleTime);
	a[1] = analogRead(A5);

	setMuxesChannel(6);
  analogRead(A5);
  delay(settleTime);
  analogRead(A5);
  delay(settleTime);
	a[5] = analogRead(A5);

	setMuxesChannel(10);
	analogRead(A5);
  delay(settleTime);
  analogRead(A5);
  delay(settleTime);
	a[9] = analogRead(A5);

	setMuxesChannel(14);
	analogRead(A5);
  delay(settleTime);
  analogRead(A5);
  delay(settleTime);
	a[13] = analogRead(A5);

	setMuxesChannel(3);
	analogRead(A4);
  delay(settleTime);
  analogRead(A4);
  delay(settleTime);
	a[2] = analogRead(A4);

	setMuxesChannel(7);
	analogRead(A4);
  delay(settleTime);
  analogRead(A4);
  delay(settleTime);
	a[6] = analogRead(A4);

	setMuxesChannel(11);
	analogRead(A4);
  delay(settleTime);
  analogRead(A4);
  delay(settleTime);
	a[10] = analogRead(A4);

	setMuxesChannel(15);
	analogRead(A4);
  delay(settleTime);
  analogRead(A4);
  delay(settleTime);
	a[14] = analogRead(A4);

	setMuxesChannel(4);
	analogRead(A4);
  delay(settleTime);
  analogRead(A4);
  delay(settleTime);
	a[3] = analogRead(A4);

	setMuxesChannel(8);
	analogRead(A4);
  delay(settleTime);
  analogRead(A4);
  delay(settleTime);
	a[7] = analogRead(A4);

  setMuxesChannel(12);
	analogRead(A4);
  delay(settleTime);
  analogRead(A4);
  delay(settleTime);
	a[11] = analogRead(A4);

	setMuxesChannel(16);
	analogRead(A4);
  delay(settleTime);
  analogRead(A4);
  delay(settleTime);
	a[15] = analogRead(A4);
}
/*
------------------------
i  |  # | x s1 s0  pin
------------------------
0  |  1 | 1  0  0   A5 |
1  |  2 | 0  0  0   A5 |
2  |  3 | 1  0  0   A4 |
3  |  4 | 0  0  0   A4 |
4  |  5 | 1  0  1   A5 |
5  |  6 | 0  0  1   A5 |
6  |  7 | 1  0  1   A4 |
7  |  8 | 0  0  1   A4 |
8  |  9 | 1  0  1   A5 |
9  | 10 | 0  1  0   A5 |
10 | 11 | 1  1  0   A4 |
11 | 12 | 0  1  0   A4 |
12 | 13 | 1  1  0   A5 |
13 | 14 | 0  1  1   A5 |
14 | 15 | 1  1  1   A4 |
15 | 16 | 0  1  1   A4 |
*/
