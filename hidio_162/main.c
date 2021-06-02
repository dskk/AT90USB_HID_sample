#include "util.h"
#include "usb.h"

// DFU address
#if defined(__AVR_AT90USB162__)
func_ptr_t start_bootloader=(func_ptr_t)0x1800;
#elif defined(__AVR_ATmega32U4__)
func_ptr_t start_bootloader=(funcp_tr_t)0x3800;
#elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB647__)
func_ptr_t start_bootloader=(func_ptr_t)0x7800;
#elif defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB1287__)
func_ptr_t start_bootloader=(func_ptr_t)0xf000;
#endif

inline void io_init(void){
    ACSR |= (1<<ACD); //ADC off
    CLKPR = 0x80; //clock prescaler change enable
    CLKPR = 0; //set for 16MHz clock
    //if(...) start_bootloader();
    DDRB=0xff;
    PORTC=0xff;
    PORTD=0xff;

    return;
}

inline void io_task(void){
    return;
}

inline void hid_report_recv(void) { //dir=out ep=2
    uint8_t intr_state, recv_byte;
    if (usb_configuration==0) return; // if not configured
    intr_state = SREG; // take memo for restoring SREG later
    cli(); // this will affect SREG
    UENUM = 2; // select endpoint 2
    if (!(UEINTX & (1<<RWAL))){ // if not ready to transmit
        SREG = intr_state; // restore SREG ( = sei() )
        return;
    }
    while(UEINTX&(1<<RWAL)){
        for(uint8_t byct=UEBCLX; byct; --byct){
            recv_byte=UEDATX;
            PORTB=recv_byte;
        }
    }
    UEINTX = 0x2A;
    SREG = intr_state;    // sei()
    return;
}

inline void hid_report_send(void) { //dir=in ep=1
    uint8_t intr_state;
    if (usb_configuration==0) return; // if not configured
    intr_state = SREG; // take memo for restoring SREG later
    cli(); // this will affect SREG
    UENUM = 1; // select endpoint 1
    if (!(UEINTX & (1<<RWAL))){ // if not ready to transmit
        SREG = intr_state; // restore SREG ( = sei() )
        return;
    }
    UEDATX = PINC;
    UEDATX = PINC+1;
    UEDATX = PINC+2;
    UEDATX = PINC+3;
    UEDATX = PINC+4;
    UEDATX = PINC+5;
    UEDATX = PINC+6;
    UEDATX = PINC+7;
    UEINTX = 0x3A;        // send data
    SREG = intr_state;    // sei()
    return;
}

inline void usb_task(void){
    hid_report_recv();
    hid_report_send();
}

int main(void) {
    io_init();
    usb_init();
    while (usb_configuration==0); // wait for the host to set configuration.
    // Wait an extra second for the PC's operating system to load drivers
    // and do whatever it does to actually be ready for input
    _delay_ms(1000);
    while (1) {
        io_task();
        usb_task();
    }
}
