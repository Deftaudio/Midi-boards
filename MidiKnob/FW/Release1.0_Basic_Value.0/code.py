import rotaryio
import board
import digitalio
import neopixel
import time
import busio
import microcontroller


import usb_hid
from adafruit_hid.consumer_control import ConsumerControl
from adafruit_hid.consumer_control_code import ConsumerControlCode
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode


import usb_midi
usb_midi.set_names(streaming_interface_name="Deftaudio MidiKnob", audio_control_interface_name="Deftaudio MidiKnob", in_jack_name="MidiKnob input", out_jack_name="MidiKnob output")

import adafruit_midi
from adafruit_midi.control_change import ControlChange
from adafruit_midi.note_off import NoteOff
from adafruit_midi.note_on import NoteOn
from adafruit_midi.start import Start
from adafruit_midi.stop import Stop
from adafruit_midi.timing_clock import TimingClock
from adafruit_midi.midi_continue import Continue

import time

class ElapsedMillis:
    def __init__(self):
        self._start = time.monotonic()

    def reset(self):
        self._start = time.monotonic()

    def elapsed(self):
        return (time.monotonic() - self._start) * 1000  # milliseconds


#print(usb_midi.ports)
USBmidi = [adafruit_midi.MIDI(midi_in=usb_midi.ports[0], in_channel=(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15), midi_out=usb_midi.ports[1], out_channel=0),
             adafruit_midi.MIDI(midi_in=usb_midi.ports[0], in_channel=(), midi_out=usb_midi.ports[1], out_channel=1),
             adafruit_midi.MIDI(midi_in=usb_midi.ports[0], in_channel=(), midi_out=usb_midi.ports[1], out_channel=2),
             adafruit_midi.MIDI(midi_in=usb_midi.ports[0], in_channel=(), midi_out=usb_midi.ports[1], out_channel=3),
             adafruit_midi.MIDI(midi_in=usb_midi.ports[0], in_channel=(), midi_out=usb_midi.ports[1], out_channel=4),
             adafruit_midi.MIDI(midi_in=usb_midi.ports[0], in_channel=(), midi_out=usb_midi.ports[1], out_channel=5),
             adafruit_midi.MIDI(midi_in=usb_midi.ports[0], in_channel=(), midi_out=usb_midi.ports[1], out_channel=6),
             adafruit_midi.MIDI(midi_in=usb_midi.ports[0], in_channel=(), midi_out=usb_midi.ports[1], out_channel=7),
             adafruit_midi.MIDI(midi_in=usb_midi.ports[0], in_channel=(), midi_out=usb_midi.ports[1], out_channel=8),
             adafruit_midi.MIDI(midi_in=usb_midi.ports[0], in_channel=(), midi_out=usb_midi.ports[1], out_channel=9),
             adafruit_midi.MIDI(midi_in=usb_midi.ports[0], in_channel=(), midi_out=usb_midi.ports[1], out_channel=10),
             adafruit_midi.MIDI(midi_in=usb_midi.ports[0], in_channel=(), midi_out=usb_midi.ports[1], out_channel=11),
             adafruit_midi.MIDI(midi_in=usb_midi.ports[0], in_channel=(), midi_out=usb_midi.ports[1], out_channel=12),
             adafruit_midi.MIDI(midi_in=usb_midi.ports[0], in_channel=(), midi_out=usb_midi.ports[1], out_channel=13),
             adafruit_midi.MIDI(midi_in=usb_midi.ports[0], in_channel=(), midi_out=usb_midi.ports[1], out_channel=14),
             adafruit_midi.MIDI(midi_in=usb_midi.ports[0], in_channel=(), midi_out=usb_midi.ports[1], out_channel=15)]
             

