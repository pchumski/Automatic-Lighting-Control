/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "bh1750.h"
#include "regulator.h"
#include "led.h"
#include "lcd.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define START 1
#define STOP 0

int akcja = 2;
int pulse =0;

float wartosc_zadana = 130.0f;

float duty = 0.0f;
BH1750_HandleTypeDef hbh1750_1 = {
.I2C = &hi2c1, .Address = BH1750_ADDRESS_L, .Timeout = 0xffff};
regulator_Handle_TypeDef reg_I = {
   .Ki = 50.0f,.Kd=0.0f,.Kp = 0.0f, .Ts = 0.0007f, .e_int = 0.0f,.e_der = 0.0f,.e_prev = 0.0f, .limitdown = 0.0f, .limitup=100.0f};
LED_HandleTypeDef led = {
  .R = 1.0f, .G = 0.0f, .B = 1.0f, .duty_R=0.0f, .duty_G=0.0f, .duty_B=0.0f};

LCD_HandleTypeDef moj_lcd = {
		.yref = "0", .Blue = "0", .Green = "0", .Red = "0", .message1 = "", .u = "", .y = "",.message2 = ""};
float LightIntensity = -0.1;
int light = -1;
int wartzad = 0;
int u_dutyi = 0;
char komunikat[20];

float led_R;
float led_G;
float led_B;
int pulseR=0,pulseG=0,pulseB=0;

char on[2];
char kolor[12];
char errors[50] = "Zly wzor lub wartosc poza zakresem!";
char yr[4];
char wiadomosc[23];

int lcd_yr = 0;
int lcd_light = -1;
int lcd_index = 0;
_Bool button_state;

// Odbieranie wiadomosci z aplikacji badz terminala
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	if(huart->Instance == USART3)
	{

		if(wiadomosc[0] == 'K' && wiadomosc[1] == ':' && wiadomosc[2] == 'R' && wiadomosc[6] == 'G' && wiadomosc[10] == 'B' && wiadomosc[14] == ',' && wiadomosc[15] == 'Y' && wiadomosc[16] == ':' && wiadomosc[20] == ',' && wiadomosc[21] == 'O')
		{

			if(wiadomosc[22] == 'N')
			{
			sscanf(wiadomosc,"K:R%dG%dB%d,Y:%d,ON",&pulseR,&pulseG,&pulseB,&wartzad);
			akcja = 1;
			if(pulseR >=0 && pulseR <=100)
			{
				led.R = (float)(pulseR/100.0f);

			}
			if(pulseG >=0 && pulseG <=100)
			{
				led.G = (float)(pulseG/100.0f);

			}
			if(pulseB >=0 && pulseB <=100)
			{
				led.B = (float)(pulseB/100.0f);

			}
			wartosc_zadana = (float)(wartzad*1.0f);


			lcd_yr = wartzad;

			}
			else if(wiadomosc[22] == 'F')
			{
				akcja = 0;

			}
			else
			{
				HAL_UART_Transmit(huart, (uint8_t*)errors, strlen(errors), 1000);
			}
		}
		HAL_UART_Receive_IT(&huart3, (uint8_t*)wiadomosc, 23);

	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM2)
	{
		// Zczytywanie danych z czujnika
		LightIntensity = BH1750_ReadLux(&hbh1750_1);
		light = LightIntensity*100;
		lcd_light = LightIntensity*100;
		u_dutyi = duty*100;

		if(akcja == START)
		{
//		sprintf(komunikat,"%d.%d\r\n",light/100,light%100);
//		HAL_UART_Transmit(&huart3, komunikat, strlen(komunikat), 150);

		// Obliczanie sygnalu sterujacego
		duty = Reg_SignalControl(&reg_I, wartosc_zadana, LightIntensity);
		ColorsGenerator(&led, duty);
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,(uint32_t)((led.duty_R)*10));
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,(uint32_t)((led.duty_G)*10));
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,(uint32_t)((led.duty_B)*10));
		}
		else if(akcja == STOP)
		{
			// Zatrzymanie pracy ukladu
			led.duty_R = 0.0f;
			led.duty_G = 0.0f;
			led.duty_B = 0.0f;
			duty = 0.0f;
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,(uint32_t)((led.duty_R)*10));
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,(uint32_t)((led.duty_G)*10));
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,(uint32_t)((led.duty_B)*10));

		}
	}

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	// Wysylanie wiadomosci na lcd (aktualnych z listy)
  if(GPIO_Pin == USER_Btn_Pin)
  {
	  sprintf(moj_lcd.y,"Y=%d.%d[lux]",lcd_light/100,lcd_light%100);
	  sprintf(moj_lcd.yref,"Yref=%d[lux]",lcd_yr);
	  sprintf(moj_lcd.u,"u=%d.%d[lux]",u_dutyi/100,u_dutyi%100);
	  sprintf(moj_lcd.Red,"RED=%d[%]",pulseR);
	  sprintf(moj_lcd.Green,"GREEN=%d[%]",pulseG);
	  sprintf(moj_lcd.Blue,"BLUE=%d[%]",pulseB);
	  lcd_clear();
	  send_message_to_lcd(&moj_lcd, lcd_index);
	  HAL_UART_Receive_IT(&huart3, (uint8_t*)wiadomosc, 23);
  }
  else if(GPIO_Pin == button_Pin) // Interiwanie po liscie wartosci wysiwetlanych na lcd
  {
	  if(lcd_index >=0 && lcd_index < 5)
	  {
		  lcd_index++;
	  }
	  else
	  {
		  lcd_index = 0;
	  }
	  sprintf(moj_lcd.y,"Y=%d.%d[lux]",lcd_light/100,lcd_light%100);
	  sprintf(moj_lcd.yref,"Yref=%d[lux]",lcd_yr);
	  sprintf(moj_lcd.u,"u=%d.%d[lux]",u_dutyi/100,u_dutyi%100);
	  sprintf(moj_lcd.Red,"RED=%d[%]",pulseR);
	  sprintf(moj_lcd.Green,"GREEN=%d[%]",pulseG);
	  sprintf(moj_lcd.Blue,"BLUE=%d[%]",pulseB);
	  lcd_clear();
	  send_message_to_lcd(&moj_lcd, lcd_index);
	  HAL_UART_Receive_IT(&huart3, (uint8_t*)wiadomosc, 23);
  }
}




/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  /* USER CODE BEGIN 2 */


   // Inicjalizacja czujnika cyfrowego
   uint8_t TrybPracy = BH1750_CONTINOUS_H_RES_MODE;
   BH1750_Init(&hbh1750_1, TrybPracy);

   // Wystartowanie zegarow
   HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
   HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
   HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
   HAL_TIM_Base_Start_IT(&htim2);

   // Oczekiwanie na komende UART
   HAL_UART_Receive_IT(&huart3, (uint8_t*)wiadomosc, 23);

   //Inicjalizacja LCD
   lcd_init();
   lcd_send_string("Witamy");
   lcd_put_cur(1, 0);
   lcd_send_string("LED RGB");


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_I2C2;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
