/*Nathan Orloff
 * DCOset.c
 * C file associated with its header file
 * creates functions for changing the DCO
 *  */

#define FREQ_1_5_MHz  0
#define FREQ_3_MHz  1
#define FREQ_6_MHz  2
#define FREQ_12_MHz  3
#define FREQ_24_MHz  4
#include "msp.h"
#include <stdint.h>
#include <stdio.h>
/*sets the DCO to the specified frequency*/
void set_DCO(int dco){
    CS->KEY = CS_KEY_VAL ;                  // Unlock CS module for register access
    CS->CTL0 = 0;                           // Reset tuning parameters


    switch(dco){
        case 0: CS->CTL0 = CS_CTL0_DCORSEL_0;   // Set DCO to 1.5MHz
            break;
        case 1: CS->CTL0 = CS_CTL0_DCORSEL_1;   // Set DCO to 3MHz
            break;
        case 2: CS->CTL0 = CS_CTL0_DCORSEL_2;   // Set DCO to 6MHz
            break;
        case 3: CS->CTL0 = CS_CTL0_DCORSEL_3;   // Set DCO to 12MHz (nominal, center of 8-16MHz range)
            break;
        case 4: CS->CTL0 = CS_CTL0_DCORSEL_4;   // Set DCO to 24MHz
            break;
        default: CS->CTL0 = CS_CTL0_DCORSEL_1;   // Set DCO to 3MHz

    }


    // Select ACLK = REFO, SMCLK = MCLK = DCO
    CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
    CS->KEY = 0;                            // Lock CS module from unintended accesses
}

void delay(int num){
    int i;
    for(i = num; i>0;i--);
}
