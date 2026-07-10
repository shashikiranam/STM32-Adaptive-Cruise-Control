/**
 ******************************************************************************
 * @file    main.c
 * @brief   Application entry point. Performs HAL/clock/peripheral
 *          initialization (in the style of CubeMX-generated glue code),
 *          then hands off to a fixed-period super-loop that drives the
 *          Adaptive Cruise Control / Collision Avoidance application core
 *          (acc.c) once per CONTROL_LOOP_PERIOD_MS.
 *
 * @note    The MX_*_Init() functions below follow the standard structure
 *          STM32CubeMX generates for a project with TIM2 (input capture),
 *          TIM3 (PWM), ADC1 (single channel, software trigger), USART2
 *          (115200 8N1), and the GPIO pins listed in system_config.h. If
 *          you regenerate this project from STM32CubeMX/Project.ioc, these
 *          functions will be overwritten with CubeMX's own generated
 *          versions -- verify the generated configuration still matches
 *          the constants in system_config.h (clock prescalers in
 *          particular, since timer.c/pwm.c assume specific tick rates).
 ******************************************************************************
 */

#include "main.h"
#include "gpio.h"
#include "timer.h"
#include "pwm.h"
#include "motor.h"
#include "ultrasonic.h"
#include "acc.h"

/* ------------------------------------------------------------------------
 * Peripheral handles (declared extern in main.h for use by driver modules
 * and interrupt handlers).
 * ------------------------------------------------------------------------ */
ADC_HandleTypeDef  hadc1;
TIM_HandleTypeDef  htim2;
TIM_HandleTypeDef  htim3;
UART_HandleTypeDef huart2;

/* ------------------------------------------------------------------------
 * Local function prototypes (CubeMX-style static peripheral init glue).
 * ------------------------------------------------------------------------ */
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART2_UART_Init(void);

