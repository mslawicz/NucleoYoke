/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.c
  * @version        : v1.0_Cube
  * @brief          : USB Device Custom HID interface file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_custom_hid_if.h"

/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t dataBuffer[CUSTOM_HID_EPOUT_SIZE];
/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @addtogroup USBD_CUSTOM_HID
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions USBD_CUSTOM_HID_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Defines USBD_CUSTOM_HID_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Macros USBD_CUSTOM_HID_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Variables USBD_CUSTOM_HID_Private_Variables
  * @brief Private variables.
  * @{
  */

/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
        {
                0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
                0x09, 0x04,                    // USAGE (Joystick)
                0xa1, 0x01,                    // COLLECTION (Application)
                0x05, 0x02,                    //   USAGE_PAGE (Simulation Controls)
                0xa1, 0x00,                    //   COLLECTION (Physical)
                0x09, 0xba,                    //     USAGE (Rudder)
                0x09, 0xbb,                    //     USAGE (Throttle)
                0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
                0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
                0x75, 0x08,                    //     REPORT_SIZE (8)
                0x95, 0x02,                    //     REPORT_COUNT (2)
                0x81, 0x02,                    //     INPUT (Data,Var,Abs)
                0xc0,                          //   END_COLLECTION
                0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
                0x09, 0x01,                    //   USAGE (Pointer)
                0xa1, 0x00,                    //   COLLECTION (Physical)
                0x09, 0x30,                    //     USAGE (X)
                0x09, 0x31,                    //     USAGE (Y)
                0x75, 0x08,                    //     REPORT_SIZE (8)
                0x95, 0x02,                    //     REPORT_COUNT (2)
                0x81, 0x02,                    //     INPUT (Data,Var,Abs)
                0xc0,                          //   END_COLLECTION
                0x09, 0x39,                    //   USAGE (Hat switch)
                0x15, 0x01,                    //   LOGICAL_MINIMUM (1)
                0x25, 0x08,                    //   LOGICAL_MAXIMUM (8)
                0x35, 0x00,                    //   PHYSICAL_MINIMUM (0)
                0x46, 0x3b, 0x01,              //   PHYSICAL_MAXIMUM (315)
                0x65, 0x14,                    //   UNIT (Eng Rot:Angular Pos)
                0x75, 0x04,                    //   REPORT_SIZE (4)
                0x95, 0x01,                    //   REPORT_COUNT (1)
                0x81, 0x42,                    //   INPUT (Data,Var,Abs, Null)
                0x75, 0x04,                    //   REPORT_SIZE (4)
                0x95, 0x01,                    //   REPORT_COUNT (1)
                0x81, 0x41,                    //   INPUT (Cnst,Ary,Abs,Null)
                0x05, 0x09,                    //   USAGE_PAGE (Button)
                0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
                0x29, 0x20,                    //   USAGE_MAXIMUM (Button 32)
                0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
                0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
                0x75, 0x01,                    //   REPORT_SIZE (1)
                0x95, 0x20,                    //   REPORT_COUNT (32)
                0x55, 0x00,                    //   UNIT_EXPONENT (0)
                0x65, 0x00,                    //   UNIT (None)
                0x81, 0x02,                    //   INPUT (Data,Var,Abs)
                0xc0                           // END_COLLECTION
        };

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
  * @brief Public variables.
  * @{
  */

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t* buffer);

/**
  * @}
  */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
  CUSTOM_HID_ReportDesc_FS,
  CUSTOM_HID_Init_FS,
  CUSTOM_HID_DeInit_FS,
  CUSTOM_HID_OutEvent_FS
};

/** @defgroup USBD_CUSTOM_HID_Private_Functions USBD_CUSTOM_HID_Private_Functions
  * @brief Private functions.
  * @{
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  DeInitializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_FS(void)
{
  /* USER CODE BEGIN 5 */
  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Manage the CUSTOM HID class events
  * @param  event_idx: Event index
  * @param  state: Event state
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t* buffer)
{
  /* USER CODE BEGIN 6 */
    //handle host sent data here
    return (USBD_OK);
  /* USER CODE END 6 */
}

/* USER CODE BEGIN 7 */
/**
  * @brief  Send the report to the Host
  * @param  report: The report to be sent
  * @param  len: The report length
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
/*
static int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len)
{
  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
}
*/
/* USER CODE END 7 */

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

