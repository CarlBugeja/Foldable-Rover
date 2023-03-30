#include "mcc_generated_files/mcc.h"
#include "motor.h"

#define MOTOR_NUM 4U

char stringer = 0U;
bool motor_dir = 0;
uint8_t speed_prev = 0U, rx_pointer = 0U, rev_timer = 0U, StepNum = 3U;
uint16_t speed_stimer = 0U, speed_high = 0U, speed_low = 0U, speed_phigh = 0U, speed_plow = 0U, hall_value = 0U, hall_timer = 0U, step_timer = 0U, step_counts = 0U, speed_value = 999U, step_error = 0U, step_errorlast = 0U;
char rx_array[5]= {0U};


void motor_init(void){
    step_timer = 0U;
    step_counts = INIT_SPEED;
    speed_value = INIT_SPEED;
    hall_value = INIT_SPEED;
    
    TRISCbits.TRISC4 = 1;
    ANSELCbits.ANSC4 = 0;
            
    if(motor_dir == 0U){
        for(uint16_t i = 0U; i < 5000U; i++){
            __delay_us(60);  
            step_handler(0); /*Buzz the motor*/
        }
        __delay_ms(100);
        for(uint16_t i = 0U; i < 10000U; i++){
            __delay_us(20);  
            step_handler(1); /*Buzz the motor*/
        }
        __delay_ms(100);
    }
    else{
        for(uint16_t i = 0U; i < 4000U; i++){
            __delay_us(120);  
            step_handler(0); /*Buzz the motor*/
        }
        __delay_ms(100);
    }    
}

void hall_handler(void){         
    hall_value = (hall_timer/3U);//*2U(180deg)/6U(6steps)  
    hall_timer = 0U;
}

void signal_handler(void){ 
    if((PORTCbits.RC4 == 1)&&(speed_high<20U)){
        speed_high++;
    }
    else{
        if((speed_high != 0)&&(PORTCbits.RC4 == 0)){
            speed_stimer = speed_high;
            speed_high = 0U;
        }        
    }
}

void speed_handler(void){
    uint16_t speed_error = 0U;
    
    if(speed_stimer > 10U){
        speed_value = speed_stimer - 10U;
        speed_value = INIT_SPEED - (speed_value*200U); 
    }
    else{
        speed_value = 10U - speed_stimer;
        speed_value = INIT_SPEED - (speed_value*200U); 
    }      

    step_counts = speed_value;
//    Closed Loop Controller 
    //if(speed_value == hall_value){
//        /*ignore*/
//    }                
//    else if(speed_value > hall_value){
//        speed_error = ((speed_value - hall_value)/32U);
//        step_counts = step_counts + speed_error;
//    }
//    else{
//        speed_error = ((hall_value - speed_value)/32U);
//        step_counts = step_counts - speed_error;
//    } 
}

void motor_handler(void){
    step_timer++;   
    hall_timer++;
    
    if((speed_stimer >= 9U)&&(speed_stimer <= 11U)){
        /*Stand-by*/
        LATCbits.LATC1 = 0;/*PH1_L*/
        LATCbits.LATC0 = 0;/*PH2_H*/ 
        LATCbits.LATC3 = 0;/*PH3_L*/         
        LATAbits.LATA4 = 0;/*PH3_H*/
        LATAbits.LATA5 = 0;/*PH2_L*/
        LATCbits.LATC2 = 0;/*PH1_H*/
        
        step_timer = 0U;
        hall_timer = 0U;
        step_counts = INIT_SPEED;
        speed_value = INIT_SPEED;
        hall_value = INIT_SPEED;
    }
    else{
        if(speed_stimer > 11U){
            if(motor_dir == 1){
                motor_dir = 0; /*Forward Closed Loop Rotation*/
                motor_init();
            }
        }
        else{
            if(motor_dir == 0){
                motor_dir = 1; /*Reverse Rotation*/
                motor_init();
            }       
        }
        if(step_timer >= step_counts){   
            step_timer = 0U;
            speed_handler();
            step_handler(motor_dir);
        }
        else{
            /*Wait*/
        } 
    }
}

