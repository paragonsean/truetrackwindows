#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\subs.h"

#include "asensor.h"

/***********************************************************************
*                         ANALOG_SENSOR_CLASS                          *
*                             BOARD_VALUE                              *
*                                                                      *
*                    target_psi - low_psi                              *
*    x = low_volts + -------------------- * delta_volts * 1000         *
*                         delta_psi                                    *
*                                                                      *
***********************************************************************/
BOARD_DATA ANALOG_SENSOR_CLASS::board_value( double y )
{

y -= low_pressure;
y *= delta_volts;
y /= delta_pressure;
y += low_volts;
y *= 1000.0;
 
return (BOARD_DATA) y;
}

/***********************************************************************
*                         ANALOG_SENSOR_CLASS                          *
*                              TRUE_VALUE                              *
*                                                                      *
*          x               (volts - low_volts)                         *
* volts = ----    value =  ------------------- * delta_psi + low_psi   *
*         1000                 delta_volts                             *
*                                                                      *
***********************************************************************/
double ANALOG_SENSOR_CLASS::true_value( BOARD_DATA x )
{
double y;

/*
-------------------------------------------------------------
The suretrak reports a number in [0,5000] equal to [0,5]volts
------------------------------------------------------------- */
y =  x / 1000.0;

y -= low_volts;
y *= delta_pressure;
y /= delta_volts;
y += low_pressure;

return y;
}

/***********************************************************************
*                         ANALOG_SENSOR_CLASS                          *
*                                 READ                                 *
***********************************************************************/
BOOLEAN ANALOG_SENSOR_CLASS::read( DB_TABLE & t )
{
double s;

low_pressure   = t.get_double( ASENSOR_LOW_VALUE_OFFSET );
delta_pressure = t.get_double( ASENSOR_HIGH_VALUE_OFFSET );
delta_pressure -= low_pressure;
if ( is_zero(delta_pressure) )
    delta_pressure = 1.0;

s = t.get_double( ASENSOR_SUB_OFFSET );
if ( is_zero(s) )
    low_volts = 0.0;
else
    low_volts = 1.0;

delta_volts = 5.0 - low_volts;

return TRUE;
}
