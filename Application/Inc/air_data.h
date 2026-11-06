
#ifndef AIR_DATA_H
#define AIR_DATA_H


#include <stdint.h>
#include "filtering.h"

/* Air data calculation status */
typedef enum
{
    AIR_DATA_OK = 0,
    AIR_DATA_ERROR_INVALID_PARAM,
    AIR_DATA_ERROR_INVALID_PRESSURE,
    AIR_DATA_ERROR_NEGATIVE_DYNAMIC_PRESSURE,
    AIR_DATA_ERROR_ATMOSPHERE
} AirData_Status;

/* Calculated aerodynamic data */
typedef struct
{
    float indicated_airspeed_m_s;
    float calibrated_airspeed_m_s;
    float equivalent_airspeed_m_s;
    float true_airspeed_m_s;

    float mach_number;

    float pressure_altitude_m;

    float dynamic_pressure_pa;
    float air_density_kg_m3;
    float ambient_temperature_k;
    float speed_of_sound_m_s;

    uint8_t altitude_valid;
    uint8_t airspeed_valid;
} AirData_Data;

/* Calculate aerodynamic data from filtered pressure measurements.

    Required inputs:
    - Filtered total pressure
    - Filtered static pressure
    - Filtered differential pressure
 */
AirData_Status AirData_Calculate(const Filtering_Data *filtered, AirData_Data *data);

#endif
