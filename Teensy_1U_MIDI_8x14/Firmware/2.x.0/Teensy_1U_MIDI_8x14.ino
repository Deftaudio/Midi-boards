#include <MIDI.h>
#include <USBHost_t36.h>  // access to USB MIDI devices (plugged into 2nd USB port)
#include <SPI.h>
#include <EEPROM.h>
#include <Audio.h>
#include <SD.h>
#include <SerialFlash.h>
#include <U8g2lib.h>
#include <Encoder.h>
#include <Bounce.h>
#include <FlexIO_t4.h>
#include <FlexSerial.h>


//BUILD CONFIGURATION 

//Version
const char version[15] { "Version 2.1" };

//Display init
#define OLED_ROTATION

//Voice debug display
//#define VOICE_DEBUG

//Hardware rework for 5 PWM - switch TRS-A3 MIDI vs MODE, TRS-A4 MIDI vs MODE
//#define HW_REWORK

//for SSD1306 0.96" OLED
//#ifdef OLED_ROTATION
//  U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE);
//#else
//  U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//#endif

//for Luma-1 1.3" OLED
#ifdef OLED_ROTATION
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, /* reset=*/U8X8_PIN_NONE);
#else
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
#endif


int ScrollPosition = 0;
int MaxScroll = 5;
String DisplayBuffer[10] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };

const int Enc1Button = 30;
const int Encoder1PinA = 31;
const int Encoder1PinB = 32;
const int Enc2Button = 36;
const int Encoder2PinA = 37;
const int Encoder2PinB = 38;

long newEncoder1Reading;
long newEncoder2Reading;
long oldEncoder1Reading = -999;
long oldEncoder2Reading = -999;
long maxEncoder1 = 999;
long maxEncoder2 = 100;

Bounce Button1 = Bounce(Enc1Button, 5);
Bounce Button2 = Bounce(Enc2Button, 5);


Encoder Enc1(Encoder1PinA, Encoder1PinB);
Encoder Enc2(Encoder2PinA, Encoder2PinB);


// GUItool: begin automatically generated code
AudioInputUSB usb1;            //xy=406,314
AudioSynthWaveformSine sine1;  //xy=407,355
AudioSynthSimpleDrum drum1;    //xy=483,497
AudioMixer4 mixerLeft;         //xy=623,300
AudioMixer4 mixerRight;        //xy=624,404
AudioOutputI2S2 i2s2_1;        //xy=835,355
AudioOutputMQS mqs1;           //xy=839,486
AudioConnection patchCord1(usb1, 0, mixerLeft, 0);
AudioConnection patchCord2(usb1, 1, mixerRight, 0);
AudioConnection patchCord3(sine1, 0, mixerLeft, 1);
AudioConnection patchCord4(sine1, 0, mixerRight, 1);
AudioConnection patchCord5(drum1, 0, mqs1, 0);
AudioConnection patchCord6(drum1, 0, mqs1, 1);
AudioConnection patchCord7(mixerLeft, 0, i2s2_1, 0);
AudioConnection patchCord8(mixerRight, 0, i2s2_1, 1);
// GUItool: end automatically generated code

#define MAX_POLYPHONY 6  // based on 6 TRS channels in Teensy 1U interface

//these are temp
float hz_value;
int pitchBendLumaMu = 0;
int currentPolyphony = 0;
int nextVoiceIndex = 0;

struct Voice {
  bool active;
  byte note;
  int lumamuClockPort = 0;
  int lumamuGatePort = 0;
  int lumamuPausePort = 0;
  int lumamuResetPort = 0;
  unsigned long startTime = 0;  // For voice stealing
};

// Voice pool
Voice voices[MAX_POLYPHONY];
const int retriggerDelay = 1;


// Create the Serial MIDI ports
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI1);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI2);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI3);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial4, MIDI4);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial5, MIDI5);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial6, MIDI6);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial7, MIDI7);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial8, MIDI8);


#ifdef HW_REWORK
//TRS config for 6 PWM
int TRS_MODE_PIN[6] = { 5, 6, 41, 40, 26, 27 };
int TRS_DATA_PIN[6] = { 23, 22, 9, 11, 39, 33 };
#else
//TRS config for original layout
int TRS_MODE_PIN[6] = { 5, 6, 9, 11, 26, 27 };
int TRS_DATA_PIN[6] = { 23, 22, 41, 40, 39, 33 };
#endif


int clockPulseCounter[6] = { 0, 0, 0, 0, 0, 0 };
int clock_resolution_value[8] = { 1, 6, 12, 24, 48, 96, 192, 384 };
int baseOctaveMultiplierValue[5] = { 4, 16, 32, 64, 128 };

struct Configuration {
  //Configure TRS MODE outputs (MIDI=0, TRIGGER=1, SYNC=2, MU-CLOCK=3, MU-GATE=4, MU-PAUSE=5, MU-RESET=6)
  uint8_t TRS_MODE[6] = { 0, 1, 2, 0, 1, 2 };
  // should be translated per clock_resolution_value
  int clock_resolution[6] = { 1, 2, 3, 4, 5, 6 };
  int main_volume = 100;
  int pitchBendRangeInSemitones = 2;
  int baseOctaveMultiplier = 3;
  byte triggerChannel = 10;
  byte lumaMuChannel = 1;
  //0=stopPWM 1=Pause only 2=Reset only 3=stopALL
  int lumaMuTuning;
  int lumaMuFineTuning;
  int lumaMuNoteOffMode = 0;
} Config;


char trs_mode_name[6][20] = {
  // array with item names
  { " Port  9 Mode:   " },
  { " Port 10 Mode:   " },
  { " Port 11 Mode:   " },
  { " Port 12 Mode:   " },
  { " Port 13 Mode:   " },
  { " Port 14 Mode:   " },
};

char clock_resolution_name[6][20] = {
  // array with item names
  { " Sync Res  9:   " },
  { " Sync Res 10:   " },
  { " Sync Res 11:   " },
  { " Sync Res 12:   " },
  { " Sync Res 13:   " },
  { " Sync Res 14:   " },
};

char port_mode_value[7][20] = {
  { "1:MIDI" },
  { "2:TRIGGER" },
  { "3:SYNC" },
  { "4:MU-CLOCK" },
  { "5:MU-GATE" },
  { "6:MU-PAUSE" },
  { "7:MU-RESET" },
};

char clock_resolution_value_name[8][20] = {
  { "24ppqn" },
  { "sixteen" },
  { "eight" },
  { "quarter" },
  { "half" },
  { "bar" },
  { "two bars" },
  { "four bars" }
};

char lumaMuNoteOffModeValue[4][20] = {
  { "Stop Clock" },
  { "Pause" },
  { "Reset" },
  { "Stop All" },
};


int MenuTotal = 19;
int current_parameter = 0;
int previous_parameter = 0;
int current_value = 0;
int previous_value = 0;
bool in_menu = false;


