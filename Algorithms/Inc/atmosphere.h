
#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H


#include <stdint.h>

/* ISA troposphere limits */
#define ATMOSPHERE_MIN_ALTITUDE_M    0.0f
#define ATMOSPHERE_MAX_ALTITUDE_M    11000.0f

/* ISA constants */
#define ATMOSPHERE_T0_K              288.15f
#define ATMOSPHERE_P0_PA             101325.0f
#define ATMOSPHERE_RHO0_KG_M3        1.225f

#define ATMOSPHERE_LAPSE_RATE        0.0065f
#define ATMOSPHERE_GRAVITY_M_S2      9.80665f
#define ATMOSPHERE_AIR_GAS_CONSTANT  287.05287f
#define ATMOSPHERE_GAMMA             1.4f

typedef enum
{
    ATMOSPHERE_OK = 0,
    ATMOSPHERE_ERROR_INVALID_PARAM,
    ATMOSPHERE_ERROR_OUT_OF_RANGE
} Atmosphere_Status;

/* Atmospheric properties at a given altitude */
typedef struct
{
    float altitude_m;
    float temperature_k;
    float pressure_pa;
    float density_kg_m3;
    float speed_of_sound_m_s;
} Atmosphere_Data;

/* Calculate standard atmospheric properties at a specified altitude */
Atmosphere_Status Atmosphere_StandardAtAltitude(float altitude_m, Atmosphere_Data *data);

/* Calculate pressure altitude from static pressure.
  The result corresponds to ISA pressure altitude, not necessarily the aircraft's true geometric altitude.
*/
Atmosphere_Status Atmosphere_AltitudeFromPressure(float pressure_pa, float *altitude_m);

#endif