int main(void)
{
    /* Reset all peripherals, initialize the Flash interface and SysTick. */
    HAL_Init();

    /* Configure the system clock (see SystemClock_Config() below). */
    SystemClock_Config();

    /* Initialize all configured peripherals. */
    MX_GPIO_Init();
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_ADC1_Init();
    MX_USART2_UART_Init();

    /* -------------------------------------------------------------------
     * Application-layer driver initialization, in dependency order:
     * timer -> gpio app defaults -> pwm -> motor -> ultrasonic -> acc.
     * ------------------------------------------------------------------- */
    if (Timer_InitEchoCapture(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    if (Timer_StartEchoCapture(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    GPIO_AppInit();

    if (PWM_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }

    Motor_Init();
    Ultrasonic_Init();
    ACC_Init(&hadc1, &huart2);

    /* -------------------------------------------------------------------
     * Fixed-period main control loop. ACC_Update() performs one full
     * sense -> decide -> actuate -> report cycle each call. The loop is
     * paced to CONTROL_LOOP_PERIOD_MS using HAL_GetTick() (SysTick-based)
     * rather than HAL_Delay(), so that a control cycle that runs long
     * (e.g. due to a slow ultrasonic echo) does not accumulate drift --
     * the next cycle's deadline is always computed relative to the
     * previous deadline, not relative to "now."
     * ------------------------------------------------------------------- */
    uint32_t nextCycleTick = HAL_GetTick();

    while (1)
    {
        ACC_Update();

        nextCycleTick += CONTROL_LOOP_PERIOD_MS;
        uint32_t now = HAL_GetTick();

        if (nextCycleTick > now)
        {
            HAL_Delay(nextCycleTick - now);
        }
        else
        {
            /* Cycle overran its budget (e.g. ultrasonic echo wait ran
             * long) -- resynchronize to "now" rather than issuing a
             * negative/huge delay, accepting the one-time slip. */
            nextCycleTick = now;
        }
    }
}

/**
 * @brief System Clock Configuration.
 * @details Configures the HSI (16 MHz internal oscillator) through the PLL
 *          to reach 64 MHz SYSCLK, the maximum rated frequency for the
 *          STM32G071RB, with AHB/APB prescalers set to 1 (no division).
 *          This determines the timer input clocks assumed by
 *          MX_TIM2_Init()/MX_TIM3_Init() below.
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Configure the main internal regulator output voltage. */
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Configure HSI as PLL source, targeting 64 MHz SYSCLK.
     * PLL: HSI(16MHz) / M(1) * N(8) / R(2) = 64 MHz */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
    RCC_OscInitStruct.PLL.PLLN = 8;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /* Configure bus clocks: SYSCLK from PLLR, AHB/APB at full 64 MHz. */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                 | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief GPIO Initialization Function.
 * @details Configures all GPIO pins used by the application: HC-SR04
 *          trigger/echo, L298N direction pins, status LEDs, and enables
 *          the required GPIO clocks. Pin assignments are pulled from
 *          system_config.h -- see hardware/PIN_CONNECTIONS.md for the
 *          human-readable equivalent of this table.
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* -- HC-SR04 Trigger (PA1): push-pull output, start low. -- */
    HAL_GPIO_WritePin(US_TRIG_GPIO_PORT, US_TRIG_GPIO_PIN, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin   = US_TRIG_GPIO_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(US_TRIG_GPIO_PORT, &GPIO_InitStruct);

    /* -- HC-SR04 Echo (PA0): alternate function, TIM2_CH1 input capture. -- */
    GPIO_InitStruct.Pin       = US_ECHO_GPIO_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM2;
    HAL_GPIO_Init(US_ECHO_GPIO_PORT, &GPIO_InitStruct);

    /* -- Motor PWM (PA6): alternate function, TIM3_CH1. -- */
    GPIO_InitStruct.Pin       = GPIO_PIN_6;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* -- Motor direction pins IN1 (PB0), IN2 (PB1): push-pull outputs. -- */
    HAL_GPIO_WritePin(MOTOR_IN1_GPIO_PORT, MOTOR_IN1_GPIO_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_IN2_GPIO_PORT, MOTOR_IN2_GPIO_PIN, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin   = MOTOR_IN1_GPIO_PIN | MOTOR_IN2_GPIO_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(MOTOR_IN1_GPIO_PORT, &GPIO_InitStruct);

    /* -- Status LEDs (PB3/PB4/PB5): push-pull outputs, start low. -- */
    HAL_GPIO_WritePin(GPIOB, LED_GREEN_GPIO_PIN | LED_YELLOW_GPIO_PIN | LED_RED_GPIO_PIN, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin   = LED_GREEN_GPIO_PIN | LED_YELLOW_GPIO_PIN | LED_RED_GPIO_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* -- Onboard user LED (PA5) and user button (PC13), board bring-up -- */
    GPIO_InitStruct.Pin   = NUCLEO_USER_LED_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(NUCLEO_USER_LED_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin  = NUCLEO_USER_BTN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(NUCLEO_USER_BTN_PORT, &GPIO_InitStruct);

    /* Enable the TIM2 input-capture interrupt in the NVIC. */
    HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

/**
 * @brief TIM2 Initialization Function.
 * @details Configures TIM2 as a free-running microsecond counter (1 MHz
 *          tick, i.e. prescaler = (SystemCoreClock / 1,000,000) - 1) with
 *          input capture on Channel 1 for HC-SR04 echo pulse timing. See
 *          timer.c for the rising/falling-edge capture state machine that
 *          runs on top of this base configuration.
 */
static void MX_TIM2_Init(void)
{
    __HAL_RCC_TIM2_CLK_ENABLE();

    TIM_IC_InitTypeDef sConfigIC = {0};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = (SystemCoreClock / 1000000U) - 1U; /* 1 MHz tick -> 1 tick = 1us */
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 0xFFFFFFFFU; /* Free-running, full 32-bit period on TIM2 */
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigIC.ICPolarity  = TIM_INPUTCHANNELPOLARITY_RISING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter    = 0;

    if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, US_ECHO_TIM_CHANNEL) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief TIM3 Initialization Function.
 * @details Configures TIM3 Channel 1 as a PWM output at PWM_FREQUENCY_HZ
 *          (default 1 kHz) for motor speed control via the L298N's ENA
 *          pin. Auto-reload value is computed from SystemCoreClock and
 *          the target frequency so pwm.c's duty-cycle math (see
 *          PWM_SetDutyCycle()) stays correct even if the frequency is
 *          retuned here.
 */
static void MX_TIM3_Init(void)
{
    __HAL_RCC_TIM3_CLK_ENABLE();

    TIM_OC_InitTypeDef sConfigOC = {0};

    /* Choose a prescaler that gives a convenient ~1 MHz timer tick, then
     * derive ARR from the desired PWM frequency for a clean 0-100% duty
     * cycle mapping. */
    uint32_t timerTickHz = 1000000U;
    uint32_t prescaler = (SystemCoreClock / timerTickHz) - 1U;
    uint32_t arr = (timerTickHz / PWM_FREQUENCY_HZ) - 1U;

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = prescaler;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = arr;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode     = TIM_OCMODE_PWM1;
    sConfigOC.Pulse      = 0; /* Start at 0% duty */
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, PWM_TIM_CHANNEL) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief ADC1 Initialization Function.
 * @details Configures ADC1 for single-channel, software-triggered,
 *          12-bit conversion of the cruise-speed potentiometer wiper
 *          voltage (PA4 / ADC1_IN4).
 */
static void MX_ADC1_Init(void)
{
    __HAL_RCC_ADC_CLK_ENABLE();

    ADC_ChannelConfTypeDef sConfig = {0};

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler        = ADC_CLOCK_ASYNC_DIV1;
    hadc1.Init.Resolution            = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode          = ADC_SCAN_DISABLE;
    hadc1.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
    hadc1.Init.ContinuousConvMode    = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
    hadc1.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ContinuousConvMode    = DISABLE;
    hadc1.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;
    hadc1.Init.SamplingTimeCommon1   = ADC_SAMPLETIME_39CYCLES_5;
    hadc1.Init.SamplingTimeCommon2   = ADC_SAMPLETIME_39CYCLES_5;

    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    sConfig.Channel      = ADC_POT_CHANNEL;
    sConfig.Rank         = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /* Run the ADC calibration routine once at startup for best accuracy. */
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief USART2 Initialization Function.
 * @details Configures USART2 at DEBUG_UART_BAUDRATE (115200), 8N1, which
 *          is automatically routed through the onboard ST-LINK's Virtual
 *          COM Port on the NUCLEO-G071RB -- no external USB-serial adapter
 *          required.
 */
static void MX_USART2_UART_Init(void)
{
    __HAL_RCC_USART2_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin       = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    huart2.Instance          = USART2;
    huart2.Init.BaudRate     = DEBUG_UART_BAUDRATE;
    huart2.Init.WordLength   = UART_WORDLENGTH_8B;
    huart2.Init.StopBits     = UART_STOPBITS_1;
    huart2.Init.Parity       = UART_PARITY_NONE;
    huart2.Init.Mode         = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief TIM2 Input Capture callback -- routes the HAL capture event into
 *        timer.c's echo pulse-width measurement state machine.
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        Timer_EchoCaptureCallback(htim);
    }
}

/**
 * @brief This function is called when an unrecoverable initialization
 *        error occurs (e.g. a peripheral failed to configure). It forces
 *        the motor into a safe stopped state (best-effort, since some
 *        peripherals may not be initialized yet), then blinks the onboard
 *        user LED rapidly forever so the fault is visible without needing
 *        a debugger attached.
 */
void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
        HAL_GPIO_TogglePin(NUCLEO_USER_LED_PORT, NUCLEO_USER_LED_PIN);
        for (volatile uint32_t i = 0; i < 1000000U; i++)
        {
            __NOP();
        }
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief Reports the name of the source file and the source line number
 *        where the assert_param error has occurred (standard HAL debug
 *        hook, only compiled when USE_FULL_ASSERT is defined).
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;
    /* Intentionally left as a hook for user-added debug logging. */
}
#endif /* USE_FULL_ASSERT */
