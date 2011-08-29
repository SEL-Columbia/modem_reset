#include "ModemReset.h"

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber         = 0,

				.DataINEndpointNumber           = CDC_TX_EPNUM,
				.DataINEndpointSize             = CDC_TXRX_EPSIZE,
				.DataINEndpointDoubleBank       = false,

				.DataOUTEndpointNumber          = CDC_RX_EPNUM,
				.DataOUTEndpointSize            = CDC_TXRX_EPSIZE,
				.DataOUTEndpointDoubleBank      = false,

				.NotificationEndpointNumber     = CDC_NOTIFICATION_EPNUM,
				.NotificationEndpointSize       = CDC_NOTIFICATION_EPSIZE,
				.NotificationEndpointDoubleBank = false,
			},
	};

/** Standard file stream for the CDC interface when set up, so that the virtual CDC COM port can be
 *  used like any regular character stream in the C APIs
 */
static FILE USBSerialStream;


/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
	SetupHardware();

	/* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
	CDC_Device_CreateStream(&VirtualSerial_CDC_Interface, &USBSerialStream);

	sei();

    DDRB  |= (1 << 4);
    PORTB |= (1 << 4);


	for (;;)
	{
        int16_t c = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
        if (c > 0){
            switch (c) {
                case '1':
                    // this will eventually be to strobe the modem on pin of a telit module
                    // set port to output mode and low state
                    // stall for 1000 ms
                    // set port back to high impedance
                    CDC_Device_SendString(&VirtualSerial_CDC_Interface, "received 1\r\n");
                    DDRD  |=  (1 << 0);
                    PORTD &= ~(1 << 0);

                    _delay_ms(1000);
                    DDRD  &= ~(1 << 0);
                    PORTD |=  (1 << 0);
                    break;
                case 'r':
                    // this will be to strobe the reset pin of a telit module
                    CDC_Device_SendString(&VirtualSerial_CDC_Interface, "received r\r\n");
                    PORTB &= ~(1 << 4);
                    _delay_ms(200);
                    PORTB |= (1 << 4);
                    break;
                case '?':
                    // this wil be to inquire abouth the powermon pin of a telit module
                    CDC_Device_SendString(&VirtualSerial_CDC_Interface, "received ?\r\n");
                    PORTB |= ~(1 << 4);
                    break;
                default:
                    CDC_Device_SendString(&VirtualSerial_CDC_Interface, "unrecognized input\r\n");
            }
        }
		CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
		USB_USBTask();
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	//clock_prescale_set(clock_div_1);

	/* Hardware Initialization */
	USB_Init();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	//LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	//LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);

	//LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