uart = busio.UART(board.TX, rx=None, baudrate=31250)
HWmidi = [adafruit_midi.MIDI(midi_out=uart, out_channel=0),
          adafruit_midi.MIDI(midi_out=uart, out_channel=1),
          adafruit_midi.MIDI(midi_out=uart, out_channel=2),
          adafruit_midi.MIDI(midi_out=uart, out_channel=3),
          adafruit_midi.MIDI(midi_out=uart, out_channel=4),
          adafruit_midi.MIDI(midi_out=uart, out_channel=5),
          adafruit_midi.MIDI(midi_out=uart, out_channel=6),
          adafruit_midi.MIDI(midi_out=uart, out_channel=7),
          adafruit_midi.MIDI(midi_out=uart, out_channel=8),
          adafruit_midi.MIDI(midi_out=uart, out_channel=9),
          adafruit_midi.MIDI(midi_out=uart, out_channel=10),
          adafruit_midi.MIDI(midi_out=uart, out_channel=11),
          adafruit_midi.MIDI(midi_out=uart, out_channel=12),
          adafruit_midi.MIDI(midi_out=uart, out_channel=13),
          adafruit_midi.MIDI(midi_out=uart, out_channel=14),
          adafruit_midi.MIDI(midi_out=uart, out_channel=15)]
          

def clamp(n, minn, maxn):
    return max(min(maxn, n), minn)

   
    

#SPDT LEFT Switch Configuration 
sw1_1 = digitalio.DigitalInOut(board.GP4)
sw1_1.direction = digitalio.Direction.INPUT
sw1_1.pull = digitalio.Pull.UP

sw1_2 = digitalio.DigitalInOut(board.GP5)
sw1_2.direction = digitalio.Direction.INPUT
sw1_2.pull = digitalio.Pull.UP

#SPDT RIGHT Switch Configuration 
sw2_1 = digitalio.DigitalInOut(board.GP14)
sw2_1.direction = digitalio.Direction.INPUT
sw2_1.pull = digitalio.Pull.UP

sw2_2 = digitalio.DigitalInOut(board.GP15)
sw2_2.direction = digitalio.Direction.INPUT
sw2_2.pull = digitalio.Pull.UP


def checkMode():
        
    #MIDI all down
    if sw1_1.value==0 and sw1_2.value==1 and sw2_1.value==0 and sw2_2.value==1:
            return 0
    if sw1_1.value==1 and sw1_2.value==1 and sw2_1.value==0 and sw2_2.value==1:
            return 1
    if sw1_1.value==1 and sw1_2.value==0 and sw2_1.value==0 and sw2_2.value==1:
            return 2 
    
    #SOUND
    if sw1_1.value==0 and sw1_2.value==1 and sw2_1.value==1 and sw2_2.value==0:
            return 3
    if sw1_1.value==1 and sw1_2.value==1 and sw2_1.value==1 and sw2_2.value==0:
            return 4
    if sw1_1.value==1 and sw1_2.value==0 and sw2_1.value==1 and sw2_2.value==0:
            return 5
        
    #MIDDLE
    if sw1_1.value==0 and sw1_2.value==1 and sw2_1.value==1 and sw2_2.value==1:
            return 6   
    if sw1_1.value==1 and sw1_2.value==0 and sw2_1.value==1 and sw2_2.value==1:
            return 8
    if sw1_1.value==1 and sw1_2.value==1 and sw2_1.value==1 and sw2_2.value==1:
            return 7      
    

#Encoder Configuration
encoder = rotaryio.IncrementalEncoder(board.GP1, board.GP2)

button = digitalio.DigitalInOut(board.GP3)
button.direction = digitalio.Direction.INPUT
button.pull = digitalio.Pull.UP

# Button timing parameters
DOUBLE_TAP_TIME = 0.20   # seconds (300–400 ms typical)
last_press_time = 0
waiting_for_second = False


led1 = digitalio.DigitalInOut(board.GP6)
led1.direction = digitalio.Direction.OUTPUT

led2 = digitalio.DigitalInOut(board.GP7)
led2.direction = digitalio.Direction.OUTPUT


#Onboard LED
pixel = neopixel.NeoPixel(board.NEOPIXEL, 1)


#USB Configuration
cc = ConsumerControl(usb_hid.devices)
kbd = Keyboard(usb_hid.devices)

#Variables
button_state = None
last_position = encoder.position

#cc_number = 1
#cc_value = 0
#cc_value_old = 0

midi_channel = [1,2,3]
cc_number = [1,2,3]
cc_value = [0,0,0]
cc_value_old = [0,0,0]
note_number = [24,36,48]
#msg = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]

pixel[0] = (255,0,0)
pixel.write()
time.sleep(0.5)
pixel[0] = (0,255,0)
pixel.write()
time.sleep(0.5)
pixel[0] = (0,0,255)
pixel.write()
time.sleep(0.5)

pixel[0] = (0,0,0)
pixel.write()

