
#ifndef KALMAN_H
#define KALMAN_H

#include <stdint.h>

/* Kalman filter operation status */
typedef enum
{
    KALMAN_OK = 0,
    KALMAN_ERROR_INVALID_PARAM,
    KALMAN_ERROR_NOT_INITIALIZED,
    KALMAN_ERROR_INVALID_MEASUREMENT
} Kalman_Status;


/* Kalman filter configuration */
typedef struct
{
    float process_noise;              /* Process noise covariance (Q) */
    float measurement_noise;          /* Measurement noise covariance (R) */
    float initial_error_covariance;   /* Initial estimation covariance (P0) */
} Kalman_Config;


/* Scalar Kalman filter state */
typedef struct
{
    float estimate;                   /* Current state estimate */
    float covariance;                 /* Current estimation covariance */

    float process_noise;              /* Q */
    float measurement_noise;          /* R */
    float initial_error_covariance;   /* P0 */

    uint8_t initialized;
} Kalman_Filter;


/* Initialize a Kalman filter */
Kalman_Status Kalman_Init(Kalman_Filter *filter, const Kalman_Config *config);

/* Process a new measurement */
Kalman_Status Kalman_Update( Kalman_Filter *filter, float measurement, float *filtered_value);

/* Reset the filter while preserving its configuration */
Kalman_Status Kalman_Reset(Kalman_Filter *filter);


#endif /* KALMAN_H */
