/*
 name        : Siddharth Bhalerao
 date        : 04-10-25
 description : ECU3 logic ? Receives CAN messages from ECU1 and ECU2,
               extracts and displays RPM, speed, gear, and indicator status
               on the character LCD. Indicator symbols (left, right, hazard)
               are blinked on the display based on the received flag.
 */

#include <xc.h>
#include <stdio.h>
#include "clcd.h"
#include "can.h"

void main(void) {
    uint16_t msg_id;
    uint8_t len;
    unsigned char data[8];
    unsigned int indicator = 0;
    unsigned int blink_state = 0;
    unsigned long int delay = 0;
    unsigned int RPM = 0;
    unsigned char RPM_char[5];
    init_clcd();
    init_can();
    TRISB = 0X08;
    PORTB = 0X00;

    // Print header on first line
    clcd_print("RPM SPD GEAR IND", LINE1(0));

    while (1) {
        // Wait and receive CAN message
        can_receive(&msg_id, data, &len);

        switch (msg_id) {
            case RPM_MSG_ID:
                data[4] = '\0';

                clcd_print(data, LINE2(0)); // RPM at pos 0
                break;

            case SPEED_MSG_ID:
                data[2] = '\0';
                clcd_print(data, LINE2(5)); // Speed at pos 5
                break;

            case GEAR_MSG_ID:
                data[1] = '\0';
                clcd_print(data, LINE2(8)); // Gear at pos 8
                break;

            case INDICATOR_MSG_ID:
                indicator = data[0];

                //                clcd_print(data, LINE2(13));  // Indicator at pos 13
                //                break;
        }

        if (++delay >= 1000) {
            delay = 0;
//            RB0 = 0;
//            RB1 = 0;
//            RB6 = 0;
//            RB7 = 0;
            blink_state = !blink_state;
        }
        if (indicator == 1) {
            if (blink_state) {
                clcd_print("==>", LINE2(13));
                RB0 = 1;
                RB1 = 1;
            } else {
                clcd_print("   ", LINE2(13));
                RB0 = 0;
                RB1 = 0;
            }

        } else if (indicator == 2) {
            if (blink_state) {
                clcd_print("<==", LINE2(13));
                RB6 = 1;
                RB7 = 1;
            } else {
                clcd_print("   ", LINE2(13));
                RB6 = 0;
                RB7 = 0;
            }
        } else if (indicator == 3) {
            if (blink_state) {
                clcd_print("<=>", LINE2(13));
                RB0 = 1;
                RB1 = 1;
                RB6 = 1;
                RB7 = 1;

            } else {
                clcd_print("   ", LINE2(13));
                RB0 = 0;
                RB1 = 0;
                RB6 = 0;
                RB7 = 0;
            }
        } else {
            clcd_print("   ", LINE2(13));
            RB0 = 0;
            RB1 = 0;
            RB6 = 0;
            RB7 = 0;
        }

    }
}
