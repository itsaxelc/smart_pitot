
#include "air_data.h"
#include "atmosphere.h"

#include <math.h>
#include <stddef.h>


/* Here is a glossary of all the physical variables for a better understanding
    P_t    : Total pressure (Pa)
    P_s    : Static pressure (Pa)
    q      : Dynamic pressure, P_t - P_s (Pa)
    P_0    : Standard sea-level pressure (Pa)
    T      : Local atmospheric temperature (K)
    T_0    : Standard sea-level temperature (K)
    rho    : Local air density (kg/m3)
    rho_0  : Standard sea-level air density (kg/m3)
    L      : Temperature lapse rate (K/m)
    g_0    : Standard gravitational acceleration (m/s2)
    R      : Specific gas constant for dry air (J/(kg.K))
    gamma  : Specific heat ratio (Cp/Cv)
    a      : Local speed of sound (m/s)
    a_0    : Standard sea-level speed of sound (m/s)
    h      : Altitude (m)
    M      : Mach number (dimensionless)
    IAS    : Indicated Airspeed (m/s)
    CAS    : Calibrated Airspeed (m/s)
    EAS    : Equivalent Airspeed (m/s)
    TAS    : True Airspeed (m/s)
*/

AirData_Status AirData_Calculate(const Filtering_Data *filtered, AirData_Data *data)
{
    Atmosphere_Data atmosphere;

    float static_pressure;
    float dynamic_pressure;

    float mach_squared;
    float mach;

    float sea_level_speed_of_sound;
    float calibrated_mach_squared;

    float indicated_airspeed;
    float calibrated_airspeed;
    float equivalent_airspeed;
    float true_airspeed;

    Atmosphere_Status atmosphere_status;

    if ((filtered == NULL) || (data == NULL))
    {
        return AIR_DATA_ERROR_INVALID_PARAM;
    }

    /* Initialize output validity flags */
    data->altitude_valid = 0U;
    data->airspeed_valid = 0U;

    data->indicated_airspeed_m_s = 0.0f;
    data->calibrated_airspeed_m_s = 0.0f;
    data->equivalent_airspeed_m_s = 0.0f;
    data->true_airspeed_m_s = 0.0f;

    data->mach_number = 0.0f;
    data->pressure_altitude_m = 0.0f;

    data->dynamic_pressure_pa = 0.0f;
    data->air_density_kg_m3 = 0.0f;
    data->ambient_temperature_k = 0.0f;
    data->speed_of_sound_m_s = 0.0f;

    /* Check pressure measurement validity */
    if ((filtered->total_valid == 0U) || (filtered->static_valid == 0U) || (filtered->differential_valid == 0U))
    {
        return AIR_DATA_ERROR_INVALID_PRESSURE;
    }

    static_pressure = filtered->static_pressure_pa;
    dynamic_pressure = filtered->differential_pressure_pa;

    if (!isfinite(static_pressure) || !isfinite(dynamic_pressure) || !isfinite(filtered->total_pressure_pa))
    {
        return AIR_DATA_ERROR_INVALID_PRESSURE;
    }

    if (static_pressure <= 0.0f)
    {
        return AIR_DATA_ERROR_INVALID_PRESSURE;
    }

    /* Calculate pressure altitude from static pressure */
    atmosphere_status = Atmosphere_AltitudeFromPressure(static_pressure, &data->pressure_altitude_m);
    if (atmosphere_status != ATMOSPHERE_OK) return AIR_DATA_ERROR_ATMOSPHERE;

    /* Calculate ISA atmospheric properties at pressure altitude */
    atmosphere_status = Atmosphere_StandardAtAltitude(data->pressure_altitude_m, &atmosphere);
    if (atmosphere_status != ATMOSPHERE_OK) return AIR_DATA_ERROR_ATMOSPHERE;

    data->air_density_kg_m3 = atmosphere.density_kg_m3;
    data->ambient_temperature_k = atmosphere.temperature_k;
    data->speed_of_sound_m_s = atmosphere.speed_of_sound_m_s;

    data->altitude_valid = 1U;

    /* Negative dynamic pressure indicates an invalid pitot-static measurement or sensor offset */
    if (dynamic_pressure < 0.0f) return AIR_DATA_ERROR_NEGATIVE_DYNAMIC_PRESSURE;

    data->dynamic_pressure_pa = dynamic_pressure;

    /* IAS (Indicated Airspeed): IAS = sqrt(2 * q / rho0)
        This is the ideal incompressible airspeed approximation.
    */
    indicated_airspeed = sqrtf((2.0f * dynamic_pressure)/ATMOSPHERE_RHO0_KG_M3);

    /* Mach number from the compressible pitot equation:
        M = sqrt(2/(gamma-1)*[(1+q/Ps)^((gamma-1)/gamma)-1])
    */
    mach_squared=(2.0f/(ATMOSPHERE_GAMMA-1.0f))*
        (powf(1.0f + dynamic_pressure/static_pressure,(ATMOSPHERE_GAMMA - 1.0f)/ATMOSPHERE_GAMMA)-1.0f);

    if ((mach_squared < 0.0f) || !isfinite(mach_squared)) return AIR_DATA_ERROR_INVALID_PRESSURE;
    mach = sqrtf(mach_squared);

    /* CAS (Calibrated Airspeed):
    Calculate the equivalent Mach number at sea-level standard pressure using the same impact pressure.
    CAS = a0 * sqrt(2/(gamma-1)*[(1 + q/P0)^((gamma-1)/gamma)-1])*/
    sea_level_speed_of_sound = sqrtf(ATMOSPHERE_GAMMA * ATMOSPHERE_AIR_GAS_CONSTANT * ATMOSPHERE_T0_K);

    calibrated_mach_squared =(2.0f / (ATMOSPHERE_GAMMA - 1.0f)) *
        (powf(1.0f + dynamic_pressure/ATMOSPHERE_P0_PA, (ATMOSPHERE_GAMMA-1.0f)/ATMOSPHERE_GAMMA)-1.0f);

    if ((calibrated_mach_squared < 0.0f) || !isfinite(calibrated_mach_squared))
    {
        return AIR_DATA_ERROR_INVALID_PRESSURE;
    }

    calibrated_airspeed=sea_level_speed_of_sound*sqrtf(calibrated_mach_squared);

    /* TAS (True Airspeed): TAS = Mach*local speed of sound */
    true_airspeed = mach * atmosphere.speed_of_sound_m_s;

    /* EAS (Equivalent Airspeed): EAS = TAS * sqrt(rho/rho0) */
    equivalent_airspeed = true_airspeed*sqrtf(atmosphere.density_kg_m3/ATMOSPHERE_RHO0_KG_M3);

    /* Validate calculated values */
    if (!isfinite(indicated_airspeed) || !isfinite(calibrated_airspeed) || !isfinite(equivalent_airspeed) ||
        !isfinite(true_airspeed) || !isfinite(mach))
    {
        return AIR_DATA_ERROR_INVALID_PRESSURE;
    }

    /* Store final results */
    data->indicated_airspeed_m_s = indicated_airspeed;
    data->calibrated_airspeed_m_s = calibrated_airspeed;
    data->equivalent_airspeed_m_s = equivalent_airspeed;
    data->true_airspeed_m_s = true_airspeed;

    data->mach_number = mach;
    data->airspeed_valid = 1U;

    return AIR_DATA_OK;
}