/* 
 * File:   motor.h
 * Author: carlb
 *
 * Created on August 26, 2021, 1:31 PM
 */

#ifndef MOTOR_H
#define	MOTOR_H

#ifdef	__cplusplus
extern "C" {
#endif

void motor_init(void);
void motor_handler(void);
void step_handler(bool direction);
void speed_handler(void);
void signal_handler(void);
void hall_handler(void);

#define LED_OFF_MODE 0x0U
#define LED_SQR_MODE 0x2U
#define LED_PWM_MODE 0x4U
#define LED_POS_MODE 0x8U
#define LED_BUZ_MODE 0x10U
#define LED_EXT_MODE 0x20U


#define INIT_SPEED 2700U

#ifdef	__cplusplus
}
#endif

#endif	/* MOTOR_H */

