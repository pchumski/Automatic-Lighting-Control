/*
 * led.h
 *
 *  Created on: Jan 15, 2022
 *      Author: Konstanty
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#define led_Timer LED_HandleTypeDef*

typedef struct{
	float R;
	float G;
	float B;
	float duty_R;
	float duty_G;
	float duty_B;
}LED_HandleTypeDef;


void ColorsGenerator(LED_HandleTypeDef* led, float rgb_duty);



#endif /* INC_LED_H_ */
