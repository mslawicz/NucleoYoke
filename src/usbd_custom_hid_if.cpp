
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


#if(FORCE_FEEDBACK)
/** Usb HID+PID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
    0x05, 0x01,    // USAGE_PAGE (Generic Desktop)
    0x15, 0x00,    // LOGICAL_MINIMUM (0)
    0x09, 0x04,    // USAGE (Joystick)
    0xA1, 0x01,    // COLLECTION (Application)
        //Start Joystick input definition
        0x85, 0x01,    // Report ID 1
        0x05, 0x02,    // USAGE_PAGE (Simulation Controls)
        0x09, 0xbb,    // USAGE (Throttle)
        0x15, 0x81,    // LOGICAL_MINIMUM (-127)
        0x25, 0x7f,    // LOGICAL_MAXIMUM (127)
        0x75, 0x08,    // REPORT_SIZE (8)
        0x95, 0x01,    // REPORT_COUNT (1)
        0x81, 0x02,    // INPUT (Data,Var,Abs)
        0x05, 0x01,    // USAGE_PAGE (Generic Desktop)

        // Define axes
        0x09, 0x01,    // USAGE (Pointer)
        0xa1, 0x00,    // COLLECTION (Physical)
            0x09, 0x30,    // USAGE (X)
            0x09, 0x31,    // USAGE (Y)
            0x09, 0x32,    // USAGE (Z)
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
        //End Joystick Input definition

        //Start Force Feedback command definitions
        0x05, 0x0F,    //  USAGE_PAGE (Physical Interface)
        //Effect Report Definition
        0x09, 0x21,    //  USAGE (Set Effect Report)
        0xA1, 0x02,    //  COLLECTION (Logical)
            0x09, 0x22,    //  USAGE (Effect Block Index)
            0x25, 0x7F,    //  LOGICAL_MAXIMUM (127)
            0x75, 0x07,    //  REPORT_SIZE (7)
            0x95, 0x01,    //  REPORT_COUNT (1)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x09, 0x24,    //  USAGE (ROM Flag)
            0x25, 0x01,    //  LOGICAL_MAXIMUM (1)
            0x75, 0x01,    //  REPORT_SIZE (1)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)
            //Define the available effect types. Effect Type is a named array that will
            // accept any of the ET usages listed.
            0x09, 0x25,    //  USAGE (Effect Type)
            0xA1, 0x02,    //  COLLECTION (Logical)
                0x09, 0x26,    //  USAGE (ET Constant Force)
                0x09, 0x27,    //  USAGE (ET Ramp)
                0x09, 0x30,    //  USAGE (ET Square)
                0x09, 0x31,    //  USAGE (ET Sine)
                0x09, 0x32,    //  USAGE (ET Triangle)
                0x09, 0x33,    //  USAGE (ET Sawtooth Up)
                0x09, 0x34,    //  USAGE (ET Sawtooth Down)
                0x09, 0x40,    //  USAGE (ET Spring)
                0x09, 0x41,    //  USAGE (ET Damper)
                0x09, 0x42,    //  USAGE (ET Inertia)
                0x15, 0x01,    //  LOGICAL_MINIMUM (1)
                0x25, 0x0A,    //  LOGICAL_MAXIMUM (10)
                0x75, 0x08,    //  REPORT_SIZE (8)
                0x91, 0x00,    //  OUTPUT (Data,Ary,Abs)
            0xC0,          //  END_COLLECTION

            0x09, 0x50,    //  USAGE (Duration)
            0x09, 0x54,    //  USAGE (Trigger Repeat Interval)
            0x15, 0x00,    //  LOGICAL_MINIMUM (0)
            0x26, 0x10, 0x27,    //  LOGICAL_MAXIMUM (10000)
            0x46, 0x10, 0x27,    //  PHYSICAL_MAXIMUM (10000)
            0x75, 0x10,    //  REPORT_SIZE (16)
            0x66, 0x03, 0x10,    //  UNIT (Eng Lin:Time)
            0x55, 0x0D,    //  UNIT_EXPONENT (-3)
            0x95, 0x02,    //  REPORT_COUNT (2)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x55, 0x0A,    //  UNIT_EXPONENT (-6)
            0x09, 0x51,    //  USAGE (Sample Period)
            0x95, 0x01,    //  REPORT_COUNT (1)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)
            0x45, 0x00,    //  PHYSICAL_MAXIMUM (0)
            0x55, 0x00,    //  UNIT_EXPONENT (0)
            0x65, 0x00,    //  UNIT (None)

            0x09, 0x52,    //  USAGE (Gain)
            0x09, 0x53,    //  USAGE (Trigger Button)
            0x25, 0x7F,    //  LOGICAL_MAXIMUM (127)
            0x75, 0x08,    //  REPORT_SIZE (8)
            0x95, 0x02,    //  REPORT_COUNT (2)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x09, 0x55,    //  USAGE (Axes Enable); Tie these axes to the stick
            0xA1, 0x02,    //  COLLECTION (Logical)
                0x05, 0x01,    //  USAGE_PAGE (Generic Desktop)
                0x09, 0x01,    //  USAGE (Pointer)
                0xA1, 0x00,    //  COLLECTION (Physical)
                    0x09, 0x30,    //  USAGE (X)
                    0x09, 0x31,    //  USAGE (Y)
                    0x25, 0x01,    //  LOGICAL_MAXIMUM (1)
                    0x75, 0x01,    //  REPORT_SIZE (1)
                    0x95, 0x02,    //  REPORT_COUNT (2)
                    0x91, 0x02,    //  OUTPUT (Data,Var,Abs)
                0xC0,          //  END_COLLECTION
            0xC0,          //  END_COLLECTION

            0x95, 0x06,    //  REPORT_COUNT (6); 6-bit pad
            0x91, 0x03,    //  OUTPUT (Cnst,Var,Abs)

            0x05, 0x0F,    //  USAGE_PAGE (Physical Interface)
            0x09, 0x57,    //  USAGE (Direction)
            0xA1, 0x02,    //  COLLECTION (Logical)
                0x05, 0x01,    //  USAGE_PAGE (Generic Desktop)
                0x09, 0x01,    //  USAGE (Pointer); Tie these axes to the stick
                0xA1, 0x00,    //  COLLECTION (Physical)
                    0x09, 0x30,    //  USAGE (X)
                    0x09, 0x31,    //  USAGE (Y)
                    0x15, 0x00,    //  LOGICAL_MINIMUM (0)
                    0x26, 0xFF, 0x00,    //  LOGICAL_MAXIMUM (255)
                    0x46, 0x68, 0x01,    //  PHYSICAL_MAXIMUM (360)
                    0x66, 0x14, 0x00,    //  UNIT (Eng Rot:Angular Pos)
                    0x75, 0x08,    //  REPORT_SIZE (8)
                    0x95, 0x02,    //  REPORT_COUNT (2)
                    0x91, 0x02,    //  OUTPUT (Data,Var,Abs)
                    0x65, 0x00,    //  UNIT (None)
                    0x45, 0x00,    //  PHYSICAL_MAXIMUM (0)
                0xC0,          //  END_COLLECTION
            0xC0,          //  END_COLLECTION

            0x05, 0x0F,    //  USAGE_PAGE (Physical Interface)
            0x09, 0x58,    //  USAGE (Type Specific Block Offset)
            0xA1, 0x02,    //  COLLECTION (Logical)
                0x0B, 0x01, 0x00, 0x0A, 0x00,    //  USAGE (Ordinals:Instance 1)
                0x0B, 0x02, 0x00, 0x0A, 0x00,    //  USAGE (Ordinals:Instance 2)
                0x26, 0xFD, 0x7F,    //  LOGICAL_MAXIMUM (32765); 32K RAM or ROM max.
                0x75, 0x10,    //  REPORT_SIZE (16)
                0x95, 0x02,    //  REPORT_COUNT (2)
                0x91, 0x02,    //  OUTPUT (Data,Var,Abs)
            0xC0,          //  END_COLLECTION
        0xC0,          //  END_COLLECTION

        //Envelope Report Definition
        0x09, 0x5A,    //  USAGE (Set Envelope Report)
        0xA1, 0x02,    //  COLLECTION (Logical)
            0x85, 0x02,    //  REPORT_ID (2)
            0x09, 0x23,    //  USAGE (Parameter Block Offset)
            0x26, 0xFD, 0x7F,    //  LOGICAL_MAXIMUM (32765); 32K RAM or ROM max
            0x75, 0x0F,    //  REPORT_SIZE (15)
            0x95, 0x01,    //  REPORT_COUNT (1)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x09, 0x24,    //  USAGE (ROM Flag)
            0x25, 0x01,    //  LOGICAL_MAXIMUM (1)
            0x75, 0x01,    //  REPORT_SIZE (1)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x09, 0x5B,    //  USAGE (Attack Level)
            0x09, 0x5D,    //  USAGE (Fade Level)
            0x26, 0xFF, 0x00,    //  LOGICAL_MAXIMUM (255)
            0x75, 0x08,    //  REPORT_SIZE (8)
            0x95, 0x02,    //  REPORT_COUNT (2)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x09, 0x5C,    //  USAGE (Attack Time)
            0x09, 0x5E,    //  USAGE (Fade Time)
            0x26, 0x10, 0x27,    //  LOGICAL_MAXIMUM (10000)
            0x46, 0x10, 0x27,    //  PHYSICAL_MAXIMUM (10000)
            0x66, 0x03, 0x10,    //  UNIT (Eng Lin:Time)
            0x55, 0x0D,    //  UNIT_EXPONENT (-3)
            0x75, 0x10,    //  REPORT_SIZE (16)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)
            0x45, 0x00,    //  PHYSICAL_MAXIMUM (0)
            0x65, 0x00,    //  UNIT (None)
            0x55, 0x00,    //  UNIT_EXPONENT (0)
        0xC0,          //  END_COLLECTION

        //Condition  Report Definition
        0x09, 0x5F,    //  USAGE (Set Condition Report)
        0xA1, 0x02,    //  COLLECTION (Logical)
            0x85, 0x03,    //  REPORT_ID (3)
            0x09, 0x23,    //  USAGE (Parameter Block Offset)
            0x26, 0xFD, 0x7F,    //  LOGICAL_MAXIMUM (32765)
            0x75, 0x0F,    //  REPORT_SIZE (15)
            0x95, 0x01,    //  REPORT_COUNT (1)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x09, 0x24,    //  USAGE (ROM Flag)
            0x25, 0x01,    //  LOGICAL_MAXIMUM (1)
            0x75, 0x01,    //  REPORT_SIZE (1)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x09, 0x60,    //  USAGE (CP Offset)
            0x09, 0x61,    //  USAGE (Positive Coefficient)
            0x09, 0x62,    //  USAGE (Negative Coefficient)
            0x09, 0x63,    //  USAGE (Positive Saturation)
            0x09, 0x64,    //  USAGE (Negative Saturation)
            0x09, 0x65,    //  USAGE (Dead Band)
            0x26, 0xFF, 0x00,    //  LOGICAL_MAXIMUM (255)
            0x75, 0x08,    //  REPORT_SIZE (8)
            0x95, 0x06,    //  REPORT_COUNT (6)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)
        0xC0,          //  END_COLLECTION

        //Periodic Report Definition
        0x09, 0x6E,    //  USAGE (Set Periodic Report)
        0xA1, 0x02,    //  COLLECTION (Logical)
            0x85, 0x04,    //  REPORT_ID (4)
            0x09, 0x23,    //  USAGE (Parameter Block Offset)
            0x26, 0xFD, 0x7F,    //  LOGICAL_MAXIMUM (32765)
            0x75, 0x0F,    //  REPORT_SIZE (15)
            0x95, 0x01,    //  REPORT_COUNT (1)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x09, 0x24,    //  USAGE (ROM Flag)
            0x25, 0x01,    //  LOGICAL_MAXIMUM (1)
            0x75, 0x01,    //  REPORT_SIZE (1)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x09, 0x70,    //  USAGE (Magnitude)
            0x09, 0x6F,    //  USAGE (Offset)
            0x09, 0x71,    //  USAGE (Phase)
            0x26, 0xFF, 0x00,    //  LOGICAL_MAXIMUM (255)
            0x75, 0x08,    //  REPORT_SIZE (8)
            0x95, 0x03,    //  REPORT_COUNT (3)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x09, 0x72,    //  USAGE (Period)
            0x26, 0x10, 0x27,    //  LOGICAL_MAXIMUM (10000)
            0x46, 0x10, 0x27,    //  PHYSICAL_MAXIMUM (10000)
            0x66, 0x03, 0x10,    //  UNIT (Eng Lin:Time)
            0x55, 0x0D,    //  UNIT_EXPONENT (-3)
            0x75, 0x10,    //  REPORT_SIZE (16)
            0x95, 0x01,    //  REPORT_COUNT (1)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)
            0x45, 0x00,    //  PHYSICAL_MAXIMUM (0)
            0x65, 0x00,    //  UNIT (None)
            0x55, 0x00,    //  UNIT_EXPONENT (0)
        0xC0,          //  END_COLLECTION

        //Constant Force Report Definition
        0x09, 0x73,    //  USAGE (Set Constant Force Report)
        0xA1, 0x02,    //  COLLECTION (Logical)
            0x85, 0x05,    //  REPORT_ID (5)
            0x09, 0x23,    //  USAGE (Parameter Block Offset)
            0x26, 0xFD, 0x7F,    //  LOGICAL_MAXIMUM (32765)
            0x75, 0x0F,    //  REPORT_SIZE (15)
            0x95, 0x01,    //  REPORT_COUNT (1)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x09, 0x24,    //  USAGE (ROM Flag)
            0x25, 0x01,    //  LOGICAL_MAXIMUM (1)
            0x75, 0x01,    //  REPORT_SIZE (1)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x09, 0x70,    //  USAGE (Magnitude)
            0x26, 0xFF, 0x00,    //  LOGICAL_MAXIMUM (255)
            0x75, 0x08,    //  REPORT_SIZE (8)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)
        0xC0,          //  END_COLLECTION

        //Ramp Force Report Definition
        0x09, 0x74,    //  USAGE (Set Ramp Force Report)
        0xA1, 0x02,    //  COLLECTION (Logical)
            0x85, 0x06,    //  REPORT_ID (6)
            0x09, 0x23,    //  USAGE (Parameter Block Offset)
            0x26, 0xFD, 0x7F,    //  LOGICAL_MAXIMUM (32765)
            0x75, 0x0F,    //  REPORT_SIZE (15)
            0x95, 0x01,    //  REPORT_COUNT (1)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x09, 0x24,    //  USAGE (ROM Flag)
            0x25, 0x01,    //  LOGICAL_MAXIMUM (1)
            0x75, 0x01,    //  REPORT_SIZE (1)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x09, 0x75,    //  USAGE (Ramp Start)
            0x09, 0x76,    //  USAGE (Ramp End)
            0x26, 0xFF, 0x00,    //  LOGICAL_MAXIMUM (255)
            0x75, 0x08,    //  REPORT_SIZE (8)
            0x95, 0x02,    //  REPORT_COUNT (2)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)
        0xC0,          //  END_COLLECTION

        //Custom Force Data Report Definition
        // Downloads are always into RAM space so the ROM usage is not declared.
        0x09, 0x68,    //  USAGE (Custom Force Data Report)
        0xA1, 0x02,    //  COLLECTION (Logical)
            0x85, 0x07,    //  REPORT_ID (7)
            0x09, 0x23,    //  USAGE (Parameter Block Offset)
            0x26, 0xFD, 0x7F,    //  LOGICAL_MAXIMUM (32765)
            0x75, 0x0F,    //  REPORT_SIZE (15)
            0x95, 0x01,    //  REPORT_COUNT (1)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x0B, 0x3B, 0x00, 0x01, 0x00,    //  USAGE (Generic Desktop:Byte Count)
            0x26, 0x00, 0x01,    //  LOGICAL_MAXIMUM (256)
            0x75, 0x09,    //  REPORT_SIZE (9)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x09, 0x69,    //  USAGE (Custom Force Data)
            0x26, 0xFF, 0x00,    //  LOGICAL_MAXIMUM (255)
            0x75, 0x08,    //  REPORT_SIZE (8)
            0x96, 0x00, 0x01,    //  REPORT_COUNT (256)
            0x92, 0x02, 0x01,    //  OUTPUT (Data,Var,Abs,Buf)
        0xC0,          //  END_COLLECTION

        //Download Force Sample Definition
        0x09, 0x66,    //  USAGE (Download Force Sample)
        0xA1, 0x02,    //  COLLECTION (Logical)
            0x85, 0x08,    //  REPORT_ID (8)
            0x05, 0x01,    //  USAGE_PAGE (Generic Desktop)
            0x09, 0x01,    //  USAGE (Pointer)
            0xA1, 0x02,    //  COLLECTION (Logical)
                0x09, 0x30,    //  USAGE (X)
                0x09, 0x31,    //  USAGE (Y)
                0x15, 0x81,    //  LOGICAL_MINIMUM (-127)
                0x25, 0x7F,    //  LOGICAL_MAXIMUM (127)
                0x75, 0x08,    //  REPORT_SIZE (8)
                0x95, 0x02,    //  REPORT_COUNT (2)
                0x91, 0x02,    //  OUTPUT (Data,Var,Abs)
            0xC0,          //  END_COLLECTION
        0xC0,          //  END_COLLECTION
        0x05, 0x0F,    //  USAGE_PAGE (Physical Interface)

        //Define the Custom Force parameter block
        // Custom Effects are always RAM based
        // so ROM flags are not declared.
        0x09, 0x6B,    //  USAGE (Set Custom Force Report)
        0xA1, 0x02,    //  COLLECTION (Logical)
            0x85, 0x09,    //  REPORT_ID (9)
            //Parameter block offset in pool
            //Custom Force data offset in pool
            0x09, 0x23,    //  USAGE (Parameter Block Offset)
            0x09, 0x6C,    //  USAGE (Custom Force Data Offset)
            0x09, 0x6D,    //  USAGE (Sample Count)
            0x15, 0x00,    //  LOGICAL_MINIMUM (0)
            0x26, 0xFD, 0x7F,    //  LOGICAL_MAXIMUM (32765); 32K of RAM or ROM max.
            0x95, 0x03,    //  REPORT_COUNT (3)
            0x75, 0x10,    //  REPORT_SIZE (16)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)
        0xC0,          //  END_COLLECTION

        //Effect Operation Report Definition
        0x09, 0x77,    //  USAGE (Effect Operation Report)
        0xA1, 0x02,    //  COLLECTION (Logical)
            0x85, 0x0A,    //  REPORT_ID (10)
            0x09, 0x22,    //  USAGE (Effect Block Index)
            0x25, 0x7F,    //  LOGICAL_MAXIMUM (127)
            0x75, 0x07,    //  REPORT_SIZE (7)
            0x95, 0x01,    //  REPORT_COUNT (1)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x09, 0x24,    //  USAGE (ROM Flag)
            0x25, 0x01,    //  LOGICAL_MAXIMUM (1)
            0x75, 0x01,    //  REPORT_SIZE (1)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)

            0x09, 0x78,    //  USAGE (Effect Operation)
            0xA1, 0x02,    //  COLLECTION (Logical)
                0x09, 0x79,    //  USAGE (Op Effect Start)
                0x09, 0x7A,    //  USAGE (Op Effect Start Solo)
                0x09, 0x7B,    //  USAGE (Op Effect Stop)
                0x15, 0x01,    //  LOGICAL_MINIMUM (1)
                0x25, 0x03,    //  LOGICAL_MAXIMUM (3)
                0x75, 0x08,    //  REPORT_SIZE (8)
                0x91, 0x00,    //  OUTPUT (Data,Ary,Abs)
            0xC0,          //  END_COLLECTION

            0x09, 0x7C,    //  USAGE (Loop Count)
            0x15, 0x00,    //  LOGICAL_MINIMUM (0)
            0x26, 0xFF, 0x00,    //  LOGICAL_MAXIMUM (255)
            0x91, 0x02,    //  OUTPUT (Data,Var,Abs)
        0xC0,          //  END_COLLECTION

        //PID Pool Report Definition
        0x09, 0x7F,    //  USAGE (PID Pool Report)
        0xA1, 0x02,    //  COLLECTION (Logical)
            0x85, 0x01,    //  REPORT_ID (1)
            0x09, 0x80,    //  USAGE (RAM Pool Size)
            0x09, 0x81,    //  USAGE (ROM Pool Size)
            0x09, 0x82,    //  USAGE (ROM Effect Block Count)
            0x26, 0xFD, 0x7F,    //  LOGICAL_MAXIMUM (32765)
            0x95, 0x03,    //  REPORT_COUNT (3)
            0x75, 0x10,    //  REPORT_SIZE (16)
            0xB1, 0x02,    //  FEATURE (Data,Var,Abs)

            0x09, 0xA8,    //  USAGE (Parameter Block Size)
            0xA1, 0x02,    //  COLLECTION (Logical)
                0x09, 0x21,    //  USAGE (Set Effect Report)
                0x09, 0x5A,    //  USAGE (Set Envelope Report)
                0x09, 0x5F,    //  USAGE (Set Condition Report)
                0x09, 0x6E,    //  USAGE (Set Periodic Report)
                0x09, 0x73,    //  USAGE (Set Constant Force Report)
                0x09, 0x74,    //  USAGE (Set Ramp Force Report)
                0x09, 0x6B,    //  USAGE (Set Custom Force Report)
                0x26, 0xFF, 0x00,    //  LOGICAL_MAXIMUM (255)
                0x75, 0x08,    //  REPORT_SIZE (8)
                0x95, 0x07,    //  REPORT_COUNT (7)
                0xB1, 0x02,    //  FEATURE (Data,Var,Abs)
            0xC0,          //  END_COLLECTION

            0x25, 0x01,    //  LOGICAL_MAXIMUM (1)
            0x75, 0x07,    //  REPORT_SIZE (7)
            0x95, 0x01,    //  REPORT_COUNT (1)
            0xB1, 0x03,    //  FEATURE (Cnst,Var,Abs); 7-bit pad
            0x09, 0x67,    //  USAGE (Isoch Custom Force Enable)
            0x75, 0x01,    //  REPORT_SIZE (1)
            0xB1, 0x02,    //  FEATURE (Data,Var,Abs)
        0xC0,          //  END_COLLECTION

        //PID State Report Definition
        0x09, 0x92,    //  USAGE (PID State Report)
        0xA1, 0x02,    //  COLLECTION (Logical)
            0x85, 0x02,    //  REPORT_ID (2)
            0x09, 0x22,    //  USAGE (Effect Block Index)
            0x25, 0x7F,    //  LOGICAL_MAXIMUM (127)
            0x75, 0x07,    //  REPORT_SIZE (7)
            0x81, 0x02,    //  INPUT (Data,Var,Abs)

            0x09, 0x24,    //  USAGE (ROM Flag)
            0x25, 0x01,    //  LOGICAL_MAXIMUM (1)
            0x75, 0x01,    //  REPORT_SIZE (1)
            0x95, 0x01,    //  REPORT_COUNT (1)
            0x81, 0x02,    //  INPUT (Data,Var,Abs)

            0x09, 0x94,    //  USAGE (Effect Playing)
            0x09, 0xA0,    //  USAGE (Actuators Enabled)
            0x09, 0xA4,    //  USAGE (Safety Switch)
            0x09, 0xA6,    //  USAGE (Actuator Power)
            0x75, 0x01,    //  REPORT_SIZE (1)
            0x95, 0x04,    //  REPORT_COUNT (4)
            0x81, 0x02,    //  INPUT (Data,Var,Abs)

            0x81, 0x03,    //  INPUT (Cnst,Var,Abs); 4-bit pad
        0xC0,          //  END_COLLECTION

        //PID Device Control Report Definition
        0x09, 0x95,    //  USAGE (PID Device Control Report)
        0xA1, 0x02,    //  COLLECTION (Logical)
            0x85, 0x0B,    //  REPORT_ID (11)
            0x09, 0x96,    //  USAGE (PID Device Control)
            0xA1, 0x02,    //  COLLECTION (LOGICAL)
                0x09, 0x97,    //  USAGE (DC Enable Actuators)
                0x09, 0x98,    //  USAGE (DC Disable Actuators)
                0x09, 0x99,    //  USAGE (DC Stop All Effects)
                0x09, 0x9A,    //  USAGE (DC Reset)
                0x09, 0x9B,    //  USAGE (DC Pause)
                0x09, 0x9C,    //  USAGE (DC Continue)
                0x15, 0x01,    //  LOGICAL_MINIMUM (1)
                0x25, 0x06,    //  LOGICAL_MAXIMUM (6)
                0x75, 0x01,    //  REPORT_SIZE (1)
                0x95, 0x08,    //  REPORT_COUNT (4)
                0x91, 0x02,    //  OUTPUT(Data,Ary,Abs)
            0xC0,          //  END_COLLECTION
        0xC0,          //  END_COLLECTION

        //PID Pool Move Report Definition
        0x09, 0x85,    //  USAGE (PID Pool Move Report)
        0xA1, 0x02,    //  COLLECTION (Logical)
            0x85, 0x0C,    //  REPORT_ID (12)
            0x09, 0x86,    //  USAGE (Move Source)
            0x09, 0x87,    //  USAGE (Move Destination)
            0x09, 0x88,    //  USAGE (Move Length)
            0x26, 0xFF, 0x7F,    //  LOGICAL_MAXIMUM (32767)
            0x75, 0x10,    //  REPORT_SIZE (16)
            0x95, 0x03,    //  REPORT_COUNT (3)
            0x92, 0x02, 0x01,    //  OUTPUT (Data,Var,Abs,Buf)
        0xC0,          //  END_COLLECTION

        //Device Gain Report Definition
        0x09, 0x7D,    //  USAGE (Device Gain Report)
        0xA1, 0x02,    //  COLLECTION (Logical)
            0x85, 0x02,    //  REPORT_ID (2)
            0x09, 0x7E,    //  USAGE (Device Gain)
            0x26, 0xFF, 0x00,    //  LOGICAL_MAXIMUM (255)
            0x75, 0x08,    //  REPORT_SIZE (8)
            0x95, 0x01,    //  REPORT_COUNT (1)
            0xB1, 0x02,    //  FEATURE (Data,Var,Abs)
        0xC0,          //  END_COLLECTION
    0xC0,          //  END_COLLECTION
};
#else
/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
    0x05, 0x01,    // USAGE_PAGE (Generic Desktop)
    0x15, 0x00,    // LOGICAL_MINIMUM (0)
    0x09, 0x04,    // USAGE (Joystick)
    0xA1, 0x01,    // COLLECTION (Application)
        //Start Joystick input definition
        0x85, 0x01,    // Report ID 1
        0x05, 0x02,    // USAGE_PAGE (Simulation Controls)
        0x09, 0xbb,    // USAGE (Throttle)
        0x15, 0x81,    // LOGICAL_MINIMUM (-127)
        0x25, 0x7f,    // LOGICAL_MAXIMUM (127)
        0x75, 0x08,    // REPORT_SIZE (8)
        0x95, 0x01,    // REPORT_COUNT (1)
        0x81, 0x02,    // INPUT (Data,Var,Abs)
        0x05, 0x01,    // USAGE_PAGE (Generic Desktop)

        // Define axes
        0x09, 0x01,    // USAGE (Pointer)
        0xa1, 0x00,    // COLLECTION (Physical)
            0x09, 0x30,    // USAGE (X)
            0x09, 0x31,    // USAGE (Y)
            0x09, 0x32,    // USAGE (Z)
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
        //End Joystick Input definition

        // output data
        0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
        0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
        0x75, 0x08,                    //   REPORT_SIZE (8)
        0x95, 0x40,                    //   REPORT_COUNT (64)
        0x09, 0x01,                    //   USAGE (Undefined)
        0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
        0x95, 0x01,                    //   REPORT_COUNT (1)
        0x09, 0x01,                    //   USAGE (Undefined)
        0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)
    0xc0,          // END_COLLECTION
};
#endif



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
  /* USER CODE BEGIN 6 */
    //handle host sent data here
    char* message = (char*)buffer;
    System::getInstance().getConsole()->sendMessage(Severity::Info,LogChannel::LC_USB, "Message received from host: " + std::string(message));
    return (USBD_OK);
  /* USER CODE END 6 */
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