//FlexIO create ports
FlexSerial Flex_Serial9(-1, TRS_DATA_PIN[0]);  // MIDI OUT only
MIDI_CREATE_INSTANCE(FlexSerial, Flex_Serial9, MIDI9);
FlexSerial Flex_Serial10(-1, TRS_DATA_PIN[1]);  // MIDI OUT only
MIDI_CREATE_INSTANCE(FlexSerial, Flex_Serial10, MIDI10);
FlexSerial Flex_Serial11(-1, TRS_DATA_PIN[2]);  // MIDI OUT only
MIDI_CREATE_INSTANCE(FlexSerial, Flex_Serial11, MIDI11);
FlexSerial Flex_Serial12(-1, TRS_DATA_PIN[3]);  // MIDI OUT only
MIDI_CREATE_INSTANCE(FlexSerial, Flex_Serial12, MIDI12);
FlexSerial Flex_Serial13(-1, TRS_DATA_PIN[4]);  // MIDI OUT only
MIDI_CREATE_INSTANCE(FlexSerial, Flex_Serial13, MIDI13);
FlexSerial Flex_Serial14(-1, TRS_DATA_PIN[5]);  // MIDI OUT only
MIDI_CREATE_INSTANCE(FlexSerial, Flex_Serial14, MIDI14);


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
MIDIDevice *midilist[8] = {
  &midi01,
  &midi02,
  &midi03,
  &midi04,
  &midi05,
  &midi06,
  &midi07,
  &midi08,
};


// A variable to know how long the LED has been turned on
elapsedMillis ledOnMillis;


void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);  // LED pin

  //TRS MODE + DATA PINs
  for (int i = 0; i < 6; i++) {
    pinMode(TRS_MODE_PIN[i], OUTPUT);
    pinMode(TRS_DATA_PIN[i], OUTPUT);
  }

  /*
//Encoder pullups
  pinMode(Enc1Button, INPUT_PULLUP);
  pinMode(Enc2Button, INPUT_PULLUP);
  pinMode(Encoder1PinA, INPUT_PULLUP);
  pinMode(Encoder1PinB, INPUT_PULLUP);
  pinMode(Encoder2PinA, INPUT_PULLUP);
  pinMode(Encoder2PinB, INPUT_PULLUP);
*/

  // Encoder set to zero
  Enc1.write(0);
  Enc2.write(0);


  AudioMemory(10);
  sine1.frequency(440);
  sine1.amplitude(0.5);
  drum1.frequency(60);
  drum1.length(500);
  drum1.secondMix(0.0);
  drum1.pitchMod(1.55);


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

  digitalWriteFast(13, HIGH);  // LED on
  delay(500);
  digitalWriteFast(13, LOW);

  //USB MIDI + HOST
  myusb.begin();
  usbMIDI.setHandleNoteOn(handleNoteOn);
  usbMIDI.setHandleNoteOff(handleNoteOff);
  usbMIDI.setHandleClock(handleClock);
  usbMIDI.setHandleStop(handleStop);
  usbMIDI.setHandlePitchChange(handlePitchBend);

  //usbMIDI.setHandleSystemExclusive(handleSysEx);

  //FlexIO setup
  Flex_Serial9.setClock(9795918);
  MIDI9.begin(MIDI_CHANNEL_OMNI);
  Flex_Serial10.setClock(9795918);
  MIDI10.begin(MIDI_CHANNEL_OMNI);
  Flex_Serial11.setClock(9795918);
  MIDI11.begin(MIDI_CHANNEL_OMNI);
  Flex_Serial12.setClock(9795918);
  MIDI12.begin(MIDI_CHANNEL_OMNI);
  Flex_Serial13.setClock(9795918);
  MIDI13.begin(MIDI_CHANNEL_OMNI);
  Flex_Serial14.setClock(9795918);
  MIDI14.begin(MIDI_CHANNEL_OMNI);



  //Display initialization
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_profont22_tr);
  u8g2.drawStr(10, 20, "Deftaudio");
  u8g2.setFont(u8g2_font_6x13_tr);
  u8g2.drawStr(6, 38, "MIDI 8x14 interface");
  u8g2.drawLine(4, 58, 124, 58);

  u8g2.setFont(u8g2_font_6x12_tr);

  u8g2.drawStr(6, 50, version);
  
  
  u8g2.sendBuffer();


  // Load Configuration
  EpromLoad();

  // Update the state of outputs
  UpdateTRSMode();

  //Reset Encoders
  Enc1.write(0);
  Enc2.write(Config.main_volume);

  //Restore Volume settings
  mixerRight.gain(0, float(Config.main_volume) / 100);
  mixerLeft.gain(0, float(Config.main_volume) / 100);

  delay(2000);

  //Initiate Voices and Determine Polyphony
  for (int i = 0; i < MAX_POLYPHONY; i++) {
    voices[i].active = false;
    if (Config.TRS_MODE[i] == 3) {
      analogWriteFrequency(TRS_DATA_PIN[i], 0);
      analogWrite(TRS_DATA_PIN[i], 0);
      voices[currentPolyphony].lumamuClockPort = i;
      
      //workaround to reduce glitch on some PWM ports
      pinMode(TRS_DATA_PIN[i], OUTPUT);
      digitalWrite(TRS_DATA_PIN[i], LOW);

      currentPolyphony++;
    }
    //Luma-mu GATE
    if (Config.TRS_MODE[i] == 4) {
      for (int j = 0; j < MAX_POLYPHONY; j++) {
        if (voices[j].lumamuGatePort == 0) {
          voices[j].lumamuGatePort = i;
          digitalWriteFast(TRS_DATA_PIN[i], LOW);
          break;
        }
      }
    }
    //Luma-mu PAUSE
    if (Config.TRS_MODE[i] == 5) {
      for (int j = 0; j < MAX_POLYPHONY; j++) {
        if (voices[j].lumamuPausePort == 0) {
          voices[j].lumamuPausePort = i;
          digitalWriteFast(TRS_DATA_PIN[i], LOW);
          break;
        }
      }
    }
    //Luma-mu RESET
    if (Config.TRS_MODE[i] == 6) {
      for (int j = 0; j < MAX_POLYPHONY; j++) {
        if (voices[j].lumamuResetPort == 0) {
          voices[j].lumamuResetPort = i;
          digitalWriteFast(TRS_DATA_PIN[i], LOW);
          break;
        }
      }
    }
  }




  if (currentPolyphony > 0) {
    StatusDisplay("Luma-mu Voice mode ");
    StatusDisplay("Total Voices = " + String(currentPolyphony));
  }

  delay(1000);

  for (int i = 0; i < currentPolyphony; i++) {

    if (voices[i].lumamuGatePort == 0) {
      StatusDisplay("V:" + String(i + 1) + " CLOCK=" + String(voices[i].lumamuClockPort + 9) + " GATE=NO");
      delay(500);
    }
    if (voices[i].lumamuGatePort > 0) {
      StatusDisplay("V:" + String(i + 1) + " CLOCK=" + String(voices[i].lumamuClockPort + 9) + " GATE=" + String(voices[i].lumamuGatePort + 9));
      delay(500);
    }
    if (voices[i].lumamuPausePort > 0) {
      StatusDisplay("PAUSE=" + String(voices[i].lumamuPausePort + 9));
      delay(500);
    }
    if (voices[i].lumamuResetPort > 0) {
      StatusDisplay("RESET=" + String(voices[i].lumamuResetPort + 9));
      delay(500);
    }
  }

  //StatusDisplay("triggerChannel=" + String(Config.triggerChannel));
  //StatusDisplay("lumaMuChannel=" + String(Config.lumaMuChannel));


  //beep off, End Setup
  sine1.frequency(0);
  sine1.amplitude(0);
}


