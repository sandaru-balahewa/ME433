/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "string.h"
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

COM_InitTypeDef BspCOMInit;
__IO uint32_t BspButtonState = BUTTON_RELEASED;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern UART_HandleTypeDef hcom_uart[COM1];
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

	// UART Receive buffer and index
	char uart_buffer[1000];
	int uart_index = 0;

	// Receive from computer USB
	char usb_buffer[1000];
	int usb_index = 0;

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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Initialize leds */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_BLUE);

  /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */
  BspCOMInit.BaudRate   = 115200;
  BspCOMInit.WordLength = COM_WORDLENGTH_8B;
  BspCOMInit.StopBits   = COM_STOPBITS_1;
  BspCOMInit.Parity     = COM_PARITY_NONE;
  BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;
  if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN BSP */

  /* -- Sample board code to send message over COM1 port ---- */
  printf("Welcome to STM32 world !\n\r");

  /* -- Sample board code to switch on leds ---- */
  BSP_LED_On(LED_GREEN);
  BSP_LED_On(LED_BLUE);

  /* USER CODE END BSP */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

//    // Receive from Pico (UART)
//	printf("Hello, loop is running\n");

//	  char c = getchar();
//	  HAL_Delay(10);
//
//	  printf("Got: %c\r\n", c);
//	  HAL_Delay(100);

	  //My code

//	  //	 Receive from USB
//	char usb_rx_char;
//	if (HAL_UART_Receive(&hcom_uart[COM1], (uint8_t*) &usb_rx_char, 1, 100) == HAL_OK){
//		// End of line?
//		if (usb_rx_char == '\n'){
//			usb_buffer[usb_index] = '\0';
//
//			// Send to Pico over UART
//			HAL_UART_Transmit(&huart1, (uint8_t*) usb_buffer, strlen(usb_buffer), 10);
//
//			// Send newline
//			char newline[] = "\n";
//
//			HAL_UART_Transmit(&huart1, (uint8_t*) newline, 1, 1);
//
//			//Testing
//			printf("%s\n", usb_buffer);
//
//			usb_index = 0;
//
//		}
//		else{
//			usb_buffer[usb_index] = usb_rx_char;
//
//			// increment index
//			usb_index++;
//
//			// prevent overflow
//			if (usb_index == 1000){
//				usb_index = 0;
//			}
//		}
//	}
//
//    char rx_char;
//
//    if (HAL_UART_Receive(&huart1, (uint8_t*) &rx_char, 1, 1) == HAL_OK){
//    	if (rx_char == '\n'){
//    		// got the message
//    		// Add a null terminator to make a valid C string
//    		uart_buffer[uart_index] = '\0';
//
//    		// print to STM32 virtual COM port
//    		printf("Received: %s\n", uart_buffer);
//
//    		// Echo message back to pico
////    		HAL_UART_Transmit(&huart1, (uint8_t*) uart_buffer, strlen(uart_buffer), 10);
//
//    		// Send newline
////    		char newline[] = "\n";
//
////    		HAL_UART_Transmit(&huart1, (uint8_t*) newline, 1, 1);
//
//    		// reset buffer index
//    		uart_index = 0;
//
//    	}
//    	else{
//    		// store char in buffer
//    		uart_buffer[uart_index] = rx_char;
//
//    		// increment index
//    		uart_index++;
//
//    		// prevent buffer overflow
//    		if (uart_index == 1000){
//    			uart_index = 0;
//    		}
//    	}
//    }

	  //my code end

	  //claude

  // Read from USB
	  char usb_rx_char;
	  if (HAL_UART_Receive(&hcom_uart[COM1], (uint8_t*)&usb_rx_char, 1, 100) == HAL_OK)
	  {
		  if (usb_rx_char == '\n')
		  {
			  usb_buffer[usb_index] = '\0';

			  // Send to Pico over UART
			  HAL_UART_Transmit(&huart1, (uint8_t*)usb_buffer, strlen(usb_buffer), 100);
			  HAL_UART_Transmit(&huart1, (uint8_t*)"\n", 1, 10);

			  // Print confirmation (only after full message received)
			  printf("Sent to Pico: %s\r\n", usb_buffer);

			  usb_index = 0;
		  }
		  else
		  {
			  if (usb_index < 999) usb_buffer[usb_index++] = usb_rx_char;
		  }
	  }

	  // Read from Pico
	  char rx_char;
	  if (HAL_UART_Receive(&huart1, (uint8_t*)&rx_char, 1, 1) == HAL_OK)
	  {
		  if (rx_char == '\n')
		  {
			  uart_buffer[uart_index] = '\0';
			  printf("From Pico: %s\r\n", uart_buffer);
			  uart_index = 0;
		  }
		  else
		  {
			  if (uart_index < 999) uart_buffer[uart_index++] = rx_char;
		  }
	  }



//	char usb_rx_char;
//	HAL_StatusTypeDef status = HAL_UART_Receive(&hcom_uart[COM1], (uint8_t*)&usb_rx_char, 1, 1);
//
//	if (status == HAL_OK)
//	{
//		printf("GOT: %c (%d)\r\n", usb_rx_char, usb_rx_char);
//	}
//	else if (status == HAL_TIMEOUT)
//	{
//		// nothing received, keep looping
//	}
//	else
//	{
//		// HAL_ERROR or HAL_BUSY
//		printf("UART error: %d\r\n", status);
//	}

//	HAL_UART_Transmit(&huart1, (uint8_t*)"Sandaru", 7, 100);
//	HAL_Delay(10);
//	// Send newline
//	char newline[] = "\n";
//
//	HAL_UART_Transmit(&huart1, (uint8_t*) newline, 1, 10);
//	HAL_Delay(10);
//	printf("message sent");


    // --- Read from USB (computer) using scanf, same path as printf ---
//	char usb_line[100];
//	if (scanf("%s", usb_line) == 1) {
//		// Forward to Pico
//		HAL_UART_Transmit(&huart1, (uint8_t*)usb_line, strlen(usb_line), 100);
//		char newline[] = "\n";
//		HAL_UART_Transmit(&huart1, (uint8_t*)newline, 1, 10);
//		printf("Got a line\n");
//		printf("%s\r\n", usb_line);
//	}



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

  __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_0);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV4;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

int __io_getchar(void)
{
    uint8_t ch;
    HAL_UART_Receive(&hcom_uart[COM1], &ch, 1, HAL_MAX_DELAY);
    return ch;
}
/* USER CODE END 4 */

/**
  * @brief  BSP Push Button callback
  * @param  Button Specifies the pressed button
  * @retval None
  */
void BSP_PB_Callback(Button_TypeDef Button)
{
  if (Button == BUTTON_USER)
  {
    BspButtonState = BUTTON_PRESSED;
  }
}

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
#ifdef USE_FULL_ASSERT
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
