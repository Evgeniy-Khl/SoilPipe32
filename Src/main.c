/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
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
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "fonts.h"
#include "ssd1306.h"
#include "nrf24.h"
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
char nrf_str1[20]={0};
int8_t wait_irg=0, nrf_irq=0;
union {
    uint8_t  data [TX_PLOAD_WIDTH];
    uint16_t valT[4];
} nrf_buff;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void home_screen(void);
void NRF_displ_config(void);
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
  uint8_t temp;
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
  MX_I2C2_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  home_screen();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  SSD1306_Fill(SSD1306_COLOR_BLACK);  // Clear OLED
  while (1){
    LED_TGL;
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//    NRF_displ_config();
//    if(HAL_GPIO_ReadPin(BUT_S2_GPIO_Port, BUT_S2_Pin)==GPIO_PIN_RESET) NRF_displ_config();
    wait_irg--;      
    if(wait_irg>0){
      SSD1306_Fill(SSD1306_COLOR_BLACK);           // Clear OLED
      temp = nrf_buff.valT[0]%10;                   // дробна€ часть
      sprintf(nrf_str1,"T0=%2i,%u",nrf_buff.valT[0]/10,temp); SSD1306_GotoXY(0, 0); SSD1306_Puts(nrf_str1, &Font_7x10, SSD1306_COLOR_WHITE);
      temp = nrf_buff.valT[1]%10;                   // дробна€ часть
      sprintf(nrf_str1,"T1=%2i,%u",nrf_buff.valT[1]/10,temp); SSD1306_GotoXY(70,0); SSD1306_Puts(nrf_str1, &Font_7x10, SSD1306_COLOR_WHITE);
      temp = nrf_buff.valT[2]%10;                   // дробна€ часть
      sprintf(nrf_str1,"T2=%2i,%u",nrf_buff.valT[2]/10,temp); SSD1306_GotoXY(0, 15); SSD1306_Puts(nrf_str1, &Font_7x10, SSD1306_COLOR_WHITE);
      temp = nrf_buff.valT[3]%10;                   // дробна€ часть
      sprintf(nrf_str1,"T3=%2i,%u",nrf_buff.valT[3]/10,temp); SSD1306_GotoXY(70,15); SSD1306_Puts(nrf_str1, &Font_7x10, SSD1306_COLOR_WHITE);

      sprintf(nrf_str1,"DS=%i  TI=%i  EN=%i",nrf_buff.data[DATA_TEMPR],nrf_buff.data[DATA_TEMPR + AMOUNT_SENS],nrf_buff.data[DATA_TEMPR + AMOUNT_SENS + INTERVAL]);
      SSD1306_GotoXY(0,30); SSD1306_Puts(nrf_str1, &Font_7x10, SSD1306_COLOR_WHITE);

      sprintf(nrf_str1,"wait=%2i irq=%i",wait_irg,nrf_irq); 
      SSD1306_GotoXY(0,50); SSD1306_Puts(nrf_str1, &Font_7x10, SSD1306_COLOR_WHITE);
    }
    else {
      SSD1306_Fill(SSD1306_COLOR_BLACK);            // Clear OLED
      if(NRF_RDreg(CONFIG)!= CONFIGDATA) NRF_WRreg_byte(CONFIG,CONFIGDATA);// режим приЄмника 0x0b
      NRF_displ_config();
      temp++; wait_irg=0;
      sprintf(nrf_str1,"no signal:%3i",temp);
      SSD1306_GotoXY(10,50); SSD1306_Puts(nrf_str1, &Font_7x10, SSD1306_COLOR_WHITE);
    }
    SSD1306_UpdateScreen();
    HAL_Delay(1000);
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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
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
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
 uint8_t temp;
 if(GPIO_Pin == NRF_IRQ_Pin){
    nrf_irq++;
    temp = NRF_RDreg(STATUS);
    if(temp & 0x40){
      LED_OFF; wait_irg=10; 
      NRF_read_data(R_RX_PAYLOAD, nrf_buff.data, TX_PLOAD_WIDTH); // чтение FIFO
      NRF_WRreg_byte(STATUS, temp);             // ѕросто запишем регистр обратно, тем самым сбросив биты прерываний
    }
 } 
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
