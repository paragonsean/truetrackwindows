#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\part_analog_sensor.h"
#include "..\include\subs.h"

/***********************************************************************
*                      ~PART_ANALOG_SENSOR_ENTRY                       *
***********************************************************************/
PART_ANALOG_SENSOR_ENTRY::~PART_ANALOG_SENSOR_ENTRY()
{
}

/***********************************************************************
*                     PART_ANALOG_SENSOR_ENTRY                         *
*                              READ                                    *
***********************************************************************/
BOOLEAN PART_ANALOG_SENSOR_ENTRY::read( DB_TABLE & sorc )
{
number     = sorc.get_long( ASENSOR_NUMBER_OFFSET );
units      = sorc.get_long( ASENSOR_UNITS_OFFSET );
multiplier = sorc.get_double( ASENSOR_MUL_OFFSET );
subtrahend = sorc.get_double( ASENSOR_SUB_OFFSET );
divisor    = sorc.get_double( ASENSOR_DIV_OFFSET );
adder      = sorc.get_double( ASENSOR_ADD_OFFSET );
return TRUE;
}

/***********************************************************************
*                     PART_ANALOG_SENSOR_ENTRY                         *
*                         CONVERTED_VALUE                              *
***********************************************************************/
double PART_ANALOG_SENSOR_ENTRY::converted_value( unsigned raw_analog_counts )
{
double value;

value = double( raw_analog_counts );
value -= subtrahend;
value *= multiplier;
value /= divisor;
value += adder;

return value;
}

/***********************************************************************
*                     PART_ANALOG_SENSOR_ENTRY                         *
*                              raw_count                               *
*     Convert a converted_value back to raw counts [0.0, 4095.0]       *
***********************************************************************/
double PART_ANALOG_SENSOR_ENTRY::raw_count( double x )
{
x -= adder;
x *= divisor;
x /= multiplier;
x += subtrahend;

return x;
}

/***********************************************************************
*                     PART_ANALOG_SENSOR_ENTRY                         *
*                                =                                     *
***********************************************************************/
PART_ANALOG_SENSOR_ENTRY & PART_ANALOG_SENSOR_ENTRY::operator=( const PART_ANALOG_SENSOR_ENTRY & sorc )
{
number     = sorc.number;
units      = sorc.units;
adder      = sorc.adder;
subtrahend = sorc.subtrahend;
multiplier = sorc.multiplier;
divisor    = sorc.divisor;

return (*this);
}

/***********************************************************************
*                       ~PART_ANALOG_SENSOR                            *
***********************************************************************/
PART_ANALOG_SENSOR::~PART_ANALOG_SENSOR()
{
if ( as )
    {
    delete[] as;
    n = 0;
    }
}

/***********************************************************************
*                        PART_ANALOG_SENSOR                            *
*                               GET                                    *
***********************************************************************/
BOOLEAN PART_ANALOG_SENSOR::get()
{
int      i;
DB_TABLE t;

if ( as )
    {
    delete[] as;
    n = 0;
    }

if ( t.open( asensor_dbname(), ASENSOR_RECLEN, PFL ) )
    {
    i = t.nof_recs();
    if ( i > 0 )
        {
        as = new PART_ANALOG_SENSOR_ENTRY[i];
        if ( as )
            {
            n = i;
            i = 0;
            t.rewind();
            while ( t.get_next_record(NO_LOCK) )
                {
                if ( i < n )
                    as[i].read(t);
                i++;
                }
            }
        }
    t.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                     PART_ANALOG_SENSOR_ENTRY                         *
*                             INDEX                                    *
***********************************************************************/
int PART_ANALOG_SENSOR::index( int number_to_find )
{
int i;

for ( i=0; i<n; i++ )
    if ( as[i].number == number_to_find )
        return i;

return NO_INDEX;
}

/***********************************************************************
*                     PART_ANALOG_SENSOR_ENTRY                         *
*                               []                                     *
***********************************************************************/
PART_ANALOG_SENSOR_ENTRY & PART_ANALOG_SENSOR::operator[]( int i )
{
if ( i < 0 || i >= n )
    i = NO_ANALOG_SENSOR_NUMBER;
return as[i];
}
