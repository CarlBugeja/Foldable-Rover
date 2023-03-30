/*
 * Flexar Driver
 * Hardware Version: v1.4
 * Software Version: v1.0
 * www.flexar.io
 */

#include "mcc_generated_files/mcc.h"
#include "motor.h"

extern bool timer_flag;
extern bool hall_flag; 

void main(void){
    SYSTEM_Initialize(); /*Initialize the device*/
        
    INTERRUPT_GlobalInterruptEnable(); /*Enable the Global Interrupts*/
    INTERRUPT_PeripheralInterruptEnable(); /*Enable the Peripheral Interrupts*/
                 
    motor_init();
       
    while (1){            
        if(hall_flag == 1){
            hall_flag = 0;
            hall_handler();
        }
        else{
            /*Wait*/
        }
        if(timer_flag == 1){  
            timer_flag = 0; /*Clear 50us/160us timer flag*/
            motor_handler();     
            CLRWDT(); /*Clear Watchdog*/ 
        }
        else{
            /*Wait*/
        }               
    }
}
/**
 End of File
*/