void step_handler(bool direction){   
    /*Go to next commutation step*/
    if((MOTOR_NUM == 1U)||(MOTOR_NUM == 3U)){
        if(motor_dir == 0){
            StepNum--; 
        }
        else{
            StepNum++; 
        }
    }
    else{
        if(motor_dir == 0){
            StepNum++; 
        }
        else{
            StepNum--; 
        }
    }
   
    switch (StepNum) {        
        case 1:
            /*PH1-H = HIGH, PH2-L = HIGH, PH3 - READ*/  
            LATCbits.LATC1 = 0;/*PH1_L*/
            LATCbits.LATC0 = 0;/*PH2_H*/ 
            LATCbits.LATC3 = 0;/*PH3_L*/         
            LATAbits.LATA4 = 0;/*PH3_H*/
            LATAbits.LATA5 = 1;/*PH2_L*/
            LATCbits.LATC2 = 1;/*PH1_H*/            
            if(((MOTOR_NUM == 1U)||(MOTOR_NUM == 3U))&&(motor_dir == 0)){
                 StepNum = 7;
            }
            else if(((MOTOR_NUM == 2U)||(MOTOR_NUM == 4U))&&(motor_dir == 1)){
                 StepNum = 7;
            }
            else{
                /*skip*/
            }
            break;

        case 2:
            /*PH1-H = HIGH, PH3-L = HIGH, PH2 - READ*/  
            LATCbits.LATC1 = 0;/*PH1_L*/
            LATCbits.LATC0 = 0;/*PH2_H*/        
            LATAbits.LATA4 = 0;/*PH3_H*/  
            LATAbits.LATA5 = 0;/*PH2_L*/
            LATCbits.LATC3 = 1;/*PH3_L*/
            LATCbits.LATC2 = 1;/*PH1_H*/
            break;

        case 3:
            /*PH2-H = HIGH, PH3-L = HIGH, PH1 - READ*/  
            LATCbits.LATC1 = 0;/*PH1_L*/        
            LATAbits.LATA4 = 0;/*PH3_H*/
            LATAbits.LATA5 = 0;/*PH2_L*/            
            LATCbits.LATC2 = 0;/*PH1_H*/
            LATCbits.LATC3 = 1;/*PH3_L*/
            LATCbits.LATC0 = 1;/*PH2_H*/
            break;

        case 4:
            /*PH2-H = HIGH, PH1-L = HIGH, PH3 - READ*/ 
            LATAbits.LATA4 = 0;/*PH3_H*/
            LATAbits.LATA5 = 0;/*PH2_L*/
            LATCbits.LATC2 = 0;/*PH1_H*/            
            LATCbits.LATC3 = 0;/*PH3_L*/
            LATCbits.LATC1 = 1;/*PH1_L*/
            LATCbits.LATC0 = 1;/*PH2_H*/
            break;

        case 5:
            /*PH3-H = HIGH, PH1-L = HIGH, PH2 - READ*/ 
            LATCbits.LATC3 = 0;/*PH3_L*/  
            LATAbits.LATA5 = 0;/*PH2_L*/
            LATCbits.LATC2 = 0;/*PH1_H*/            
            LATCbits.LATC0 = 0;/*PH2_H*/
            LATCbits.LATC1 = 1;/*PH1_L*/
            LATAbits.LATA4 = 1;/*PH3_H*/
            break;

        case 6:
            /*PH3-H = HIGH, PH2-L = HIGH, PH1 - READ*/ 
            LATCbits.LATC0 = 0;/*PH2_H*/ 
            LATCbits.LATC3 = 0;/*PH3_L*/   
            LATCbits.LATC2 = 0;/*PH1_H*/            
            LATCbits.LATC1 = 0;/*PH1_L*/
            LATAbits.LATA5 = 1;/*PH2_L*/
            LATAbits.LATA4 = 1;/*PH3_H*/
            
            if(((MOTOR_NUM == 1U)||(MOTOR_NUM == 3U))&&(motor_dir == 1)){
                 StepNum = 0;
            }
            else if(((MOTOR_NUM == 2U)||(MOTOR_NUM == 4U))&&(motor_dir == 0)){
                 StepNum = 0;
            }
            else{
                /*skip*/
            }
            break;
    }
}