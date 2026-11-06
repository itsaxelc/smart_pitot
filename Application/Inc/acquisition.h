#ifndef ACQUISITION_H
#define ACQUISITION_H

#include <stdint.h>
#include "bme280.h"

/* Sensor configuration */
#define ACQUISITION_SENSOR_ADDRESS    BME280_I2C_ADDR_PRIMARY

/* Acquisition status */
typedef enum
{
    ACQUISITION_OK = 0,
    ACQUISITION_ERROR,
    ACQUISITION_ERROR_TOTAL_SENSOR,
    ACQUISITION_ERROR_STATIC_SENSOR,
    ACQUISITION_ERROR_INVALID_PARAM
} Acquisition_Status;

/* Acquired measurements from both sensors */
typedef struct
{
    BME280_Data total;       /* Total pressure sensor: I2C2 */
    BME280_Data static_;      /* Static pressure sensor: I2C3 */

    double differential_pressure_pa;

    uint8_t total_valid;
    uint8_t static_valid;
    uint8_t differential_valid;

} Acquisition_Data;

/* Acquisition module initialization */
Acquisition_Status Acquisition_Init(I2C_Handle *total_i2c, I2C_Handle *static_i2c);

/* Read measurements from both sensors */
Acquisition_Status Acquisition_Read(Acquisition_Data *data);

/* Check whether both sensors are initialized */
uint8_t Acquisition_IsInitialized(void);

#endif /* ACQUISITION_H */
