#include <windows.h>
#include "..\include\visiparm.h"
#include "..\include\subs.h"
#include "..\include\fileclas.h"
#include "..\include\names.h"
#include "..\include\gpvar.h"

static const int32 MAX_RECORD_LEN = GPVAR_NUMBER_LEN+1+5*(MAX_FLOAT_LEN+1)+MAX_INTEGER_LEN+1+GPVAR_NAME_LEN;

static const TCHAR NoGpvarName[] = NO_GPVAR_NAME;

static const TCHAR CommaChar    = TEXT( ',' );
static const TCHAR NullChar     = TEXT( '\0' );
static const TCHAR ZeroString[] = TEXT( "0" );
static const TCHAR OneString[]  = TEXT( "1" );
static GPVAR_ENTRY EmptyEntry;

/***********************************************************************
*                           COPY_ONE_VARIABLE                          *
*                                                                      *
*     Copy until I encounter a comma or a NULL. Return a pointer       *
*     to the start of the next string if not a NULL.                   *
*                                                                      *
***********************************************************************/
static TCHAR * copy_one_variable( TCHAR * dest, TCHAR * sorc )
{
while ( *sorc != CommaChar && *sorc != NullChar )
    {
    *dest = *sorc;
    sorc++;
    dest++;
    }
*dest = NullChar;
if ( *sorc != NullChar )
    sorc++;

return sorc;
}

/***********************************************************************
*                              GPVAR_ENTRY                             *
***********************************************************************/
GPVAR_ENTRY::GPVAR_ENTRY()
{

lstrcpy( number, ZeroString  );
lstrcpy( name,   NoGpvarName );
lstrcpy( value,  ZeroString  );
lstrcpy( sub,    ZeroString  );
lstrcpy( mul,    OneString   );
lstrcpy( div,    OneString   );
lstrcpy( add,    ZeroString  );
type = GPVAR_OTHER_TYPE;

}

/***********************************************************************
*                             ~GPVAR_ENTRY                             *
***********************************************************************/
GPVAR_ENTRY::~GPVAR_ENTRY()
{
}

/***********************************************************************
*                              GPVAR_ENTRY                             *
*                                  =                                   *
***********************************************************************/
void GPVAR_ENTRY::operator=( const GPVAR_ENTRY & sorc )
{
lstrcpy( number, sorc.number );
lstrcpy( name,   sorc.name   );
lstrcpy( value,  sorc.value  );
lstrcpy( sub,    sorc.sub    );
lstrcpy( mul,    sorc.mul    );
lstrcpy( div,    sorc.div    );
lstrcpy( add,    sorc.add    );
type = sorc.type;
}

/***********************************************************************
*                              GPVAR_ENTRY                             *
*                                RECORD                                *
***********************************************************************/
TCHAR * GPVAR_ENTRY::record( void )
{
static TCHAR buf[MAX_RECORD_LEN+1];
TCHAR * cp;

cp = copy_w_char( buf, number, CommaChar );
cp = copy_w_char( cp,  name,   CommaChar );
cp = copy_w_char( cp,  value,  CommaChar );
cp = copy_w_char( cp,  sub,    CommaChar );
cp = copy_w_char( cp,  mul,    CommaChar );
cp = copy_w_char( cp,  div,    CommaChar );
cp = copy_w_char( cp,  add,    CommaChar );
copystring(  cp,  int32toasc(type) );

return buf;
}

/***********************************************************************
*                              GPVAR_ENTRY                             *
*                                 READ                                 *
***********************************************************************/
BOOLEAN GPVAR_ENTRY::read( TCHAR * sorc )
{
TCHAR * cp;

if ( sorc )
    {
    cp = copy_one_variable( number, sorc );
    cp = copy_one_variable( name,   cp   );
    cp = copy_one_variable( value,  cp   );
    cp = copy_one_variable( sub,    cp   );
    cp = copy_one_variable( mul,    cp   );
    cp = copy_one_variable( div,    cp   );
    cp = copy_one_variable( add,    cp   );
    type = asctoint32( cp );

    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                              GPVAR_ENTRY                             *
*                            VARIABLE_NUMBER                           *
***********************************************************************/
BOARD_DATA GPVAR_ENTRY::variable_number( void )
{
return (BOARD_DATA) asctoul( number );
}

/***********************************************************************
*                              GPVAR_ENTRY                             *
*                            VARIABLE_VALUE                            *
***********************************************************************/
BOARD_DATA GPVAR_ENTRY::variable_value( void )
{
double x;

x  = extdouble( value );
x -= extdouble( sub );
x *= extdouble( mul );
x /= extdouble( div );
x += extdouble( add );

return (BOARD_DATA) x;
}

/***********************************************************************
*                              GPVAR_CLASS                             *
***********************************************************************/
GPVAR_CLASS::GPVAR_CLASS()
{
int32 i;
int32 n;

n = MIN_GPVAR_NUMBER;
for ( i=0; i<NOF_GPVARS; i++ )
    {
    lstrcpy( varlist[i].number, int32toasc(n) );
    n++;
    }

}

/***********************************************************************
*                             ~GPVAR_CLASS                             *
***********************************************************************/
GPVAR_CLASS::~GPVAR_CLASS()
{
}

/***********************************************************************
*                              GPVAR_CLASS                             *
*                                  =                                   *
***********************************************************************/
void GPVAR_CLASS::operator=( const GPVAR_CLASS & sorc )
{
int32 i;

for ( i=0; i<NOF_GPVARS; i++ )
    varlist[i] = sorc.varlist[i];
}

/***********************************************************************
*                              GPVAR_CLASS                             *
*                              operator[]                              *
***********************************************************************/
GPVAR_ENTRY & GPVAR_CLASS::operator[]( int32 i )
{
if ( i>=0 && i<NOF_GPVARS  )
    return varlist[i];
else
    return EmptyEntry;
}

/***********************************************************************
*                          GPVAR_CLASS::SAVE                           *
***********************************************************************/
BOOLEAN GPVAR_CLASS::save( TCHAR * computer, TCHAR * machine, TCHAR * part )
{
FILE_CLASS f;
int32 i;

if ( f.open_for_write(gpvar_dbname(computer, machine, part)) )
    {
    for ( i=0; i<NOF_GPVARS; i++ )
        f.writeline( varlist[i].record() );
    f.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                          GPVAR_CLASS::LOAD                           *
***********************************************************************/
BOOLEAN GPVAR_CLASS::load( TCHAR * computer, TCHAR * machine, TCHAR * part )
{
FILE_CLASS f;
int32 i;

if ( f.open_for_read( gpvar_dbname(computer, machine, part)) )
    {
    for ( i=0; i<NOF_GPVARS; i++ )
        varlist[i].read( f.readline() );
    f.close();
    return TRUE;
    }

return FALSE;
}

