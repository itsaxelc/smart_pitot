
#ifndef DISPLAY_H
#define DISPLAY_H


#include "i2c.h"
#include "air_data.h"

I2C_Status Display_Init(I2C_Handle *hi2c);
void Display_Update(I2C_Handle *hi2c, const AirData_Data *air_data);
void Display_Clear(I2C_Handle *hi2c);

/* Reset the sequential airspeed display */
void Display_Reset(void);


#endif
