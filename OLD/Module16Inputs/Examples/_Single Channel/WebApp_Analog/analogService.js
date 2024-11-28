/*This examples shows how to read one 16-bit integer value from a characteristic, and how to write
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
const chrAnalogValueUUID     = '0b0b0b0b-0b0b-0b0b-0b0b-c1000000aa07';

listOfServices.push(analogServiceUUID); //appends this service to the array (defined in conditions.js).

async function initAnalogService(flowio) {
  try{
    flowio.analogService = await flowio.bleServer.getPrimaryService(analogServiceUUID);
    flowio.analogService.chrAnalogValue = await flowio.analogService.getCharacteristic(chrAnalogValueUUID);

    //Subscribe to receive notifications from chaAnalogValue.
    await flowio.analogService.chrAnalogValue.startNotifications();
    flowio.analogService.chrAnalogValue.addEventListener('characteristicvaluechanged', event => { //an event is returned
      let adcValue = event.target.value.getUint16(0,true); //the 'true' is for the endianness.
      flowio.analog.value = adcValue;
      flowio.log("Ch" + flowio.analog.channel + " = " + adcValue);
      //console.log(event); //we can use this in the console to see all the goodies in the event object.
    });

    //########################--- Define .analog object and API Methods ---######################
    flowio.analog = new Object(); //This will be the object to which we will attach all our analog-related objects and Methods
    flowio.analog.numberOfSamples = 1; //used for the moving average computation. Single AND 16 channel. Changed from the GUI.
    flowio.analog.setNumberOfSamples = async function(){
      flowio.analog.numberOfSamples = document.getElementById(`samples_select${flowio.instanceNumber}`).value;
      //Continuous reading should stop if active as soon as I change the number of samples. Sending the stop command
      //to either characteristic is fine because the embedded code has a global stop flag that is set by both characteristics.
      let stopValue = new Uint8Array([0x00]); //stop Continuous reading.
      await flowio.analogService.chrAnalogValue.writeValue(stopValue);
    }
    flowio.analog.stopReadContinuous = async function(){
      let value = new Uint8Array([0x00]);
      await flowio.analogService.chrAnalogValue.writeValue(value);
    }
    //***********************--FOR Single Channel--*******************************
    flowio.analog.channel = 1; //the channel number.
    flowio.analog.setAnalogChannel = async function(){ //set the channel to the value chosen from the GUI
      //As soon as a new channel is chosen, request a reading from the new channel.
      //This means that in my requestOneReading function, I should check the channel value.
      flowio.analog.channel = document.getElementById(`channel_select${flowio.instanceNumber}`).value;
      if(flowio.analog.channel > 16 || flowio.analog.channel < 1) flowio.log("Incorrect channel number!");

      let value = new Uint8Array([0x01,flowio.analog.channel]); //Rquest 0x01 reading from the channel specified in the second byte.
      await flowio.analogService.chrAnalogValue.writeValue(value);
    }
    flowio.analog.requestReadSingle = async function(){
      let value = new Uint8Array([0x01,flowio.analog.channel]); //Rquest 0x01 reading from the channel specified in the second byte.
      await flowio.analogService.chrAnalogValue.writeValue(value);
    }
    flowio.analog.requestReadSingleContinuous = async function(){
      let value = new Uint8Array([0x02, flowio.analog.channel, flowio.analog.numberOfSamples]);
      await flowio.analogService.chrAnalogValue.writeValue(value);
    }
    flowio.analog.getValue = async function(){ //we don't use this in the GUI, but we want to have it in our API.
      let adcDataView = await flowio.analogService.chrAnalogValue.readValue(); //this returns a DataView
      //this triggers our event listener, and the value is logged by the event listener.
      let adcValue = adcDataView.getUint16(0,true); //the 'true' is for the endianness.
      return adcValue;
    }

    //########################################################################
    //Get the analog value after service is initialized.
    flowio.analog.requestReadSingle();
  }
  catch(error){
    flowio.log("FlowIO failed to initAnalogService(). " + error);
  }
}
