#include "motor.h"
void A_speed(int speed){//[-1000,1000]
	if(speed<0){
		DL_GPIO_clearPins(MOTOR_AIN1_PORT,MOTOR_AIN1_PIN);
		DL_GPIO_setPins(MOTOR_AIN2_PORT,MOTOR_AIN2_PIN);
		speed=-speed;
	}else{
		DL_GPIO_setPins(MOTOR_AIN1_PORT,MOTOR_AIN1_PIN);
		DL_GPIO_clearPins(MOTOR_AIN2_PORT,MOTOR_AIN2_PIN);
	}
	if(speed>1000) speed=1000;
	DL_TimerA_setCaptureCompareValue(PWM_MOTOR_INST,speed,GPIO_PWM_MOTOR_C1_IDX);
}
void B_speed(int speed){
	if(speed<0){
		DL_GPIO_clearPins(MOTOR_BIN1_PORT,MOTOR_BIN1_PIN);
		DL_GPIO_setPins(MOTOR_BIN2_PORT,MOTOR_BIN2_PIN);
		speed=-speed;
	}else{
		DL_GPIO_setPins(MOTOR_BIN1_PORT,MOTOR_BIN1_PIN);
		DL_GPIO_clearPins(MOTOR_BIN2_PORT,MOTOR_BIN2_PIN);
	}
	if(speed>1000) speed=1000;
	DL_TimerA_setCaptureCompareValue(PWM_MOTOR_INST,speed,GPIO_PWM_MOTOR_C0_IDX);
}