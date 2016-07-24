#powerdetect

##Purpose

This program was written to provide a notification when a remotely-located audio system is left on past a certain time of day. This is accomplished by triggering a button on a USB-connected gamepad via a low-voltage circuit that is activated when the audio system is on. If the gamepad button is triggered during a certain time, a text message is sent to a cell phone.

##Method

Because of intermittent errors that arise from reading the gamepad input, a wrapper program is used to run the main program. To ensure continuous operation, the wrapper program will restart the main program when it exits. Errors, events, and the audio system's status are logged for a minimum of one week.

##Configuration

- Replace all instances of `[path]` with the absolute path of the powerdetect folder in the following files:
  - powerdetect.c
  - wrapper.c
  - cleanup.sh
  - powerdetect.sh

- Change the macros `PHONE` and `TEXTMESSAGE` in `powerdetect.c` to the phone number to be notified and the message that will be sent.
  - Numbers outside the U.S. need a [different POST request](http://textbelt.com/) in `powerdetect.c`

- Enable dependencies (for Debian-based systems):

```sudo modprobe joydev

sudo modprobe analog

sudo modprobe xpad

sudo apt-get install curl```

- Compile programs with `make`

- Schedule scripts for automatic operation and weekly cleanup:
`sudo crontab -e`
Add these lines (replace `[path]` with the absolute path of the powerdetect folder):

```@reboot [path]/powerdetect/powerdetect.sh

0 0 * * 1 [path]/powerdetect/cleanup.sh```
