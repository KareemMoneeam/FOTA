/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Node_TX_Pin GPIO_PIN_2
#define Node_TX_GPIO_Port GPIOA
#define Node_RX_Pin GPIO_PIN_3
#define Node_RX_GPIO_Port GPIOA
#define M1_P1_Pin GPIO_PIN_12
#define M1_P1_GPIO_Port GPIOB
#define M1_P2_Pin GPIO_PIN_13
#define M1_P2_GPIO_Port GPIOB
#define M2_P1_Pin GPIO_PIN_14
#define M2_P1_GPIO_Port GPIOB
#define M2_P2_Pin GPIO_PIN_15
#define M2_P2_GPIO_Port GPIOB
#define Buzzer_Pin GPIO_PIN_10
#define Buzzer_GPIO_Port GPIOA
#define Bluetooth_TX_Pin GPIO_PIN_11
#define Bluetooth_TX_GPIO_Port GPIOA
#define Bluetooth_RX_Pin GPIO_PIN_12
#define Bluetooth_RX_GPIO_Port GPIOA
#define Echo_Pin GPIO_PIN_6
#define Echo_GPIO_Port GPIOB
#define Trigger_Pin GPIO_PIN_7
#define Trigger_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
