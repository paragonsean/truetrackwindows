#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\fileclas.h"
#include "..\include\iniclass.h"
#include "..\include\subs.h"

static TCHAR LeftBracket = TEXT( '[' );
static TCHAR RightBracket = TEXT( ']' );
static TCHAR NChar    = TEXT( 'N' );
static TCHAR NullChar = TEXT( '\0' );
static TCHAR YChar    = TEXT( 'Y' );
static TCHAR LowerYChar    = TEXT( 'y' );

static TCHAR UnknownString[] = UNKNOWN;
static DWORD UnknownLen = sizeof(UnknownString)/sizeof(TCHAR) - 1;

/***********************************************************************
*                      CHECK_FOR_UNICODE_INI_FILE                      *
* WritePrivateProfileString will always create an ansii file so        *
* if the ini file doesn't exist I need to create one with the section  *
* in it. Because the section name is in unicode the                    *
* WritePrivateProfileString writes the rest in unicode as well.        *
***********************************************************************/
static void check_for_unicode_ini_file( const TCHAR * section, const TCHAR * filename )
{
FILE_CLASS f;
TCHAR buf[MAX_PATH+1];
TCHAR * cp;

lstrcpy( buf, filename );
if ( !file_exists(buf) )
    {
    f.open_for_write( buf );
    cp = buf;
    *cp = LeftBracket;
    cp++;
    cp = copystring( cp, section );
    *cp = RightBracket;
    cp++;
    *cp = NullChar;
    f.writeline( buf );
    f.close();
    }
}

/***********************************************************************
*                               INI_CLASS                              *
*                                cleanup                               *
***********************************************************************/
void INI_CLASS::cleanup()
{
if ( section_name )
    {
    delete[] section_name;
    section_name = 0;
    }

if ( filename )
    {
    delete[] filename;
    filename = 0;
    }

if ( buf )
    {
    delete[] buf;
    buf = 0;
    }
}

/***********************************************************************
*                              ~INI_CLASS                              *
***********************************************************************/
INI_CLASS::~INI_CLASS()
{
cleanup();
}

