#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\subs.h"
 
#include "fixaxis.h"

/***********************************************************************
*                          FIXED_AXIS_ENTRY                            *
*                               WRITE                                  *
***********************************************************************/
BOOLEAN FIXED_AXIS_ENTRY::write( DB_TABLE & dest )
{
dest.put_boolean( FIXAXIS_ENABLED_OFFSET, enabled );
dest.put_float( FIXAXIS_MIN_OFFSET, min, FIXAXIS_FLOAT_LEN );
dest.put_float( FIXAXIS_MAX_OFFSET, max, FIXAXIS_FLOAT_LEN );
return TRUE;
}

/***********************************************************************
*                          FIXED_AXIS_ENTRY                            *
*                                READ                                  *
***********************************************************************/
BOOLEAN FIXED_AXIS_ENTRY::read( DB_TABLE & sorc )
{
enabled = sorc.get_boolean( FIXAXIS_ENABLED_OFFSET );
min     = sorc.get_float(   FIXAXIS_MIN_OFFSET );
max     = sorc.get_float(   FIXAXIS_MAX_OFFSET );

return TRUE;
}

/***********************************************************************
*                           FIXED_AXIS_CLASS                           *
*                                EMPTY                                 *
***********************************************************************/
void FIXED_AXIS_CLASS::empty( void )
{
n = 0;

if ( f )
    {
    delete[] f;
    f = 0;
    }

}

/***********************************************************************
*                           FIXED_AXIS_CLASS                           *
*                                 INIT                                 *
***********************************************************************/
BOOLEAN FIXED_AXIS_CLASS::init( int32 nof_entries )
{
empty();

if ( nof_entries > 0 )
    {
    f = new FIXED_AXIS_ENTRY[nof_entries];
    if ( f )
        {
        n = nof_entries;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                           FIXED_AXIS_CLASS                           *
*                                  []                                  *
***********************************************************************/
FIXED_AXIS_ENTRY & FIXED_AXIS_CLASS::operator[]( int32 i )
{
static FIXED_AXIS_ENTRY dummy;

if ( i >= 0 && i < n )
    return f[i];

return dummy;
}

/***********************************************************************
*                           FIXED_AXIS_CLASS                           *
*                                 READ                                 *
***********************************************************************/
BOOLEAN FIXED_AXIS_CLASS::read( TCHAR * filename )
{
int32    i;
int32    nr;
DB_TABLE t;
BOOLEAN  status;

empty();
status = FALSE;

if ( file_exists(filename) )
    {
    if ( t.open(filename, FIXAXIS_RECLEN, PFL) )
        {
        nr = (int32) t.nof_recs();
        if ( nr > 0 )
            {
            if ( init(nr) )
                {
                i = 0;
                while ( t.get_next_record(NO_LOCK) && i < nr )
                    {
                    f[i].read( t );
                    i++;
                    }
                status = TRUE;
                }
            }
        t.close();
        }
    }

return status;
}
/***********************************************************************
*                           FIXED_AXIS_CLASS                           *
*                                WRITE                                 *
***********************************************************************/
BOOLEAN FIXED_AXIS_CLASS::write( TCHAR * filename )
{
int32 i;
DB_TABLE t;

if ( !file_exists(filename ) )
    t.create( filename );

if ( t.open(filename, FIXAXIS_RECLEN, FL) )
    {
    t.empty();
    for ( i=0; i<n; i++ )
        {
        f[i].write( t );
        t.rec_append();
        }

    t.close();
    return TRUE;
    }

return FALSE;
}

