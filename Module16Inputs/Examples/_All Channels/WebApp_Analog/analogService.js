/*This examples shows how to read 16-bit integer values from a characteristic, and how to write
commands of different byte size back to the same characteristic.

The API has the following methods and constants:

  flowios[0].analogService
  flowios[0].analogService.chrAnalogValue
  flowios[0].analogService.chrAnalogValues16

  flowios[0].analog
  flowis[0]o.analog.channel
  flowios[0].analog.numberOfSamples
  flowios[0].analog.setNumberOfSamples() //takes the value from the selector in the GUI

  flowios[0].analog.setAnalogChannel() //takes the value from the selector in the GUI
  flowios[0].analog.requestReadSingle() //sends a request for one value from the channel in .channel
  flowios[0].analog.requestReadSingleContinuous() // sends a request for continuous reading of moving average from the channel in .channel over .numberOfSamples samples.
  flowios[0].analog.getValue() //returns the analaog value and stores it in the property below. We don't use this in the GUI, but good to have in the API
  flowios[0].analog.value

  flowios[0].analog.requestRead16()
  flowios[0].analog.requestRead16Continuous()
  flowios[0].analog.getValues16() - places the new values into the .values16 array on the next line.
  flowios[0].analog.values16 - an array that holds our 16 values.

  flowios[0].analog.stopReadContinuous() //send the stop singnal (can be sent to either characteristic because the value is chared in the embedded code)
*/
'use strict'
const analogServiceUUID      = '0b0b0b0b-0b0b-0b0b-0b0b-00000000aa07';
const chrAnalogValues16UUID     = '0b0b0b0b-0b0b-0b0b-0b0b-c2000000aa07';

listOfServices.push(analogServiceUUID); //appends this service to the array (defined in conditions.js).

async function initAnalogService(flowio) {
  try{
    flowio.analogService = await flowio.bleServer.getPrimaryService(analogServiceUUID);
    flowio.analogService.chrAnalogValues16 = await flowio.analogService.getCharacteristic(chrAnalogValues16UUID);

    //Subscribe to receive notifications from chaAnalogValues16.
    await flowio.analogService.chrAnalogValues16.startNotifications();
    flowio.analogService.chrAnalogValues16.addEventListener('characteristicvaluechanged', event => { //an event is returned
      let adcValues = [];
      for(let i=0; i<16; i++){
        adcValues[i] = event.target.value.getUint16(i*2, true); //the true is for endianness. The first argument is
        //the offset in BYTES, which must be a multiple of 2, since each value is 2 bytes.
        // flowio.log("Ch" + (i+1) + " =\t " + adcValues[i]);
      }
      flowio.analog.values16 = adcValues;
      flowio.log(adcValues[0] + "\t" + adcValues[1] + "\t" + adcValues[2] + "\t" + adcValues[3] + "\t" + adcValues[4] + "\t" +
        adcValues[5] + "\t" + adcValues[6] + "\t" + adcValues[7] + "\t" + adcValues[8] + "\t" + adcValues[9] + "\t" + adcValues[10] +
         "\t" + adcValues[11] + "\t" + adcValues[12] + "\t" + adcValues[13] + "\t" + adcValues[14] + "\t" + adcValues[15]
      );
    });

    //########################--- Define .analog object and API Methods ---######################
    flowio.analog = new Object(); //This will be the object to which we will attach all our analog-related objects and Methods
    flowio.analog.numberOfSamples = 1; //used for the moving average computation.Changed from the GUI.
    flowio.analog.setNumberOfSamples = async function(){
      flowio.analog.numberOfSamples = document.getElementById(`samples_select${flowio.instanceNumber}`).value;
      //Continuous reading should stop if active as soon as I change the number of samples. Sending the stop command
      //to either characteristic is fine because the embedded code has a global stop flag that is set by both characteristics.
      let stopValue = new Uint8Array([0x00]); //stop Continuous reading
      await flowio.analogService.chrAnalogValues16.writeValue(stopValue);
    }
    flowio.analog.stopReadContinuous = async function(){
      let value = new Uint8Array([0x00]);
      await flowio.analogService.chrAnalogValues16.writeValue(value);
    }
    //***********************--For 16 Channels--*******************************
    flowio.analog.requestRead16 = async function(){
      let value = new Uint8Array([0x01]); //Rquest 0x01 reading from all the channels.
      await flowio.analogService.chrAnalogValues16.writeValue(value);
    }
    flowio.analog.requestRead16Continuous = async function(){
      let value = new Uint8Array([0x02,flowio.analog.numberOfSamples]);
      await flowio.analogService.chrAnalogValues16.writeValue(value);
    }
    flowio.analog.getValues16 = async function(){ //we don't use this in the GUI, but we want to have it in our API.
      let adc16DataView = await flowio.analogService.chrAnalogValues16.readValue(); //this returns a DataView
      //this triggers our event listener, and the value is logged by the event listener.
      let adcValues16 = [];
      for(let i=0; i<16; i++){
        adcValues16[i] = adc16DataView.getUint16(i*2, true); //the true is for endianness. The first argument is
        //the offset in BYTES, which must be a multiple of 2, since each value is 2 bytes.
      }
      flowio.analog.values16 = adcValues16;
      return adcValues16;
    }
    //########################################################################
    //Get the analog value after service is initialized.
    flowio.analog.requestRead16();
  }
  catch(error){
    flowio.log("FlowIO failed to initAnalogService(). " + error);
  }
}
