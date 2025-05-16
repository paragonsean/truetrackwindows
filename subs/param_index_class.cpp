#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\fileclas.h"
#include "..\include\iniclass.h"
#include "..\include\nameclas.h"
#include "..\include\structs.h"
#include "..\include\strarray.h"

#include "..\include\param_index_class.h"

static const TCHAR CommaChar     = TEXT( ',' );
static const TCHAR CommaString[] = TEXT( "," );

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
***********************************************************************/
void PARAM_INDEX_CLASS::init()
{
int i;
for ( i=0; i<MAX_PARMS; i++ )
    x[i] = i;
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
***********************************************************************/
PARAM_INDEX_CLASS::PARAM_INDEX_CLASS()
{
init();
}

/***********************************************************************
*                           ~PARAM_INDEX_CLASS                         *
***********************************************************************/
PARAM_INDEX_CLASS::~PARAM_INDEX_CLASS()
{
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
*                               OPERATOR[]                             *
***********************************************************************/
int & PARAM_INDEX_CLASS::operator[](int i )
{
static int empty_int = 0;

if ( i>=0 && i< MAX_PARMS )
    return x[i];

return empty_int;
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
*                            parameter_number                          *
***********************************************************************/
int PARAM_INDEX_CLASS::parameter_number( int i )
{
if ( i>=0 && i< MAX_PARMS )
    return x[i];

return 0;
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
*                                 index                                *
***********************************************************************/
int PARAM_INDEX_CLASS::index( int parameter_number )
{
int i;

for ( i=0; i<MAX_PARMS; i++ )
    {
    if ( x[i] == parameter_number )
        return i;
    }

return 0;
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
*                                  move                                *
***********************************************************************/
bool PARAM_INDEX_CLASS::move( int new_index,  int current_index )
{
int i;
int copy;

if ( new_index<0 || new_index>=MAX_PARMS )
    return false;

if ( current_index<0 || current_index>=MAX_PARMS )
    return false;

copy = x[new_index];

if ( new_index > current_index )
    {
    for ( i=new_index; i>current_index; i-- )
        x[i] = x[i-1];
    }
else
    {
    for ( i=new_index; i<current_index; i++ )
        x[i] = x[i+1];
    }

x[current_index] = copy;

return true;
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
*                                  load                                *
***********************************************************************/
bool PARAM_INDEX_CLASS::load( TCHAR * co, TCHAR * ma, TCHAR * pa )
{
int                i;
STRING_ARRAY_CLASS s;
NAME_CLASS         fn;
FILE_CLASS         f;

init();

fn.get_parameter_sort_order_file_name( co, ma, pa );
if ( fn.file_exists() )
    {
    if ( f.open_for_read(fn) )
        {
        s = f.readline();
        f.close();
        s.split( CommaString );
        s.rewind();
        i = 0;
        while ( s.next() )
            {
            x[i] = s.int_value();
            i++;
            if ( i == MAX_PARMS )
                break;
            }
        }

    }
return false;
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
*                                  load                                *
***********************************************************************/
bool PARAM_INDEX_CLASS::load( PART_NAME_ENTRY & pn )
{
return load( pn.computer, pn.machine, pn.part );
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
*                                  save                                *
***********************************************************************/
bool PARAM_INDEX_CLASS::save( TCHAR * co, TCHAR * ma, TCHAR * pa )
{
const int SLEN = MAX_PARMS * 3;
const int LAST_INDEX = MAX_PARMS - 1;
int          i;
STRING_CLASS s;
NAME_CLASS   fn;
FILE_CLASS   f;

s.init( SLEN );
for ( i=0; i<MAX_PARMS; i++ )
    {
    s += x[i];
    if ( i<LAST_INDEX )
        s += CommaChar;
    }

fn.get_parameter_sort_order_file_name( co, ma, pa );
if ( f.open_for_write(fn) )
    {
    f.writeline(s);
    f.close();
    return true;
    }

return false;
}

/***********************************************************************
*                            PARAM_INDEX_CLASS                         *
*                                  save                                *
***********************************************************************/
bool PARAM_INDEX_CLASS::save( PART_NAME_ENTRY & pn )
{
return save( pn.computer, pn.machine, pn.part );
}
