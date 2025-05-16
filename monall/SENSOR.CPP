#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\verrors.h"
#include "..\include\names.h"
#include "..\include\dbclass.h"

struct ANALOG_SENSOR_ENTRY
    {
    struct ANALOG_SENSOR_ENTRY * next;
    short number;
    struct ANALOG_SENSOR_SETUP_DATA data;
    };

struct POS_SENSOR_ENTRY
    {
    struct POS_SENSOR_ENTRY * next;
    TCHAR rod_pitch[RODPITCH_NAME_LEN+1];
    short dist_units;
    short vel_units;
    short mhz;
    struct POS_SENSOR_SETUP_DATA data;
    };

static ANALOG_SENSOR_ENTRY * AnalogSensorList = 0;
static POS_SENSOR_ENTRY    * PosSensorList    = 0;

/***********************************************************************
*                         FIND_POS_SENSOR                              *
***********************************************************************/
POS_SENSOR_SETUP_DATA * find_pos_sensor( const TCHAR * rod_pitch, short dist_units, short vel_units, short mhz )
{

POS_SENSOR_ENTRY * newpe;
POS_SENSOR_ENTRY * pe;
DB_TABLE  t;

pe = PosSensorList;
while ( pe )
    {
    /*
    --------------------------------
    See if this is the sensor I want
    -------------------------------- */
    if ( lstrcmp( pe->rod_pitch, rod_pitch) == 0 && (pe->dist_units == dist_units) && (pe->vel_units == vel_units) && (pe->mhz == mhz) )
        return &pe->data;

    /*
    ------------------------------------------------
    Exit pointing to the last entry so I can add one
    ------------------------------------------------ */
    if ( !pe->next )
        break;

    pe = pe->next;
    }

/*
-------------------------------------------------------
I didn't find a match in my list, get it from the table
------------------------------------------------------- */
newpe = 0;
t.open( psensor_dbname(), PSENSOR_RECLEN, PFL );

t.put_alpha( PSENSOR_RODPITCH_OFFSET,   rod_pitch, RODPITCH_NAME_LEN );
t.put_short( PSENSOR_DIST_UNITS_OFFSET, dist_units, UNITS_ID_LEN );
t.put_short( PSENSOR_VEL_UNITS_OFFSET,  vel_units,  UNITS_ID_LEN );
t.put_short( PSENSOR_MHZ_OFFSET,        mhz,        PSENSOR_MHZ_LEN );
t.reset_search_mode();

if ( t.get_next_key_match(4, NO_LOCK) )
    {
    newpe = new POS_SENSOR_ENTRY;
    if ( newpe )
        {
        lstrcpy( newpe->rod_pitch, rod_pitch);
        newpe->dist_units = dist_units;
        newpe->vel_units  = vel_units;
        newpe->mhz        = mhz;
        newpe->data.x4_pos_divisor = t.get_float( PSENSOR_X4_POS_DIVISOR_OFFSET );
        newpe->data.x1_pos_divisor = t.get_float( PSENSOR_X1_POS_DIVISOR_OFFSET );
        newpe->data.vel_dividend   = t.get_float( PSENSOR_VEL_FACTOR_OFFSET );
        newpe->data.min_vel        = t.get_float( PSENSOR_MIN_VEL_OFFSET );
        newpe->data.crystal_freq   = (float) mhz;
        newpe->next = 0;
        if ( pe )
            pe->next = newpe;
        else
            PosSensorList = newpe;
        }
    else
        {
        error_message( FIND_POSITION_SENSOR, NO_MEM_ALLOC_ERROR );
        }
    }

t.close();

if ( newpe )
    return &newpe->data;

return 0;
}

/***********************************************************************
*                          GET_POLY_EQ_NUMBER                          *
***********************************************************************/
BOOLEAN get_poly_eq_number( float * dest, short poly_eq_number )
{

BOOLEAN     status;
short       i;
float     * fp;
DB_TABLE    t;
FIELDOFFSET offset;

status = TRUE;
fp     = dest;

/*
---------------------------
Initialize the coefficients
--------------------------- */
for ( i=0; i<MAX_POLY_COEF; i++ )
    *(fp++) = 0.0;

if ( poly_eq_number != NO_EQUATION_NUMBER )
    {
    t.open( polyeq_dbname(), POLYEQ_RECLEN, PFL );
    t.put_short( POLYEQ_NUMBER_OFFSET, poly_eq_number, POLYEQ_NUMBER_LEN );
    if ( t.get_next_key_match(1, NO_LOCK) )
        {
        offset = POLYEQ_COEF_0_OFFSET;
        fp = dest;
        for ( i=0; i<MAX_POLY_COEF; i++ )
            {
            *fp = t.get_float( offset );
            fp++;
            offset += POLYEQ_COEF_LEN + 1;
            }
        }
    else
        {
        error_message( GET_POLY_EQ_NUMBER, NOT_FOUND );
        status = FALSE;
        }

    t.close();
    }

return status;
}