void loop() {
  bool activity = false;


  // Control and menu processing

  Button1.update();
  Button2.update();

  if (Button1.risingEdge()) {

    // NOTHING TODO HERE

    //StatusDisplay("Enc1 button released");
    //Config.TRS_MODE[0] = 0;
    //UpdateTRSMode();
    //Flex_Serial9.setIOPinToFlexMode
    //displayTRSMode(0, 1);
  }

  if (Button2.fallingEdge()) {
    if (!in_menu) {
      StatusDisplay("Drum hit");
      drum1.noteOn();
    } else if ((in_menu) && (current_parameter == MenuTotal)) {
      EpromSave();
      StatusDisplay("Configuration Saved!");
      delay(1000);
      StatusDisplay("Restarting in 3");
      delay(1000);
      StatusDisplay("Restarting in 2");
      delay(1000);
      StatusDisplay("Restarting in 1");
      SCB_AIRCR = 0x05FA0004;
    }
  }

  newEncoder1Reading = Enc1.read();

  if ((newEncoder1Reading > oldEncoder1Reading + 3) && (in_menu)) {
    oldEncoder1Reading = newEncoder1Reading;
    current_parameter++;
  }
  if ((newEncoder1Reading < oldEncoder1Reading - 3) && (in_menu)) {
    oldEncoder1Reading = newEncoder1Reading;
    current_parameter--;
    if (current_parameter < 0) current_parameter = 0;
  }

  newEncoder2Reading = Enc2.read();

  if ((newEncoder2Reading > oldEncoder2Reading + 3) && (in_menu)) {
    oldEncoder2Reading = newEncoder2Reading;
    current_value++;
  }
  if ((newEncoder2Reading < oldEncoder2Reading - 3) && (in_menu)) {
    oldEncoder2Reading = newEncoder2Reading;
    current_value--;
    if (current_value < 0) current_value = 0;
  }


  // use for volume!
  if ((newEncoder2Reading != oldEncoder2Reading) && (!in_menu)) {
    if (newEncoder2Reading > maxEncoder2) {
      newEncoder2Reading = maxEncoder2;
      Enc2.write(maxEncoder2);
    }
    if (newEncoder2Reading < 0) {
      newEncoder2Reading = 0;
      Enc2.write(0);
    }
    oldEncoder2Reading = newEncoder2Reading;

    Config.main_volume = newEncoder2Reading;
    mixerRight.gain(0, float(Config.main_volume) / 100);
    mixerLeft.gain(0, float(Config.main_volume) / 100);

    StatusDisplay("Main Volume " + String(newEncoder2Reading) + "%");
  }


  if (Button1.fallingEdge()) {
    if (!in_menu) {
      //StatusDisplay("Enc1 button pressed");
      //Config.TRS_MODE[0] = 1;
      //UpdateTRSMode();
      current_parameter = 0;
      previous_parameter = 0;
      in_menu = true;
      oldEncoder1Reading = newEncoder1Reading;
      oldEncoder2Reading = newEncoder2Reading;
      displayTRSMode(current_parameter, Config.TRS_MODE[current_parameter]);
    } else {
      in_menu = false;
      StatusDisplay("Exited without saving");
    }
  }


//current_parameter=Enc1, current_value=Enc2
  if (current_parameter != previous_parameter) {

    if (current_parameter < 6) {
      displayTRSMode(current_parameter, Config.TRS_MODE[current_parameter]);
      //previous_parameter = current_parameter;
    } else if ((current_parameter >= 6) && (current_parameter < 12)) {
      displayClockResolution(current_parameter - 6, Config.clock_resolution[current_parameter - 6]);
      //previous_parameter = current_parameter;
    } else if (current_parameter == 12) {
      displayTriggerChannel(Config.triggerChannel);
    } else if (current_parameter == 13) {
      displayLumaMuChannel(Config.lumaMuChannel);
    } else if (current_parameter == 14) {
      displayLumaMuTuning(Config.lumaMuTuning);
    } else if (current_parameter == 15) {
      displayLumaMuFineTuning(Config.lumaMuFineTuning);
    } else if (current_parameter == 16) {
      displayBaseOctaveMultiplier(Config.baseOctaveMultiplier);
    } else if (current_parameter == 17) {
      displayLumaMuPitchBendRange(Config.pitchBendRangeInSemitones);
    } else if (current_parameter == 18) {
      displaylumaMuNoteOffMode(Config.lumaMuNoteOffMode);
    } else if (current_parameter >= MenuTotal) {
      current_parameter = MenuTotal;
      displayExitMenu();
    }


    previous_parameter = current_parameter;
  }

//current_parameter=Enc1, current_value=Enc2
  if (current_value != previous_value) {

    #ifdef HW_REWORK
    //code for 6 PWM
    /*
    if ((current_value < 7) && (current_parameter < 6)) {
      Config.TRS_MODE[current_parameter] = current_value;
      displayTRSMode(current_parameter, Config.TRS_MODE[current_parameter]);
    } else if ((current_value >= 7) && (current_parameter < 6)) {
      current_value = 6;
      Config.TRS_MODE[current_parameter] = current_value;
      displayTRSMode(current_parameter, Config.TRS_MODE[current_parameter]);
    }
    */

    //code for 5 PWM 9,10,11,12,14
    if ((current_value < 7) && (current_parameter < 4)) {
      Config.TRS_MODE[current_parameter] = current_value;
      displayTRSMode(current_parameter, Config.TRS_MODE[current_parameter]);
    } else if ((current_value >= 7) && (current_parameter < 4)) {
      current_value = 6;
      Config.TRS_MODE[current_parameter] = current_value;
      displayTRSMode(current_parameter, Config.TRS_MODE[current_parameter]);
    }
    //here we skip MU-CLOCK for prts 11-13
    if ((current_value < 7) && (current_parameter == 4)) {
      if (current_value == 3 && current_value > previous_value) current_value++;
        else if (current_value == 3 && current_value < previous_value) current_value--;
      Config.TRS_MODE[current_parameter] = current_value;
      displayTRSMode(current_parameter, Config.TRS_MODE[current_parameter]);
    } else if ((current_value >= 7) && (current_parameter == 4)) {
      current_value = 6;
      Config.TRS_MODE[current_parameter] = current_value;
      displayTRSMode(current_parameter, Config.TRS_MODE[current_parameter]);
    }
    if ((current_value < 7) && (current_parameter == 5)) {
      Config.TRS_MODE[current_parameter] = current_value;
      displayTRSMode(current_parameter, Config.TRS_MODE[current_parameter]);
    } else if ((current_value >= 7) && (current_parameter == 5)) {
      current_value = 6;
      Config.TRS_MODE[current_parameter] = current_value;
      displayTRSMode(current_parameter, Config.TRS_MODE[current_parameter]);
    }


    #else
    if ((current_value < 7) && (current_parameter < 2)) {
      Config.TRS_MODE[current_parameter] = current_value;
      displayTRSMode(current_parameter, Config.TRS_MODE[current_parameter]);
    } else if ((current_value >= 7) && (current_parameter < 2)) {
      current_value = 6;
      Config.TRS_MODE[current_parameter] = current_value;
      displayTRSMode(current_parameter, Config.TRS_MODE[current_parameter]);
    }
    //here we skip MU-CLOCK for prts 11-13
    if ((current_value < 7) && (current_parameter < 5) && (current_parameter >= 2)) {
      if (current_value == 3 && current_value > previous_value) current_value++;
        else if (current_value == 3 && current_value < previous_value) current_value--;
      Config.TRS_MODE[current_parameter] = current_value;
      displayTRSMode(current_parameter, Config.TRS_MODE[current_parameter]);
    } else if ((current_value >= 7) && (current_parameter < 5) && (current_parameter >= 2)) {
      current_value = 6;
      Config.TRS_MODE[current_parameter] = current_value;
      displayTRSMode(current_parameter, Config.TRS_MODE[current_parameter]);
    }
    if ((current_value < 7) && (current_parameter == 5)) {
      Config.TRS_MODE[current_parameter] = current_value;
      displayTRSMode(current_parameter, Config.TRS_MODE[current_parameter]);
    } else if ((current_value >= 7) && (current_parameter == 5)) {
      current_value = 6;
      Config.TRS_MODE[current_parameter] = current_value;
      displayTRSMode(current_parameter, Config.TRS_MODE[current_parameter]);
    }
    #endif

    //Clock Resolution Menu
    if ((current_value < 8) && (current_parameter >= 6) && (current_parameter < 12)) {
      Config.clock_resolution[current_parameter - 6] = current_value;
      displayClockResolution(current_parameter - 6, Config.clock_resolution[current_parameter - 6]);
    } else if ((current_value >= 8) && (current_parameter >= 6) && (current_parameter < 12)) {
      current_value = 7;
      Config.clock_resolution[current_parameter - 6] = current_value;
      displayClockResolution(current_parameter - 6, Config.clock_resolution[current_parameter - 6]);
    }
    //Trigger MIDI Channel Menu
    if ((current_value < 1) && (current_parameter == 12)) {
      current_value = 1;
      Config.triggerChannel = current_value;
      displayTriggerChannel(Config.triggerChannel);
    } else if ((current_value < 17) && (current_parameter == 12)) {
      Config.triggerChannel = current_value;
      displayTriggerChannel(Config.triggerChannel);
    } else if ((current_value >= 17) && (current_parameter == 12)){
      current_value = 16;
      Config.triggerChannel = current_value;
      displayTriggerChannel(Config.triggerChannel);
    }

    //Luma-mu MIDI Channel Menu
    if ((current_value < 1) && (current_parameter == 13)) {
      current_value = 1;
      Config.lumaMuChannel = current_value;
      displayLumaMuChannel(Config.lumaMuChannel);
    } else if ((current_value < 17) && (current_parameter == 13)) {
      Config.lumaMuChannel = current_value;
      displayLumaMuChannel(Config.lumaMuChannel);
    } else if ((current_value >= 17) && (current_parameter == 13)){
      current_value = 16;
      Config.lumaMuChannel = current_value;
      displayLumaMuChannel(Config.lumaMuChannel);
    }

    //Luma-mu Tuning Menu
    if ((current_value <= 0) && (current_parameter == 14)) {
      current_value = 0;
      Config.lumaMuTuning = current_value - 100;
      displayLumaMuTuning(100*Config.lumaMuTuning);
    } else if ((current_value < 201) && (current_parameter == 14)) {
      Config.lumaMuTuning = current_value - 100;
      displayLumaMuTuning(100*Config.lumaMuTuning);
    } else if ((current_value >= 200) && (current_parameter == 14)){
      current_value = 200;
      Config.lumaMuTuning = current_value - 100;
      displayLumaMuTuning(100*Config.lumaMuTuning);
    }

    //Luma-mu Fine Tuning Menu
    if ((current_value <= 0) && (current_parameter == 15)) {
      current_value = 0;
      Config.lumaMuFineTuning = current_value - 100;
      displayLumaMuFineTuning(Config.lumaMuFineTuning);
    } else if ((current_value < 201) && (current_parameter == 15)) {
      Config.lumaMuFineTuning = current_value - 100;
      displayLumaMuFineTuning(Config.lumaMuFineTuning);
    } else if ((current_value >= 200) && (current_parameter == 15)){
      current_value = 200;
      Config.lumaMuFineTuning = current_value - 100;
      displayLumaMuFineTuning(Config.lumaMuFineTuning);
    }

    //Luma-mu Base Octave Multiplier Menu
    if ((current_value < 5) && (current_parameter == 16)) {
      Config.baseOctaveMultiplier = current_value;
      displayBaseOctaveMultiplier(Config.baseOctaveMultiplier);
    } else if ((current_value >= 5) && (current_parameter == 16)){
      current_value = 4;
      Config.baseOctaveMultiplier = current_value;
      displayBaseOctaveMultiplier(Config.baseOctaveMultiplier);
    }

    //Luma-mu PitchBend Range Menu
    if ((current_value < 1) && (current_parameter == 17)) {
      current_value = 1;
      Config.pitchBendRangeInSemitones = current_value;
      displayLumaMuPitchBendRange(Config.pitchBendRangeInSemitones);
    } else if ((current_value < 128) && (current_parameter == 17)) {
      Config.pitchBendRangeInSemitones = current_value;
      displayLumaMuPitchBendRange(Config.pitchBendRangeInSemitones);
    } else if ((current_value >= 6) && (current_parameter == 17)){
      current_value = 5;
      Config.pitchBendRangeInSemitones = current_value;
      displayLumaMuPitchBendRange(Config.pitchBendRangeInSemitones);
    }

    //Luma-mu Note Off Mode Menu
    if ((current_value < 4) && (current_parameter == 18)) {
      Config.lumaMuNoteOffMode = current_value;
      displaylumaMuNoteOffMode(Config.lumaMuNoteOffMode);
    } else if ((current_value >= 4) && (current_parameter == 18)){
      current_value = 3;
      Config.lumaMuNoteOffMode = current_value;
      displaylumaMuNoteOffMode(Config.lumaMuNoteOffMode);
    }


    previous_value = current_value;
    UpdateTRSMode();
  }

  //MIDI Processing


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
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI1.getSysExArray(), true, 0);
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
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI2.getSysExArray(), true, 1);
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
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI3.getSysExArray(), true, 2);
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
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI4.getSysExArray(), true, 3);
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
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI5.getSysExArray(), true, 4);
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
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI6.getSysExArray(), true, 5);
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
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI7.getSysExArray(), true, 6);
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
    } else {
      // SysEx messages are special.  The message length is given in data1 & data2
      unsigned int SysExLength = data1 + data2 * 256;
      usbMIDI.sendSysEx(SysExLength, MIDI8.getSysExArray(), true, 7);
    }
    activity = true;
  }

  // Next read messages arriving from the (up to) 10 USB devices plugged into the USB Host port
  for (int port = 0; port < 2; port++) {
    if (midilist[port]->read()) {
      uint8_t type = midilist[port]->getType();
      uint8_t data1 = midilist[port]->getData1();
      uint8_t data2 = midilist[port]->getData2();
      uint8_t channel = midilist[port]->getChannel();
      const uint8_t *sys = midilist[port]->getSysExArray();
      sendToComputer(type, data1, data2, channel, sys, 14 + port);
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
        case 8:
          if (Config.TRS_MODE[0] == 0)
            MIDI9.send(mtype, data1, data2, channel);
          break;
        case 9:
          if (Config.TRS_MODE[1] == 0)
            MIDI10.send(mtype, data1, data2, channel);
          break;
        case 10:
          if (Config.TRS_MODE[2] == 0)
            MIDI11.send(mtype, data1, data2, channel);
          break;
        case 11:
          if (Config.TRS_MODE[3] == 0)
            MIDI12.send(mtype, data1, data2, channel);
          break;
        case 12:
          if (Config.TRS_MODE[4] == 0)
            MIDI13.send(mtype, data1, data2, channel);
          break;
        case 13:
          if (Config.TRS_MODE[5] == 0)
            MIDI14.send(mtype, data1, data2, channel);
          break;
        default:  // USB devices on Port 15+16
          midilist[cable - 14]->send(type, data1, data2, channel);
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
        case 8:
          if (Config.TRS_MODE[0] == 0)
            MIDI9.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
        case 9:
          if (Config.TRS_MODE[1] == 0)
            MIDI10.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
        case 10:
          if (Config.TRS_MODE[2] == 0)
            MIDI11.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
        case 11:
          if (Config.TRS_MODE[3] == 0)
            MIDI12.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
        case 12:
          if (Config.TRS_MODE[4] == 0)
            MIDI13.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
        case 13:
          if (Config.TRS_MODE[5] == 0)
            MIDI14.sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
        case 14:
          midilist[cable - 14]->sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          break;
        case 15:
          //midilist[cable - 14]->sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
          byte *sysbytes = usbMIDI.getSysExArray();
          if (sysbytes[0] == 0xf0 && sysbytes[1] == 0x7d) {  // check start f0 and ID 7D.
            StatusDisplay("SysEx received");
            if (sysbytes[5] == 0x7f) {
              StatusDisplay("It's valid");
              StatusDisplay("Port " + String(sysbytes[2] + 9) + ": " + String(sysbytes[4] | sysbytes[3] << 8));
              //StatusDisplay(String(sysbytes[4] | sysbytes[3] << 8));

              if (Config.TRS_MODE[sysbytes[2]] == 3) {

                analogWriteFrequency(TRS_DATA_PIN[sysbytes[2]], sysbytes[4] | sysbytes[3] << 8);
                analogWrite(TRS_DATA_PIN[sysbytes[2]], 128);

                if (sysbytes[4] == 0 && sysbytes[3] == 0) {
                  analogWrite(TRS_DATA_PIN[sysbytes[2]], 0);
                  //workaround to avoid pulse glitch
                  pinMode(TRS_DATA_PIN[sysbytes[2]], OUTPUT);
                  digitalWrite(TRS_DATA_PIN[sysbytes[2]], LOW);
                  StatusDisplay("Set output to LOW");
                }

                StatusDisplay("DATA PIN " + String(TRS_DATA_PIN[sysbytes[2]]));
              }

            } else StatusDisplay("Not valid SysEx");
          }
          break;

          //default:  // other cases
          //midilist[cable - 14]->sendSysEx(SysExLength, usbMIDI.getSysExArray(), true);
      }
    }
    activity = true;
  }

  // blink the LED when any activity has happened
  if (activity) {
    digitalWriteFast(13, HIGH);  // LED on
    ledOnMillis = 0;
  }
  if (ledOnMillis > 15) {
    digitalWriteFast(13, LOW);  // LED off
  }
}

