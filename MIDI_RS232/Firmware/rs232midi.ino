/* RS232 <-> MIDI
HW Serial for serial
SoftSerial for MIDI
*/

#include <MIDI.h>
#include <AltSoftSerial.h>

AltSoftSerial SWSerial;

// A variable to know how long the LED has been turned on
elapsedMillis ledOnMillis;


// Create the Serial MIDI ports
//MIDI_CREATE_INSTANCE(SoftwareSerial, SWSerial, MIDIsoft);
MIDI_CREATE_INSTANCE(AltSoftSerial, SWSerial, MIDIsoft);

//OLD to run RS232 at 31250
//MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDIserial); 

//NEW1 to run RS232 at 38400
/*
struct CustomBaudRate : public MIDI_NAMESPACE::DefaultSettings {
  static const long BaudRate = 38400;
};

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial1, MIDIserial, CustomBaudRate);

*/

//NEW2 to run RS232 at 38400
struct CustomBaudRateSettings : public MIDI_NAMESPACE::DefaultSerialSettings {
  static const long BaudRate = 38400;
};

MIDI_NAMESPACE::SerialMIDI<HardwareSerial, CustomBaudRateSettings> serialMIDI(Serial1);
MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial, CustomBaudRateSettings>> MIDIserial((MIDI_NAMESPACE::SerialMIDI<HardwareSerial, CustomBaudRateSettings>&)serialMIDI);




void setup() {
  pinMode(11, OUTPUT); // LED pin
  pinMode(10, INPUT); // rx pin for MIDI
  pinMode(9, OUTPUT);// tx pin for MIDI
  
  Serial1.begin(38400);

  MIDIserial.begin(MIDI_CHANNEL_OMNI);
  MIDIserial.turnThruOff();

  MIDIsoft.begin(MIDI_CHANNEL_OMNI);
  MIDIsoft.turnThruOff();

/*
  usbMIDI.setHandleStart(USBStart);
  usbMIDI.setHandleStop(USBStop);
  usbMIDI.setHandleContinue(USBContinue);
  usbMIDI.setHandleClock(USBClock);
*/

    digitalWriteFast(11, HIGH); // LED on
    delay(50);
    digitalWriteFast(11, LOW);
    delay(50);
    digitalWriteFast(11, HIGH); // LED on
    delay(50);
    digitalWriteFast(11, LOW);

}

/*
void USBStart {

}

void USBStop {
  
}

void USBContinue {

}

void USBClock {
  
}

*/


void loop() {
  bool activity = false;

  if (MIDIserial.read()) {
    // get a MIDI IN1 (Serial) message
    byte type = MIDIserial.getType();
    byte channel = MIDIserial.getChannel();
    byte data1 = MIDIserial.getData1();
    byte data2 = MIDIserial.getData2();

    // forward the message to USB MIDI virtual cable #0
    if (type != midi::SystemExclusive) {
      // Normal messages, simply give the data to the usbMIDI.send()
      usbMIDI.send(type, data1, data2, channel, 0);
      MIDIsoft.send(type, data1, data2, channel);
   
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDIserial.getSysExArray(), true, 0);
      MIDIsoft.sendSysEx(SysExLength, MIDIserial.getSysExArray(), true);

    }
    activity = true;
  }

  if (MIDIsoft.read()) {
    // get a MIDI IN2 (Serial) message
    byte type = MIDIsoft.getType();
    byte channel = MIDIsoft.getChannel();
    byte data1 = MIDIsoft.getData1();
    byte data2 = MIDIsoft.getData2();

    // forward the message to USB MIDI virtual cable #1
    if (type != midi::SystemExclusive) {
      // Normal messages, simply give the data to the usbMIDI.send()
      usbMIDI.send(type, data1, data2, channel, 0);
      MIDIserial.send(type, data1, data2, channel);
      
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDIsoft.getSysExArray(), true, 0);
      MIDIserial.sendSysEx(SysExLength, MIDIsoft.getSysExArray(), true);

    }
    activity = true;
  }

  if (usbMIDI.read()) {
    // get a MIDI IN1 (Serial) message
    byte type = usbMIDI.getType();
    byte channel = usbMIDI.getChannel();
    byte data1 = usbMIDI.getData1();
    byte data2 = usbMIDI.getData2();

    // forward the message to USB MIDI virtual cable #0
    if (type != midi::SystemExclusive) {
      // Normal messages, simply give the data to the usbMIDI.send()
      MIDIserial.send(type, data1, data2, channel);
      MIDIsoft.send(type, data1, data2, channel);
      
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      MIDIserial.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
      MIDIsoft.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);

    }
    activity = true;
  }


  // blink the LED when any activity has happened
  if (activity) {
    digitalWriteFast(11, HIGH); // LED on
    ledOnMillis = 0;
  }
  if (ledOnMillis > 15) {
    digitalWriteFast(11, LOW);  // LED off
  }

}
