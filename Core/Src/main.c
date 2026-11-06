
#include "main.h"

#include <stdint.h>
#include <string.h>

#include "i2c.h"
#include "acquisition.h"
#include "filtering.h"
#include "air_data.h"
#include "display.h"

static void SystemClock_Config(void);

/* ============================================================
 * APPLICATION CONFIGURATION
 * ============================================================ */

#define ACQUISITION_PERIOD_MS    100U
#define DISPLAY_PERIOD_MS        1000U

/* ============================================================
 * I2C HANDLES
 * ============================================================ */

static I2C_Handle hi2c1;
static I2C_Handle hi2c2;
static I2C_Handle hi2c3;

/* ============================================================
 * APPLICATION DATA
 * ============================================================ */

static Acquisition_Data acquisition_data;
static Filtering_Data filtered_data;
static AirData_Data air_data;

/* ============================================================
 * FILTER CONFIGURATION
 * ============================================================ */

/*
 * Initial Kalman parameters.
 * These values are provisional and must be tuned using
 * actual sensor measurements.
 */

static const Filtering_Config filtering_config =
{
    .total_pressure_config =
    {
        .process_noise = 0.1f,
        .measurement_noise = 4.0f,
        .initial_error_covariance = 10.0f
    },

    .static_pressure_config =
    {
        .process_noise = 0.1f,
        .measurement_noise = 4.0f,
        .initial_error_covariance = 10.0f
    }
};

/* ============================================================
 * INITIALIZATION
 * ============================================================ */

static void Application_Init(void)
{
    I2C_Status i2c_status;
    Acquisition_Status acquisition_status;
    Filtering_Status filtering_status;

    /* Initialize I2C1: LCD1602 */
    i2c_status = I2C_Init(
        &hi2c1,
        I2C_INSTANCE_1,
        I2C1_PINS_PB6_PB7
    );

    if (i2c_status != I2C_OK)
        Error_Handler();

    /* Initialize I2C2: Total pressure BME280 */
    i2c_status = I2C_Init(
        &hi2c2,
        I2C_INSTANCE_2,
        I2C2_PINS_PB10_PB11
    );

    if (i2c_status != I2C_OK)
        Error_Handler();

    /* Initialize I2C3: Static pressure BME280 */
    i2c_status = I2C_Init(
        &hi2c3,
        I2C_INSTANCE_3,
        I2C3_PINS_PC0_PC1
    );

    if (i2c_status != I2C_OK)
        Error_Handler();

    /* Initialize LCD */
    i2c_status = Display_Init(&hi2c1);

    if (i2c_status != I2C_OK)
        Error_Handler();

    /* Initialize pressure acquisition */
    acquisition_status = Acquisition_Init(&hi2c2, &hi2c3);

    if (acquisition_status != ACQUISITION_OK)
        Error_Handler();

    /* Initialize pressure filters */
    filtering_status = Filtering_Init(&filtering_config);

    if (filtering_status != FILTERING_OK)
        Error_Handler();

    /* Initialize application data */
    memset(&acquisition_data, 0, sizeof(acquisition_data));
    memset(&filtered_data, 0, sizeof(filtered_data));
    memset(&air_data, 0, sizeof(air_data));
}

/* ============================================================
 * MAIN
 * ============================================================ */

int main(void)
{
    uint32_t current_tick;
    uint32_t last_acquisition_tick;
    uint32_t last_display_tick;

    uint8_t air_data_valid = 0U;

    /* Initialize HAL */
    HAL_Init();

    /* Configure system clock: 80 MHz */
    SystemClock_Config();

    /* Initialize SMART PITOT application */
    Application_Init();

    last_acquisition_tick = HAL_GetTick();
    last_display_tick = HAL_GetTick();

    /* Main application loop */
    while (1)
    {
        current_tick = HAL_GetTick();

        /* --------------------------------------------
         * PRESSURE ACQUISITION AND AIR DATA COMPUTATION
         * -------------------------------------------- */

        if ((uint32_t)(current_tick - last_acquisition_tick)
            >= ACQUISITION_PERIOD_MS)
        {
            last_acquisition_tick = current_tick;
            air_data_valid = 0U;

            /* Read both BME280 sensors */
            if (Acquisition_Read(&acquisition_data) == ACQUISITION_OK)
            {
                /* Filter total and static pressures */
                if (Filtering_Update(
                        &acquisition_data,
                        &filtered_data
                    ) == FILTERING_OK)
                {
                    /* Calculate aerodynamic air data */
                    if (AirData_Calculate(
                            &filtered_data,
                            &air_data
                        ) == AIR_DATA_OK)
                    {
                        air_data_valid = 1U;
                    }
                }
            }
        }

        /* --------------------------------------------
         * LCD UPDATE
         * -------------------------------------------- */

        if ((uint32_t)(current_tick - last_display_tick)
            >= DISPLAY_PERIOD_MS)
        {
            last_display_tick = current_tick;

            if (air_data_valid != 0U)
            {
                Display_Update(&hi2c1, &air_data);
            }
        }
    }
}

/* ============================================================
 * SYSTEM CLOCK CONFIGURATION
 * ============================================================ */

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Enable power interface clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* Voltage scaling */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Configure HSI and PLL */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;

    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 1U;
    RCC_OscInitStruct.PLL.PLLN = 10;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /* Configure AHB, APB1 and APB2 clocks */
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct,
            FLASH_LATENCY_4
        ) != HAL_OK)
    {
        Error_Handler();
    }
}

/* ============================================================
 * ERROR HANDLER
 * ============================================================ */

void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
        /* Fatal application error */
    }
}