"""
led1.value = True  # Turn the LED on
time.sleep(0.5)   # Wait for 0.5 seconds
led1.value = False # Turn the LED off
time.sleep(0.5)   # Wait for 0.5 seconds

led1.value = True  # Turn the LED on
time.sleep(0.5)   # Wait for 0.5 seconds
led1.value = False # Turn the LED off
time.sleep(0.5)   # Wait for 0.5 seconds

led2.value = True  # Turn the LED on
time.sleep(0.5)   # Wait for 0.5 seconds
led2.value = False # Turn the LED off
time.sleep(0.5)   # Wait for 0.5 seconds

led2.value = True  # Turn the LED on
time.sleep(0.5)   # Wait for 0.5 seconds
led2.value = False # Turn the LED off
time.sleep(0.5)   # Wait for 0.5 seconds
"""


mode = 9

tick_count = 0

LedMidiClock = ElapsedMillis()
LedMidiClock.reset()

LedBarStart = ElapsedMillis()
LedBarStart.reset()

start = False
running = False

def readNVM():
    if microcontroller.nvm[100] == 0xA1:
        midi_channel[0] = microcontroller.nvm[0]
        midi_channel[1] = microcontroller.nvm[1]
        midi_channel[2] = microcontroller.nvm[2]
        cc_number[0] = microcontroller.nvm[3]
        cc_number[1] = microcontroller.nvm[4]
        cc_number[2] = microcontroller.nvm[5]
        note_number[0] = microcontroller.nvm[6]
        note_number[1] = microcontroller.nvm[7]
        note_number[2] = microcontroller.nvm[8]


def writeNVM():
    microcontroller.nvm[0] = midi_channel[0]
    microcontroller.nvm[1] = midi_channel[1]
    microcontroller.nvm[2] = midi_channel[2]
    microcontroller.nvm[3] = cc_number[0]
    microcontroller.nvm[4] = cc_number[1]
    microcontroller.nvm[5] = cc_number[2]
    microcontroller.nvm[6] = note_number[0]
    microcontroller.nvm[7] = note_number[1]
    microcontroller.nvm[8] = note_number[2]
    microcontroller.nvm[100] = 0xA1


def MidiCapture():
    #print("Capture!")
    pixel[0] = (255,0,0)
    pixel.write()
    capture = True
    time.sleep(0.5)
    
    while capture:
        #Parsing USB MIDI Input
        msg = USBmidi[0].receive()
        
        if msg is not None:
            # Note On/Off
            if isinstance(msg, NoteOn) or isinstance(msg, NoteOff):
                #print(f"Note={msg.note} ch={msg.channel}")
                note_number[mode] = msg.note
                midi_channel[mode] = msg.channel+1
                writeNVM()
                capture = False
            
            if isinstance(msg, ControlChange):
                cc_number[mode] = msg.control      # CC number (0–127)
                midi_channel[mode] = msg.channel+1
                #print(f"Note={msg.control} ch={msg.channel}")
                writeNVM()
                capture = False
        
        if not button.value:
            capture = False

readNVM()

