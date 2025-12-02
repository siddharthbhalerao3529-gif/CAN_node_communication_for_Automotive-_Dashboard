#ifndef CAN_H
#define CAN_H
#include <stdint.h>
#include <stdio.h>
#define SPEED_MSG_ID 0x10
#define GEAR_MSG_ID 0x20
#define RPM_MSG_ID 0x30
#define ENG_TEMP_MSG_ID 0x40
#define INDICATOR_MSG_ID 0x50

/* Defines ECAN / ECCAN FIFO status macros */
#define F_ECAN_MODE2_FP      (CANCON & 0x0F)
#define F_ECANFIFO_0         (RXB0CONbits.RXFUL)
#define F_ECANFIFO_1         (RXB1CONbits.RXFUL)
#define F_ECANFIFO_2         (B0CONbits.RXFUL)
#define F_ECANFIFO_3         (B1CONbits.RXFUL)
#define F_ECANFIFO_4         (B2CONbits.RXFUL)
#define F_ECANFIFO_5         (B3CONbits.RXFUL)
#define F_ECANFIFO_6         (B4CONbits.RXFUL)
#define F_ECANFIFO_7         (B5CONbits.RXFUL)

/* Macro to set CAN operation mode (no waiting) */
#define CAN_SET_OPERATION_MODE_NO_WAIT(mode) \
    { \
        CANCON &= 0x1F; \
        CANCON |= (mode); \
    } 

/* Indices / offsets in a message buffer array if using a flat array */
#define EIDH   0
#define EIDL   1
#define SIDH   2
#define SIDL   3
#define DLC    4
#define D0     5
#define D1     6
#define D2     7
#define D3     8
#define D4     9
#define D5     10
#define D6     11
#define D7     12

/* Function prototypes */
void init_can(void);

void can_transmit(uint16_t msg_id, const uint8_t *data, uint8_t len);

//void can_receive(unsigned int *msg_id, unsigned char *data, unsigned char *len);


#endif  /* CAN_H */
