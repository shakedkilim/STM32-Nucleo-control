/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "liquidcrystal_i2c.h"
#include <string.h> /* memset */
#pragma pack(1)
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFFER_SIZE (sizeof(RS232Message))
#define BUFFER_QUEUE_SIZE 512
#define DIR_PIN1 GPIO_PIN_1
#define DIR_PIN2 GPIO_PIN_5
#define DIR_PORT1 GPIOA
#define DIR_PORT2 GPIOA
#define STEP_PIN1 GPIO_PIN_4
#define STEP_PIN2 GPIO_PIN_6
#define STEP_PORT1 GPIOA
#define STEP_PORT2 GPIOA
#define RESPONSE_SIZE 59
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
I2C_HandleTypeDef hi2c1;

#ifdef __ICCARM__
int putchar(int c)
#else
int fputc(int c, FILE *f)
#endif
{
  uint8_t t = c;
  HAL_UART_Transmit(&huart2, &t, 1, 1000);
  return c;
}


enum Device { Motor, LED };

typedef enum{false, true} bool;

#pragma pack(push, 1)
struct RS232Message
{
    unsigned long long MessageID;
    unsigned char DeviceName;
    unsigned int MessageNum;
    int Data[3];
} RS232Message;
#pragma pack(pop)

uint8_t BufferQueue[BUFFER_QUEUE_SIZE][BUFFER_SIZE];

