/*
 * lcd.h
 *
 *  Created on: 20 sty 2022
 *      Author: Konstanty
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "stm32f7xx_hal.h"

void lcd_init(void);   // initialize lcd

void lcd_send_cmd(char cmd);  // send command to the lcd

void lcd_send_data(char data);  // send data to the lcd

void lcd_send_string(char *str);  // send string to the lcd

void lcd_put_cur(int row, int col);  // put cursor at the entered position row (0 or 1), col (0-15);

void lcd_clear(void);

#define lcd_type LCD_HandleTypeDef*

typedef struct{
	char u[20];
	char yref[20];
	char y[20];
	char Red[20];
	char Green[20];
	char Blue[20];
	char message1[20];
	char message2[20];
}LCD_HandleTypeDef;

void send_message_to_lcd(LCD_HandleTypeDef* mlcd,int index);




#endif /* INC_LCD_H_ */
