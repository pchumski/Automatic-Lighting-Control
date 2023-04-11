/*
 * led.c
 *
 *  Created on: Jan 15, 2022
 *      Author: Konstanty
 */
#include "led.h"


void ColorsGenerator(LED_HandleTypeDef* led, float rgb_duty){
	led->duty_B = rgb_duty*led->B;
	led->duty_G = rgb_duty*led->G;
	led->duty_R = rgb_duty*led->R;
};
