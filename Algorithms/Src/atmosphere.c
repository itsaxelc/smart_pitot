
#include "atmosphere.h"

#include <math.h>
#include <stddef.h>

/* Calculate atmospheric properties using the ISA tropospheric model.
    Valid altitude range: 0 to 11000 m.
*/
Atmosphere_Status Atmosphere_StandardAtAltitude(float altitude_m, Atmosphere_Data *data)
{
    float temperature;
    float pressure;
    float density;
    float speed_of_sound;

    float temperature_ratio;
    float pressure_exponent;

    if (data == NULL) return ATMOSPHERE_ERROR_INVALID_PARAM;
    if (!isfinite(altitude_m)) return ATMOSPHERE_ERROR_INVALID_PARAM;

    if ((altitude_m < ATMOSPHERE_MIN_ALTITUDE_M) || (altitude_m > ATMOSPHERE_MAX_ALTITUDE_M))
    {
        return ATMOSPHERE_ERROR_OUT_OF_RANGE;
    }

    /* Temperature: T = T0-L*h */
    temperature = ATMOSPHERE_T0_K-ATMOSPHERE_LAPSE_RATE*altitude_m;

    if (temperature <= 0.0f) return ATMOSPHERE_ERROR_OUT_OF_RANGE;

    /* Pressure: P = P0 * (T/T0)^(g0/(R*L)) */
    temperature_ratio = temperature / ATMOSPHERE_T0_K;

    pressure_exponent = ATMOSPHERE_GRAVITY_M_S2/(ATMOSPHERE_AIR_GAS_CONSTANT * ATMOSPHERE_LAPSE_RATE);

    pressure = ATMOSPHERE_P0_PA*powf(temperature_ratio, pressure_exponent);

    /* Density: rho = P/(R*T) */
    density = pressure/(ATMOSPHERE_AIR_GAS_CONSTANT*temperature);

    /* Speed of sound: a = sqrt(gamma*R*T)*/
    speed_of_sound = sqrtf(ATMOSPHERE_GAMMA*ATMOSPHERE_AIR_GAS_CONSTANT*temperature);

    if (!isfinite(pressure) || !isfinite(density) || !isfinite(speed_of_sound))
    {
        return ATMOSPHERE_ERROR_OUT_OF_RANGE;
    }

    /* Store calculated values */
    data->altitude_m = altitude_m;
    data->temperature_k = temperature;
    data->pressure_pa = pressure;
    data->density_kg_m3 = density;
    data->speed_of_sound_m_s = speed_of_sound;

    return ATMOSPHERE_OK;
}


/* Calculate ISA pressure altitude from static pressure: h = (T0/L) * [1 - (P/P0)^(R*L/g0)] 
Where:
    P = Measured static pressure (Pa)
    P0 = Sea-level standard pressure (101325 Pa)
    L = Temperature lapse rate (0.0065 K/m)
    T0 = Sea-level standard temperature (288.15 K)
    g0 = Standard gravitational acceleration (9.80665 m/s²)
    R = Specific gas constant for dry air (287.05287 J/(kg·K))*/
Atmosphere_Status Atmosphere_AltitudeFromPressure(float pressure_pa, float *altitude_m)
{
    float pressure_ratio;
    float altitude_exponent;
    float altitude;
    /*
        Minimum ISA pressure at the maximum supported altitude:
        P_min = P0 * (1 - (L*h_max)/T0)^(g0/(R*L))

    Where:
        P0 = Sea-level standard pressure (101325 Pa)
        L = Temperature lapse rate (0.0065 K/m)
        h_max = Maximum supported altitude (11000 m)
        T0 = Sea-level standard temperature (288.15 K)
        g0 = Standard gravitational acceleration (9.80665 m/s²)
        R = Specific gas constant for dry air (287.05287 J/(kg·K))
    */
    const float minimum_pressure = ATMOSPHERE_P0_PA*powf((ATMOSPHERE_T0_K-ATMOSPHERE_LAPSE_RATE*ATMOSPHERE_MAX_ALTITUDE_M)/ATMOSPHERE_T0_K,
            ATMOSPHERE_GRAVITY_M_S2/(ATMOSPHERE_AIR_GAS_CONSTANT*ATMOSPHERE_LAPSE_RATE));

    if (altitude_m == NULL) return ATMOSPHERE_ERROR_INVALID_PARAM;
    if (!isfinite(pressure_pa)) return ATMOSPHERE_ERROR_INVALID_PARAM;

    /* Pressure must correspond to the supported altitude range */
    if ((pressure_pa > ATMOSPHERE_P0_PA) || (pressure_pa < minimum_pressure) || (pressure_pa <= 0.0f))
    {
        return ATMOSPHERE_ERROR_OUT_OF_RANGE;
    }

    pressure_ratio = pressure_pa/ATMOSPHERE_P0_PA;
    altitude_exponent = (ATMOSPHERE_AIR_GAS_CONSTANT*ATMOSPHERE_LAPSE_RATE)/ATMOSPHERE_GRAVITY_M_S2;
    altitude = (ATMOSPHERE_T0_K/ATMOSPHERE_LAPSE_RATE)*(1.0f - powf(pressure_ratio, altitude_exponent));

    if (!isfinite(altitude))
    {
        return ATMOSPHERE_ERROR_OUT_OF_RANGE;
    }

    *altitude_m = altitude;

    return ATMOSPHERE_OK;
}