void sendToComputer(byte type, byte data1, byte data2, byte channel, const uint8_t *sysexarray, byte cable) {
  if (type != midi::SystemExclusive) {
    usbMIDI.send(type, data1, data2, channel, cable);
  } else {
    unsigned int SysExLength = data1 + data2 * 256;
    usbMIDI.sendSysEx(SysExLength, sysexarray, true, cable);
  }
}




void StatusDisplay(String text) {
  if (ScrollPosition > MaxScroll - 1) {
    for (int i = 0; i < MaxScroll; i++) {
      DisplayBuffer[i] = DisplayBuffer[i + 1];
    }
    DisplayBuffer[MaxScroll] = text;
    ScrollPosition = MaxScroll - 1;
  } else {
    DisplayBuffer[ScrollPosition] = text;
  }

  DisplayBuffer[ScrollPosition] = text;
  u8g2.clearBuffer();

  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_6x13_mf);
  u8g2.drawButtonUTF8(0, 10, U8G2_BTN_BW0 + U8G2_BTN_INV, 0, 0, 0, " Status:             ");

  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_6x13_tr);

  // u8g2.setCursor(0,10);

  for (int i = 0; i < MaxScroll; i++) {
    u8g2.setCursor(0, i * 10 + 22);
    u8g2.print(DisplayBuffer[i]);
  }

  u8g2.sendBuffer();
  ScrollPosition++;
}