uint8_t volatile write_index = 0;
unsigned int PrevMsgNum = 0;
int volatile Diff;
uint8_t read_index = 0;
int Stepper1Distance;
int Stepper2Distance;
int Intensity;
int PositionDiplay = 0;
int Position1 = 0;
int Position2 = 0;
const int IntensityIntervalFactor = 100;
const int StepsIntervalFactor = 20;
const int MaxIntensity = 5000;
const int UARTTransitTimeout = 1000;
int IntensityInteval;
char Response[RESPONSE_SIZE];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
static char* itoa(int num, char* str, int base);
int abs(int x);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void MoveSteps (int Stepper1Steps, uint8_t Stepper1Direction,int Stepper2Steps, uint8_t Stepper2Direction, uint16_t Delay, long long int MsgID)
{
  HD44780_Clear();
  HD44780_SetCursor(0,1);
  HD44780_PrintStr("Direction: ");
  HD44780_SetCursor(0,0);
  if(!Stepper1Steps)
  {
    HD44780_PrintStr("Position 2: ");
    HD44780_SetCursor(11,1);
    if (Stepper2Direction == 1)
    {
      HAL_GPIO_WritePin(DIR_PORT2, DIR_PIN2, GPIO_PIN_SET);  
      HD44780_PrintStr("Left");
    }
    else
    {
      HAL_GPIO_WritePin(DIR_PORT2, DIR_PIN2, GPIO_PIN_RESET);
      HD44780_PrintStr("Right");
    }
  }
  else
  {
    HD44780_PrintStr("Position 1: ");
    HD44780_SetCursor(11,1);
    if (Stepper1Direction == 1)
    {
      HAL_GPIO_WritePin(DIR_PORT1, DIR_PIN1, GPIO_PIN_SET);  
      HD44780_PrintStr("Left");
    }
    else
    {
      HAL_GPIO_WritePin(DIR_PORT1, DIR_PIN1, GPIO_PIN_RESET);
      HD44780_PrintStr("Right");
    }
  }
  int i, j;
  for(i=0, j=0; i<Stepper1Steps || j < Stepper2Steps;)
  {
    if(j<Stepper2Steps)
      HAL_GPIO_WritePin(STEP_PORT2, STEP_PIN2, GPIO_PIN_SET);
    if(i<Stepper1Steps)
      HAL_GPIO_WritePin(STEP_PORT1, STEP_PIN1, GPIO_PIN_SET);
    HAL_Delay(Delay);
    if(j<Stepper2Steps)
    {
      HAL_GPIO_WritePin(STEP_PORT2, STEP_PIN2, GPIO_PIN_RESET);
      j++;
    }
    if(i<Stepper1Steps)
    {
      HAL_GPIO_WritePin(STEP_PORT1, STEP_PIN1, GPIO_PIN_RESET);
      i++;
    }
    HAL_Delay(Delay);
    if(Stepper1Direction == 1 || Stepper2Direction == 1)
    {
      if(!Stepper1Steps)
      {
        Position1 = (Position1 + 1) % 200;
        PositionDiplay = Position1;
      }
      else
      {
        Position2 = (Position2 + 1) % 200;
        PositionDiplay = Position2;
      }
    }
    else
    {
      if(!Stepper1Steps)
      {
        Position1 = (Position1 - 1) % 200;
        PositionDiplay = Position1;
      }
      else
      {
        Position2 = (Position2 - 1) % 200;
        PositionDiplay = Position2;
      }
    }
    char sNum[5];
    itoa(PositionDiplay,sNum,10);
    HD44780_SetCursor(12,0);
    if(!PositionDiplay)
    {
      HD44780_PrintStr("00");
    }
    else
    {
      HD44780_PrintStr(sNum);
    }
  }
  if(Stepper2Direction == 1 && Stepper2Steps > 0)
  {
    sprintf (Response, "Stepper 2 moved %d steps left.\n ID:%llu", Stepper2Steps, MsgID);
  }
  else if(Stepper2Direction == 0 && Stepper2Steps > 0)
  {
    sprintf (Response, "Stepper 2 moved %d steps right.\n ID:%llu", Stepper2Steps, MsgID);
  }
  else if(Stepper1Direction == 1 && Stepper1Steps > 0)
  {
    sprintf (Response, "Stepper 1 moved %d steps left.\n ID:%llu",Stepper1Steps, MsgID);
  }
  else if(Stepper1Direction == 0 && Stepper1Steps > 0)
  {
    sprintf (Response, "Stepper 1 moved %d steps right.\n ID:%llu",Stepper1Steps, MsgID);
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  // Enable reception for the next message
  write_index = (write_index + 1) % BUFFER_QUEUE_SIZE;
  HAL_UART_Receive_IT(&huart2, BufferQueue[write_index], BUFFER_SIZE);
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
  MX_TIM1_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  
  HAL_TIM_Base_Start(&htim3);
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
  
  HAL_UART_Receive_IT(&huart2, BufferQueue[write_index], BUFFER_SIZE);
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HD44780_Init(2);
  HD44780_Clear();
  volatile struct RS232Message Message;
  
  while (1)
  {     
     if (write_index != read_index) 
     {    
            Message = *((struct RS232Message *) BufferQueue[read_index]);
            Diff = PrevMsgNum - (Message.MessageNum - 1);
            if (Diff)
            {
              sprintf (Response, "Error. Message number is inconsistent.\n ID:%llu",Message.MessageID);
              HAL_UART_Transmit(&huart2, (uint8_t*)&Response, RESPONSE_SIZE, UARTTransitTimeout);
            }
            
            // Process the message
            enum Device DeviceName = (enum Device)Message.DeviceName;
            switch (DeviceName)
            {
              case Motor:
                  Stepper1Distance = Message.Data[0];
                  Stepper2Distance = Message.Data[1];
                  if(!Stepper1Distance && !Stepper2Distance)
                    break;
                  MoveSteps((int)abs(Stepper1Distance*StepsIntervalFactor),(uint8_t)(Stepper1Distance>0),(int)abs(Stepper2Distance*StepsIntervalFactor),(uint8_t)(Stepper2Distance>0),(uint16_t)100,Message.MessageID);
                  break;
              case LED:
                IntensityInteval = Message.Data[0] * IntensityIntervalFactor;
                if( (IntensityInteval+Intensity > MaxIntensity) || (IntensityInteval+Intensity < 0) ) // If Intensity is 0 or Maximun. As it changes only via interval amount.
                {
                  sprintf (Response, "Light intensity exceeded its' boundries Intensity:%d.\n ID:%llu",Intensity,Message.MessageID);
                  break;
                }
                Intensity += (Message.Data[0] * IntensityIntervalFactor);
                __HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, Intensity); // From 0 to 5000 by intervals of +/- IntensityFactor
                sprintf (Response, "Light intensity:%d.\n ID:%llu",Intensity,Message.MessageID);
                break;   
              default:
                break;
            }
            
            HAL_UART_Transmit(&huart2, (uint8_t*)&Response, RESPONSE_SIZE, UARTTransitTimeout);       
            
            read_index = (read_index + 1) % BUFFER_QUEUE_SIZE;
            PrevMsgNum = Message.MessageNum;
      }
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
  RCC_OscInitStruct.PLL.PLLN = 96;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 71;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 2;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 4096;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA1 PA4 PA5 PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void swap(char *a, char *b)                                                                                                                                                                       
  {
       if(!a || !b)
           return;

       char temp = *(a);
       *(a) = *(b);
       *(b) = temp;
   }

void reverse(char *str, int length) 
{ 
	int start = 0; 
	int end = length -1; 
	while (start < end) 
	{ 
		swap((str+start), (str+end)); 
		start++; 
		end--; 
	} 
} 

char* itoa(int num, char* str, int base) 
{ 
	int i = 0; 
	bool isNegative = false; 

	if (num == 0) 
	{ 
		str[i++] = '0'; 
		str[i] = '\0'; 
		return str; 
	}

	if (num < 0 && base == 10) 
	{ 
		isNegative = true;
		num = -num; 
	} 

	while (num != 0) 
	{ 
		int rem = num % base; 
		str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
		num = num/base; 
	}

	if (isNegative == true) 
		str[i++] = '-'; 

	str[i] = '\0';
	reverse(str, i); 
	return str; 
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
