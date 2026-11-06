
#include "filtering.h"
#include <stddef.h>


/* Private Kalman filter instances */
static Kalman_Filter total_pressure_filter;
static Kalman_Filter static_pressure_filter;

/* Module initialization state */
static uint8_t filtering_initialized = 0U;


/**
 * @brief Initialize the two pressure Kalman filters.
 *
 * Each pressure sensor has an independent filter configuration.
 *
 * @param config Pointer to filtering configuration.
 *
 * @return FILTERING_OK on success.
 */
Filtering_Status Filtering_Init(const Filtering_Config *config)
{
    Kalman_Status status;

    if (config == NULL) return FILTERING_ERROR_INVALID_PARAM;

    filtering_initialized = 0U;

    /* Initialize total pressure filter */
    status = Kalman_Init(&total_pressure_filter, &config->total_pressure_config);
    if (status != KALMAN_OK) return FILTERING_ERROR_TOTAL_PRESSURE;

    /* Initialize static pressure filter */
    status = Kalman_Init(&static_pressure_filter, &config->static_pressure_config);
    if (status != KALMAN_OK) return FILTERING_ERROR_STATIC_PRESSURE;

    filtering_initialized = 1U;

    return FILTERING_OK;
}


/**
 * @brief Filter total and static pressure measurements.
 *
 * The differential pressure is calculated only after
 * both pressure measurements have been filtered.
 *
 * @param acquisition Pointer to raw acquisition data.
 * @param filtered Pointer to filtered output data.
 *
 * @return FILTERING_OK on success.
 */
Filtering_Status Filtering_Update(const Acquisition_Data *acquisition, Filtering_Data *filtered)
{
    Kalman_Status total_status = KALMAN_OK;
    Kalman_Status static_status = KALMAN_OK;

    if ((acquisition == NULL) || (filtered == NULL)) return FILTERING_ERROR_INVALID_PARAM;

    if (filtering_initialized == 0U) return FILTERING_ERROR_NOT_INITIALIZED;

    /* Reset output validity flags */
    filtered->total_valid = 0U;
    filtered->static_valid = 0U;
    filtered->differential_valid = 0U;

    /* Filter total pressure */
    if (acquisition->total_valid != 0U)
    {
        total_status = Kalman_Update( &total_pressure_filter, (float)acquisition->total.pressure_pa, &filtered->total_pressure_pa);
        if (total_status == KALMAN_OK) filtered->total_valid = 1U;
    }

    /* Filter static pressure */
    if (acquisition->static_valid != 0U)
    {
        static_status = Kalman_Update( &static_pressure_filter, (float)acquisition->static_.pressure_pa, &filtered->static_pressure_pa);
        if (static_status == KALMAN_OK) filtered->static_valid = 1U;
    }

    /* Calculate differential pressure only when both
       filtered pressure measurements are valid. */
    if ((filtered->total_valid != 0U) && (filtered->static_valid != 0U))
    {
        filtered->differential_pressure_pa = filtered->total_pressure_pa - filtered->static_pressure_pa;
        filtered->differential_valid = 1U;
    }

    /* Report sensor-specific errors */
    if ((acquisition->total_valid == 0U) || (total_status != KALMAN_OK))
    {
        return FILTERING_ERROR_TOTAL_PRESSURE;
    }

    if ((acquisition->static_valid == 0U) || (static_status != KALMAN_OK))
    {
        return FILTERING_ERROR_STATIC_PRESSURE;
    }

    return FILTERING_OK;
}

/**
 * @brief Reset both pressure filters.
 *
 * The filter configurations are preserved.
 *
 * @return FILTERING_OK on success.
 */
Filtering_Status Filtering_Reset(void)
{
    Kalman_Status total_status;
    Kalman_Status static_status;

    if (filtering_initialized == 0U) return FILTERING_ERROR_NOT_INITIALIZED;

    total_status = Kalman_Reset(&total_pressure_filter);
    static_status = Kalman_Reset(&static_pressure_filter);

    if (total_status != KALMAN_OK) return FILTERING_ERROR_TOTAL_PRESSURE;
    if (static_status != KALMAN_OK) return FILTERING_ERROR_STATIC_PRESSURE;

    return FILTERING_OK;
}


/**
 * @brief Check filtering module initialization.
 *
 * @return 1 if initialized, otherwise 0.
 */
uint8_t Filtering_IsInitialized(void)
{
    return filtering_initialized;
}
