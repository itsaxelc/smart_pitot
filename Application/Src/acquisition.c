#include "acquisition.h"

#include <stddef.h>

/* Private sensor instances */
static BME280_Handle total_sensor;
static BME280_Handle static_sensor;

/* Module state */
static uint8_t acquisition_initialized = 0U;


/* Initialize both pressure sensors.
 
    Sensor 1: Total pressure  - I2C2
    Sensor 2: Static pressure - I2C3
    
    Each sensor has its own BME280 handle.
 */
Acquisition_Status Acquisition_Init(I2C_Handle *total_i2c, I2C_Handle *static_i2c)
{
    if (total_i2c == NULL || static_i2c == NULL || total_i2c->instance == NULL || static_i2c->instance == NULL)
    {
        return ACQUISITION_ERROR_INVALID_PARAM;
    }

    BME280_Config config;

    acquisition_initialized = 0U;

    /* Load default configuration */
    BME280_GetDefaultConfig(&config);

    /* Configure both sensors */
    config.temperature_oversampling = BME280_OVERSAMPLING_1X;
    config.pressure_oversampling = BME280_OVERSAMPLING_16X;
    config.humidity_oversampling = BME280_OVERSAMPLING_SKIPPED;

    config.filter = BME280_FILTER_4;
    config.mode = BME280_MODE_NORMAL;
    config.standby = BME280_STANDBY_62_5_MS;

    /* Initialize total pressure sensor */
    if (BME280_Init(&total_sensor, total_i2c, ACQUISITION_SENSOR_ADDRESS, &config) != BME280_OK)
    {
        return ACQUISITION_ERROR_TOTAL_SENSOR;
    }

    /* Initialize static pressure sensor */
    if (BME280_Init(&static_sensor, static_i2c, ACQUISITION_SENSOR_ADDRESS, &config) != BME280_OK)
    {
        return ACQUISITION_ERROR_STATIC_SENSOR;
    }

    acquisition_initialized = 1U;

    return ACQUISITION_OK;
}


/* Read measurements from both pressure sensors. Pressures are returned in Pascals. */
Acquisition_Status Acquisition_Read(Acquisition_Data *data)
{
    if (data == NULL) return ACQUISITION_ERROR_INVALID_PARAM;
    if (acquisition_initialized == 0U) return ACQUISITION_ERROR;

    /* Reset validity flags */
    data->total_valid = 0U;
    data->static_valid = 0U;
    data->differential_valid = 0U;

    /* Read total pressure */
    if (BME280_ReadMeasurements(&total_sensor, &data->total) != BME280_OK)
    {
        return ACQUISITION_ERROR_TOTAL_SENSOR;
    }

    data->total_valid = 1U;

    /* Read static pressure */
    if (BME280_ReadMeasurements(&static_sensor, &data->static_) != BME280_OK)
    {
        return ACQUISITION_ERROR_STATIC_SENSOR;
    }

    data->static_valid = 1U;

    /* Calculate differential pressure */
    data->differential_pressure_pa = data->total.pressure_pa - data->static_.pressure_pa;

    data->differential_valid = 1U;

    return ACQUISITION_OK;
}


/*
    Return acquisition initialization state.
*/
uint8_t Acquisition_IsInitialized(void)
{
    return acquisition_initialized;
}
