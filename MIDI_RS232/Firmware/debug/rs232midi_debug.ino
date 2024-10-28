/* RS232 <-> MIDI
HW Serial for serial
SoftSerial for MIDI
*/

#include <MIDI.h>
#include <AltSoftSerial.h>

AltSoftSerial SWSerial;

// A variable to know how long the LED has been turned on
elapsedMillis ledOnMillis;
int incomingByte = 0; 

void setup() {
  pinMode(11, OUTPUT); // LED pin
  pinMode(10, INPUT); // rx pin for MIDI
  pinMode(9, OUTPUT);// tx pin for MIDI
  
  Serial1.begin(38400); //RS232
  Serial.begin(38400); //USB
  SWSerial.begin(31250); //MIDI DIN5


    digitalWriteFast(11, HIGH); // LED on
    delay(50);
    digitalWriteFast(11, LOW);
    delay(50);
    digitalWriteFast(11, HIGH); // LED on
    delay(50);
    digitalWriteFast(11, LOW);

}


void loop() {
  bool activity = false;

  if (SWSerial.available()) {        // If anything comes in Serial (USB),
    incomingByte = SWSerial.read();
    Serial1.write(incomingByte);  // read it and send it out Serial1 (pins 0 & 1)
    Serial.write(incomingByte);  // read it and send it out Serial1 (pins 0 & 1)
    activity = true;
  }

  if (Serial1.available()) {       // If anything comes in Serial1 (pins 0 & 1)
    incomingByte = Serial1.read();
    SWSerial.write(incomingByte);  // read it and send it out Serial (USB)
    Serial.write(incomingByte); 
    activity = true;
  }

  if (Serial.available()) {       // If anything comes in Serial1 (pins 0 & 1)
    incomingByte = Serial.read();
    SWSerial.write(incomingByte);  // read it and send it out Serial (USB)
    Serial1.write(incomingByte); 
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
