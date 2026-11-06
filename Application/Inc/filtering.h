
#ifndef FILTERING_H
#define FILTERING_H


#include <stdint.h>

#include "acquisition.h"
#include "kalman.h"


/* Filtering module status */
typedef enum
{
    FILTERING_OK = 0,
    FILTERING_ERROR_INVALID_PARAM,
    FILTERING_ERROR_NOT_INITIALIZED,
    FILTERING_ERROR_TOTAL_PRESSURE,
    FILTERING_ERROR_STATIC_PRESSURE
} Filtering_Status;


/* Filtering configuration */
typedef struct
{
    Kalman_Config total_pressure_config;
    Kalman_Config static_pressure_config;
} Filtering_Config;


/* Filtered pressure measurements */
typedef struct
{
    float total_pressure_pa;
    float static_pressure_pa;
    float differential_pressure_pa;

    uint8_t total_valid;
    uint8_t static_valid;
    uint8_t differential_valid;
} Filtering_Data;


/* Initialize both pressure filters */
Filtering_Status Filtering_Init(const Filtering_Config *config);

/* Filter acquired pressure measurements */
Filtering_Status Filtering_Update(const Acquisition_Data *acquisition, Filtering_Data *filtered);

/* Reset both Kalman filters */
Filtering_Status Filtering_Reset(void);


/* Check filtering module initialization */
uint8_t Filtering_IsInitialized(void);


#endif /* FILTERING_H */
