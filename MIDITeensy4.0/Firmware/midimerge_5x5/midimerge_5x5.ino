/* Create a "class compliant " USB to 3 MIDI IN and 3 MIDI OUT interface.

   MIDI receive (6N138 optocoupler) input circuit and series resistor
   outputs need to be connected to Serial1, Serial2 and Serial3.

   You must select MIDIx4 from the "Tools > USB Type" menu

   This example code is in the public domain.
*/

#include <MIDI.h>

// Create the Serial MIDI ports
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI1);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI2);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI3);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial4, MIDI4);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial5, MIDI5);

// A variable to know how long the LED has been turned on
elapsedMillis ledOnMillis;


void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT); // LED pin
  MIDI1.begin(MIDI_CHANNEL_OMNI);
  MIDI2.begin(MIDI_CHANNEL_OMNI);
  MIDI3.begin(MIDI_CHANNEL_OMNI);
  MIDI4.begin(MIDI_CHANNEL_OMNI);
  MIDI5.begin(MIDI_CHANNEL_OMNI);
  MIDI1.turnThruOff();
  MIDI2.turnThruOff();
  MIDI3.turnThruOff();
  MIDI4.turnThruOff();
  MIDI5.turnThruOff();
  
    digitalWriteFast(13, HIGH); // LED on
    delay(500);
    digitalWriteFast(13, LOW);
}


