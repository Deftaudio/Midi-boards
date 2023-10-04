
#include <AccelStepper.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MIDI.h>
#include <Bounce.h>
#include <EEPROM.h>


#define OLED_RESET 4
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3D
//Adafruit_SSD1306 display(OLED_RESET);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int ScrollPosition = 0;
int MaxScroll = 8;

String DisplayBuffer[10] = {' ', ' ', ' ', ' ',' ', ' ', ' ', ' ', ' ', ' '};


//MIDI port set up
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI1);

const int ledPin = 13;

#define MOT1 2
#define MOT2 3
#define MOT3 4
#define MOT4 5
#define dirPin 2
#define stepPin 3
#define enablePin 4
const int  M0 = 5;
const int  M1 = 6;
const int  M2 = 7;


const int MaxSpeed = 600;
const int MaxAcceleration = 2096;
int Position = 0;
bool StopBounce=false;
  
bool activity = false;
elapsedMillis ledOnMillis;
int CV_input = 0;
bool Gate_input = false;
#define CVPin 14
#define GatePin 8
const int MaxCV = 161;

struct Configuration{
  uint8_t CurrentMode=0;          //Current mode - 1:Follow MIDI, 2:Constant Rotation, 3:Bounce Mode, 100: Programming Mode
  uint16_t Preset[11];          //Store Preset Positions C#2-B2 for Follow MIDI Mode
  uint16_t MaxPosition=0;         //Max Position for Follow MIDI Mode
  uint16_t LowBounceRange=0;      //Low value for Bounce Mode range
  uint16_t HighBounceRange=0;     //High value for Bounce Mode range
  uint16_t Acceleration=MaxAcceleration;        //Store Acceleration for all modes
  int RunSpeed=MaxSpeed;         //Rotation Speed for Bounce mode
  int CC3Speed=0;         //Speed from MIDI CC3 
  int Bend=0;
} CurrentConfiguration;

//Uncomment for driver board DRV8825
AccelStepper stepper = AccelStepper(AccelStepper::DRIVER, stepPin, dirPin);

//Uncomment for any bipolar Stepper with DRV8833
//AccelStepper stepper(AccelStepper::FULL4WIRE, MOT1, MOT2, MOT3, MOT4);

//Uncomment for 28BYJ-48 Stepper with ULN2003 board wired MOT1=IN1, MOT2=IN2, MOT3=IN3, MOT4=IN4
//AccelStepper stepper(AccelStepper::FULL4WIRE, MOT1, MOT3, MOT2, MOT4);


void EpromSave() {
/*  double addr=0;
  for (i = 0; i < TotalMacro; i++) {
    EEPROM.put(addr, MacroPot[i]);
    addr=addr+sizeof(MacroPot[i]);  
  }
  EEPROM.put(addr+sizeof(MacroPot[TotalMacro]), CurrentMacro);
  */ 
  EEPROM.put(0, CurrentConfiguration);
}

void EpromLoad() {
/*  double addr=0;
  for (i = 0; i < TotalMacro; i++) {
    EEPROM.get(addr, MacroPot[i]);
    addr=addr+sizeof(MacroPot[i]);  
  }
  EEPROM.get(addr+sizeof(MacroPot[TotalMacro]), CurrentMacro);    
*/
  EEPROM.get(0, CurrentConfiguration);

  switch (CurrentConfiguration.CurrentMode) {    
    //C1 Note: set to Follow MIDI mode
    case 1:
      EnterFollowMIDIMode();
      break;
      
    //D1 Note: set to Constant Rotation mode
    case 2:
      EnterRotationMode();   
      break;
      
    //E1 Note: set to Bounce mode
    case 3:
      EnterBounceMode();
      break;
      
    default:
      DebugDisplay("Invalid configuration"); 
      //DebugDisplay(CurrentConfiguration.CurrentMode); 
      //delay(500);
      DebugDisplay("Loading Empty Default");
      CurrentConfiguration.CurrentMode = 2;
      stepper.setCurrentPosition(0);
      CurrentConfiguration.Preset[0] = 0;
      CurrentConfiguration.RunSpeed = MaxSpeed;
      CurrentConfiguration.CC3Speed = MaxSpeed/2;
      CurrentConfiguration.Acceleration = MaxAcceleration;   
      CurrentConfiguration.Bend = 0;
      EpromSave();
      EnterRotationMode(); 
      break;
  }
}

