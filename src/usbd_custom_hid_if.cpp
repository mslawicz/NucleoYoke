
/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.cpp
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

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "usbd_custom_hid_if.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t dataBuffer[CUSTOM_HID_EPOUT_SIZE];


/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
    0x05, 0x01,    // USAGE_PAGE (Generic Desktop)
    0x09, 0x04,    // USAGE (Joystick)
    0xA1, 0x01,    // COLLECTION (Application)
        //Start Joystick input definition
        0x85, 0x01,    // Report ID 1
        // Define axes
        0x09, 0x01,    // USAGE (Pointer)
        0xa1, 0x00,    // COLLECTION (Physical)
            0x16, 0x01, 0xF0,    // LOGICAL_MINIMUM (-4095)
            0x26, 0xFF, 0x0F,    // LOGICAL_MAXIMUM (4095)
            0x09, 0x30,    // USAGE (X)
            0x09, 0x31,    // USAGE (Y)
            0x09, 0x32,    // USAGE (Z)
            0x75, 0x10,    // REPORT_SIZE (16)
            0x95, 0x03,    // REPORT_COUNT (3)
            0x81, 0x02,    // INPUT (Data,Var,Abs)
        0xc0,          // END_COLLECTION

        0xa1, 0x00,    // COLLECTION (Physical)
            0x15, 0x00,    // LOGICAL_MINIMUM (0)
            0x26, 0xFF, 0x00,    // LOGICAL_MAXIMUM (255)
            0x09, 0x33,    // USAGE (Rx)
            0x09, 0x34,    // USAGE (Ry)
            0x09, 0x35,    // USAGE (Rz)
            0x75, 0x08,    // REPORT_SIZE (8)
            0x95, 0x03,    // REPORT_COUNT (3)
            0x81, 0x02,    // INPUT (Data,Var,Abs)
        0xc0,          // END_COLLECTION

        //Define HAT switch
        0x09, 0x39,    // USAGE (Hat switch)
        0x15, 0x01,    // LOGICAL_MINIMUM (1)
        0x25, 0x08,    // LOGICAL_MAXIMUM (8)
        0x35, 0x00,    // PHYSICAL_MINIMUM (0)
        0x46, 0x3b, 0x01,    // PHYSICAL_MAXIMUM (315)
        0x65, 0x14,    // UNIT (Eng Rot:Angular Pos)
        0x75, 0x04,    // REPORT_SIZE (4)
        0x95, 0x01,    // REPORT_COUNT (1)
        0x81, 0x42,    // INPUT (Data,Var,Abs, Null)
        0x75, 0x04,    // REPORT_SIZE (4)
        0x95, 0x01,    // REPORT_COUNT (1)
        0x81, 0x41,    // INPUT (Cnst,Ary,Abs,Null)

        //Define buttons
        0x05, 0x09,    // USAGE_PAGE (Button)
        0x19, 0x01,    // USAGE_MINIMUM (Button 1)
        0x29, 0x20,    // USAGE_MAXIMUM (Button 32)
        0x15, 0x00,    // LOGICAL_MINIMUM (0)
        0x25, 0x01,    // LOGICAL_MAXIMUM (1)
        0x75, 0x01,    // REPORT_SIZE (1)
        0x95, 0x20,    // REPORT_COUNT (32)
        0x55, 0x00,    // UNIT_EXPONENT (0)
        0x65, 0x00,    // UNIT (None)
        0x81, 0x02,    // INPUT (Data,Var,Abs)
    0xC0,           // END_COLLECTION - application joystick
        //End Joystick Input definition

    // define keyboard
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
        0x85, 0x02,                    //   REPORT_ID (2)
        0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
        0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
        0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
        0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
        0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
        0x75, 0x01,                    //   REPORT_SIZE (1)
        0x95, 0x08,                    //   REPORT_COUNT (8)
        0x81, 0x02,                    //   INPUT (Data,Var,Abs)
        0x95, 0x01,                    //   REPORT_COUNT (1)
        0x75, 0x08,                    //   REPORT_SIZE (8)
        0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
        0x95, 0x06,                    //   REPORT_COUNT (6)
        0x75, 0x08,                    //   REPORT_SIZE (8)
        0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
        0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
        0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
        0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
        0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
        0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0,                          // END_COLLECTION - application keyboard

    // define bi-directional user data buffer for force feedback protocol purpose
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x00,                    // USAGE (Undefined)
    0xa1, 0x01,                    // COLLECTION (Application)
        0x85, 0x03,                    //   REPORT_ID (3)
        0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
        0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
        0x75, 0x08,                    //   REPORT_SIZE (8)
        0x95, 0x3F,                    //   REPORT_COUNT (63)
        0x09, 0x00,                    //   USAGE (Undefined)
        0x81, 0x02,                    //   INPUT (Data,Var,Abs)
        0x95, 0x3F,                    //   REPORT_COUNT (63)
        0x09, 0x00,                    //   USAGE (Undefined)
        0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0xc0                           // END_COLLECTION
};


static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t* buffer);


USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
  CUSTOM_HID_ReportDesc_FS,
  CUSTOM_HID_Init_FS,
  CUSTOM_HID_DeInit_FS,
  CUSTOM_HID_OutEvent_FS
};

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
    System::getInstance().getYoke()->forceFeedbackHandler(buffer);
    return (USBD_OK);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

