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
const float noteFreqs[128] = {8.176, 8.662, 9.177, 9.723, 10.301, 10.913, 11.562, 12.25, 12.978, 13.75, 14.568, 15.434, 16.352, 17.324, 18.354, 19.445, 20.602, 21.827, 23.125, 24.5, 25.957, 27.5, 29.135, 30.868, 32.703, 34.648, 36.708, 38.891, 41.203, 43.654, 46.249, 48.999, 51.913, 55, 58.27, 61.735, 65.406, 69.296, 73.416, 77.782, 82.407, 87.307, 92.499, 97.999, 103.826, 110, 116.541, 123.471, 130.813, 138.591, 146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220, 233.082, 246.942, 261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440, 466.164, 493.883, 523.251, 554.365, 587.33, 622.254, 659.255, 698.456, 739.989, 783.991, 830.609, 880, 932.328, 987.767, 1046.502, 1108.731, 1174.659, 1244.508, 1318.51, 1396.913, 1479.978, 1567.982, 1661.219, 1760, 1864.655, 1975.533, 2093.005, 2217.461, 2349.318, 2489.016, 2637.02, 2793.826, 2959.955, 3135.963, 3322.438, 3520, 3729.31, 3951.066, 4186.009, 4434.922, 4698.636, 4978.032, 5274.041, 5587.652, 5919.911, 6271.927, 6644.875, 7040, 7458.62, 7902.133, 8372.018, 8869.844, 9397.273, 9956.063, 10548.08, 11175.3, 11839.82, 12543.85};
const float DIV127 = (1.0 / 127.0);

#include <Audio.h>

// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine1;          //xy=180,469
//AudioSynthWaveformSineHires   sine1;
AudioOutputMQS        mqs1;           //xy=380,468
AudioConnection          patchCord1(sine1, mqs1);
// GUItool: end automatically generated code

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


  AudioMemory(12);
  sine1.amplitude(0.0);
  sine1.frequency(1000);
  
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
/* MIDI1.send(type, data1, data2, channel);
      MIDI2.send(type, data1, data2, channel);
      MIDI3.send(type, data1, data2, channel);      
      MIDI4.send(type, data1, data2, channel);  
      MIDI5.send(type, data1, data2, channel);  
   */ 
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI1.getSysExArray(), true, 0);
 /*     MIDI1.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
      MIDI2.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
      MIDI3.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
      MIDI4.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
      MIDI5.sendSysEx(SysExLength, MIDI1.getSysExArray(), true);
   */
    
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
/*      MIDI1.send(type, data1, data2, channel);
      MIDI2.send(type, data1, data2, channel);
      MIDI3.send(type, data1, data2, channel);  
      MIDI4.send(type, data1, data2, channel);  
      MIDI5.send(type, data1, data2, channel);  
  */    
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI2.getSysExArray(), true, 1);
  /*    MIDI1.sendSysEx(SysExLength, MIDI2.getSysExArray(), true);
      MIDI2.sendSysEx(SysExLength, MIDI2.getSysExArray(), true);
      MIDI3.sendSysEx(SysExLength, MIDI2.getSysExArray(), true);
      MIDI4.sendSysEx(SysExLength, MIDI2.getSysExArray(), true);
      MIDI5.sendSysEx(SysExLength, MIDI2.getSysExArray(), true);      
  */  
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
  /*    MIDI1.send(type, data1, data2, channel);
      MIDI2.send(type, data1, data2, channel);
      MIDI3.send(type, data1, data2, channel);
      MIDI4.send(type, data1, data2, channel);  
      MIDI5.send(type, data1, data2, channel);          
   */   
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI3.getSysExArray(), true, 2);
 /*     MIDI1.sendSysEx(SysExLength, MIDI3.getSysExArray(), true);
      MIDI2.sendSysEx(SysExLength, MIDI3.getSysExArray(), true);
      MIDI3.sendSysEx(SysExLength, MIDI3.getSysExArray(), true);
      MIDI4.sendSysEx(SysExLength, MIDI3.getSysExArray(), true);
      MIDI5.sendSysEx(SysExLength, MIDI3.getSysExArray(), true);      
 */  
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
 /*     MIDI1.send(type, data1, data2, channel);
      MIDI2.send(type, data1, data2, channel);
      MIDI3.send(type, data1, data2, channel);
      MIDI4.send(type, data1, data2, channel);  
      MIDI5.send(type, data1, data2, channel);          
  */    
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI4.getSysExArray(), true, 3);
  /*    MIDI1.sendSysEx(SysExLength, MIDI4.getSysExArray(), true);
      MIDI2.sendSysEx(SysExLength, MIDI4.getSysExArray(), true);
      MIDI3.sendSysEx(SysExLength, MIDI4.getSysExArray(), true);
      MIDI4.sendSysEx(SysExLength, MIDI4.getSysExArray(), true);
      MIDI5.sendSysEx(SysExLength, MIDI4.getSysExArray(), true);      
   */ 
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
/*      MIDI1.send(type, data1, data2, channel);
      MIDI2.send(type, data1, data2, channel);
      MIDI3.send(type, data1, data2, channel);
      MIDI4.send(type, data1, data2, channel);  
      MIDI5.send(type, data1, data2, channel);          
  */    
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI5.getSysExArray(), true, 4);
 /*     MIDI1.sendSysEx(SysExLength, MIDI5.getSysExArray(), true);
      MIDI2.sendSysEx(SysExLength, MIDI5.getSysExArray(), true);
      MIDI3.sendSysEx(SysExLength, MIDI5.getSysExArray(), true);
      MIDI4.sendSysEx(SysExLength, MIDI5.getSysExArray(), true);
      MIDI5.sendSysEx(SysExLength, MIDI5.getSysExArray(), true);      
 */  
    
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
        case 5:
          switch (type) {
           case midi::NoteOn:
            sine1.frequency(noteFreqs[data1]);
            sine1.amplitude(data2 * DIV127);
            
           break;
           
           case midi::NoteOff:
            sine1.amplitude(0.0);
           break;
          }
            
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