void EnterFollowMIDIMode()
{
  CurrentConfiguration.CurrentMode=1;
  stepper.setMaxSpeed(MaxSpeed);
  DebugDisplay("Follow MIDI mode");
  EpromSave();
  stepper.enableOutputs();
}

void EnterRotationMode()
{
  CurrentConfiguration.CurrentMode=2;
  stepper.setMaxSpeed(MaxSpeed);
  stepper.setSpeed(CurrentConfiguration.CC3Speed);
  DebugDisplay("Rotation mode");     
  EpromSave();   
  stepper.enableOutputs();
}

void EnterBounceMode()
{
  CurrentConfiguration.CurrentMode=3;
  EpromSave();
  stepper.setSpeed(CurrentConfiguration.RunSpeed);
  stepper.enableOutputs();      
  stepper.moveTo(CurrentConfiguration.HighBounceRange);
  while (stepper.distanceToGo() != 0) {
    stepper.run();
  }
  Position = stepper.currentPosition() - 1;  
  StopBounce=false;
  DebugDisplay("Bounce mode");
}

void EnterProgrammingMode()
{
  CurrentConfiguration.CurrentMode=100;  
  DebugDisplay("---------------------");  
  delay(100);
  DebugDisplay("Programming mode");  
  delay(100);
  DebugDisplay("Press B1 to Save");  
  delay(100);
  DebugDisplay("Use Pitch to Move Pot"); 
  stepper.enableOutputs();
}


