
#include "kalman.h"

#include <math.h>
#include <stddef.h>


/**
 * @brief Initialize a scalar Kalman filter: The first valid measurement initializes the state estimate.
 *
 * @param filter Pointer to the filter instance.
 * @param config Pointer to the filter configuration.
 *
 * @return KALMAN_OK on success.
 */
Kalman_Status Kalman_Init(Kalman_Filter *filter, const Kalman_Config *config)
{
    if ((filter == NULL) || (config == NULL))
    {
        return KALMAN_ERROR_INVALID_PARAM;
    }

    /* Validate configuration values */
    if ((!isfinite(config->process_noise)) || (!isfinite(config->measurement_noise)) || (!isfinite(config->initial_error_covariance)))
    {
        return KALMAN_ERROR_INVALID_PARAM;
    }

    if ((config->process_noise < 0.0f) || (config->measurement_noise <= 0.0f) || (config->initial_error_covariance < 0.0f))
    {
        return KALMAN_ERROR_INVALID_PARAM;
    }

    /* Initialize filter state */
    filter->estimate = 0.0f;

    filter->process_noise = config->process_noise;
    filter->measurement_noise = config->measurement_noise;
    filter->initial_error_covariance = config->initial_error_covariance;

    filter->covariance = config->initial_error_covariance;

    filter->initialized = 0U;

    return KALMAN_OK;
}


/**
 * @brief Update the Kalman filter with a new measurement.
 *
 * Scalar Kalman filter:
 *
 * Prediction:
 *     x_pred = x_previous
 *     P_pred = P_previous + Q
 *
 * Correction:
 *     K = P_pred / (P_pred + R)
 *     x = x_pred + K * (z - x_pred)
 *     P = (1 - K) * P_pred
 *
 * @param filter Pointer to the filter instance.
 * @param measurement New measurement.
 * @param filtered_value Pointer to the filtered output.
 *
 * @return KALMAN_OK on success.
 */
Kalman_Status Kalman_Update(Kalman_Filter *filter, float measurement, float *filtered_value)
{
    float predicted_covariance;
    float denominator;
    float kalman_gain;
    float innovation;
    float updated_estimate;
    float updated_covariance;

    if ((filter == NULL) || (filtered_value == NULL)) return KALMAN_ERROR_INVALID_PARAM;
    if (filter->initialized > 1U) return KALMAN_ERROR_INVALID_PARAM;
    if (!isfinite(measurement)) return KALMAN_ERROR_INVALID_MEASUREMENT;

    /* Validate filter parameters and state */
    if ((!isfinite(filter->process_noise)) || (!isfinite(filter->measurement_noise)) || (!isfinite(filter->covariance)) ||
        (!isfinite(filter->estimate)) || (filter->process_noise < 0.0f) || (filter->measurement_noise <= 0.0f) ||
        (filter->covariance < 0.0f))
    {
        return KALMAN_ERROR_INVALID_PARAM;
    }

    /* First measurement: initialize state estimate */
    if (filter->initialized == 0U)
    {
        filter->estimate = measurement;
        filter->initialized = 1U;

        *filtered_value = filter->estimate;

        return KALMAN_OK;
    }

    /* Prediction step */
    predicted_covariance = filter->covariance + filter->process_noise;
    if (!isfinite(predicted_covariance)) return KALMAN_ERROR_INVALID_PARAM;

    /* Kalman gain */
    denominator = predicted_covariance + filter->measurement_noise;
    if ((!isfinite(denominator)) || (denominator <= 0.0f)) return KALMAN_ERROR_INVALID_PARAM;

    kalman_gain = predicted_covariance / denominator;

    /* Measurement innovation */
    innovation = measurement - filter->estimate;

    /* State update */
    updated_estimate = filter->estimate + kalman_gain * innovation;

    /* Covariance update */
    updated_covariance = (1.0f - kalman_gain) * predicted_covariance;

    /* Validate results before updating filter state */
    if ((!isfinite(updated_estimate)) || (!isfinite(updated_covariance)) || (updated_covariance < 0.0f))
    {
        return KALMAN_ERROR_INVALID_PARAM;
    }

    filter->estimate = updated_estimate;
    filter->covariance = updated_covariance;

    *filtered_value = filter->estimate;

    return KALMAN_OK;
}


/**
 * @brief Reset the Kalman filter.
 *
 * The configuration parameters are preserved.
 *
 * @param filter Pointer to the filter instance.
 *
 * @return KALMAN_OK on success.
 */
Kalman_Status Kalman_Reset(Kalman_Filter *filter)
{
    if (filter == NULL) return KALMAN_ERROR_INVALID_PARAM;

    if ((!isfinite(filter->initial_error_covariance)) || (filter->initial_error_covariance < 0.0f))
    {
        return KALMAN_ERROR_INVALID_PARAM;
    }

    filter->estimate = 0.0f;
    filter->covariance = filter->initial_error_covariance;

    filter->initialized = 0U;

    return KALMAN_OK;
}
