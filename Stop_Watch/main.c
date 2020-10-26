/*Nathan Orloff
 * Stop watch project, when records time passed since
 * start button is pressed until stop button is pressed
 * and displays the total time to LCD*/


#include "DCOset.h"
#include "LCD.h"
#include "msp.h"
#include <stdint.h>
#include <stdio.h>

#define FREQ_1_5_MHz  0
#define FREQ_3_MHz  1
#define FREQ_6_MHz  2
#define FREQ_12_MHz  3
#define FREQ_24_MHz  4

#define START BIT0
#define CLEAR BIT1


void timer_init(void);
void start_clear_interrupt_init(void);

void set_DCO(int dco);
void delay(int num);

void LCD_nibble_write(unsigned char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void Clear_LCD();
void Home_LCD();
void Line_two_LCD();
void Write_char_LCD(unsigned char data);
void delayMs(int n);
void Write_string_LCD(unsigned char* data);

int cnt = 0; //when cnt == 100, 1 second had passed
int time_count = 0; //holds current seconds passed
int stop_val = 0; //holds ccr count when stop button is pressed
float sec_val = 0; //holds stop_val converted to seconds
float final_val = 0; //holds total time
int on = 0; //stop watch running or not

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	set_DCO(FREQ_1_5_MHz); //set DCO and SMCLK to 1.5MHz

	start_clear_interrupt_init(); //initialize P5 interrupts
	timer_init(); //Initialize Timer
	LCD_init(); //initialize LCD
	Clear_LCD(); //Clear the Display
	Home_LCD(); //set cursor at first position of first line


	while(1);
}



/*initializes Timer_A and enables its interrupts*/
void timer_init(void){
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // SMCLK, UP Count
    //0.01 second delay
    TIMER_A0->CCR[4] = 1875;
    TIMER_A0->CTL |= TIMER_A_CTL_ID_3;

    /*capture mode setup 4*/
    TIMER_A0->CCTL[4] |= TIMER_A_CCTLN_CM_1; // rising
    TIMER_A0->CCTL[4] |= TIMER_A_CCTLN_SCS; //synchronous capture
    TIMER_A0->CCTL[4] |= TIMER_A_CCTLN_CAP; //capture mode
    TIMER_A0->CCTL[4] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
    TIMER_A0->CCTL[4] |= TIMER_A_CCTLN_CCIE; //enable interrupt
    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31); // set NVIC interrupt
    __enable_irq();
}

/*handles timer interrupts, includes capture and display final time to LCD when stop button is pressed,
 * when the ccr count == 1875 1 second has passed increments time_count and
 * writes the current time to the LCD*/
void TA0_N_IRQHandler(void) {
    if (TIMER_A0->CCTL[4] & TIMER_A_CCTLN_CCIFG) // CCR4 interrupt
    {
        TIMER_A0->CCTL[4] &= ~TIMER_A_CCTLN_CCIFG; // clear CCR4 interrupt

        if((P2->IN & BIT7) && on){//if stop button pressed
            stop_val = TIMER_A0->CCR[4]; //get ccr value
            on = 0;//turn stop watch off
            sec_val = (stop_val / 1875) * 60;//convert ccr to secs
            final_val = time_count + sec_val;//get final time
            //convert time to string
            char time_final[10];
            sprintf(time_final, "%f", final_val);
            //print time to LCD
            Clear_LCD(); //Clear the Display
            Home_LCD();
            Write_char_LCD(time_final[0]);
            Write_char_LCD(time_final[1]);
            Write_char_LCD(time_final[2]);
            Write_char_LCD(time_final[3]);
            Write_char_LCD(time_final[4]);
            Write_char_LCD(' ');
            Write_char_LCD('s');
            Write_char_LCD('e');
            Write_char_LCD('c');

        }else if(on && (cnt == 100)){ //if cnt == 100, 1 second has passed, update the LCD
            cnt = 0;//set cnt to 0
            time_count++; //increase time_count
            //convert time to a string
            char time[10];
            sprintf(time, "%i", time_count);
            Clear_LCD(); //Clear the Display
            Home_LCD();
            //writes out current time on stopwatch
            if(time_count < 10){
                Write_char_LCD(time[0]);
                Write_char_LCD(' ');
                Write_char_LCD('s');
                Write_char_LCD('e');
                Write_char_LCD('c');
            }else{
                Write_char_LCD(time[0]);
                Write_char_LCD(time[1]);
                Write_char_LCD(' ');
                Write_char_LCD('s');
                Write_char_LCD('e');
                Write_char_LCD('c');
            }

        }else{
            cnt++; //increase cnt
        }
    }



}


/*initializes the P5 interrupts*/
void start_clear_interrupt_init(void){
       //P3 pins is connected to P5 pins with a button. will drive P5 pins high when button pressed
       P3->DIR |= (BIT5 | BIT6);  // make the column pins outputs
       P3->OUT |= (BIT5 | BIT6);  // drive all column pins high
       P5->IES &= ~(START | CLEAR);        // Interrupt on low-to-high transition

       P5->IFG &= ~(START | CLEAR);       // Clear all P5 interrupt flags
       P5->IE  |= START | CLEAR;        // Enable interrupt

       // Enable Port 5 interrupt on the NVIC
       NVIC->ISER[1] = 1 << ((PORT5_IRQn) & 31);

       // Enable global interrupt
       __enable_irq();
}

/*handles interrupts from the Port 5 pins.
 * starts or clears the stop watch*/
void PORT5_IRQHandler(void)
{


   if((P5->IN & START) && ~on){ //if start button pressed and not on
       TIMER_A0->CTL |= BIT2; //reset timer
       on = 1; //turn on
       cnt = 0; //set count to 0
       time_count = 0; //set timer_count to
       Clear_LCD(); //Clear the Display
       Home_LCD(); //set cursor at first position of first line
   }
   if((P5->IN & CLEAR) && ~on){ //if clear button pressed and not on
       Clear_LCD(); //Clear the Display
       Home_LCD(); //set cursor at first position of first line
   }


    //prepares for next interrupt

   P5->IFG &= ~(START | CLEAR); //clear interrupt flags



}
