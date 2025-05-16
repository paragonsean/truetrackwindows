#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

#include "extern.h"

/***********************************************************************
*                              IS_OUT                                  *
***********************************************************************/
static BOOLEAN is_out( short parameter_index, float value, short min_index, short max_index )
{
PARAMETER_ENTRY * p;
float low;
float high;

p = &CurrentParam.parameter[parameter_index];
if ( p->input.type != NO_PARAMETER_TYPE )
    {
    high = p->limits[max_index].value;
    low  = p->limits[min_index].value;
    if ( not_float_zero(high - low) )
        {
        if ( is_float_zero(value-low) || is_float_zero(high-value) )
            return FALSE;
        if ( value < low || value > high )
            return TRUE;
        }
    }

return FALSE;
}


/***********************************************************************
*                           IS_ALARM                                   *
***********************************************************************/
BOOLEAN is_alarm( short parameter_index, float value )
{
return is_out( parameter_index, value, ALARM_MIN, ALARM_MAX );
}


/***********************************************************************
*                              IS_WARNING                              *
***********************************************************************/
BOOLEAN is_warning( short parameter_index, float value )
{
return is_out( parameter_index, value, WARNING_MIN, WARNING_MAX );
}

