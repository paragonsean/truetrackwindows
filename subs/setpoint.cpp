#include <windows.h>
#include "..\include\visiparm.h"
#include "..\include\fileclas.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\setpoint.h"

static SETPOINT_ENTRY empty_entry;

static TCHAR CommaChar = TEXT( ','  );
static TCHAR NullChar  = TEXT( '\0' );

static TCHAR CommaString[] = TEXT( "," );
static TCHAR EmptyString[] = TEXT( ""  );

/***********************************************************************
*                             empty                                    *
***********************************************************************/
void SETPOINT_CLASS::empty()
{
int i;
for ( i=0; i<n; i++ )
    {
    if ( !array[i].value.isempty() )
        array[i].value = EmptyString;

    if ( !array[i].desc.isempty() )
        array[i].desc = EmptyString;
    }
}

/***********************************************************************
*                            cleanup                                   *
***********************************************************************/
void SETPOINT_CLASS::cleanup()
{

if ( array )
    {
    n = 0;
    delete[] array;
    array = 0;
    }
}

/***********************************************************************
*                          ~SETPOINT_CLASS                             *
***********************************************************************/
SETPOINT_CLASS::~SETPOINT_CLASS()
{
cleanup();
}

/***********************************************************************
*                             SETSIZE                                  *
***********************************************************************/
BOOLEAN SETPOINT_CLASS::setsize( int new_n )
{
int              i;
SETPOINT_ENTRY * se;

if ( n == new_n )
    return TRUE;

if ( new_n <= 0 )
    {
    cleanup();
    return TRUE;
    }

se = new SETPOINT_ENTRY[new_n];
if ( se )
    {
    for ( i=0; i<n; i++ )
        {
        if ( i < new_n )
            {
            if ( !array[i].value.isempty() )
                se[i].value = array[i].value;
            if ( !array[i].desc.isempty() )
                se[i].desc = array[i].desc;
            }
        }

    cleanup();
    array = se;
    n = new_n;
    }

return n;
}

/***********************************************************************
*                                 GET                                  *
***********************************************************************/
int SETPOINT_CLASS::get( TCHAR * path )
{
FILE_CLASS       f;
int              i;
SETPOINT_ENTRY * se;
TCHAR          * cp;
TCHAR          * s;

cleanup();

if ( f.open_for_read(path) )
    {
    i = f.nof_lines();
    if ( i > 0 )
        {
        se = new SETPOINT_ENTRY[i];
        if ( se )
            {
            array = se;
            n = i;
            }

        i = 0;
        while ( i < n )
            {
            s = f.readline();
            if ( !s )
                break;

            cp = findchar( CommaChar, s );

            if ( cp )
                {
                *cp = NullChar;
                array[i].desc = s;

                cp++;
                array[i].value = cp;

                i++;
                }
            }
        n = i;
        }
    f.close();
    }

return n;
}

/***********************************************************************
*                                  []                                  *
***********************************************************************/
SETPOINT_ENTRY & SETPOINT_CLASS::operator[]( int i )
{
if ( i<0 || i >= n )
    return empty_entry;

return array[i];
}

/***********************************************************************
*                                 PUT                                  *
***********************************************************************/
BOOLEAN SETPOINT_CLASS::put( TCHAR * path )
{
FILE_CLASS   f;
STRING_CLASS s;
int i;

s.init( 50 );
if ( f.open_for_write(path) )
    {
    for( i=0; i<n; i++ )
        {
        s = array[i].desc;
        s += CommaString;
        s += array[i].value;
        f.writeline( s.text() );
        }
    f.close();
    return TRUE;
    }

return FALSE;
}
