/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @author  Ac6
  * @version V1.0
  * @date    02-Feb-2015
  * @brief   Default Interrupt Service Routines.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "UART.h"
#include "USB.h"
#include "I2C.h"
#include "SPI.h"
#include "ADC.h"
#include "System.h" //XXX

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1)
  {

  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  while (1)
  {

  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  while (1)
  {

  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  while (1)
  {

  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{

}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{

}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{

}

/**
  * @brief  This function handles SysTick Handler, but only if no RTOS defines it.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    HAL_IncTick();
    //HAL_SYSTICK_IRQHandler();
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 stream0 global interrupt.
  * it is called after completed I2C reception
  */
void DMA1_Stream0_IRQHandler(void)
{
    HAL_DMA_IRQHandler(I2cBus::pI2c1->getDmaRxHandle());
}

/**
  * @brief This function handles DMA1 stream1 global interrupt.
  * it is called two bytes before completed I2C transmission
  */
void DMA1_Stream1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(I2cBus::pI2c1->getDmaTxHandle());
}

/**
  * @brief This function handles DMA1 stream2 global interrupt.
  * it is called after completed I2C reception
  */
void DMA1_Stream2_IRQHandler(void)
{
    HAL_DMA_IRQHandler(I2cBus::pI2c2->getDmaRxHandle());
}

/**
  * @brief This function handles DMA1 stream5 global interrupt.
  * it is called after completed SPI2 transmission
  */
void DMA1_Stream4_IRQHandler(void)
{
    HAL_DMA_IRQHandler(SpiBus::pSpi2->getDmaTxHandle());
}

/**
  * @brief This function handles DMA1 stream7 global interrupt.
  * it is called two bytes before completed I2C transmission
  */
void DMA1_Stream7_IRQHandler(void)
{
    HAL_DMA_IRQHandler(I2cBus::pI2c2->getDmaTxHandle());
}

/**
  * @brief This function handles DMA2 stream0 global interrupt.
  */
void DMA2_Stream0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(ADConverter::pADC1->getDmaHandle());
}

/**
  * @brief This function handles DMA1 stream5 global interrupt.
  * it is called after completed SPI4 transmission
  */
void DMA2_Stream1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(SpiBus::pSpi4->getDmaTxHandle());
}

/**
  * @brief This function handles I2C1 event interrupt.
  * in DMA mode it is called after completed transmission (not reception!)
  */
void I2C1_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(I2cBus::pI2c1->getHandle());
}

/**
  * @brief This function handles I2C1 error interrupt.
  */
void I2C1_ER_IRQHandler(void)
{
    System::getInstance().getConsole()->sendMessage(Severity::Info, LogChannel::LC_I2C, "I2C1 error interrupt");
    HAL_I2C_ER_IRQHandler(I2cBus::pI2c1->getHandle());
}

/**
  * @brief This function handles I2C2 event interrupt.
  * in DMA mode it is called after completed transmission (not reception!)
  */
void I2C2_EV_IRQHandler(void)
{
    HAL_I2C_EV_IRQHandler(I2cBus::pI2c2->getHandle());
}

/**
  * @brief This function handles I2C2 error interrupt.
  */
void I2C2_ER_IRQHandler(void)
{
    System::getInstance().getConsole()->sendMessage(Severity::Info, LogChannel::LC_I2C, "I2C2 error interrupt");
    HAL_I2C_ER_IRQHandler(I2cBus::pI2c2->getHandle());
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  HAL_UART_IRQHandler(UART::pUSART3->getUartHandle());
}

/**
  * @brief  This function handles USB-On-The-Go FS global interrupt request.
  * @param  None
  * @retval None
  */
void OTG_FS_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
