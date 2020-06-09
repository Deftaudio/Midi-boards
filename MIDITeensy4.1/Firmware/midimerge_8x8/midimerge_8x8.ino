/* Create a "class compliant " USB to 3 MIDI IN and 3 MIDI OUT interface.

   MIDI receive (6N138 optocoupler) input circuit and series resistor
   outputs need to be connected to Serial1, Serial2 and Serial3.

   You must select MIDIx4 from the "Tools > USB Type" menu

   This example code is in the public domain.
*/

#include <MIDI.h>
#include <USBHost_t36.h> // access to USB MIDI devices (plugged into 2nd USB port)


// Create the Serial MIDI ports
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI1);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI2);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI3);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial4, MIDI4);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial5, MIDI5);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial6, MIDI6);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial7, MIDI7);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial8, MIDI8);


// Create the ports for USB devices plugged into Teensy's 2nd USB port (via hubs)
USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
USBHub hub3(myusb);
USBHub hub4(myusb);
MIDIDevice midi01(myusb);
MIDIDevice midi02(myusb);
MIDIDevice midi03(myusb);
MIDIDevice midi04(myusb);
MIDIDevice midi05(myusb);
MIDIDevice midi06(myusb);
MIDIDevice midi07(myusb);
MIDIDevice midi08(myusb);
MIDIDevice * midilist[8] = {
  &midi01, &midi02, &midi03, &midi04, &midi05, &midi06, &midi07, &midi08,
};


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
  MIDI6.begin(MIDI_CHANNEL_OMNI);
  MIDI7.begin(MIDI_CHANNEL_OMNI);
  MIDI8.begin(MIDI_CHANNEL_OMNI);  
  MIDI1.turnThruOff();
  MIDI2.turnThruOff();
  MIDI3.turnThruOff();
  MIDI4.turnThruOff();
  MIDI5.turnThruOff();
  MIDI6.turnThruOff();
  MIDI7.turnThruOff();
  MIDI8.turnThruOff();
  
    digitalWriteFast(13, HIGH); // LED on
    delay(500);
    digitalWriteFast(13, LOW);

  myusb.begin();
  
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
      MIDI6.send(type, data1, data2, channel);
      MIDI7.send(type, data1, data2, channel);
      MIDI8.send(type, data1, data2, channel);
    
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI1.getSysExArray(), true, 0);
      MIDI1.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
      MIDI2.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
      MIDI3.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
      MIDI4.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
      MIDI5.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
      MIDI6.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
      MIDI7.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
      MIDI8.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
  
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
      MIDI6.send(type, data1, data2, channel);  
      MIDI7.send(type, data1, data2, channel);  
      MIDI8.send(type, data1, data2, channel);  
     
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI2.getSysExArray(), true, 1);
      MIDI1.sendSysEx(SysExLength, MIDI2.getSysExArray(), true);
      MIDI2.sendSysEx(SysExLength, MIDI2.getSysExArray(), true);
      MIDI3.sendSysEx(SysExLength, MIDI2.getSysExArray(), true);
      MIDI4.sendSysEx(SysExLength, MIDI2.getSysExArray(), true);
      MIDI5.sendSysEx(SysExLength, MIDI2.getSysExArray(), true);  
      MIDI6.sendSysEx(SysExLength, MIDI2.getSysExArray(), true); 
      MIDI7.sendSysEx(SysExLength, MIDI2.getSysExArray(), true); 
      MIDI8.sendSysEx(SysExLength, MIDI2.getSysExArray(), true);     
   
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
      MIDI6.send(type, data1, data2, channel);
      MIDI7.send(type, data1, data2, channel);
      MIDI8.send(type, data1, data2, channel);      
     
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI3.getSysExArray(), true, 2);
      MIDI1.sendSysEx(SysExLength, MIDI3.getSysExArray(), true);
      MIDI2.sendSysEx(SysExLength, MIDI3.getSysExArray(), true);
      MIDI3.sendSysEx(SysExLength, MIDI3.getSysExArray(), true);
      MIDI4.sendSysEx(SysExLength, MIDI3.getSysExArray(), true);
      MIDI5.sendSysEx(SysExLength, MIDI3.getSysExArray(), true);  
      MIDI6.sendSysEx(SysExLength, MIDI3.getSysExArray(), true);  
      MIDI7.sendSysEx(SysExLength, MIDI3.getSysExArray(), true);  
      MIDI8.sendSysEx(SysExLength, MIDI3.getSysExArray(), true);      
   
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
      MIDI6.send(type, data1, data2, channel);    
      MIDI7.send(type, data1, data2, channel);    
      MIDI8.send(type, data1, data2, channel);            
      
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI4.getSysExArray(), true, 3);
      MIDI1.sendSysEx(SysExLength, MIDI4.getSysExArray(), true);
      MIDI2.sendSysEx(SysExLength, MIDI4.getSysExArray(), true);
      MIDI3.sendSysEx(SysExLength, MIDI4.getSysExArray(), true);
      MIDI4.sendSysEx(SysExLength, MIDI4.getSysExArray(), true);
      MIDI5.sendSysEx(SysExLength, MIDI4.getSysExArray(), true);     
      MIDI6.sendSysEx(SysExLength, MIDI4.getSysExArray(), true);
      MIDI7.sendSysEx(SysExLength, MIDI4.getSysExArray(), true);
      MIDI8.sendSysEx(SysExLength, MIDI4.getSysExArray(), true);
      
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
      MIDI6.send(type, data1, data2, channel); 
      MIDI7.send(type, data1, data2, channel); 
      MIDI8.send(type, data1, data2, channel);      
      
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI5.getSysExArray(), true, 4);
      MIDI1.sendSysEx(SysExLength, MIDI5.getSysExArray(), true);
      MIDI2.sendSysEx(SysExLength, MIDI5.getSysExArray(), true);
      MIDI3.sendSysEx(SysExLength, MIDI5.getSysExArray(), true);
      MIDI4.sendSysEx(SysExLength, MIDI5.getSysExArray(), true);
      MIDI5.sendSysEx(SysExLength, MIDI5.getSysExArray(), true); 
      MIDI6.sendSysEx(SysExLength, MIDI5.getSysExArray(), true);
      MIDI7.sendSysEx(SysExLength, MIDI5.getSysExArray(), true);
      MIDI8.sendSysEx(SysExLength, MIDI5.getSysExArray(), true);  
        
    }
    activity = true;
  }

  if (MIDI6.read()) {
    // get a MIDI IN1 (Serial) message
    byte type = MIDI6.getType();
    byte channel = MIDI6.getChannel();
    byte data1 = MIDI6.getData1();
    byte data2 = MIDI6.getData2();

    // forward the message to USB MIDI virtual cable #0
    if (type != midi::SystemExclusive) {
      // Normal messages, simply give the data to the usbMIDI.send()
      usbMIDI.send(type, data1, data2, channel, 5);
      MIDI1.send(type, data1, data2, channel);
      MIDI2.send(type, data1, data2, channel);
      MIDI3.send(type, data1, data2, channel);
      MIDI4.send(type, data1, data2, channel);  
      MIDI5.send(type, data1, data2, channel);     
      MIDI6.send(type, data1, data2, channel); 
      MIDI7.send(type, data1, data2, channel); 
      MIDI8.send(type, data1, data2, channel);      
      
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI6.getSysExArray(), true, 5);
      MIDI1.sendSysEx(SysExLength, MIDI6.getSysExArray(), true);
      MIDI2.sendSysEx(SysExLength, MIDI6.getSysExArray(), true);
      MIDI3.sendSysEx(SysExLength, MIDI6.getSysExArray(), true);
      MIDI4.sendSysEx(SysExLength, MIDI6.getSysExArray(), true);
      MIDI5.sendSysEx(SysExLength, MIDI6.getSysExArray(), true); 
      MIDI6.sendSysEx(SysExLength, MIDI6.getSysExArray(), true);
      MIDI7.sendSysEx(SysExLength, MIDI6.getSysExArray(), true);
      MIDI8.sendSysEx(SysExLength, MIDI6.getSysExArray(), true);  
        
    }
    activity = true;
  }


    if (MIDI7.read()) {
    // get a MIDI IN1 (Serial) message
    byte type = MIDI7.getType();
    byte channel = MIDI7.getChannel();
    byte data1 = MIDI7.getData1();
    byte data2 = MIDI7.getData2();

    // forward the message to USB MIDI virtual cable #0
    if (type != midi::SystemExclusive) {
      // Normal messages, simply give the data to the usbMIDI.send()
      usbMIDI.send(type, data1, data2, channel, 6);
      MIDI1.send(type, data1, data2, channel);
      MIDI2.send(type, data1, data2, channel);
      MIDI3.send(type, data1, data2, channel);
      MIDI4.send(type, data1, data2, channel);  
      MIDI5.send(type, data1, data2, channel);     
      MIDI6.send(type, data1, data2, channel); 
      MIDI7.send(type, data1, data2, channel); 
      MIDI8.send(type, data1, data2, channel);      
      
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI7.getSysExArray(), true, 6);
      MIDI1.sendSysEx(SysExLength, MIDI7.getSysExArray(), true);
      MIDI2.sendSysEx(SysExLength, MIDI7.getSysExArray(), true);
      MIDI3.sendSysEx(SysExLength, MIDI7.getSysExArray(), true);
      MIDI4.sendSysEx(SysExLength, MIDI7.getSysExArray(), true);
      MIDI5.sendSysEx(SysExLength, MIDI7.getSysExArray(), true); 
      MIDI6.sendSysEx(SysExLength, MIDI7.getSysExArray(), true);
      MIDI7.sendSysEx(SysExLength, MIDI7.getSysExArray(), true);
      MIDI8.sendSysEx(SysExLength, MIDI7.getSysExArray(), true);  
       
    }
    activity = true;
  }


    if (MIDI8.read()) {
    // get a MIDI IN1 (Serial) message
    byte type = MIDI8.getType();
    byte channel = MIDI8.getChannel();
    byte data1 = MIDI8.getData1();
    byte data2 = MIDI8.getData2();

    // forward the message to USB MIDI virtual cable #0
    if (type != midi::SystemExclusive) {
      // Normal messages, simply give the data to the usbMIDI.send()
      usbMIDI.send(type, data1, data2, channel, 7);
      MIDI1.send(type, data1, data2, channel);
      MIDI2.send(type, data1, data2, channel);
      MIDI3.send(type, data1, data2, channel);
      MIDI4.send(type, data1, data2, channel);  
      MIDI5.send(type, data1, data2, channel);     
      MIDI6.send(type, data1, data2, channel); 
      MIDI7.send(type, data1, data2, channel); 
      MIDI8.send(type, data1, data2, channel);      
      
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI8.getSysExArray(), true, 7);
      MIDI1.sendSysEx(SysExLength, MIDI8.getSysExArray(), true);
      MIDI2.sendSysEx(SysExLength, MIDI8.getSysExArray(), true);
      MIDI3.sendSysEx(SysExLength, MIDI8.getSysExArray(), true);
      MIDI4.sendSysEx(SysExLength, MIDI8.getSysExArray(), true);
      MIDI5.sendSysEx(SysExLength, MIDI8.getSysExArray(), true); 
      MIDI6.sendSysEx(SysExLength, MIDI8.getSysExArray(), true);
      MIDI7.sendSysEx(SysExLength, MIDI8.getSysExArray(), true);
      MIDI8.sendSysEx(SysExLength, MIDI8.getSysExArray(), true);  
         
    }
    activity = true;
  }


  // Next read messages arriving from the (up to) 10 USB devices plugged into the USB Host port
  for (int port=0; port < 8; port++) {
    if (midilist[port]->read()) {
      uint8_t type =       midilist[port]->getType();
      uint8_t data1 =      midilist[port]->getData1();
      uint8_t data2 =      midilist[port]->getData2();
      uint8_t channel =    midilist[port]->getChannel();
      const uint8_t *sys = midilist[port]->getSysExArray();
      sendToComputer(type, data1, data2, channel, sys, 8 + port);
      activity = true;
    }
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
        case 5:
          MIDI6.send(mtype, data1, data2, channel);
          break;
        case 6:
          MIDI7.send(mtype, data1, data2, channel);
          break;
        case 7:
          MIDI8.send(mtype, data1, data2, channel);
          break;
        default: // cases 6-15
          midilist[cable - 8]->send(type, data1, data2, channel);
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
        case 5:
          MIDI6.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
        case 6:
          MIDI7.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
        case 7:
          MIDI8.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
        default: // cases 6-15
          midilist[cable - 8]->sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
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

void sendToComputer(byte type, byte data1, byte data2, byte channel, const uint8_t *sysexarray, byte cable)
{
  if (type != midi::SystemExclusive) {
    usbMIDI.send(type, data1, data2, channel, cable);
  } else {
    unsigned int SysExLength = data1 + data2 * 256;
    usbMIDI.sendSysEx(SysExLength, sysexarray, true, cable);
  }
}