/***********************************************************************
*                               INI_CLASS                              *
*                               set_file                               *
* Return = TRUE says I have remembered the name, not that the file     *
* exists.                                                              *
***********************************************************************/
BOOLEAN INI_CLASS::set_file( const TCHAR * new_file_name )
{
my_file_exists = FALSE;

if ( filename )
    {
    delete[] filename;
    filename = 0;
    }

if ( new_file_name )
    {
    filename = maketext( new_file_name );
    if ( filename )
        {
        my_file_exists = file_exists( filename );
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                               INI_CLASS                              *
*                              set_section                             *
***********************************************************************/
BOOLEAN INI_CLASS::set_section( const TCHAR * new_section )
{

if ( section_name )
    {
    delete[] section_name;
    section_name = 0;
    }

if ( new_section )
    {
    section_name = maketext( new_section );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                               INI_CLASS                              *
*                                 find                                 *
***********************************************************************/
BOOLEAN INI_CLASS::find( const TCHAR * section, const TCHAR * key )
{
DWORD n;

if ( !my_file_exists )
    return FALSE;

if ( !buf )
    buf = maketext( MAX_LONG_INI_STRING_LEN );

if ( buf )
    {
    n = GetPrivateProfileString( section, key, UnknownString, buf, MAX_LONG_INI_STRING_LEN, filename );
    if ( n == UnknownLen )
        {
        if ( strings_are_equal(UnknownString, buf) )
            return FALSE;
        }
    }

return TRUE;
}

/***********************************************************************
*                               INI_CLASS                              *
*                                 find                                 *
***********************************************************************/
BOOLEAN INI_CLASS::find( const TCHAR * key )
{
if ( section_name )
    return find( section_name, key );

return FALSE;
}

/***********************************************************************
*                               INI_CLASS                              *
*                               get_string                             *
***********************************************************************/
TCHAR * INI_CLASS::get_string()
{
static TCHAR empty_string[] = TEXT( "" );

if ( buf )
    return buf;
else
    return empty_string;

}

/***********************************************************************
*                               INI_CLASS                              *
*                               get_string                             *
***********************************************************************/
TCHAR * INI_CLASS::get_string( const TCHAR * section, const TCHAR * key )
{
if ( buf )
    *buf = NullChar;

find( section, key );

return get_string();
}

/***********************************************************************
*                               INI_CLASS                              *
*                               get_string                             *
***********************************************************************/
TCHAR * INI_CLASS::get_string( const TCHAR * key )
{
if ( buf )
    *buf = NullChar;

find( key );

return get_string();
}

/***********************************************************************
*                               INI_CLASS                              *
*                               get_boolean                            *
***********************************************************************/
BOOLEAN INI_CLASS::get_boolean()
{
if ( buf )
    {
    if ( *buf == YChar || *buf == LowerYChar )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                               INI_CLASS                              *
*                               get_boolean                            *
***********************************************************************/
BOOLEAN INI_CLASS::get_boolean( const TCHAR * section, const TCHAR * key )
{
if ( find(section, key) )
    return get_boolean();

return FALSE;
}

/***********************************************************************
*                               INI_CLASS                              *
*                               get_boolean                            *
***********************************************************************/
BOOLEAN INI_CLASS::get_boolean( const TCHAR * key )
{
if ( section_name )
    return get_boolean( section_name, key );

return FALSE;
}

/***********************************************************************
*                               INI_CLASS                              *
*                                get_int                               *
***********************************************************************/
int INI_CLASS::get_int()
{
int i;

i = 0;

if ( buf )
    i = (int) asctoint32( buf );

return i;
}

/***********************************************************************
*                               INI_CLASS                              *
*                                get_int                               *
***********************************************************************/
int INI_CLASS::get_int(  const TCHAR * section, const TCHAR * key )
{
if ( find(section, key) )
    return get_int();

return 0;
}

/***********************************************************************
*                               INI_CLASS                              *
*                                get_int                               *
***********************************************************************/
int INI_CLASS::get_int(  const TCHAR * key )
{
if ( section_name )
    return get_int( section_name, key );

return 0;
}

/***********************************************************************
*                               INI_CLASS                              *
*                                get_ul                                *
***********************************************************************/
unsigned long INI_CLASS::get_ul()
{
unsigned long i;

i = 0;

if ( buf )
    i = (int) asctoul( buf );


return i;
}

/***********************************************************************
*                               INI_CLASS                              *
*                               get_ul                                 *
***********************************************************************/
unsigned long INI_CLASS::get_ul( const TCHAR * key )
{
find( key );

return get_ul();
}

/***********************************************************************
*                               INI_CLASS                              *
*                               get_ul                                 *
***********************************************************************/
unsigned long INI_CLASS::get_ul( const TCHAR * section, const TCHAR * key )
{
find( section, key );

return get_ul();
}

/***********************************************************************
*                               INI_CLASS                              *
*                               get_double                             *
***********************************************************************/
double INI_CLASS::get_double()
{
double x;

x = 0.0;

if ( buf )
    x = extdouble( buf );

return x;
}

/***********************************************************************
*                               INI_CLASS                              *
*                               get_double                             *
***********************************************************************/
double INI_CLASS::get_double( const TCHAR * key )
{
if ( find(key) )
    return get_double();

return 0.0;
}

/***********************************************************************
*                               INI_CLASS                              *
*                               get_double                             *
***********************************************************************/
double INI_CLASS::get_double( const TCHAR * section, const TCHAR * key )
{
if ( find(section, key) )
    return get_double();

return 0.0;
}

/***********************************************************************
*                               INI_CLASS                              *
*                               put_string                             *
***********************************************************************/
BOOLEAN INI_CLASS::put_string( const TCHAR * section, const TCHAR * key, const TCHAR * sorc )
{
if ( is_empty(sorc) )
    sorc = UnknownString;

if ( !filename )
    return FALSE;

#ifdef UNICODE
check_for_unicode_ini_file( section, filename );
#endif

if ( WritePrivateProfileString(section, key, sorc, filename) )
    {
    my_file_exists = TRUE;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                               INI_CLASS                              *
*                               put_string                             *
***********************************************************************/
BOOLEAN INI_CLASS::put_string( const TCHAR * key, const TCHAR * sorc )
{
if ( section_name )
    return put_string( section_name, key, sorc );

return FALSE;
}

/***********************************************************************
*                               INI_CLASS                              *
*                               put_string                             *
***********************************************************************/
BOOLEAN INI_CLASS::put_string( const TCHAR * dest_file_name, const TCHAR * section, const TCHAR * key, const TCHAR * sorc )
{
set_file( dest_file_name );
return put_string( section, key, sorc );
}

/***********************************************************************
*                               INI_CLASS                              *
*                              put_boolean                             *
***********************************************************************/
BOOLEAN INI_CLASS::put_boolean( const TCHAR * section, const TCHAR * key, BOOLEAN sorc )
{
static TCHAR checkedchar[] = TEXT( "Y" );

if ( sorc )
    *checkedchar = YChar;
else
    *checkedchar = NChar;

return put_string( section, key, checkedchar );
}

/***********************************************************************
*                               INI_CLASS                              *
*                              put_boolean                             *
***********************************************************************/
BOOLEAN INI_CLASS::put_boolean( const TCHAR * key, BOOLEAN sorc )
{
if ( section_name )
    return put_boolean( section_name, key, sorc );

return FALSE;
}

/***********************************************************************
*                               INI_CLASS                              *
*                              put_boolean                             *
***********************************************************************/
BOOLEAN INI_CLASS::put_boolean( const TCHAR * dest_file_name, const TCHAR * section, const TCHAR * key, BOOLEAN sorc )
{
set_file( dest_file_name );
return put_boolean( section, key, sorc );
}

/***********************************************************************
*                               INI_CLASS                              *
*                                put_int                               *
***********************************************************************/
BOOLEAN INI_CLASS::put_int( const TCHAR * section, const TCHAR * key, int sorc )
{
TCHAR s[MAX_INTEGER_LEN+1];

int32toasc( s, (int32) sorc, DECIMAL_RADIX );

return put_string( section, key, s );
}

/***********************************************************************
*                               INI_CLASS                              *
*                                put_int                               *
***********************************************************************/
BOOLEAN INI_CLASS::put_int( const TCHAR * key, int sorc )
{
TCHAR s[MAX_INTEGER_LEN+1];

int32toasc( s, (int32) sorc, DECIMAL_RADIX );

return put_string( key, s );
}

/***********************************************************************
*                               INI_CLASS                              *
*                                put_ul                                *
***********************************************************************/
BOOLEAN INI_CLASS::put_ul( const TCHAR * section, const TCHAR * key, unsigned long sorc )
{
TCHAR s[MAX_INTEGER_LEN+1];

ultoascii( s, sorc, DECIMAL_RADIX );

return put_string( section, key, s );
}

/***********************************************************************
*                               INI_CLASS                              *
*                                put_ul                                *
***********************************************************************/
BOOLEAN INI_CLASS::put_ul( const TCHAR * key, unsigned long sorc )
{
TCHAR s[MAX_INTEGER_LEN+1];

ultoascii( s, sorc, DECIMAL_RADIX );

return put_string( key, s );
}

/***********************************************************************
*                               INI_CLASS                              *
*                               put_double                             *
***********************************************************************/
BOOLEAN INI_CLASS::put_double( const TCHAR * section, const TCHAR * key, double sorc )
{
TCHAR s[MAX_DOUBLE_LEN+1];

rounded_double_to_tchar( s, sorc );

return put_string( section, key, s );
}

/***********************************************************************
*                               INI_CLASS                              *
*                               put_double                             *
***********************************************************************/
BOOLEAN INI_CLASS::put_double( const TCHAR * key, double sorc )
{
TCHAR s[MAX_DOUBLE_LEN+1];

rounded_double_to_tchar( s, sorc );

return put_string( key, ascii_double(sorc) );
}

/***********************************************************************
*                               INI_CLASS                              *
*                               remove_key                             *
***********************************************************************/
void INI_CLASS::remove_key( const TCHAR * section, const TCHAR * key )
{
WritePrivateProfileString( section, key, 0, filename );
}

/***********************************************************************
*                               INI_CLASS                              *
*                               remove_key                             *
***********************************************************************/
void INI_CLASS::remove_key( const TCHAR * key )
{
if ( section_name )
    remove_key(section_name, key );
}