void UpdateTRSMode(void) {
  for (int i = 0; i < 6; i++) {
    if (Config.TRS_MODE[i] == 0)
      digitalWriteFast(TRS_MODE_PIN[i], LOW);
    else {
      digitalWriteFast(TRS_MODE_PIN[i], HIGH);
      pinMode(TRS_DATA_PIN[i], OUTPUT);
      digitalWriteFast(TRS_DATA_PIN[i], LOW);
    }
  }
}

void handleNoteOn(byte inChannel, byte inNote, byte inVelocity) {
  //StatusDisplay("Channel:" + String(inChannel) + " ON");

  //Luma-mu code
  if ((usbMIDI.getCable() == 15) && (inChannel == Config.lumaMuChannel)) {
    
    #ifdef VOICE_DEBUG
    StatusDisplay("New Note:" + String(inNote));
    #endif

    //Get Voice number
    int voiceIndex = findFreeVoice();
  /*
    if (voiceIndex == -1) {      
      voiceIndex = stealOldestVoice();
*/

    //New code for note OFF with roundrobin
    if (voices[voiceIndex].active) {

      //if (voiceIndex >= 0) {
        digitalWriteFast(TRS_DATA_PIN[voices[voiceIndex].lumamuGatePort], LOW);
        //0=stopPWM 1=Pause only 2=Reset only 3=stopALL
        switch (Config.lumaMuNoteOffMode) {
          case 0:
          analogWriteFrequency(TRS_DATA_PIN[voices[voiceIndex].lumamuClockPort], 0);
          analogWrite(TRS_DATA_PIN[voices[voiceIndex].lumamuClockPort], 0);
          //workaround to avoid pulse glitch
          //pinMode(TRS_DATA_PIN[voices[voiceIndex].lumamuClockPort], OUTPUT);
          //digitalWrite(TRS_DATA_PIN[voices[voiceIndex].lumamuClockPort], LOW);
          break;

          case 1:
          digitalWriteFast(TRS_DATA_PIN[voices[voiceIndex].lumamuPausePort], HIGH);
          break;

          case 2:
          digitalWriteFast(TRS_DATA_PIN[voices[voiceIndex].lumamuResetPort], HIGH);  
          break;

          case 3:
          analogWriteFrequency(TRS_DATA_PIN[voices[voiceIndex].lumamuClockPort], 0);
          analogWrite(TRS_DATA_PIN[voices[voiceIndex].lumamuClockPort], 0);

          //workaround to avoid pulse glitch
          //pinMode(TRS_DATA_PIN[voices[voiceIndex].lumamuClockPort], OUTPUT);
          //digitalWrite(TRS_DATA_PIN[voices[voiceIndex].lumamuClockPort], LOW);

          digitalWriteFast(TRS_DATA_PIN[voices[voiceIndex].lumamuPausePort], HIGH);
          digitalWriteFast(TRS_DATA_PIN[voices[voiceIndex].lumamuResetPort], HIGH);     
          break; 
        }
        #ifdef VOICE_DEBUG
        StatusDisplay("V:" + String(voiceIndex) + " OFF");
        #endif
        delay(retriggerDelay);
      //}
    }
    
    //Generate new voice
    if (voiceIndex >= 0) {
      voices[voiceIndex].active = true;
      voices[voiceIndex].note = inNote;
      voices[voiceIndex].startTime = millis();

      hz_value = baseOctaveMultiplierValue[Config.baseOctaveMultiplier] * pow(2.0, ((float)(inNote - 69) / 12.0) + (float)(pitchBendLumaMu) / (8192.0 / Config.pitchBendRangeInSemitones * 12.0)) * 440 + 100*Config.lumaMuTuning + Config.lumaMuFineTuning;

      analogWriteFrequency(TRS_DATA_PIN[voices[voiceIndex].lumamuClockPort], hz_value);
      analogWrite(TRS_DATA_PIN[voices[voiceIndex].lumamuClockPort], 128);

      if (voices[voiceIndex].lumamuGatePort > 0) {
        digitalWriteFast(TRS_DATA_PIN[voices[voiceIndex].lumamuGatePort], HIGH);
      }

      if (voices[voiceIndex].lumamuPausePort > 0) {
        digitalWriteFast(TRS_DATA_PIN[voices[voiceIndex].lumamuPausePort], LOW);
      }

      if (voices[voiceIndex].lumamuResetPort > 0) {
        digitalWriteFast(TRS_DATA_PIN[voices[voiceIndex].lumamuResetPort], LOW);
      }
      #ifdef VOICE_DEBUG
      StatusDisplay("V:" + String(voiceIndex) + " ON");
      #endif
    }
  }

  //this are standard GATES not related to Luma-mu
  if ((usbMIDI.getCable() == 15) && (inChannel == Config.triggerChannel)) {
    //Gate processing
    switch (inNote) {
      //C2 Note: trigger drum1
      case 36:
        drum1.noteOn();
        break;
      //C3 Note: set to TRS-1
      case 48:
        if (Config.TRS_MODE[0] == 1) {
          digitalWriteFast(TRS_DATA_PIN[0], HIGH);
          //StatusDisplay("Note On C3");
        }
        break;
      //D3 Note: set to TRS-2
      case 50:
        if (Config.TRS_MODE[1] == 1) {
          digitalWriteFast(TRS_DATA_PIN[1], HIGH);
          //StatusDisplay("Note On D3");
        }
        break;
      //E3 Note: set to TRS-3
      case 52:
        if (Config.TRS_MODE[2] == 1) {
          digitalWriteFast(TRS_DATA_PIN[2], HIGH);
          //StatusDisplay("Note On E3");
        }
        break;
      //F3 Note: set to TRS-4
      case 53:
        if (Config.TRS_MODE[3] == 1) {
          digitalWriteFast(TRS_DATA_PIN[3], HIGH);
          //StatusDisplay("Note On F3");
        }
        break;
      //G3 Note: set to TRS-5
      case 55:
        if (Config.TRS_MODE[4] == 1) {
          digitalWriteFast(TRS_DATA_PIN[4], HIGH);
          //StatusDisplay("Note On G3");
        }
        break;
      //A3 Note: set to TRS-5
      case 57:
        if (Config.TRS_MODE[5] == 1) {
          digitalWriteFast(TRS_DATA_PIN[5], HIGH);
          //StatusDisplay("Note On A3");
        }
        break;
    }
  }
}

