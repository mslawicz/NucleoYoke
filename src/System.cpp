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

ErrorCode System::initErrorCode = ErrorCode::OK;

System::System() :
    systemLED(LED1_GPIO_PORT, LED1_PIN, GPIO_MODE_OUTPUT_PP),   //green LED
    errorLED(LED3_GPIO_PORT, LED3_PIN, GPIO_MODE_OUTPUT_PP),   //red LED
    dataLED(LED2_GPIO_PORT, LED2_PIN, GPIO_MODE_OUTPUT_PP),   //blue LED
    systemPushbutton(KEY_BUTTON_GPIO_PORT, KEY_BUTTON_PIN, GPIO_MODE_INPUT, GPIO_NOPULL),
    testPin1(TEST1_PORT, TEST1_PIN, GPIO_MODE_OUTPUT_PP),
    testPin2(TEST2_PORT, TEST2_PIN, GPIO_MODE_OUTPUT_PP)
{
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
        System::initErrorCode = ErrorCode::RccOscConfig;
    }
    //Select the PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        System::initErrorCode = ErrorCode::RccClockConfig;
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CLK48;
    PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLQ;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        System::initErrorCode = ErrorCode::RccPeriphClkConfig;
    }
}

/*
 * user configuration
 */
void System::config(void)
{
    errorLED.write(System::initErrorCode == ErrorCode::OK ? GPIO_PinState::GPIO_PIN_RESET : GPIO_PinState::GPIO_PIN_SET);
    Timer::config();
    pConsole = new Console;
    pConsole->sendMessage(Severity::Info,LogChannel::LC_SYSTEM, "Nucleo Yoke program started");
    // SPI2 is used for RGB LEDs in 1-wire mode
    pSPI2 = new SpiBus(SPI2);
    // SPI4 is used for display
    pSPI4 = new SpiBus(SPI4);
    pRGBLeds = new RGB(pSPI2, NumberOfRgbLeds);
    pDisplay = new Display;
    Servo::config();    // configure servo timer (must be called before Yoke creation)
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

/*
 * displays system status
 */
void System::displayStatus(void)
{
    getDisplay()->print(0, 20, "mode=" + getYoke()->getYokeModeText() + "     ", FontTahoma11);
    getDisplay()->getController().requestUpdate();
}

/*
 * things to do in demo mode
 */
void System::demoHandler(void)
{
    if(pYoke->getYokeMode() == YokeMode::demo)
    {
        static Timer cycleTimer;
        if(cycleTimer.elapsed(50000))
        {
            static uint8_t step = 0;
            cycleTimer.reset();
            pRGBLeds->setValue(0, pRGBLeds->getCycledValue(step, 0));
            pRGBLeds->setValue(1, pRGBLeds->getCycledValue(step, 3));
            pRGBLeds->setValue(2, pRGBLeds->getCycledValue(step, 6));
            pRGBLeds->setValue(3, pRGBLeds->getCycledValue(step, 9));
            pRGBLeds->setValue(4, pRGBLeds->getCycledValue(step, 12));
            pRGBLeds->setValue(5, pRGBLeds->getCycledValue(step, 15));
            pRGBLeds->setValue(6, pRGBLeds->getCycledValue(step, 18));
            pRGBLeds->setValue(7, pRGBLeds->getCycledValue(step, 21));
            pRGBLeds->requestUpdate();
            step = (step+1) % 24;
        }
    }
}
