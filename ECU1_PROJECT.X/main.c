/*
 name - siddharth bhalerao
 date - 4-10-25
 description-   ECU1 logic ? Reads RPM from ADC channel 4, displays RPM on SSD,
               reads keypad inputs to control indicator signals (left, right, hazard, off),
               toggles LEDs accordingly, and transmits RPM and indicator status
               over CAN bus to other ECUs.
 */

#include <xc.h>
#include <string.h>
#include "adc.h"
#include "digital_keypad.h"
#include "ssd_display.h"
#include "can.h"   

static unsigned int delay;
static unsigned char ssd[MAX_SSD_CNT];
static unsigned char digit[] = {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE};

void init_config(void) {
    TRISB = 0X08;
    PORTB = 0X00;
    init_digital_keypad();
    init_ssd_control();
    init_adc();
    init_can(); // CAN init
}

void main(void) {
    unsigned short adc_value;
    unsigned char key = 0;
    unsigned int flag = 0, prev_flag = 0;
    unsigned int RPM = 0;
    unsigned int prev_RPM = 0;
    unsigned char data[8];

    init_config();

    while (1) {
        /* --- Read ADC & calculate RPM --- */
        adc_value = read_adc(CHANNEL4);
        RPM = (unsigned int) (adc_value * 5.88); // Mapping to RPM
        if(RPM > 6000)
        {
            RPM = 6000;
        }
      
        sprintf(data, "%u", RPM); // stores RPM as string in data[]
        //            data[5]='\0';
        if(prev_RPM != RPM)
        {
            prev_RPM = RPM;
           can_transmit(RPM_MSG_ID, data, 4);
        }
        /* --- SSD Display --- */
        ssd[0] = digit[(RPM / 1000) % 10];
        ssd[1] = digit[(RPM / 100) % 10];
        ssd[2] = digit[(RPM / 10) % 10];
        ssd[3] = digit[RPM % 10];
        display(ssd);

        /* --- Read Keypad --- */
        key = read_digital_keypad(STATE_CHANGE);
        if (key == SWITCH1) {
            flag = 1;
        } else if (key == SWITCH2) {
            flag = 2;
        } else if (key == SWITCH3) {
            flag = 3;
        } else if (key == SWITCH4) {
            flag = 4;
        }



        /* --- LED Control Logic --- */
        if (flag != prev_flag) {
            RB0 = 0;
            RB1 = 0;
            RB6 = 0;
            RB7 = 0;
            prev_flag = flag;
            //            sprintf( data, "%u", flag); // stores flag as string in data[]
            //            data[1]='\0';
            //            can_transmit(INDICATOR_MSG_ID, &flag,1);
            can_transmit(INDICATOR_MSG_ID, &flag, 1);
        }

        if (flag == 1) {
            if (delay++ == 50) {
                RB0 = !RB0;
                RB1 = !RB1;
                delay = 0;
            }
        } else if (flag == 2) {
            if (delay++ == 50) {
                RB6 = !RB6;
                RB7 = !RB7;
                delay = 0;
            }
        } else if (flag == 3) {
            if (delay++ == 50) {
                RB0 = !RB0;
                RB1 = !RB1;
                RB6 = !RB6;
                RB7 = !RB7;
                delay = 0;
            }
        } else if (flag == 4) {
            RB0 = 0;
            RB1 = 0;
            RB6 = 0;
            RB7 = 0;
        }

        //        data[0] = flag; // Send the indicator state
        //        can_transmit(INDICATOR_MSG_ID, data, 1);
        //         for (unsigned long int delay = 5000; delay--; );
//        can_transmit(INDICATOR_MSG_ID, &flag, 1);
    }
}
