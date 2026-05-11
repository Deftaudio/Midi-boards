# Don't do just this! You'll lock yourself out from editing files.
import storage
#storage.disable_usb_drive()

import usb_midi
usb_midi.set_names(streaming_interface_name="Deftaudio MidiKnob", audio_control_interface_name="Deftaudio MidiKnob", in_jack_name="MidiKnob input", out_jack_name="MidiKnob output")

import usb_hid
#usb_hid.enable(())
usb_hid.set_interface_name("Deftaudio")

#usb_hid.enable(
#    (usb_hid.Device.KEYBOARD,
#     usb_hid.Device.CONSUMER_CONTROL)
#)





