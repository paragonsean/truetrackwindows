#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

static TCHAR YChar    = TEXT( 'Y' );
static TCHAR NChar    = TEXT( 'N' );
static TCHAR NullChar = TEXT( '\0' );
static TCHAR UnknownString[] = UNKNOWN;

static TCHAR Buf[MAX_LONG_INI_STRING_LEN+1];

/***********************************************************************
*                            GET_LONG_INI_STRING                       *
***********************************************************************/
TCHAR * get_long_ini_string( const TCHAR * fname, const TCHAR * section, const TCHAR * key )
{

TCHAR path[MAX_PATH+1];

*Buf = 0;

get_exe_directory( path );
catpath( path, fname );

GetPrivateProfileString( section, key, UnknownString, Buf, MAX_LONG_INI_STRING_LEN, path );

return Buf;
}

/***********************************************************************
*                            GET_INI_STRING                            *
***********************************************************************/
TCHAR * get_ini_string( const TCHAR * fname, const TCHAR * section, const TCHAR * key )
{
get_long_ini_string( fname, section, key );

*(Buf+MAX_INI_STRING_LEN) = NullChar;

return Buf;
}

/***********************************************************************
*                             PUT_INI_STRING                           *
***********************************************************************/
void put_ini_string( const TCHAR * fname, const TCHAR * section, const TCHAR * key, const TCHAR * sorc )
{

TCHAR path[MAX_PATH+1];

get_exe_directory( path );
catpath( path, fname );

WritePrivateProfileString( section, key, sorc, path );
}

/***********************************************************************
*                             BOOL_FROM_INI                            *
***********************************************************************/
bool bool_from_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key )
{

if ( *get_ini_string( fname, section, key) == YChar )
    return true;

return false;
}

/***********************************************************************
*                             BOOL_TO_INI                              *
***********************************************************************/
void bool_to_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key, bool b )
{
static TCHAR checkedchar[] = TEXT( "Y" );

if ( b )
    *checkedchar = YChar;
else
    *checkedchar = NChar;

put_ini_string( fname, section, key, checkedchar );
}

/***********************************************************************
*                           BOOLEAN_FROM_INI                           *
***********************************************************************/
BOOLEAN boolean_from_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key )
{

if ( *get_ini_string( fname, section, key) == YChar )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                            BOOLEAN_TO_INI                            *
***********************************************************************/
void boolean_to_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key, BOOLEAN b )
{
static TCHAR checkedchar[] = TEXT( "Y" );

if ( b )
    *checkedchar = YChar;
else
    *checkedchar = NChar;

put_ini_string( fname, section, key, checkedchar );
}

/***********************************************************************
*                               UL_FROM_INI                            *
***********************************************************************/
unsigned long ul_from_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key )
{
return asctoul( get_ini_string(fname, section, key)  );
}

/***********************************************************************
*                                 UL_TO_INI                            *
***********************************************************************/
void ul_to_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key, unsigned long i )
{
TCHAR s[MAX_INTEGER_LEN+1];

ultoascii( s, i, DECIMAL_RADIX );
put_ini_string( fname, section, key, s );
}

/***********************************************************************
*                            INT32_FROM_INI                            *
***********************************************************************/
int32 int32_from_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key )
{
return asctoint32( get_ini_string(fname, section, key)  );
}

/***********************************************************************
*                              INT32_TO_INI                            *
***********************************************************************/
void int32_to_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key, int32 i )
{
TCHAR s[MAX_INTEGER_LEN+1];

int32toasc( s, i, DECIMAL_RADIX );

put_ini_string( fname, section, key, s );
}

/***********************************************************************
*                            FLOAT_FROM_INI                            *
***********************************************************************/
float float_from_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key )
{
return extfloat( get_ini_string(fname, section, key)  );
}

/***********************************************************************
*                              FLOAT_TO_INI                            *
***********************************************************************/
void float_to_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key, float x )
{
TCHAR s[MAX_DOUBLE_LEN+1];

rounded_double_to_tchar( s, (double) x );

put_ini_string( fname, section, key, s );
}

/***********************************************************************
*                           DOUBLE_FROM_INI                            *
***********************************************************************/
double double_from_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key )
{
return extdouble( get_ini_string(fname, section, key)  );
}

/***********************************************************************
*                             DOUBLE_TO_INI                            *
***********************************************************************/
void double_to_ini( const TCHAR * fname, const TCHAR * section, const TCHAR * key, double x )
{
TCHAR s[MAX_DOUBLE_LEN+1];

rounded_double_to_tchar( s, x );
put_ini_string( fname, section, key, s );
}
