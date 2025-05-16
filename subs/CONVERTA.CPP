#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

/***********************************************************************
*                            CONVERT_ANALOG                            *
***********************************************************************/
float convert_analog( BOARD_DATA raw_value, ANALOG_SENSOR_SETUP_DATA * ap )
{
float value;

value = 0.0;

if ( ap )
    {
    value = float( raw_value );
    value -= ap->sub;
    value *= ap->mul;
    if ( not_float_zero(ap->div) )
        value /= ap->div;
    value += ap->add;
    }

return value;
}