void handleNoteOn(byte inChannel, byte inNote, byte inVelocity)
{
  //DebugDisplay("NOTE ON message");
  //DebugDisplay(inNote);

  if (CurrentConfiguration.CurrentMode == 1) { // Follow MIDI Mode
    //DebugDisplay(inNote); 
    //DebugDisplay("was in Follow"); 
    switch (inNote) {    
      //C1 Note: set to Follow MIDI mode
      case 24:
        EnterFollowMIDIMode();
        break;
      
      //D1 Note: set to Constant Rotation mode
      case 26:
        EnterRotationMode();
        break;
      
      //E1 Note: set to Bounce mode
      case 28:
        EnterBounceMode();
        break;
      
      
      //A1 Note: Disable Stepper
      case 33:
        stepper.disableOutputs();
        DebugDisplay("Disable stepper");  
        break;

      //B1 Note: set to Programming mode
      case 35:
        EnterProgrammingMode();
        break;
    
      //C2 Note: Goto ZERO position for Follow MIDI mode
      case 36:
        DebugDisplay("Goto Zero Position");
        stepper.moveTo(0);
        break;
      
      //C2# Note: Goto Preset 1 Position
      case 37:
        DebugDisplay("Goto Preset 1");     
        stepper.moveTo(CurrentConfiguration.Preset[0]);
        break;
        
      //D2 Note: Goto Preset 2 Position
      case 38:
        DebugDisplay("Goto Preset 2");
        stepper.moveTo(CurrentConfiguration.Preset[1]);
        break;
      
      //D#2 Note: Goto Preset 3 Position
      case 39:
        DebugDisplay("Goto Preset 3");
        stepper.moveTo(CurrentConfiguration.Preset[2]);
        break;
       
       //E2 Note: Goto Preset 4 Position      
      case 40:
        DebugDisplay("Goto Preset 4");
        stepper.moveTo(CurrentConfiguration.Preset[3]);
        break;
        
       //F2 Note: Goto Preset 5 Position      
      case 41:
        DebugDisplay("Goto Preset 5");
        stepper.moveTo(CurrentConfiguration.Preset[4]);
        break;

       //F#2 Note: Goto Preset 6 Position
      case 42:
        DebugDisplay("Goto Preset 6");
        stepper.moveTo(CurrentConfiguration.Preset[5]);
        break;

       //G2 Note: Goto Preset 7 Position
      case 43:
        DebugDisplay("Goto Preset 7");
        stepper.moveTo(CurrentConfiguration.Preset[6]);
        break;

       //G#2 Note: Goto Preset 8 Position
      case 44:
        DebugDisplay("Goto Preset 8");
        stepper.moveTo(CurrentConfiguration.Preset[7]);
        break;
       
       //A2 Note: Goto Preset 9 Position
      case 45:
        DebugDisplay("Goto Preset 9");
        stepper.moveTo(CurrentConfiguration.Preset[8]);
        break;
       
       //A#2 Note: Goto Preset 10 Position
      case 46:
        DebugDisplay("Goto Preset10");
        stepper.moveTo(CurrentConfiguration.Preset[9]);
        break;
       
       //B2 Note: Goto Preset 11 Position
      case 47:
        DebugDisplay("Goto Preset11");
        stepper.moveTo(CurrentConfiguration.Preset[10]);
        break;
       
      //C3 Note: Goto Max position for Follow MIDI mode 
      case 48:
        stepper.moveTo(CurrentConfiguration.MaxPosition);
        DebugDisplay("Goto Max Position");  
        break;
      
      //C4 Note: Goto Low value for Bounce Mode range
      case 60:
        stepper.moveTo(CurrentConfiguration.LowBounceRange);
        DebugDisplay("Goto Low Bounce Range");  
        break;

      //C5 Note: Goto High value for Bounce Mode range
      case 72:
        stepper.moveTo(CurrentConfiguration.HighBounceRange);
        DebugDisplay("Goto High Bounce Rnge");  
        break;

      default:
        DebugDisplay("Not Implemented");    
        break;
      
    }
  }

  else if (CurrentConfiguration.CurrentMode ==2 || CurrentConfiguration.CurrentMode ==0) { // Constant Rotation Mode & Empty
    //DebugDisplay("was in 0 2 3"); 
    //DebugDisplay(inNote); 
    switch (inNote) {    
      //C1 Note: set to Follow MIDI mode
      case 24:
        stepper.setCurrentPosition(0);
        EnterFollowMIDIMode();
        break;
      
      //D1 Note: set to Constant Rotation mode
      case 26:
        EnterRotationMode();     
        break;
      
      //E1 Note: set to Bounce mode
      case 28:
        stepper.setCurrentPosition(0);
        EnterBounceMode();
        break;
      
      //A1 Note: Disable Stepper
      case 33:
        stepper.disableOutputs();
        DebugDisplay("Disable stepper"); 
        break;
      
      //B1 Note: set to Programming mode
      case 35:
        stepper.setCurrentPosition(0);
        EnterProgrammingMode();
        break;

      default:
        DebugDisplay("Not Implemented");    
        break;
      
    }
  }

  else if (CurrentConfiguration.CurrentMode ==3) { // Bounce Mode
    //DebugDisplay("was in 0 2 3"); 
    //DebugDisplay(inNote); 
    switch (inNote) {    
      //C1 Note: set to Follow MIDI mode
      case 24:
        EnterFollowMIDIMode();
        break;
      
      //D1 Note: set to Constant Rotation mode
      case 26:
        EnterRotationMode();     
        break;
      
      //E1 Note: set to Bounce mode
      case 28:
        EnterBounceMode();
        break;
      
      //A1 Note: Disable Stepper
      case 33:
        stepper.moveTo(Position); 
        while (stepper.distanceToGo() != 0)
          stepper.run();
        stepper.disableOutputs();
        StopBounce=true;
        DebugDisplay("Disable stepper"); 
        break;
      
      //B1 Note: set to Programming mode
      case 35:
        EnterProgrammingMode();
        break;

      default:
        DebugDisplay("Not Implemented");    
        break;
      
    }
  }
  
  else if (CurrentConfiguration.CurrentMode==100) { // Programming Mode
    //DebugDisplay("was in Programming"); 
      switch (inNote) {
      //DebugDisplay("Programming #");  
      //DebugDisplay(inNote); 
      
      //C1 Note: set to Follow MIDI mode
      case 24:
        EnterFollowMIDIMode();
        break;
     
      //D1 Note: set to Constant Rotation mode
      case 26:
        EnterRotationMode();
        break;
      
      //E1 Note: set to Bounce mode
      case 28:
        EnterBounceMode();
        break;

      //A1 Note: Disable Stepper
      case 33:
        stepper.disableOutputs();
        DebugDisplay("Disable stepper");  
        break;

      //B1 Note: Set to SAVE EPROM
      case 35:
        EpromSave();
        DebugDisplay("Eprom Saved");  
        break;

      //C2 Note: Set ZERO position for Follow MIDI mode
      case 36:
        DebugDisplay("Set Zero Position");
        stepper.setCurrentPosition(0);
        break;
      
      //C2# Note: Set Preset 1 Position
      case 37:
        CurrentConfiguration.Preset[0] = stepper.currentPosition();
        DebugDisplay("Set Preset 1 Position");     
        break;
        
      //D2 Note: Set Preset 2 Position
      case 38:
        CurrentConfiguration.Preset[1] = stepper.currentPosition();
        DebugDisplay("Set Preset 2 Position");
        break;
      
      //D#2 Note: Set Preset 3 Position
      case 39:
        CurrentConfiguration.Preset[2] = stepper.currentPosition();
        DebugDisplay("Set Preset 3 Position");
        break;
       
       //E2 Note: Set Preset 4 Position      
      case 40:
        CurrentConfiguration.Preset[3] = stepper.currentPosition();
        DebugDisplay("Set Preset 4 Position");
        break;
        
       //F2 Note: Set Preset 5 Position      
      case 41:
        CurrentConfiguration.Preset[4] = stepper.currentPosition();
        DebugDisplay("Set Preset 5 Position");
        break;

       //F#2 Note: Set Preset 6 Position
      case 42:
        CurrentConfiguration.Preset[5] = stepper.currentPosition();
        DebugDisplay("Set Preset 6 Position");
        break;

       //G2 Note: Set Preset 7 Position
      case 43:
        CurrentConfiguration.Preset[6] = stepper.currentPosition();
        DebugDisplay("Set Preset 7 Position");
        break;

       //G#2 Note: Set Preset 8 Position
      case 44:
        CurrentConfiguration.Preset[7] = stepper.currentPosition();
        DebugDisplay("Set Preset 8 Position");
        break;
       
       //A2 Note: Set Preset 9 Position
      case 45:
        CurrentConfiguration.Preset[8] = stepper.currentPosition();
        DebugDisplay("Set Preset 9 Position");
        break;
       
       //A#2 Note: Set Preset 10 Position
      case 46:
        CurrentConfiguration.Preset[9] = stepper.currentPosition();
        DebugDisplay("Set Preset10 Position");
        break;
       
       //B2 Note: Set Preset 11 Position
      case 47:
        CurrentConfiguration.Preset[10] = stepper.currentPosition();
        DebugDisplay("Set Preset11 Position");
        break;
       
      //C3 Note: Set Max position for Follow MIDI mode 
      case 48:
        CurrentConfiguration.MaxPosition = stepper.currentPosition();
        DebugDisplay("Set Max Position");  
        break;
      
      //C4 Note: Set Low value for Bounce Mode range
      case 60:
        CurrentConfiguration.LowBounceRange = stepper.currentPosition();
        DebugDisplay("Set Low Bounce Range");  
        break;

      //C5 Note: Set High value for Bounce Mode range
      case 72:
        CurrentConfiguration.HighBounceRange = stepper.currentPosition();
        DebugDisplay("Set High Bounce Range");  
        break;

      default:
        DebugDisplay("Not Implemented");    
        break;
      }
  }

usbMIDI.sendNoteOn(inNote, inVelocity, inChannel);  
}