void handleNoteOff(byte inChannel, byte inNote, byte inVelocity) {
  //StatusDisplay("Channel:" + String(inChannel) + " OFF");

  //Luma-mu code
  if ((usbMIDI.getCable() == 15) && (inChannel == Config.lumaMuChannel)) {

    for (int i = 0; i < currentPolyphony; i++) {
      if (voices[i].active && voices[i].note == inNote) {
        voices[i].active = false;
        digitalWriteFast(TRS_DATA_PIN[voices[i].lumamuGatePort], LOW);

        //0=stopPWM 1=Pause only 2=Reset only 3=stopALL
        switch (Config.lumaMuNoteOffMode) {
          case 0:
          analogWriteFrequency(TRS_DATA_PIN[voices[i].lumamuClockPort], 0);
          analogWrite(TRS_DATA_PIN[voices[i].lumamuClockPort], 0);

          //workaround to avoid pulse glitch
          pinMode(TRS_DATA_PIN[voices[i].lumamuClockPort], OUTPUT);
          digitalWrite(TRS_DATA_PIN[voices[i].lumamuClockPort], LOW);
          break;

          case 1:
          digitalWriteFast(TRS_DATA_PIN[voices[i].lumamuPausePort], HIGH);
          break;

          case 2:
          digitalWriteFast(TRS_DATA_PIN[voices[i].lumamuResetPort], HIGH);  
          break;

          case 3:
          analogWriteFrequency(TRS_DATA_PIN[voices[i].lumamuClockPort], 0);
          analogWrite(TRS_DATA_PIN[voices[i].lumamuClockPort], 0);

          //workaround to avoid pulse glitch
          pinMode(TRS_DATA_PIN[voices[i].lumamuClockPort], OUTPUT);
          digitalWrite(TRS_DATA_PIN[voices[i].lumamuClockPort], LOW); 

          digitalWriteFast(TRS_DATA_PIN[voices[i].lumamuPausePort], HIGH);
          digitalWriteFast(TRS_DATA_PIN[voices[i].lumamuResetPort], HIGH);     
          break;
        }
        
        #ifdef VOICE_DEBUG
        StatusDisplay("V:" + String(i) + " OFF");
        #endif

        break;
      }
    }
  }

  //this are standard GATES not related to Luma-mu
  if ((usbMIDI.getCable() == 15) && (inChannel == Config.triggerChannel)) {
    switch (inNote) {
      //C3 Note: set to Follow MIDI mode
      case 48:
        if (Config.TRS_MODE[0] == 1) {
          digitalWriteFast(TRS_DATA_PIN[0], LOW);
          //StatusDisplay("Note Off C3");
        }
        break;
      //D3 Note: set to Follow MIDI mode
      case 50:
        if (Config.TRS_MODE[1] == 1) {
          digitalWriteFast(TRS_DATA_PIN[1], LOW);
          //StatusDisplay("Note On D3");
        }
        break;
      //E3 Note: set to TRS-3
      case 52:
        if (Config.TRS_MODE[2] == 1) {
          digitalWriteFast(TRS_DATA_PIN[2], LOW);
          //StatusDisplay("Note On E3");
        }
        break;
      //F3 Note: set to TRS-4
      case 53:
        if (Config.TRS_MODE[3] == 1) {
          digitalWriteFast(TRS_DATA_PIN[3], LOW);
          //StatusDisplay("Note On F3");
        }
        break;
      //G3 Note: set to TRS-5
      case 55:
        if (Config.TRS_MODE[4] == 1) {
          digitalWriteFast(TRS_DATA_PIN[4], LOW);
          //StatusDisplay("Note On G3");
        }
        break;
      //A3 Note: set to TRS-5
      case 57:
        if (Config.TRS_MODE[5] == 1) {
          digitalWriteFast(TRS_DATA_PIN[5], LOW);
          //StatusDisplay("Note On A3");
        }
        break;
    }
  }
}

