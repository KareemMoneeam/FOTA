/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "dcmotor.h"
#include "ultrasonic_lcfg.h" 
#include "MEM_Handler.h"
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
volatile uint8_t ultrasonic_reading_finished_flag = 0;
static volatile uint8_t is_firt_captured = 0;
static volatile uint32_t IC_Val1 = 0;
static volatile uint32_t IC_Val2 = 0;
static volatile uint32_t IC_Difference = 0;
uint8_t Jump_Flag_Packet[2] = {0};
uint16_t distance = 0;
uint8_t Report[200] = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void adjust_PWM_dutyCycle(TIM_HandleTypeDef* pwmHandle, uint32_t pwmChannel, float dutyCycle)
{

    // Calculate the new pulse width based on the duty cycle percentage
    uint32_t maxCCR = pwmHandle->Instance->ARR;
    uint32_t newCCR = (uint32_t)((dutyCycle / 100.0f) * maxCCR);

    // Update the CCR value for the specified channel
    __HAL_TIM_SET_COMPARE(pwmHandle, pwmChannel, newCCR);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	/* Capture rising edge */
	if(0 == is_firt_captured)
	{
		IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
		is_firt_captured = 1;
		__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
		ultrasonic_reading_finished_flag = 0;
	}
	/* Capture falling edge */
	else if(1 == is_firt_captured)
	{
		IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
		__HAL_TIM_SET_COUNTER(htim, 0);

		if(IC_Val2 > IC_Val1)
		{
			IC_Difference = IC_Val2 - IC_Val1;
		}
		else if(IC_Val1 > IC_Val2)
		{
			IC_Difference = (0xFFFF - IC_Val1) + IC_Val2;
		}

		distance = IC_Difference * 0.017;

		is_firt_captured = 0;
		__HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
		__HAL_TIM_DISABLE_IT(&htim4, TIM_IT_CC1);
		ultrasonic_reading_finished_flag = 1;
	}
	else{ /* Nothing */ }
}

// UART receive complete callback
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART2) {
    // Handle the received data
			Jump_Flag_Packet[0] = 0x01;
  		HAL_UART_Transmit(&huart2,(uint8_t*)&Jump_Flag_Packet,1,HAL_MAX_DELAY);	
    // Perform the jump to a specific address
    void (*jump_to_address)(void) = (void (*)(void)) 0x08000000; // Replace with your specific address
    jump_to_address();
    
    // Restart UART reception in interrupt mode
    HAL_UART_Receive_IT(&huart2, (uint8_t*)&Jump_Flag_Packet, 2);
  }
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t bt_value = 0;
	uint8_t last_bt_value = 0;
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
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
  adjust_PWM_dutyCycle(&htim3, TIM_CHANNEL_1, 50);
  HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	HAL_UART_Receive_IT(&huart2, (uint8_t*)&Jump_Flag_Packet, 2);
  while (1)
  {	 
//		HAL_UART_Receive(&huart2,(uint8_t*)&packet,2,HAL_MAX_DELAY);
//		HAL_GPIO_WritePin(Buzzer_GPIO_Port,Buzzer_Pin,GPIO_PIN_SET);
//		if( 0xFF == packet[1]){
//		packet[0] = 0x01;
//		HAL_UART_Transmit(&huart2,(uint8_t*)&packet,1,HAL_MAX_DELAY);	
//		
//		User_APP_Jump_Bootloader();
//		}
		
		// making sure ultrasonic has finished reading 
		if(ultrasonic_reading_finished_flag)
	  {
		  Ultrasonic_Get_Distance(&ultrasonic);
		 // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	  }
	  else{/* Nothing */}
		// recieve character from bluetooth 
	  HAL_UART_Receive(&huart6, (uint8_t*)&bt_value, 1, 20);


	  if(bt_value){ last_bt_value = bt_value; }
	  else{ bt_value = last_bt_value; }
    
		
		if('F' == bt_value){
			if(distance <= 20){
				Car_Stop();
				HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
			}
			else{
			HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);	
			Car_Move_Forward();
			}
		}else if('B' == bt_value){
		  Car_Move_Backward();
		}else if('R' == bt_value){
		  Car_Move_Right();
		}else if('L' == bt_value){
		  Car_Move_Left();
		}else if('S' == bt_value){
		  Car_Stop();
		}else if('M'== bt_value){
		/// something
			HAL_UART_Receive(&huart6,(uint8_t*)&Report,200,HAL_MAX_DELAY);
			if(0 != Report[0]){HAL_UART_Transmit(&huart2,(uint8_t*)&Report,200,HAL_MAX_DELAY);} 
		}
		
//		switch(bt_value){
//			case 'F':
//				Car_Move_Forward();
//			break;
//		
//			default:
//				Car_Stop();
//			 break;
//		}


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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
