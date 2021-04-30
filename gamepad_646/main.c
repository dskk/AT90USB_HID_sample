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
    asm volatile (
        "out %1, %0"      "\n\t"
        "out %1, %0"      "\n\t"
        :
        : "r" ((uint8_t) 1 << JTD), "i" (_SFR_IO_ADDR(MCUCR))
    );// Disable JTAG so as to use PORTF bit4-7 as I/O pins.
    CLKPR = 0x80; //clock prescaler change enable
    CLKPR = 0; //set for 16MHz clock
    //if(...) start_bootloader();
    PORTA=0xff;
    PORTB=0xff;
    PORTC=0xff;
    PORTD=0xff;
    PORTE=0xff;
    PORTF=0xff;
    return;
}

inline void io_task(void){
    return;
}

inline void hid_report_recv(void) { //dir=out ep=2
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
    UEDATX = PINA;        //signed char X_AXIS
    UEDATX = PINB;        //signed char Y_AXIS
    UEDATX = PINC;        //unsigned char button1-8
    UEDATX = PIND;        //unsigned char button9-16
    UEINTX = 0x3A;     // send data
    SREG = intr_state; // sei()
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
