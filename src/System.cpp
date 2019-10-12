/*
 * system.cpp
 *
 *  Created on: 02.08.2019
 *      Author: Marcin
 */

#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h"
#include "Timer.h"
#include "System.h"


#define USE_HSE_OSCILLATOR  1

System::System() :
    systemLED(LED1_GPIO_PORT, LED1_PIN, GPIO_MODE_OUTPUT_PP),   //green LED
    errorLED(LED3_GPIO_PORT, LED3_PIN, GPIO_MODE_OUTPUT_PP),   //red LED
    dataLED(LED2_GPIO_PORT, LED2_PIN, GPIO_MODE_OUTPUT_PP),   //blue LED
    systemPushbutton(KEY_BUTTON_GPIO_PORT, KEY_BUTTON_PIN, GPIO_MODE_INPUT, GPIO_NOPULL),
    testPin1(TEST1_PORT, TEST1_PIN, GPIO_MODE_OUTPUT_PP),
    testPin2(TEST2_PORT, TEST2_PIN, GPIO_MODE_OUTPUT_PP)
{
    pI2C1 = nullptr;
    pI2C2 = nullptr;
    pSPI2 = nullptr;
    pSPI4 = nullptr;
    pConsole = nullptr;
    pYoke = nullptr;
    pDisplay = nullptr;
    pRGBLeds = nullptr;
}

System::~System()
{

}

/*
 * get the singleton instance
 * object constructor is executed in the first call
 */
System& System::getInstance(void)
{
    static System singleton;
    return singleton;
}

/*
 * configuration of system clocks
 */
void System::configMCU(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);

    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

#if(USE_HSE_OSCILLATOR)
    //Enable HSE Oscillator
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 192;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 8;
    RCC_OscInitStruct.PLL.PLLR = 2;
#else
    //Enable HSI Oscillator
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 192;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 8;
#endif

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
      //errorHandler();
    }
    //Select the PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        //errorHandler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CLK48;
    PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLQ;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        //errorHandler();
    }
}

/*
 * user configuration
 */
void System::config(void)
{
    errorLED.write(GPIO_PinState::GPIO_PIN_RESET);
    Timer::config();
    pConsole = new Console;
    pConsole->sendMessage(Severity::Info,LogChannel::LC_SYSTEM, "Nucleo Yoke program started");
    // I2C1 is used for stepper motor controllers
    pI2C1 = new I2cBus(I2C1);
    // I2C2 is used for IMU sensor and GPIO expanders
    pI2C2 = new I2cBus(I2C2);
    // SPI2 is used for RGB LEDs in 1-wire mode
    pSPI2 = new SpiBus(SPI2);
    // SPI4 is used for display
    pSPI4 = new SpiBus(SPI4);
    gpioExpander.emplace_back(I2cBus::pI2c2, DeviceAddress::MCP23017_0_ADD, MCP23017_0_INT_PORT, MCP23017_0_INT_PIN, 0x0001);
    pRGBLeds = new WS2812(pSPI2);
    pDisplay = new Display;
    pYoke = new Yoke;
    pConsole->registerCommands();
}

/*
 * clean up system before exit
 */
void System::terminate(void)
{
    delete pYoke;
    delete pDisplay;
    delete pConsole;
    delete pRGBLeds;
    delete pSPI4;
    delete pSPI2;
    delete pI2C2;
    delete pI2C1;
}

/*
 * blinks the system LED in 1 second interval
 */
void System::blinkSystemLED(void)
{
    static Timer ledTimer;
    if(ledTimer.elapsed(500000))
    {
        ledTimer.reset();
        systemLED.toggle();
    }
}