void handlePitchBend(byte channel, int pitch) {
  //Luma-mu code
  if (usbMIDI.getCable() == 15 && channel == Config.lumaMuChannel) {

    pitchBendLumaMu = pitch;

    #ifdef VOICE_DEBUG
    StatusDisplay("PitchBend " + String(pitchBendLumaMu));
    #endif
  
    for (int i = 0; i < currentPolyphony; i++) {
      if (voices[i].active) {
        hz_value = baseOctaveMultiplierValue[Config.baseOctaveMultiplier] * pow(2.0, ((float)(voices[i].note - 69) / 12.0) + (float)(pitchBendLumaMu) / (8192.0 / Config.pitchBendRangeInSemitones * 12.0)) * 440  + 100*Config.lumaMuTuning + Config.lumaMuFineTuning;
        analogWriteFrequency(TRS_DATA_PIN[voices[i].lumamuClockPort], hz_value);
        analogWrite(TRS_DATA_PIN[voices[i].lumamuClockPort], 128);
      }
    }
  }
}

void handleClock() {

  // Only for managing TRS CLOCK SYNC output
  for (int i = 0; i < 6; i++) {
    if ((Config.TRS_MODE[i] == 2) && (usbMIDI.getCable() == 8 + i)) {
      if (Config.clock_resolution[i] != 0) {
        if (clockPulseCounter[i] == clock_resolution_value[Config.clock_resolution[i]])
          clockPulseCounter[i] = 0;

        if (clockPulseCounter[i] < int(clock_resolution_value[Config.clock_resolution[i]] / 2)) {
          digitalWriteFast(TRS_DATA_PIN[i], HIGH);
          // StatusDisplay("Tick");
        }

        clockPulseCounter[i]++;

        if (clockPulseCounter[i] >= int(clock_resolution_value[Config.clock_resolution[i]] / 2))
          digitalWriteFast(TRS_DATA_PIN[i], LOW);
      }

      else {

        if (clockPulseCounter[i] == 0) {
          digitalWriteFast(TRS_DATA_PIN[i], HIGH);
          clockPulseCounter[i]++;
        } else {
          digitalWriteFast(TRS_DATA_PIN[i], LOW);
          clockPulseCounter[i] = 0;
        }
      }
    }
  }
}

void handleStop() {
  // handle MIDI STOP events when DAW sends it on any of USB MIDI 9-14 ports
  StatusDisplay("MIDI STOP on Port #" + String(usbMIDI.getCable() + 1));
  for (int i = 0; i < 6; i++) {
    if ((Config.TRS_MODE[i] == 2) && (usbMIDI.getCable() == 8 + i)) {
      digitalWriteFast(TRS_DATA_PIN[i], LOW);
      digitalWrite(TRS_DATA_PIN[i], LOW);
      clockPulseCounter[i] = 0;
    }
  }
}

void EpromSave() {
  EEPROM.put(0, Config);
}

void EpromLoad() {
  EEPROM.get(0, Config);
}

int findFreeVoice() {

//no round robin
//  for (int i = 0; i < currentPolyphony; i++) {
//    if (!voices[i].active) return i;
//  }
  
  if (currentPolyphony ==1) return 0;

//with round robin
  int tries = 0;
  int i = nextVoiceIndex;

  while (voices[i].active && tries < currentPolyphony) {
    i = (i + 1) % currentPolyphony;
    tries++;
  }

  nextVoiceIndex = (i + 1) % currentPolyphony;

  #ifdef VOICE_DEBUG
  StatusDisplay("nextVoiceIndex:" + String(nextVoiceIndex));
  StatusDisplay("i:" + String(i));
  #endif
  
  if (i >= 0) return i;
    else return -1;

}

int stealOldestVoice() {
  
  if (currentPolyphony == 1) return 0;
  
  int oldestIndex = -1;
  unsigned long oldestTime = millis();

  for (int i = 0; i < currentPolyphony; i++) {
    if (voices[i].active && voices[i].startTime <= oldestTime) {
      oldestTime = voices[i].startTime;
      oldestIndex = i;
    }
  }
  return oldestIndex;
}


void displayTRSMode(int name, int value) {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_7x13_mf);
  u8g2.drawButtonUTF8(0, 15, U8G2_BTN_BW1 + U8G2_BTN_INV + U8G2_BTN_SHADOW2, 0, 2, 2, trs_mode_name[name]);
  u8g2.setFontMode(0);
  u8g2.setFont(u8g2_font_10x20_mf);
  u8g2.drawButtonUTF8(64, 48, U8G2_BTN_BW2 + U8G2_BTN_HCENTER, 0, 2, 2, port_mode_value[value]);
  u8g2.sendBuffer();
}

