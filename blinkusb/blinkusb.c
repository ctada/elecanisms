#include <p24FJ128GB206.h> // processor 
#include "config.h"
#include "common.h"
#include "usb.h"
#include "pin.h"
#include "ui.h"
#include "timer.h"
#include "oc.h"

// C pre-process directives
#define TOGGLE_LED1      0   
#define SET_DUTY    	 1   // sets PWM duty cycle, i.e. for motor driver, etc.
#define GET_DUTY    	 2   // reads PWM duty cycle from program

//uint16_t val1, val2;

//void ClassRequests(void) {
//    switch (USB_setup.bRequest) {
//        default:
//            USB_error_flags |= 0x01;                    // set Request Error Flag
//    }
//}

void VendorRequests(void) {
    WORD temp;

    switch (USB_setup.bRequest) {
        case TOGGLE_LED1:
	    led_toggle(&led1); // from ui module
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case SET_DUTY:
	    //pin_write(&D[13], USB_setup.wValue.w); 
	    pin_write(&D[13], 0x8000);
	    // below needed to finish all vendor requests
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case GET_DUTY:
            // gets PWM as unsigned int(?) word interface of temp variable
	    temp.w = pin_read(&D[13]); 
	    BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            BD[EP0IN].bytecount = 2;    // set EP0 IN byte count to 2
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;            
        default:
            USB_error_flags |= 0x01;    // set Request Error Flag
    }
}

void VendorRequestsIn(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

void VendorRequestsOut(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

int16_t main(void) {
    init_clock();
    init_ui();
    init_pin(); 
    init_oc();

    // quitoc_pwm(&oc1, &D[13], NULL, 10e3,0x8000);  

    InitUSB();                              // initialize the USB registers and serial interface engine
    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        ServiceUSB();                       // ...service USB requests
    }
    while (1) {
        ServiceUSB();                       // service any pending USB requests
    }
}

