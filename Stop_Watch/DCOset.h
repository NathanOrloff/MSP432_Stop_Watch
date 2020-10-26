/*Nathan Orloff
 * DCOset.h
 * Header file that declares all of the
 * functions of the associated C file
 * */

#include "msp.h"
#include <stdint.h>
#include <stdio.h>

#define FREQ_1_5_MHz  0
#define FREQ_3_MHz  1
#define FREQ_6_MHz  2
#define FREQ_12_MHz  3
#define FREQ_24_MHz  4

void set_DCO(int dco);
void delay(int num);