while True:
    LED_activity = False;
    
    current_position = encoder.position
    position_change = current_position - last_position
    
    
    #cc_value = clamp(current_position, 0, 127)

    if checkMode() != mode:
        mode = checkMode()
        #print("Current mode:")
        #print(mode)
        if mode == 0 or mode == 1 or mode == 2:
            led1.value = True
            led2.value = False
            
        elif mode == 3 or mode == 4 or mode == 5:
            led1.value = False
            led2.value = True
        
        elif mode == 6 or mode == 7 or mode == 8:
            led1.value = True
            led2.value = True
               

    #Parsing USB MIDI Input
    msg = USBmidi[0].receive()
    if msg is not None:
        # MIDI Clock messages
        if isinstance(msg, Start):
            tick_count = 0
            running = True
            start = True
            #print("MIDI Start")
            #pixel[0] = (20,20,20)
            #pixel.write()
            pixel[0] = (255,255,0)
            pixel.write()
            LedBarStart = ElapsedMillis()
        elif isinstance(msg, Continue):
            running = True
            start = False
            #print("MIDI Continue: resume from current position")
        elif isinstance(msg, Stop):
            running = False
            start = False
            #print("MIDI Stop")
            pixel[0] = (0,0,0)
            pixel.write()
        elif isinstance(msg, TimingClock):
        #and running:
            tick_count += 1
            
            if tick_count == 24 or tick_count == 48 or tick_count == 72 or tick_count == 96:
                #print("Quarter note")
                led1.value = False
                led2.value = True
                LedMidiClock = ElapsedMillis()
            if tick_count >= 96:
                tick_count = 0
                if start:
                    pixel[0] = (255,255,0)
                    pixel.write()
                    LedBarStart = ElapsedMillis()


    if position_change > 0:
        for _ in range(position_change):
            if mode == 3 or mode == 4 or mode == 5:
                cc.send(ConsumerControlCode.VOLUME_INCREMENT)
            
            elif mode==0 or mode==1 or mode==2:
                cc_value[mode] +=position_change
                cc_value[mode] = clamp(cc_value[mode], 0, 127)
            
            
    elif position_change < 0:
        for _ in range(-position_change):
            if mode==3 or mode==4 or mode==5:
                cc.send(ConsumerControlCode.VOLUME_DECREMENT)

            elif mode==0 or mode==1 or mode==2:
                cc_value[mode] +=position_change
                cc_value[mode] = clamp(cc_value[mode], 0, 127)
               
            
    for i in range(3):
        if (cc_value[i] - cc_value_old[i])!= 0:
        #print("Raw value:", current_position, "MIDI Value:", cc_value)
        #if mode==0 or mode==1 or mode==2:
            USBmidi[midi_channel[i]-1].send(ControlChange(cc_number[i], cc_value[i]))
            HWmidi[midi_channel[i]-1].send(ControlChange(cc_number[i], cc_value[i]))
    
    
    last_position = current_position
    
    if mode==0 or mode==1 or mode==2:
        cc_value_old[mode] = cc_value[mode]
    

    
    if not button.value and button_state is None:
        button_state = "pressed"
        if mode==0 or mode==1 or mode==2:
            USBmidi[midi_channel[mode]-1].send(NoteOn(note_number[mode], 127))
            HWmidi[midi_channel[mode]-1].send(NoteOn(note_number[mode], 127))
            pixel[0] = (255,0,0)
            pixel.write()
        #time.sleep(0.2)
    
        now = time.monotonic()

        # simple debounce
        time.sleep(0.02)
        if not button.value:
            if waiting_for_second and (now - last_press_time) <= DOUBLE_TAP_TIME:
                #print("DOUBLE TAP")
                waiting_for_second = False
                MidiCapture()
            else:
                waiting_for_second = True
                last_press_time = now

            # wait for release
         #   while not button.value:
          #      pass


    
    
    
    if button.value and button_state == "pressed":
       # print("Button pressed.")
       # cc.send(ConsumerControlCode.PLAY_PAUSE)
       # cc.send(ConsumerControlCode.MUTE)
       
       # Phone mute - seems not working
       # cc.send(47)
       
       # Zoom mic mute shortcut
        
        if mode == 3:
            kbd.send(Keycode.WINDOWS, Keycode.SHIFT, Keycode.A)
            kbd.release_all()
            
       # Zoom camera on/off shortcut
        elif mode == 4:
            kbd.send(Keycode.WINDOWS, Keycode.SHIFT, Keycode.V)
            kbd.release_all()
            
        elif mode == 5:
            kbd.send(Keycode.SPACEBAR)
            kbd.release_all()
            
        
       # Note Off on release
        elif mode==0 or mode==1 or mode==2:
            USBmidi[midi_channel[mode]-1].send(NoteOff(note_number[mode], 127))
            HWmidi[midi_channel[mode]-1].send(NoteOff(note_number[mode], 127))
            pixel[0] = (0,0,0)
            pixel.write()
            button_state = None
    
    
    
    # Button timeout → single tap
    if waiting_for_second and (time.monotonic() - last_press_time) > DOUBLE_TAP_TIME:
        #print("SINGLE TAP")
        waiting_for_second = False
        
        
        
        
    #LED indication
    if LedMidiClock.elapsed() > 20:
        led2.value = False
        led1.value = True    
        LedMidiClock.reset()
        
    if LedBarStart.elapsed() > 20 and not running:
        pixel[0] = (0,0,0)
        pixel.write()
        LedBarStart.reset()
    
    if LedBarStart.elapsed() > 20 and running:
        pixel[0] = (20,20,20)
        pixel.write()
        LedBarStart.reset()