void handleNoteOff(byte inChannel, byte inNote, byte inVelocity){

usbMIDI.sendNoteOff(inNote, inVelocity, inChannel);  
}



void HandlePitchBend(byte channel, int bend)
{
  //DebugDisplay("Pitch Bend message");
  //DebugDisplay(bend);
  
  
  if (CurrentConfiguration.CurrentMode == 2) { // Constant Rotation mode

    if (bend > 10 || bend < -10) { // Check later if hysteresis is good enough. 
    CurrentConfiguration.Bend = map(bend,-8192,8192,-MaxSpeed,MaxSpeed);
    } else CurrentConfiguration.Bend = 0;
        
    CurrentConfiguration.RunSpeed = CurrentConfiguration.Bend + CurrentConfiguration.CC3Speed;    
    if (CurrentConfiguration.RunSpeed >= MaxSpeed) {
      CurrentConfiguration.RunSpeed = MaxSpeed;
    } 
  stepper.setSpeed(CurrentConfiguration.RunSpeed);
  }
  

  if (CurrentConfiguration.CurrentMode == 100 ) { // Programming Mode
    if (bend > 128 || bend < -128) { // Check later if hysteresis is good enough. 
      CurrentConfiguration.Bend=map(bend,-8192,8192,-MaxSpeed/5,MaxSpeed/5);
    } else CurrentConfiguration.Bend = 0;
    
    stepper.setSpeed(CurrentConfiguration.Bend);
  } 
 
  
usbMIDI.sendPitchBend(bend, channel);
  
}

 
void HandleControlChange(byte channel, byte number, byte value)
{
//  DebugDisplay("CC message");
//  DebugDisplay(number);
//  DebugDisplay(value);  
  switch (number) {
      case 1: // Follow CC1 position
        if (CurrentConfiguration.CurrentMode == 1 ) { // For Follow MIDI Mode only
          stepper.moveTo(map(value,0,127,0,CurrentConfiguration.MaxPosition));
        }
        break;
        
      case 2: // Set Acceleration
        if (CurrentConfiguration.CurrentMode != 2 ) { 
          stepper.setAcceleration(map(value,0,127,0,MaxAcceleration));
        }
        break;

      case 3: // Set Speed
        if (CurrentConfiguration.CurrentMode == 2 ) { // For Constant Rotation mode
          if (value==64) {
            CurrentConfiguration.CC3Speed = 0;
          } else {
            CurrentConfiguration.CC3Speed = map(value,0,127,-MaxSpeed,MaxSpeed);
            stepper.enableOutputs();
          }
  
          CurrentConfiguration.RunSpeed = CurrentConfiguration.Bend + CurrentConfiguration.CC3Speed;    
          if (CurrentConfiguration.RunSpeed >= MaxSpeed) {
            CurrentConfiguration.RunSpeed = MaxSpeed;
          } 
         
          stepper.setSpeed(CurrentConfiguration.RunSpeed);
        }

        if (CurrentConfiguration.CurrentMode == 1 || CurrentConfiguration.CurrentMode == 3 ) { // For Follow MIDI and Bounce mode
          CurrentConfiguration.RunSpeed = map(value,0,127,0,MaxSpeed);
          //stepper.setSpeed(CurrentConfiguration.RunSpeed);
          stepper.setMaxSpeed(CurrentConfiguration.RunSpeed);
        }
        
        break;
  }

usbMIDI.sendControlChange(number, value, channel);

}