/***********************************************************************
*                           FIND_ANALOG_SENSOR                         *
***********************************************************************/
ANALOG_SENSOR_SETUP_DATA * find_analog_sensor( short sensor_to_find )
{

static struct ANALOG_SENSOR_ENTRY * AnalogSensorList = 0;

short poly_eq_number;
ANALOG_SENSOR_ENTRY * ae;
ANALOG_SENSOR_ENTRY * newae;
DB_TABLE  t;

ae = AnalogSensorList;
while ( ae )
    {
    if ( ae->number == sensor_to_find )
        return &ae->data;

    /*
    ------------------------------------------------------------
    If there are no more entries, break pointing to the last one
    ------------------------------------------------------------ */
    if ( !ae->next )
        break;

    ae = ae->next;
    }

/*
-------------------------------------------------------
I didn't find a match in my list, get it from the table
------------------------------------------------------- */
newae = 0;
t.open( asensor_dbname(),ASENSOR_RECLEN, PFL );
t.put_short( ASENSOR_NUMBER_OFFSET, sensor_to_find, ASENSOR_NUMBER_LEN );
t.reset_search_mode();
if ( t.get_next_key_match(1, FALSE) )
    {
    newae = new ANALOG_SENSOR_ENTRY;
    if ( newae )
        {
        newae->number          = sensor_to_find;
        newae->data.maxval     = t.get_float( ASENSOR_HIGH_VALUE_OFFSET );
        newae->data.minval     = t.get_float( ASENSOR_LOW_VALUE_OFFSET );
        newae->data.gain       = t.get_short( ASENSOR_GAIN_OFFSET );
        newae->data.mul        = t.get_float( ASENSOR_MUL_OFFSET );
        newae->data.sub        = t.get_float( ASENSOR_SUB_OFFSET );
        newae->data.div        = t.get_float( ASENSOR_DIV_OFFSET );
        newae->data.add        = t.get_float( ASENSOR_ADD_OFFSET );
        newae->data.m_volt_con = t.get_float( ASENSOR_MILLI_VOLT_OFFSET );
        newae->data.vartype    = (BITSETYPE) t.get_long( ASENSOR_VARIABLE_TYPE_OFFSET );
        newae->data.units      = t.get_short( ASENSOR_UNITS_OFFSET );
        poly_eq_number         = t.get_short( ASENSOR_POLY_EQ_OFFSET );
        newae->next = 0;

        get_poly_eq_number( newae->data.coef, poly_eq_number );

        /*
        --------------------------------------------------
        Either the list is empty or I am at the last entry
        -------------------------------------------------- */
        if ( ae )
            ae->next = newae;
        else
            AnalogSensorList =  newae;
        }
    else
        {
        error_message( FIND_ANALOG_SENSOR, NO_MEM_ALLOC_ERROR );
        newae = 0;
        }
    }

t.close();

if ( newae )
    return &newae->data;

return 0;
}

/***********************************************************************
*                        DEFAULT_ANALOG_SENSOR                         *
*  The ft board needs a structure even if the channel is not being     *
*  used. This just passes the address of the first struct in the list. *
***********************************************************************/
ANALOG_SENSOR_SETUP_DATA * default_analog_sensor( void )
{
return find_analog_sensor( DEFAULT_ANALOG_SENSOR );
}

/***********************************************************************
*                        SENSOR_LIST_SHUTDOWN                          *
***********************************************************************/
void sensor_list_shutdown( void )
{
void                * next_struct;
ANALOG_SENSOR_ENTRY * ae;
POS_SENSOR_ENTRY    * pe;

ae = AnalogSensorList;
while( ae )
    {
    next_struct = ae->next;
    delete ae;
    ae = (ANALOG_SENSOR_ENTRY *) next_struct;
    }

pe = PosSensorList;
while( pe )
    {
    next_struct = pe->next;
    delete pe;
    pe = (POS_SENSOR_ENTRY *) next_struct;
    }

AnalogSensorList = 0;
PosSensorList    = 0;
}

/***********************************************************************
*                        SENSOR_LIST_STARTUP                           *
***********************************************************************/
BOOLEAN sensor_list_startup( void )
{
AnalogSensorList = 0;
PosSensorList    = 0;
return TRUE;
}
