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
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "DELAY/delay.h"
#include "debug/debug.h"

//uint8_t uartSendTitle[] = "\r\n======UART Send Receive Test=====\r\n";
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define ADC_DATA_LENGTH  4096   //定义采集数据长度为1000
uint32_t g_adc1_dma_data1[ADC_DATA_LENGTH];//定义adc1采集数据存放数组
uint8_t g_adc1_dma_complete_flag = 0;	//adc1数据dma采集完成标志，在dma采集完成回调函数设置
uint8_t receive_flag = 0;	

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



void HMISends(void)
{
	uint8_t sflag[]={0xFF,0xFF,0xFF};
			HAL_UART_Transmit(&huart4,sflag,sizeof(sflag),100);
			while(__HAL_UART_GET_FLAG(&huart4,UART_FLAG_TC)==RESET){};
}

void HMISendStr(uint32_t * buf)
{
  uint8_t i =0;
  while(1){
  if(buf[i]!=0){
    HAL_UART_Transmit(&huart4,(uint8_t *)buf[i],sizeof(buf[i]),1000);
    while(__HAL_UART_GET_FLAG(&huart4,UART_FLAG_TC)==RESET){};
    i++;
  }
  else 
	 return ;
  }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{	
	if(hadc->Instance==ADC1)
	{
		g_adc1_dma_complete_flag = 1;//采集完成标志
		HAL_TIM_Base_Stop_IT(&htim1);//tim1关闭
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
	uint32_t i=0;
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
  MX_UART4_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
	delay_init(480);
	
	HAL_TIM_Base_Start_IT(&htim1);//tim1开启
	HAL_ADC_Start_DMA(&hadc1,g_adc1_dma_data1,ADC_DATA_LENGTH);//ADC的dma开始采集

	float Freq = 10.0;
//	delay_ms(100);


//开起串口4接收中断
//	__HAL_UART_ENABLE_IT(&huart4, UART_IT_RXNE);
//	
//	HAL_UART_Transmit(&huart4,uartSendTitle,sizeof(uartSendTitle),1000);	//发送抬头
//	while(__HAL_UART_GET_FLAG(&huart4,UART_FLAG_TC)!=SET);		//等待发送结束

	

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if(g_adc1_dma_complete_flag == 1)
			{
				HAL_ADC_Stop_DMA(&hadc1);
		uint32_t sendbuf[100];
		int i=0;
			for(i=0;i<4096;i++)
				{
					sprintf((char*)sendbuf,"%d\r\n",g_adc1_dma_data1[i]&0x0000ffff);
					HMISendStr(sendbuf);
				}
			g_adc1_dma_complete_flag = 0;		
			memset(g_adc1_dma_data1,0,ADC_DATA_LENGTH);//清除数据
			//HAL_TIM_Base_Start_IT(&htim1);//tim1开启
			//HAL_ADC_Start_DMA(&hadc1,g_adc1_dma_data1,ADC_DATA_LENGTH);
			}
		if(receive_flag == 1)
      {
		  	HMISends();
		  	uint32_t uartstr[100];
		  	sprintf((char*)uartstr,"main.t2.txt=\"%6.4f Hz\"",Freq);
		  	HMISendStr(uartstr);
		  	Freq+=10.3f;
		  	HMISends();
		  	delay_ms(1000);
		  }
		
//		if(USART_RX_STA&0x8000)
//		{
//			uart_rx_len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
//			printf("\r\nsend data:\r\n");
//			HAL_UART_Transmit_IT(&huart3,(uint8_t*)USART_RX_BUF,len);	//发送接收到的数据
//			HAL_UART_Transmit(&huart4,(uint8_t*)USART_RX_BUF,uart_rx_len,1000);	//发送接收到的数据
//			while(__HAL_UART_GET_FLAG(&huart4,UART_FLAG_TC)!=SET);		//等待发送结束
//			USART_RX_STA=0;
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
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