void setup() {
//  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
    
  pinMode(GatePin, INPUT_PULLUP);
  pinMode(CVPin, INPUT_PULLDOWN);


//MIDI and USB MIDI intialization Part
  MIDI1.begin(MIDI_CHANNEL_OMNI);
  MIDI1.turnThruOff();  
  MIDI1.setHandleNoteOn(handleNoteOn);
  MIDI1.setHandleNoteOff(handleNoteOff);
  MIDI1.setHandlePitchBend(HandlePitchBend);
  MIDI1.setHandleControlChange(HandleControlChange);
  
  usbMIDI.setHandleNoteOn(handleNoteOn);
  usbMIDI.setHandleNoteOff(handleNoteOff);
  usbMIDI.setHandlePitchChange(HandlePitchBend);
  usbMIDI.setHandleControlChange(HandleControlChange);





//Display initialization
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  delay(5);
  display.clearDisplay();

  // start display text
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(" Deftaudio");
  display.display();
  delay(500);
  display.println(" ");
  display.println("  MotorPot");
  display.println("   v1.0");
  display.display();
  delay(2000);

//Stepper Motor initialization
  //myStepper.setSpeed(10);
  stepper.setMaxSpeed(MaxSpeed);
  stepper.setAcceleration(MaxAcceleration);
  stepper.setCurrentPosition(0);

//Uncomment for any bipolar Stepper with DRV8833  
//  pinMode(MOT1, OUTPUT);
//  pinMode(MOT2, OUTPUT);
//  pinMode(MOT3, OUTPUT);
//  pinMode(MOT4, OUTPUT);

//Uncomment for driver board DRV8825
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  pinMode(enablePin, OUTPUT);
  stepper.setEnablePin(enablePin);
  stepper.setPinsInverted(false, false, true);
  stepper.enableOutputs();

  EpromLoad();

  Gate_input = digitalRead(GatePin);

//  CurrentConfiguration.CurrentMode=1;
//  DebugDisplay("Follow MIDI mode");

}

