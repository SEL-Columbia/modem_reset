code for a small ATmega8U2 based device to reset the sparkfun modem
devices in the SharedSolar enclosures.

to compile and program:
use usbtiny (sparkfun pocket avr) over icsp.

> make 
to compile

> make program
to burn to chip

if an ascii "1" is sent over the serial port, PD0 will go low for 1 sec and 
then go back to tri-state.  the serial port will respond with a "1 recieved"
message.  any other character is met with an "unrecognized input" message.

