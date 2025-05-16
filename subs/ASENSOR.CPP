#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\verrors.h"
#include "..\include\subs.h"

struct ANALOG_SENSOR_ENTRY
    {
    short number;
    short units;
    BITSETYPE vartype;
    TCHAR desc[ASENSOR_DESC_LEN+1];
    float high;
    float low;
    };

static ANALOG_SENSOR_ENTRY * SensorArray = 0;
static short                 NofSensors  = 0;

/***********************************************************************
*                        ANALOG_SENSOR_SHUTDOWN                        *
***********************************************************************/
void analog_sensor_shutdown( void )
{
if ( SensorArray )
    {
    delete[] SensorArray;
    SensorArray = 0;
    }

NofSensors  = 0;
}

/***********************************************************************
*                       ANALOG_SENSOR_STARTUP                          *
***********************************************************************/
BOOLEAN analog_sensor_startup( void )
{

DB_TABLE t;
short    n;
BOOLEAN  status;

status = TRUE;
if ( t.open( asensor_dbname(), ASENSOR_RECLEN, PFL ) )
    {
    n = (short) t.nof_recs();
    if ( n > 0 )
        {
        NofSensors = 0;

        SensorArray = new ANALOG_SENSOR_ENTRY[n];
        if ( SensorArray )
            {
            while ( t.get_next_record(NO_LOCK) && NofSensors < n )
                {
                SensorArray[NofSensors].number  = t.get_short( ASENSOR_NUMBER_OFFSET );
                SensorArray[NofSensors].units   = t.get_short( ASENSOR_UNITS_OFFSET );
                SensorArray[NofSensors].vartype = (BITSETYPE) t.get_long( ASENSOR_VARIABLE_TYPE_OFFSET );
                t.get_alpha( SensorArray[NofSensors].desc, ASENSOR_DESC_OFFSET, ASENSOR_DESC_LEN );
                SensorArray[NofSensors].high = t.get_float(ASENSOR_HIGH_VALUE_OFFSET);
                SensorArray[NofSensors].low = t.get_float(ASENSOR_LOW_VALUE_OFFSET);
                NofSensors++;
                }
            }
        else
            {
            NofSensors = 0;
            error_message( ANALOG_SENSOR_STARTUP, NO_MEM_ALLOC_ERROR );
            status = FALSE;
            }

        }
    t.close();
    }

return status;
}

/***********************************************************************
*                             SENSOR_INDEX                             *
***********************************************************************/
static short sensor_index( short analog_sensor_number )
{
short i;

for ( i=0; i<NofSensors; i++ )
    {
    if ( SensorArray[i].number == analog_sensor_number )
        return i;
    }

/*
-------------------------------------------------------------
I couldn't find it. Return the last sensor in the list, which
should be the NO_ANALOG_SENSOR_NUMBER sensor.
------------------------------------------------------------- */
i = NofSensors;
i--;

if ( i >= 0 )
    return i;

return NO_INDEX;
}

/***********************************************************************
*                         ANALOG_SENSOR_EXISTS                         *
***********************************************************************/
BOOLEAN analog_sensor_exists( short analog_sensor_number )
{
short i;

for ( i=0; i<NofSensors; i++ )
    {
    if ( SensorArray[i].number == analog_sensor_number )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                        ANALOG_SENSOR_VARTYPE                         *
***********************************************************************/
BITSETYPE analog_sensor_vartype( short analog_sensor_number )
{

short i;

i = sensor_index( analog_sensor_number );

if ( i != NO_INDEX )
    return SensorArray[i].vartype;

return NO_VARIABLE_TYPE;
}

/***********************************************************************
*                         ANALOG_SENSOR_UNITS                          *
***********************************************************************/
short analog_sensor_units( short analog_sensor_number )
{

short i;

i = sensor_index( analog_sensor_number );

if ( i != NO_INDEX )
    return SensorArray[i].units;

return NO_UNITS;
}

/***********************************************************************
*                       ANALOG_SENSOR_DESCRIPTION                      *
***********************************************************************/
TCHAR * analog_sensor_description( short analog_sensor_number )
{
short i;

i = sensor_index( analog_sensor_number );

if ( i != NO_INDEX )
    return SensorArray[i].desc;

return 0;
}

/***********************************************************************
*                         ANALOG_SENSOR_NUMBER                         *
***********************************************************************/
short analog_sensor_number( TCHAR * analog_sensor_desc )
{

short i;

for ( i=0; i<NofSensors; i++ )
    {
    if ( lstrcmpi(SensorArray[i].desc, analog_sensor_desc) == 0 )
        return SensorArray[i].number;
    }

return NO_ANALOG_SENSOR_NUMBER;
}

/***********************************************************************
*                         ANALOG_SENSOR_HIGH_VALUE                         *
***********************************************************************/
float analog_sensor_high_value(short analog_sensor_number)
{
short i;

i = sensor_index( analog_sensor_number );

if ( i != NO_INDEX )
    return SensorArray[i].high;

return -1.0;
}

/***********************************************************************
*                         ANALOG_SENSOR_LOW_VALUE                         *
***********************************************************************/
float analog_sensor_low_value(short analog_sensor_number)
{
short i;

i = sensor_index( analog_sensor_number );

if ( i != NO_INDEX )
    return SensorArray[i].low;

return -1.0;
}
