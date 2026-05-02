/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32h7xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32h7xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "interrupts.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

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
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern uint32_t HAL_GetHighResolutionTick();
extern void debugWriteInfo(const char* className, const uint32_t lineNumber, const char* data);
extern void debugWriteError(const char* className, const uint32_t lineNumber, const char* data);
extern void uartRxHandler(UART_HandleTypeDef* u, uint8_t instanceId);
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart10;
/* USER CODE BEGIN EV */
extern const char* textClassNameCInterrupts;
extern const char* textFaultHard;
extern const char* textFaultMemory;
extern const char* textFaultBus;
extern const char* textFaultUsage;
extern volatile uint16_t __interruptRisingAt[16];
extern volatile uint16_t __interruptFallingAt[16];
extern volatile uint16_t __interruptDelta[16];
extern volatile uint8_t  __interruptTriggered[16];
extern volatile uint8_t  __uartRxInterruptBuffer[12];

//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//	if (GPIO_Pin == GPIO_PIN_9)
//	{
//		uint8_t pin = 9;
//		if (HAL_GPIO_ReadPin(GPIOE, GPIO_Pin) == GPIO_PIN_SET)
//		{
//			// rising edge...
//			interruptSetRisingEdge(pin, (uint16_t)HAL_GetHighResolutionTick());
//		}
//		else
//		{
//			// falling edge...
//			interruptSetFallingEdge(pin, (uint16_t)HAL_GetHighResolutionTick());
//			interruptExternalSet(pin);
//		}
//		//debugWriteInfo("test", 0, "callback");
//	}
//}



/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler()
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
  debugWriteError(textClassNameCInterrupts, __LINE__, textFaultHard);
  FAIL();
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */
  debugWriteError(textClassNameCInterrupts, __LINE__, textFaultMemory);
  FAIL();
  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */
  debugWriteError(textClassNameCInterrupts, __LINE__, textFaultBus);
  FAIL();
  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */
  debugWriteError(textClassNameCInterrupts, __LINE__, textFaultUsage);
  FAIL();
  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32H7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h7xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */
  //NVIC_DisableIRQ(EXTI9_5_IRQn);
  /* USER CODE END EXTI9_5_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */
  //NVIC_EnableIRQ(EXTI9_5_IRQn);
  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
  //NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */
  uartRxHandler(&huart2, 2);
  //NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles UART5 global interrupt.
  */
void UART5_IRQHandler(void)
{
  /* USER CODE BEGIN UART5_IRQn 0 */
  //NVIC_DisableIRQ(UART5_IRQn);
  /* USER CODE END UART5_IRQn 0 */
  HAL_UART_IRQHandler(&huart5);
  /* USER CODE BEGIN UART5_IRQn 1 */
  uartRxHandler(&huart5, 5);
  //NVIC_EnableIRQ(UART5_IRQn);
  /* USER CODE END UART5_IRQn 1 */
}

/**
  * @brief This function handles UART7 global interrupt.
  */
void UART7_IRQHandler(void)
{
  /* USER CODE BEGIN UART7_IRQn 0 */
  //NVIC_DisableIRQ(UART7_IRQn);
  /* USER CODE END UART7_IRQn 0 */
  HAL_UART_IRQHandler(&huart7);
  /* USER CODE BEGIN UART7_IRQn 1 */
  uartRxHandler(&huart7, 7);
  //NVIC_EnableIRQ(UART7_IRQn);
  /* USER CODE END UART7_IRQn 1 */
}

/**
  * @brief This function handles USART10 global interrupt.
  */
void USART10_IRQHandler(void)
{
  /* USER CODE BEGIN USART10_IRQn 0 */
  //NVIC_DisableIRQ(USART10_IRQn);
  /* USER CODE END USART10_IRQn 0 */
  HAL_UART_IRQHandler(&huart10);
  /* USER CODE BEGIN USART10_IRQn 1 */
  uartRxHandler(&huart10, 10);
  //NVIC_EnableIRQ(USART10_IRQn);
  /* USER CODE END USART10_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
