#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\genlist.h"
#include "..\include\names.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

#include "ctrlimit.h"

/***********************************************************************
*                         CONTROL_LIMIT_ENTRY                          *
*                                READ                                  *
***********************************************************************/
BOOLEAN CONTROL_LIMIT_ENTRY::read( DB_TABLE & t )
{
number = t.get_long( CTRLIMIT_PARM_NUMBER_OFFSET );
low    = t.get_float( CTRLIMIT_LOW_LIMIT_OFFSET );
high   = t.get_float( CTRLIMIT_HIGH_LIMIT_OFFSET );

return TRUE;
}

/***********************************************************************
*                         CONTROL_LIMIT_ENTRY                          *
*                                WRITE                                 *
***********************************************************************/
BOOLEAN CONTROL_LIMIT_ENTRY::write( DB_TABLE & t )
{
t.put_long(  CTRLIMIT_PARM_NUMBER_OFFSET,  number,  PARAMETER_NUMBER_LEN );
t.put_float( CTRLIMIT_LOW_LIMIT_OFFSET,  low,  CTRLIMIT_FLOAT_LEN );
t.put_float( CTRLIMIT_HIGH_LIMIT_OFFSET, high, CTRLIMIT_FLOAT_LEN );

return TRUE;
}

/***********************************************************************
*                         CONTROL_LIMIT_CLASS                          *
*                                EMPTY                                 *
***********************************************************************/
void CONTROL_LIMIT_CLASS::empty( void )
{

list.rewind();
while ( TRUE )
    {
    current_entry = (CONTROL_LIMIT_ENTRY *) list.next();
    if ( !current_entry )
        break;
    delete current_entry;
    }

list.remove_all();
}

/***********************************************************************
*                         CONTROL_LIMIT_CLASS                          *
***********************************************************************/
CONTROL_LIMIT_CLASS::CONTROL_LIMIT_CLASS()
{
current_entry = 0;
}

/***********************************************************************
*                         ~CONTROL_LIMIT_CLASS                         *
***********************************************************************/
CONTROL_LIMIT_CLASS::~CONTROL_LIMIT_CLASS()
{
empty();
}

/***********************************************************************
*                         CONTROL_LIMIT_CLASS                          *
*                                 NEXT                                 *
***********************************************************************/
BOOLEAN CONTROL_LIMIT_CLASS::next( void )
{
current_entry = (CONTROL_LIMIT_ENTRY *) list.next();

if ( current_entry )
    return TRUE;

return FALSE;
}
 
/***********************************************************************
*                         CONTROL_LIMIT_CLASS                          *
*                                 ADD                                  *
***********************************************************************/
BOOLEAN CONTROL_LIMIT_CLASS::add( int32 new_number, float new_low_limit, float new_high_limit )
{
CONTROL_LIMIT_ENTRY * ce;

ce = new CONTROL_LIMIT_ENTRY;
if ( !ce )
    return FALSE;

ce->number = new_number;
ce->low    = new_low_limit;
ce->high   = new_high_limit;

rewind();
while ( TRUE )
    {
    if ( !next() )
        {
        list.append( ce );
        break;
        }

    if ( current_entry->number == new_number )
        {
        current_entry->low  = new_low_limit;
        current_entry->high = new_high_limit;
        delete ce;
        break;
        }

    if ( current_entry->number > new_number )
        {
        list.insert( ce );
        break;
        }
    }

return TRUE;
}

/***********************************************************************
*                         CONTROL_LIMIT_CLASS                          *
*                                 FIND                                 *
***********************************************************************/
BOOLEAN CONTROL_LIMIT_CLASS::find( int32 number_to_find )
{

list.rewind();
while ( next() )
    {
    if ( current_entry->number == number_to_find )
        return TRUE;
    }
return FALSE;
}

/***********************************************************************
*                         CONTROL_LIMIT_CLASS                          *
*                                 READ                                 *
***********************************************************************/
BOOLEAN CONTROL_LIMIT_CLASS::read( TCHAR * filename )
{

CONTROL_LIMIT_ENTRY * ce;
DB_TABLE t;
BOOLEAN  status;

empty();
status = TRUE;

if ( file_exists(filename) )
    {
    if ( t.open(filename, CTRLIMIT_RECLEN, PFL) )
        {
        while ( t.get_next_record(NO_LOCK) )
            {
            ce = new CONTROL_LIMIT_ENTRY;
            if ( !ce )
                {
                status = FALSE;
                break;
                }
            ce->read( t );
            list.append( ce );
            }

        t.close();
        }
    }

return status;
}

/***********************************************************************
*                         CONTROL_LIMIT_CLASS                          *
*                                WRITE                                 *
***********************************************************************/
BOOLEAN CONTROL_LIMIT_CLASS::write( TCHAR * filename )
{

CONTROL_LIMIT_ENTRY * ce;
DB_TABLE t;

if ( !file_exists(filename ) )
    t.create( filename );

if ( t.open(filename, CTRLIMIT_RECLEN, FL) )
    {
    t.empty();
    list.rewind();
    while ( TRUE )
        {
        ce = (CONTROL_LIMIT_ENTRY *) list.next();
        if ( !ce )
            break;
        ce->write( t );
        t.rec_append();
        }

    t.close();
    return TRUE;
    }

return FALSE;
}