void loop() {
  activity = false;


if (usbMIDI.read() || MIDI1.read()) {
    activity = true;
     // DebugDisplay("MIDI message");
}

//CV/Gate process
if (digitalRead(GatePin) == false) {
  if ((analogRead(CVPin) - CV_input) > 1 || (analogRead(CVPin) - CV_input) < -1) {
    switch (CurrentConfiguration.CurrentMode) {
    
      case 1: //Follow MIDI Mode
      stepper.moveTo(map(analogRead(CVPin),0,MaxCV,0,CurrentConfiguration.MaxPosition));
      activity = true;
      break;

      case 2: //Rotation Mode
      CurrentConfiguration.RunSpeed = map(analogRead(CVPin),0,MaxCV,0,MaxSpeed);
      stepper.setMaxSpeed(CurrentConfiguration.RunSpeed);
      activity = true;
      break;
      
      case 3: //Bounce Mode
      CurrentConfiguration.RunSpeed = map(analogRead(CVPin),0,MaxCV,0,MaxSpeed);
      stepper.setMaxSpeed(CurrentConfiguration.RunSpeed);
      activity = true;
      break;
    }
 
  }

  CV_input = analogRead(CVPin);
}


/*
if ((analogRead(CVPin) - CV_input) > 2 || (analogRead(CVPin) - CV_input) < -2) {
//debug 
  DebugDisplay("analog CV is: ");
  DebugDisplay(CV_input);
  CV_input = analogRead(CVPin);
}
*/



switch (CurrentConfiguration.CurrentMode) {

  case 1: // Follow MIDI Mode
    stepper.run();
    break;
  
  case 2: // // Constant Rotation Mode
    if (CurrentConfiguration.RunSpeed !=0) {
      stepper.runSpeed();
      //stepper.enableOutputs();
    } else stepper.disableOutputs();
    
    break;

  case 3: // Bounce Mode
    
    if (!StopBounce)
      stepper.run();
    
    if (stepper.distanceToGo() == 0) {
     
      if (Position > stepper.currentPosition())   
        stepper.moveTo(CurrentConfiguration.HighBounceRange);
      if (Position < stepper.currentPosition())   
        stepper.moveTo(CurrentConfiguration.LowBounceRange);
    }
    
    Position = stepper.currentPosition();
    
    break;

  case 100: // Programming Mode
 
    if (CurrentConfiguration.Bend !=0) { 
      stepper.runSpeed();
      stepper.enableOutputs();
    } // else stepper.disableOutputs();
   
    break;    
}




// blink the LED when any activity has happened
  if (activity) {
    digitalWriteFast(ledPin, HIGH); // LED on
    ledOnMillis = 0;
  }
  if (ledOnMillis > 15) {
    digitalWriteFast(ledPin, LOW);  // LED off
  } 
  

  
  //delayMicroseconds(50); // limits message frequency

}

void DebugDisplay(String text)
{
  if (ScrollPosition > MaxScroll - 1)
  {
    for (int i = 0; i < MaxScroll; i++) {
      DisplayBuffer[i] = DisplayBuffer[i+1];
    }
    DisplayBuffer[MaxScroll] = text;
    ScrollPosition = MaxScroll-1;
  } else {
    DisplayBuffer[ScrollPosition] = text;
  }

  DisplayBuffer[ScrollPosition] = text;
  display.clearDisplay();
  display.setTextSize(1); 
  display.setCursor(0,0);
  for (int i = 0; i < 8; i++) {
    display.println(DisplayBuffer[i]);
  }

  display.display();
  ScrollPosition++;
  
}