void displayClockResolution(int name, int value) {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_7x13_mf);
  u8g2.drawButtonUTF8(0, 15, U8G2_BTN_BW1 + U8G2_BTN_INV + U8G2_BTN_SHADOW2, 0, 2, 2, clock_resolution_name[name]);
  u8g2.setFontMode(0);
  u8g2.setFont(u8g2_font_10x20_mf);
  u8g2.drawButtonUTF8(64, 48, U8G2_BTN_BW2 + U8G2_BTN_HCENTER, 0, 2, 2, clock_resolution_value_name[value]);
  u8g2.sendBuffer();
}

void displayTriggerChannel(int value) {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_7x13_mf);
  u8g2.drawButtonUTF8(0, 15, U8G2_BTN_BW1 + U8G2_BTN_INV + U8G2_BTN_SHADOW2, 0, 2, 2, "TRS Triggers");
  u8g2.setFontMode(0);

  u8g2.setFont(u8g2_font_6x13_tr);
  u8g2.drawStr(31, 41, "MIDI");
  u8g2.drawStr(19, 52, "Channel");

  u8g2.setFont(u8g2_font_10x20_mf);
  char str[20];
  sprintf(str, "%d", value);
  u8g2.drawButtonUTF8(100, 48, U8G2_BTN_BW2 + U8G2_BTN_HCENTER, 0, 2, 2, str);

  u8g2.sendBuffer();
}

void displayLumaMuChannel(int value) {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_7x13_mf);
  u8g2.drawButtonUTF8(0, 15, U8G2_BTN_BW1 + U8G2_BTN_INV + U8G2_BTN_SHADOW2, 0, 2, 2, "Luma-mu options");
  u8g2.setFontMode(0);

  u8g2.setFont(u8g2_font_6x13_tr);
  u8g2.drawStr(31, 41, "MIDI");
  u8g2.drawStr(19, 52, "Channel");

  u8g2.setFont(u8g2_font_10x20_mf);
  char str[20];
  sprintf(str, "%d", value);
  u8g2.drawButtonUTF8(100, 48, U8G2_BTN_BW2 + U8G2_BTN_HCENTER, 0, 2, 2, str);

  u8g2.sendBuffer();
}

void displayLumaMuTuning(int value) {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_7x13_mf);
  u8g2.drawButtonUTF8(0, 15, U8G2_BTN_BW1 + U8G2_BTN_INV + U8G2_BTN_SHADOW2, 0, 2, 2, "Luma-mu options");
  u8g2.setFontMode(0);

  u8g2.setFont(u8g2_font_6x13_tr);
  u8g2.drawStr(15, 41, "Tuning");
  u8g2.drawStr(16, 52, "100x");

  u8g2.setFont(u8g2_font_10x20_mf);
  char str[20];
  sprintf(str, "%d", value);
  u8g2.drawButtonUTF8(90, 48, U8G2_BTN_BW2 + U8G2_BTN_HCENTER, 0, 2, 2, str);

  u8g2.sendBuffer();
}

void displayLumaMuFineTuning(int value) {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_7x13_mf);
  u8g2.drawButtonUTF8(0, 15, U8G2_BTN_BW1 + U8G2_BTN_INV + U8G2_BTN_SHADOW2, 0, 2, 2, "Luma-mu options");
  u8g2.setFontMode(0);

  u8g2.setFont(u8g2_font_6x13_tr);
  u8g2.drawStr(25, 41, "Fine");
  u8g2.drawStr(16, 52, "Tuning");

  u8g2.setFont(u8g2_font_10x20_mf);
  char str[20];
  sprintf(str, "%d", value);
  u8g2.drawButtonUTF8(100, 48, U8G2_BTN_BW2 + U8G2_BTN_HCENTER, 0, 2, 2, str);

  u8g2.sendBuffer();
}

void displayBaseOctaveMultiplier(int value) {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_7x13_mf);
  u8g2.drawButtonUTF8(0, 15, U8G2_BTN_BW1 + U8G2_BTN_INV + U8G2_BTN_SHADOW2, 0, 2, 2, "Luma-mu options");
  u8g2.setFontMode(0);

  u8g2.setFont(u8g2_font_6x13_tr);
  u8g2.drawStr(35, 41, "Base");
  u8g2.drawStr(26, 52, "Octave");

  u8g2.setFont(u8g2_font_10x20_mf);
  char str[20];
  sprintf(str, "%d", baseOctaveMultiplierValue[value]);
  u8g2.drawButtonUTF8(100, 48, U8G2_BTN_BW2 + U8G2_BTN_HCENTER, 0, 2, 2, str);

  u8g2.sendBuffer();
}

void displayLumaMuPitchBendRange(int value) {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_7x13_mf);
  u8g2.drawButtonUTF8(0, 15, U8G2_BTN_BW1 + U8G2_BTN_INV + U8G2_BTN_SHADOW2, 0, 2, 2, "Luma-mu options");
  u8g2.setFontMode(0);

  u8g2.setFont(u8g2_font_6x13_tr);
  u8g2.drawStr(16, 41, "Pitchbend");
  u8g2.drawStr(31, 52, "Rage");

  u8g2.setFont(u8g2_font_10x20_mf);
  char str[20];
  sprintf(str, "%d", value);
  u8g2.drawButtonUTF8(100, 48, U8G2_BTN_BW2 + U8G2_BTN_HCENTER, 0, 2, 2, str);

  u8g2.sendBuffer();
}


void displaylumaMuNoteOffMode(int value) {
  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_7x13_mf);
  u8g2.drawButtonUTF8(0, 15, U8G2_BTN_BW1 + U8G2_BTN_INV + U8G2_BTN_SHADOW2, 0, 2, 2, "Luma-mu options");
  u8g2.setFontMode(0);

  u8g2.setFont(u8g2_font_6x13_tr);
  u8g2.drawStr(2, 35, "Note Off Mode");
  
  u8g2.setFont(u8g2_font_10x20_mf);
  u8g2.drawButtonUTF8(64, 56, U8G2_BTN_BW2 + U8G2_BTN_HCENTER, 0, 2, 2, lumaMuNoteOffModeValue[value]);

  u8g2.sendBuffer();
}


void displayExitMenu() {

  u8g2.clearBuffer();
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.setFont(u8g2_font_profont17_tr);
  u8g2.drawStr(42, 23, "Exit?");

  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(11, 52, "Or MENU to Exit");

  u8g2.drawLine(2, 4, 2, 14);

  u8g2.drawLine(3, 4, 20, 4);

  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.drawStr(5, 40, "Press ASSIGN to Save");

  u8g2.drawLine(2, 3, 20, 3);
  u8g2.sendBuffer();
}
