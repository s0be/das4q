# das4q
Library and Example code for Das 4Q backlight control

## libdas4q
A simple C Library that begins implementing backlight control for the das4q.

Depends on libusb-1.0

Current features:
  - Checks firwmare version
  - Can set RGB on a per key basis
  - Can enable blinking

Currently missing:
  - Any active key feature
  - Q Button integration
  - Firmware update
  - Basically anything else

## examples/das_udev
A simple C Program using libdas4q.  It's intended to be used in a udev rule to set your lights.

Depends on libdas4q, cJSON

Current features:
  - Can set all the lights to 1 value on the command line
  - Can do advanced things in json
