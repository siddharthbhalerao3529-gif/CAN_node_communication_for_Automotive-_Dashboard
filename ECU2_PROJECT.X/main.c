/*
 name        : Siddharth Bhalerao
 date        : 04-10-25
 description : ECU2 logic ? Reads analog input from ADC channel 4 to calculate
               vehicle speed, displays the current speed and gear on the SSD,
               reads keypad inputs to shift gears (up or down), and transmits
               both speed and gear information over the CAN bus to other ECUs.
 */



#include <xc.h>
#include <stdio.h>

#include "adc.h"
#include "clcd.h"
#include "digital_keypad.h"
#include "can.h"
#include "ssd_display.h"

#define _XTAL_FREQ 20000000

static unsigned long int count = 0;
static unsigned char ssd[MAX_SSD_CNT];
static unsigned char digit[] = {
    ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, DOT, G
};
unsigned char *gear_p[] = {"N", "1", "2", "3", "4", "5", "R"};
unsigned char data[8]; // CAN transmit buffer

void init_config() {

    init_adc();
    init_ssd_control();
    init_digital_keypad();
    init_can();
}

void main(void) {
    unsigned short adc_value;
    int gear_pos = 0;
    int prev_pos = 0;
    unsigned char gear = 0;

    unsigned int speed = 0; //  added
    unsigned int pre_speed = 0;
    unsigned char key;
    init_config();

    while (1) {
        adc_value = read_adc(CHANNEL4);
        //        speed =  ((adc_value * 99)/1023); // cast to unsigned int
        speed = (unsigned short) (adc_value / 10.33);

        if (pre_speed != speed) {
            pre_speed = speed;
            sprintf(data, "%u", speed);
            can_transmit(SPEED_MSG_ID, data, 2);
        }

        key = read_digital_keypad(STATE_CHANGE);

        if (key == SWITCH1 && gear < 6) {
            gear_pos++;
            gear++;
        } else if (key == SWITCH2 && gear > 0) {
            gear--;
            gear_pos--;
        }

        ssd[1] = digit[speed % 10]; // Units
        ssd[0] = digit[(speed / 10) % 10]; // Tens
        if (++count >= 200) // Reset count periodically
            count = 0;

        if (count < 100)
            ssd[2] = digit[11] | 1 << 4; // DP on
        else
            ssd[2] = digit[11]; // DP off



        // G
        ssd[3] = digit[gear]; // Gear

        //        display(ssd);

        /* --- Transmit over CAN --- */

        if (prev_pos != gear_pos) {
            prev_pos = gear_pos;
            can_transmit(GEAR_MSG_ID, (unsigned char *) gear_p[gear_pos], 1);
        }
        __delay_ms(200);

    }
}