void loop() {
  bool activity = false;

  if (MIDI1.read()) {
    // get a MIDI IN1 (Serial) message
    byte type = MIDI1.getType();
    byte channel = MIDI1.getChannel();
    byte data1 = MIDI1.getData1();
    byte data2 = MIDI1.getData2();

    // forward the message to USB MIDI virtual cable #0
    if (type != midi::SystemExclusive) {
      // Normal messages, simply give the data to the usbMIDI.send()
      usbMIDI.send(type, data1, data2, channel, 0);
      MIDI1.send(type, data1, data2, channel);
      MIDI2.send(type, data1, data2, channel);
      MIDI3.send(type, data1, data2, channel);      
      MIDI4.send(type, data1, data2, channel);  
      MIDI5.send(type, data1, data2, channel);  
    
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI1.getSysExArray(), true, 0);
      MIDI1.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
      MIDI2.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
      MIDI3.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
      MIDI4.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
      MIDI5.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
    }
    activity = true;
  }

  if (MIDI2.read()) {
    // get a MIDI IN2 (Serial) message
    byte type = MIDI2.getType();
    byte channel = MIDI2.getChannel();
    byte data1 = MIDI2.getData1();
    byte data2 = MIDI2.getData2();

    // forward the message to USB MIDI virtual cable #1
    if (type != midi::SystemExclusive) {
      // Normal messages, simply give the data to the usbMIDI.send()
      usbMIDI.send(type, data1, data2, channel, 1);
      MIDI1.send(type, data1, data2, channel);
      MIDI2.send(type, data1, data2, channel);
      MIDI3.send(type, data1, data2, channel);  
      MIDI4.send(type, data1, data2, channel);  
      MIDI5.send(type, data1, data2, channel);  
      
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI2.getSysExArray(), true, 1);
      MIDI1.sendSysEx(SysExLength, MIDI2.getSysExArray(), true);
      MIDI2.sendSysEx(SysExLength, MIDI2.getSysExArray(), true);
      MIDI3.sendSysEx(SysExLength, MIDI2.getSysExArray(), true);
      MIDI4.sendSysEx(SysExLength, MIDI2.getSysExArray(), true);
      MIDI5.sendSysEx(SysExLength, MIDI2.getSysExArray(), true);      
    }
    activity = true;
  }

  if (MIDI3.read()) {
    // get a MIDI IN1 (Serial) message
    byte type = MIDI3.getType();
    byte channel = MIDI3.getChannel();
    byte data1 = MIDI3.getData1();
    byte data2 = MIDI3.getData2();

    // forward the message to USB MIDI virtual cable #0
    if (type != midi::SystemExclusive) {
      // Normal messages, simply give the data to the usbMIDI.send()
      usbMIDI.send(type, data1, data2, channel, 2);
      MIDI1.send(type, data1, data2, channel);
      MIDI2.send(type, data1, data2, channel);
      MIDI3.send(type, data1, data2, channel);
      MIDI4.send(type, data1, data2, channel);  
      MIDI5.send(type, data1, data2, channel);          
      
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI3.getSysExArray(), true, 2);
      MIDI1.sendSysEx(SysExLength, MIDI3.getSysExArray(), true);
      MIDI2.sendSysEx(SysExLength, MIDI3.getSysExArray(), true);
      MIDI3.sendSysEx(SysExLength, MIDI3.getSysExArray(), true);
      MIDI4.sendSysEx(SysExLength, MIDI3.getSysExArray(), true);
      MIDI5.sendSysEx(SysExLength, MIDI3.getSysExArray(), true);      
    }
    activity = true;
  }

  if (MIDI4.read()) {
    // get a MIDI IN1 (Serial) message
    byte type = MIDI4.getType();
    byte channel = MIDI4.getChannel();
    byte data1 = MIDI4.getData1();
    byte data2 = MIDI4.getData2();

    // forward the message to USB MIDI virtual cable #0
    if (type != midi::SystemExclusive) {
      // Normal messages, simply give the data to the usbMIDI.send()
      usbMIDI.send(type, data1, data2, channel, 3);
      MIDI1.send(type, data1, data2, channel);
      MIDI2.send(type, data1, data2, channel);
      MIDI3.send(type, data1, data2, channel);
      MIDI4.send(type, data1, data2, channel);  
      MIDI5.send(type, data1, data2, channel);          
      
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI4.getSysExArray(), true, 3);
      MIDI1.sendSysEx(SysExLength, MIDI4.getSysExArray(), true);
      MIDI2.sendSysEx(SysExLength, MIDI4.getSysExArray(), true);
      MIDI3.sendSysEx(SysExLength, MIDI4.getSysExArray(), true);
      MIDI4.sendSysEx(SysExLength, MIDI4.getSysExArray(), true);
      MIDI5.sendSysEx(SysExLength, MIDI4.getSysExArray(), true);      
    }
    activity = true;
  }

  if (MIDI5.read()) {
    // get a MIDI IN1 (Serial) message
    byte type = MIDI5.getType();
    byte channel = MIDI5.getChannel();
    byte data1 = MIDI5.getData1();
    byte data2 = MIDI5.getData2();

    // forward the message to USB MIDI virtual cable #0
    if (type != midi::SystemExclusive) {
      // Normal messages, simply give the data to the usbMIDI.send()
      usbMIDI.send(type, data1, data2, channel, 4);
      MIDI1.send(type, data1, data2, channel);
      MIDI2.send(type, data1, data2, channel);
      MIDI3.send(type, data1, data2, channel);
      MIDI4.send(type, data1, data2, channel);  
      MIDI5.send(type, data1, data2, channel);          
      
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI5.getSysExArray(), true, 4);
      MIDI1.sendSysEx(SysExLength, MIDI5.getSysExArray(), true);
      MIDI2.sendSysEx(SysExLength, MIDI5.getSysExArray(), true);
      MIDI3.sendSysEx(SysExLength, MIDI5.getSysExArray(), true);
      MIDI4.sendSysEx(SysExLength, MIDI5.getSysExArray(), true);
      MIDI5.sendSysEx(SysExLength, MIDI5.getSysExArray(), true);      
    }
    activity = true;
  }



  if (usbMIDI.read()) {
    // get the USB MIDI message, defined by these 5 numbers (except SysEX)
    byte type = usbMIDI.getType();
    byte channel = usbMIDI.getChannel();
    byte data1 = usbMIDI.getData1();
    byte data2 = usbMIDI.getData2();
    byte cable = usbMIDI.getCable();

    // forward this message to 1 of the 3 Serial MIDI OUT ports
    if (type != usbMIDI.SystemExclusive) {
      // Normal messages, first we must convert usbMIDI's type (an ordinary
      // byte) to the MIDI library's special MidiType.
      midi::MidiType mtype = (midi::MidiType)type;

      // Then simply give the data to the MIDI library send()
      switch (cable) {
        case 0:
          MIDI1.send(mtype, data1, data2, channel);
          break;
        case 1:
          MIDI2.send(mtype, data1, data2, channel);
          break;
        case 2:
          MIDI3.send(mtype, data1, data2, channel);
          break;
        case 3:
          MIDI4.send(mtype, data1, data2, channel);
          break;
        case 4:
          MIDI5.send(mtype, data1, data2, channel);
          break;
      }

    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      switch (cable) {
        case 0:
          MIDI1.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
        case 1:
          MIDI2.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
        case 2:
          MIDI3.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
        case 3:
          MIDI4.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
        case 4:
          MIDI5.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
      }
    }
    activity = true;
  }

  // blink the LED when any activity has happened
  if (activity) {
    digitalWriteFast(13, HIGH); // LED on
    ledOnMillis = 0;
  }
  if (ledOnMillis > 15) {
    digitalWriteFast(13, LOW);  // LED off
  }

}
