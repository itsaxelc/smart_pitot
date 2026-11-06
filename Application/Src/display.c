
#include "display.h"
#include "lcd1602.h"

#include <stdio.h>
#include <stddef.h>
#include <math.h>

#define DISPLAY_LINE_LENGTH       16U
#define DISPLAY_BUFFER_SIZE      17U

/* Sequential display configuration */
#define DISPLAY_SPEED_COUNT       4U

typedef enum
{
    DISPLAY_IAS = 0,
    DISPLAY_CAS,
    DISPLAY_EAS,
    DISPLAY_TAS
} Display_SpeedType;

static Display_SpeedType current_speed = DISPLAY_IAS;

/* Write exactly 16 characters to one LCD row */
static void Display_WriteLine(I2C_Handle *hi2c, uint8_t row, const char *text)
{
    uint8_t i = 0U;
    LCD1602_SetCursor(hi2c, row, 0U);
    while ((i < DISPLAY_LINE_LENGTH) && (text[i] != '\0'))
    {
        LCD1602_WriteChar(hi2c, text[i]);
        i++;
    }
    while (i < DISPLAY_LINE_LENGTH)
    {
        LCD1602_WriteChar(hi2c, ' ');
        i++;
    }
}

/* Select the speed to display */
static float Display_GetSpeed(const AirData_Data *data)
{
    switch (current_speed)
    {
        case DISPLAY_IAS: return data->indicated_airspeed_m_s;
        case DISPLAY_CAS: return data->calibrated_airspeed_m_s;
        case DISPLAY_EAS: return data->equivalent_airspeed_m_s;
        case DISPLAY_TAS: return data->true_airspeed_m_s;
        default: return 0.0f;
    }
}

/* Return the selected speed label */
static const char *Display_GetSpeedLabel(void)
{
    switch (current_speed)
    {
        case DISPLAY_IAS: return "IAS";
        case DISPLAY_CAS: return "CAS";
        case DISPLAY_EAS: return "EAS";
        case DISPLAY_TAS: return "TAS";
        default: return "---";
    }
}

/* Initialize LCD */
I2C_Status Display_Init(I2C_Handle *hi2c)
{
    I2C_Status status;

    if (hi2c == NULL) return I2C_ERROR;

    status = LCD1602_Init(hi2c);
    if (status == 0)
    {
        LCD1602_Clear(hi2c);
        current_speed = DISPLAY_IAS;
    }
    return status;
}

/* Update sequential airspeed and altitude display */
void Display_Update(I2C_Handle *hi2c, const AirData_Data *air_data)
{
    char line1[DISPLAY_BUFFER_SIZE] = {0};
    char line2[DISPLAY_BUFFER_SIZE] = {0};

    float speed_m_s;
    unsigned int speed_kmh;
    unsigned int altitude_m;
    unsigned int mach_x100;

    if ((hi2c == NULL) || (air_data == NULL)) return;

    /* First line: sequential airspeed */
    if ((air_data->airspeed_valid != 0U) && isfinite(speed_m_s = Display_GetSpeed(air_data)) && (speed_m_s >= 0.0f))
    {
        speed_kmh = (unsigned int)(speed_m_s * 3.6f + 0.5f);
        (void)snprintf(line1, sizeof(line1), "%s:%3u km/h", Display_GetSpeedLabel(), speed_kmh);
    }
    else
    {
        (void)snprintf(line1, sizeof(line1), "%s:--- km/h", Display_GetSpeedLabel());
    }

    /* Second line: pressure altitude and Mach */
    if ((air_data->altitude_valid != 0U) && isfinite(air_data->pressure_altitude_m) && (air_data->pressure_altitude_m >= 0.0f))
    {
        /* This expression is used to round a floating-point value to the nearest integer 
        before converting it to an unsigned int*/
        altitude_m = (unsigned int)(air_data->pressure_altitude_m + 0.5f);
        if ((air_data->airspeed_valid != 0U) && isfinite(air_data->mach_number) && (air_data->mach_number >= 0.0f))
        {
            mach_x100 = (unsigned int)(air_data->mach_number * 100.0f + 0.5f);

            if (altitude_m > 99999U) altitude_m = 99999U;
            if (mach_x100 > 999U) mach_x100 = 999U;
            (void)snprintf(line2, sizeof(line2), "H:%5um M:%1u.%02u", altitude_m, mach_x100 / 100U, mach_x100 % 100U);
        }
        else
        {
            (void)snprintf(line2, sizeof(line2), "H:%5um M:--", altitude_m);
        }
    }
    else
    {
        (void)snprintf(line2, sizeof(line2), "H:-----m M:--");
    }

    Display_WriteLine(hi2c, 0U, line1);
    Display_WriteLine(hi2c, 1U, line2);

    /* Advance to the next airspeed */
    current_speed++;
    if (current_speed >= DISPLAY_SPEED_COUNT)
    {
        current_speed = DISPLAY_IAS;
    }
}

/* Reset sequential display */
void Display_Reset(void)
{
    current_speed = DISPLAY_IAS;
}

/* Clear LCD */
void Display_Clear(I2C_Handle *hi2c)
{
    if (hi2c != NULL)
    {
        LCD1602_Clear(hi2c);
    }
}
