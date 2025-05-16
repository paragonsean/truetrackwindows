#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\fileclas.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\ftii.h"

static TCHAR EqualsString[] = TEXT( "=" );
static TCHAR OrHexString[]  = TEXT( "|H" );
static TCHAR HString[]      = TEXT( "H" );
static TCHAR UVString[]     = TEXT( ".UV"  );
static TCHAR VString[]      = TEXT( "V"  );
static TCHAR HChar          = TEXT( 'H'  );
static TCHAR MinusChar      = TEXT( '-'  );
static TCHAR PoundChar      = TEXT( '#'  );
static TCHAR OrChar         = TEXT( '|'  );
static TCHAR SpaceChar      = TEXT( ' '  );
static TCHAR TabChar        = TEXT( '\t'  );

static const TCHAR EqualsChar  = TEXT( '=' );
static const TCHAR NullChar    = TEXT( '\0' );
static const TCHAR PercentChar = TEXT( '%'  );
static const TCHAR PeriodChar  = TEXT( '.'  );
static const TCHAR UChar       = TEXT( 'U'  );
static const TCHAR VChar       = TEXT( 'V'  );

const unsigned char NO_FTII_DATA_TYPE   =  0;
const unsigned char X4_DISTANCE         =  1;
const unsigned char X1_VELOCITY         =  2;
const unsigned char BITS                =  3;
const unsigned char TIME_INTERVAL_COUNT =  4;
const unsigned char U_SEC               =  5;
const unsigned char M_SEC               =  6;
const unsigned char HZ                  =  7;
const unsigned char NUMBER              =  8;
const unsigned char NUMBER_TIMES_1000   =  9;
const unsigned char PERCENT_TIMES_100   = 10;
const unsigned char ST_ACCEL_STEP       = 11;
const unsigned char ST_VELOCITY_STEP    = 12;

static unsigned Bit30    = 0x40000000;
static unsigned Bit31    = 0x80000000;
static unsigned HighMask = 0xC0000000;
static unsigned LowMask  = 0x3FFFFFFF;

static unsigned char types[] =
    {
    11, 12,  1,  8, 11, 12,  1,  8, 11, 12,  1,  8, 11, 12,  1,  8, 11, 12,  1,  8, // 0-19
    11, 12,  1,  8, 11, 12,  1,  8, 11, 12,  1,  8, 11, 12,  1,  8, 11, 12,  1,  8, //20-39
    11, 12,  1,  8, 11, 12,  1,  8, 11, 12,  1,  8, 11, 12,  1,  8, 11, 12,  1,  8, //40-59
    11, 12,  1,  8, 11, 12,  1,  8, 11, 12,  1,  8, 11, 12,  1,  8, 11, 12,  1,  8, //60-79

     0,  0,  9,  0,  9,  9,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5, //80-99
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //100-119
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //120-139
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //140-159
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //160-179
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //180-199

     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //200-219
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //220-239
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //240-259
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //260-279
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //280-299

     7,  7,  8,  0,  0,  0,  0,  0,  0,  0,  7,  3,  3,  3,  3,  8,  8,  0,  1,  1, //300-319
     2,  4,  2,  1,  1,  1,  2,  2,  5,  8,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0, //320-339
     8,  2,  2,  2,  9,  9,  9,  0,  0,  1,  9,  9,  9,  9,  1,  9,  0,  1,  8,  0, //340-359
     0,  9,  9,  0,  8,  8,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  3,  3, //360-379
     3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  9,  8,  5,  5,  8,  2,  2,  2,  9,  9, //380-399

     9,  0,  0,  1,  9,  9,  9,  9,  1,  9,  1,  8,  0,  0,  3,  3,  0,  0,  0,  0, //400-419
     9,  9,  0,  0,  0,  0,  0,  0,  0,  0,  3,  3,  0,  0,  0,  0,  0,  0,  0,  0, //420-439
     0,  0,  0,  0,  0,  0,  0,  0,  0,  8,  8,  8,  0,  0,  0,  0,  0,  0,  0,  0, //440-459
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //460-479
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0  //480-499
    };

/***********************************************************************
*                              MYFINDCHAR                              *
*             This only searches until it encounters a #               *
***********************************************************************/
const TCHAR * myfindchar( TCHAR c_to_find, const TCHAR * sorc )
{
while ( *sorc )
    {
    if ( *sorc == PoundChar )
        break;

    if ( *sorc == c_to_find )
        return sorc;

    sorc++;
    }

return 0;
}

/***********************************************************************
*                              MYFINDCHAR                              *
***********************************************************************/
TCHAR * myfindchar( TCHAR c_to_find, TCHAR * sorc )
{
while ( *sorc )
    {
    if ( *sorc == PoundChar )
        break;

    if ( *sorc == c_to_find )
        return sorc;

    sorc++;
    }

return 0;
}

/***********************************************************************
*                            VARIABLE_NUMBER                           *
***********************************************************************/
static int variable_number( const TCHAR * sorc )
{
/*
-------------------------------------------------------------------------------
If this is a control variable, the line begins with .U before the V. Skip this.
------------------------------------------------------------------------------- */
if ( *sorc == PeriodChar )
    {
    sorc++;
    if ( *sorc == UChar )
        sorc++;
    else
        return NO_FTII_VAR;
    }

if ( *sorc == VChar )
    {
    sorc++;
    if ( is_numeric(*sorc) )
        return (int) asctoint32( sorc );
    }

return NO_FTII_VAR;
}

/***********************************************************************
*                               FTII_VAR                               *
*                                 get                                  *
* This assumes you have read a string from the control file and want   *
* to extract the variable number and value.                            *
***********************************************************************/
bool FTII_VAR::get( const TCHAR * sorc )
{
int new_vn;

new_vn = variable_number( sorc );
if ( new_vn == -1 )
    return false;
vn = new_vn;

sorc = myfindchar( EqualsChar, sorc );
if ( sorc )
    {
    sorc++;
    operator=( sorc );
    return true;
    }

return false;
}

/***********************************************************************
*                               FTII_VAR                               *
*                           is_same_variable                           *
*  This assumes the text is a line from the control file and returns   *
*  true if the variable number in that line is the same as mine.       *
***********************************************************************/
bool FTII_VAR::is_same_variable( const TCHAR * sorc )
{
if ( variable_number(sorc) == vn )
    return true;

return false;
}

/***********************************************************************
*                               FTII_VAR                               *
*                             append_value                             *
***********************************************************************/
bool FTII_VAR::append_value( STRING_CLASS & dest )
{
static TCHAR buf[10];
unsigned u;
unsigned char mytype;

mytype = types[vn];

if ( mytype == ST_ACCEL_STEP || mytype == ST_VELOCITY_STEP )
    {
    u = x & LowMask;
    dest += u;
    u = x & HighMask;
    if ( u )
        {
        dest += OrHexString;
        dest += ultohex( (unsigned long) u );
        }
    }
else if ( mytype == NUMBER_TIMES_1000 || mytype == PERCENT_TIMES_100 )
    {
    dest += (int) x;
    }
else if ( mytype == BITS || (x & Bit31) )
    {
    lstrcpy( buf, ultohex(x) );
    rjust( buf, 8, TEXT('0') );
    dest += HString;
    dest += buf;
    }
else
    {
    dest += x;
    }

return true;
}

/***********************************************************************
*                               FTII_VAR                               *
*                               makeline                               *
* This makes a line like V315=H80002100 or V315=455 or .UV0=0          *
***********************************************************************/
bool FTII_VAR::makeline( STRING_CLASS & dest )
{
bool return_status;
return_status = true;

if ( vn == NO_FTII_VAR )
    return_status = false;

if ( vn <= ST_LAST_STEP_VN )
    dest = UVString;
else
    dest = VString;

dest += vn;
dest += EqualsString;
append_value( dest );

return return_status;
}

/***********************************************************************
*                               FTII_VAR                               *
*                                 put                                  *
* I assume the string already contains a line. This replaces the       *
* value in the line with the current value.                            *
***********************************************************************/
bool FTII_VAR::put( STRING_CLASS & dest )
{
STRING_CLASS s;
TCHAR * cp;
TCHAR * np;

/*
--------------------------------------
Copy the comment and preceeding spaces
-------------------------------------- */
cp = myfindchar( EqualsChar, dest.text() );
if ( cp )
    {
    cp++;
    np = cp;
    while ( *cp != NullChar )
        {
        if ( *cp == SpaceChar || *cp == PoundChar || *cp == TabChar )
            break;
        cp++;
        }

    if ( *cp != NullChar )
        s = cp;

    /*
    ---------------------------------------
    Add the new value after the equals sign
    --------------------------------------- */
    *np = NullChar;
    append_value( dest );

    /*
    -------------------
    Restore the comment
    ------------------- */
    dest += s;

    return true;
    }

return false;
}

/***********************************************************************
*                               FTII_VAR                               *
*                               realval                                *
***********************************************************************/
double FTII_VAR::realval()
{
double d;

d = double( (int) x );

if ( types[vn] == NUMBER_TIMES_1000 )
    {
    d /= 1000.0;
    }
else if ( types[vn] == PERCENT_TIMES_100 )
    {
    d /= 100.0;
    }
else if ( types[vn] == ST_ACCEL_STEP || types[vn] == ST_VELOCITY_STEP )
    {
    d = double( x & LowMask );

    if ( x & Bit31 )
        d /= 100.0;
    }

return d;
}

/***********************************************************************
*                               FTII_VAR                               *
*                                  =                                   *
***********************************************************************/
void FTII_VAR::operator=( double new_value )
{
if ( types[vn] == NUMBER_TIMES_1000 )
    new_value *= 1000.0;
else if ( types[vn] == PERCENT_TIMES_100 )
    new_value *= 100.0;

x = (unsigned) round( new_value, 1.0 );
}

/***********************************************************************
*                               FTII_VAR                               *
*                                  =                                   *
* This assumes you are giving it a string from the                     *
* ftii_control_program.txt file beginning just after the =.            *
***********************************************************************/
void FTII_VAR::operator=( const TCHAR * sorc )
{
TCHAR   c;
int     i;

c = *sorc;

if ( c == HChar )
    {
    sorc++;
    x = (unsigned) hextoul( sorc );
    }
else if ( myfindchar( MinusChar, sorc) )
    {
    i = asctoint32( sorc );
    x = unsigned( i );
    }
else
    {
    x = (unsigned) asctoul( sorc );
    }

/*
---------------------------
Look for |H40000000 or such
--------------------------- */
while ( *sorc )
    {
    if ( *sorc == PoundChar )
        break;

    if ( *sorc == OrChar )
        {
        sorc++;
        if ( *sorc == HChar )
            {
            sorc++;
            x += (unsigned) hextoul( sorc );
            }
        }
    sorc++;
    }
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                               cleanup                                *
***********************************************************************/
void FTII_VAR_ARRAY::cleanup()
{

if ( p )
    {
    maxn = 0;
    ci   = NO_INDEX;
    n    = 0;
    delete[] p;
    p    = 0;
    }
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                           ~FTII_VAR_ARRAY                            *
***********************************************************************/
FTII_VAR_ARRAY::~FTII_VAR_ARRAY()
{
cleanup();
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                               get_from                               *
* This is like get but only gets the variables that already exist      *
* in the array.                                                        *
***********************************************************************/
bool FTII_VAR_ARRAY::get_from( TCHAR * filename )
{
TCHAR    * cp;
FILE_CLASS sorc;
int        i;
int        match_count;

if ( n == 0 )
    return true;

match_count = 0;

if ( file_exists(filename) )
    {
    if ( sorc.open_for_read(filename) )
        {
        while ( match_count < n )
            {
            cp = sorc.readline();
            if ( !cp )
                break;

            for ( i=0; i<n; i++ )
                {
                if ( p[i].is_same_variable(cp) )
                    {
                    p[i].get( cp );
                    match_count++;
                    break;
                    }
                }
            }
        sorc.close();
        return true;
        }
    }

return false;
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                                 get                                  *
* This reads all the variable values from a file. Any that exist       *
* in the array before are forgotten.                                   *
***********************************************************************/
bool FTII_VAR_ARRAY::get( TCHAR * filename )
{
TCHAR    * cp;
FILE_CLASS sorc;
int        nof_vars;

cleanup();

nof_vars = 0;

if ( file_exists(filename) )
    {
    if ( sorc.open_for_read(filename) )
        {
        while ( true )
            {
            cp = sorc.readline();
            if ( !cp )
                break;

            if ( *cp != PoundChar )
                nof_vars++;
            }

        if ( nof_vars > 0 )
            {
            upsize( nof_vars );
            sorc.rewind();
            while ( true )
                {
                cp = sorc.readline();
                if ( !cp )
                    break;

                if ( *cp != PoundChar )
                    {
                    if ( p[n].get(cp) )
                        n++;
                    }
                }
            if ( n == 0 )
                cleanup();
            }
        sorc.close();
        }
    }

if ( n > 0 )
    return true;

return false;
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                               put_into                               *
*  Replace the values in any lines that match one of my variables.     *
***********************************************************************/
bool FTII_VAR_ARRAY::put_into( TCHAR * filename )
{
int          i;
int          match_count;
bool         have_file;
TCHAR      * cp;
STRING_CLASS s;
STRING_CLASS tempfile;
FILE_CLASS   sorc;
FILE_CLASS   dest;

have_file = false;
tempfile  = filename;
dir_from_filename( tempfile.text() );
tempfile.cat_path( TEMP_CONTROL_PROGRAM_FILE );

if ( dest.open_for_write(tempfile.text()) )
    {
    if ( sorc.open_for_read(filename) )
        {
        match_count = 0;
        while ( true )
            {
            cp = sorc.readline();
            if ( !cp )
                {
                /*
                -------------------------------
                This is the normal break point.
                ------------------------------- */
                have_file = true;
                break;
                }

            s = cp;
            if ( match_count < n )
                {
                for ( i=0; i<n; i++ )
                    {
                    if ( p[i].is_same_variable(cp) )
                        {
                        p[i].put( s );
                        match_count++;
                        break;
                        }
                    }
                }
            dest.writeline( s.text() );
            }
        sorc.close();
        }
    dest.close();
    }

if ( have_file )
    {
    DeleteFile( filename );
    MoveFile( tempfile.text(), filename );
    }

return have_file;
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                                 put                                  *
*  If the variable number is -1, the line will not be saved. If there  *
*  are no lines, the file will be deleted.                             *
***********************************************************************/
bool FTII_VAR_ARRAY::put( TCHAR * filename )
{
int          i;
int          count;
STRING_CLASS s;
FILE_CLASS   dest;

count = 0;
if ( n )
    {
    if ( dest.open_for_write(filename) )
        {
        for ( i=0; i<n; i++ )
            {
            if ( p[i].makeline( s ) )
                {
                dest.writeline( s.text() );
                count++;
                }
            }
        dest.close();

        /*
        -----------------------------------------------
        If there are no valid lines then delete the file
        ------------------------------------------------ */
        if ( count )
            return true;
        else
            DeleteFile( filename );
        }
    }


return false;
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                                  []                                  *
***********************************************************************/
FTII_VAR & FTII_VAR_ARRAY::operator[]( int i )
{
if ( i >= maxn )
    {
    if ( !upsize(i+1) )
        return *this;
    }

if ( i >= n )
    n = i+1;

return p[i];
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                                 next                                 *
***********************************************************************/
bool FTII_VAR_ARRAY::next()
{
if ( ci < (n-1) )
    {
    ci++;
    *this = p[ci];
    return true;
    }

return false;
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                                 prev                                 *
***********************************************************************/
bool FTII_VAR_ARRAY::prev()
{
if ( ci > 0 )
    {
    ci--;
    vn = p[ci].vn;
    x  = p[ci].x;
    return true;
    }

return false;
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                                 find                                 *
***********************************************************************/
bool FTII_VAR_ARRAY::find( int vn_to_find )
{
int i;

for ( i=0; i<n; i++ )
    {
    if ( p[i].vn == vn_to_find )
        {
        ci = i;
        vn = p[i].vn;
        x  = p[i].x;
        return true;
        }
    }

return false;
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                                 find                                 *
***********************************************************************/
bool FTII_VAR_ARRAY::find( TCHAR * sorc )
{
int vn_to_find;

vn_to_find = variable_number( sorc );
return find( vn_to_find );
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                                 set                                  *
***********************************************************************/
void FTII_VAR_ARRAY::set( int new_vn, unsigned new_value )
{
vn = new_vn;
x  = new_value;
if ( ci >= 0 )
    p[ci] = *this;
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                                  =                                   *
***********************************************************************/
void FTII_VAR_ARRAY::operator=( const FTII_VAR_ARRAY & sorc )
{
int i;
if ( sorc.n > maxn )
    upsize( sorc.n );

if ( maxn >= sorc.n )
    {
    for ( i=0; i<sorc.n; i++ )
        p[i] = sorc.p[i];
    n  = sorc.n;
    ci = NO_INDEX;
    }
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                                  =                                   *
***********************************************************************/
void FTII_VAR_ARRAY::operator=( const FTII_VAR & sorc )
{
set( sorc.vn, sorc.x );
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                                  =                                   *
***********************************************************************/
void FTII_VAR_ARRAY::operator=( unsigned new_value )
{
x  = new_value;
if ( ci >= 0 )
    p[ci] = *this;
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                                  =                                   *
***********************************************************************/
void FTII_VAR_ARRAY::operator=( int new_value )
{
operator=( (unsigned) new_value );
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                                  =                                   *
***********************************************************************/
void FTII_VAR_ARRAY::operator=( double new_value )
{
FTII_VAR::operator=( new_value );
if ( ci >= 0 )
    p[ci] = *this;
}

/***********************************************************************
*                               FTII_VAR                               *
*                                  =                                   *
***********************************************************************/
void FTII_VAR_ARRAY::operator=( TCHAR * sorc )
{
FTII_VAR::operator=( sorc );
if ( ci >= 0 )
    p[ci] = *this;
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                                remove                                *
***********************************************************************/
bool FTII_VAR_ARRAY::remove( int di )
{
int i;
int j;

i = di;
if ( i>=0 && i<n )
    {
    j = i+1;
    while ( j<n )
        {
        p[i] = p[j];
        i++;
        j++;
        }
    n--;

    if ( ci >= di )
        {
        if ( ci >= n )
            ci--;
        if ( ci >= 0 )
            {
            vn = p[ci].vn;
            x  = p[ci].x;
            }
        }
    return true;
    }

return false;
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                                                                      *
***********************************************************************/
bool FTII_VAR_ARRAY::remove()
{
if ( ci >= 0 )
    return remove( ci );

return false;
}

/***********************************************************************
*                            FTII_VAR_ARRAY                            *
*                                                                      *
***********************************************************************/
bool FTII_VAR_ARRAY::upsize( int new_n )
{
FTII_VAR * dp;
FTII_VAR * np;
int        i;

if ( new_n <= maxn )
    return true;

np = new FTII_VAR[new_n];
if ( !np )
    return false;

dp = 0;
if ( p )
    {
    for ( i=0; i<n; i++ )
        np[i] = p[i];
    dp = p;
    }

p    = np;
maxn = new_n;

if ( dp )
    delete[] dp;

return true;
}

/***********************************************************************
*                      FTII_LIMIT_SWITCH_WIRE_CLASS                    *
*                                MASK                                  *
* Convert a wire number to a bit mask, assuming wire 1 is bit 0, etc   *
***********************************************************************/
unsigned FTII_LIMIT_SWITCH_WIRE_CLASS::mask( int index )
{
unsigned mywire;
unsigned mask;

if ( index<0 || index>=MAX_FTII_LIMIT_SWITCHES )
    return 0;

mask = 0;
mywire = wire[index];

if ( mywire > 0 )
    {
    mask = 1;
    mywire--;
    mask <<= mywire;
    }

return mask;
}

/***********************************************************************
*                      FTII_LIMIT_SWITCH_WIRE_CLASS                    *
*                            ASCII_MASK                                *
***********************************************************************/
TCHAR * FTII_LIMIT_SWITCH_WIRE_CLASS::ascii_mask( int index )
{
static TCHAR buf[10];

lstrcpy( buf, ultohex(mask(index)) );
rjust( buf, 9, TEXT('0') );
buf[0] = TEXT('H');

return buf;
}

/***********************************************************************
*                      FTII_LIMIT_SWITCH_WIRE_CLASS                    *
*                            ASCII_WIRE                                *
***********************************************************************/
TCHAR * FTII_LIMIT_SWITCH_WIRE_CLASS::ascii_wire( int index )
{
static TCHAR buf[4];
unsigned    mywire;

mywire = NO_WIRE;
if ( index>=0 && index<MAX_FTII_LIMIT_SWITCHES )
    mywire = wire[index];

lstrcpy( buf, ultoascii(mywire) );

return buf;
}

/***********************************************************************
*                      FTII_LIMIT_SWITCH_WIRE_CLASS                    *
*                                 GET                                  *
* A return value of FALSE just means there were no old settings        *
***********************************************************************/
BOOLEAN FTII_LIMIT_SWITCH_WIRE_CLASS::get( TCHAR * computer, TCHAR * machine, TCHAR * part )
{
static int maskvn[MAX_FTII_LIMIT_SWITCHES] = { LS1_MASK_VN, LS2_MASK_VN, LS3_MASK_VN, LS4_MASK_VN, LS5_MASK_VN, LS6_MASK_VN };
int            i;
FTII_VAR_ARRAY va;
STRING_CLASS   s;
FILE_CLASS     f;

/*
----------------------------------------------
The limit switch wires have their own file now
---------------------------------------------- */
s = ftii_limit_switch_wires_name( computer, machine, part );
if ( file_exists(s.text()) )
    {
    f.open_for_read( s.text() );
    for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
        {
        s = f.readline();
        wire[i] = s.int_value();
        }
    f.close();
    return TRUE;
    }

/*
------------------------------------------------------------------
If that file doesn't exist then look in the ftii_part_settings.txt
------------------------------------------------------------------ */
s = ftii_part_settings_name( computer, machine, part );
if ( file_exists(s.text()) )
    {
    va.upsize( MAX_FTII_LIMIT_SWITCHES );
    for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
        va[i].set( maskvn[i], 0 );

    va.get_from( s.text() );

    for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
        wire[i] = wire_from_mask( va[i].uval() );

    return TRUE;
    }

for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    wire[i] = NO_WIRE;
return FALSE;
}

/***********************************************************************
*                      FTII_LIMIT_SWITCH_WIRE_CLASS                    *
*                                PUT                                   *
* A return value of FALSE means I couldn't write the file.             *
***********************************************************************/
BOOLEAN FTII_LIMIT_SWITCH_WIRE_CLASS::put( TCHAR * computer, TCHAR * machine, TCHAR * part )
{
int            i;
FILE_CLASS     f;
STRING_CLASS   s;
s = ftii_limit_switch_wires_name( computer, machine, part );

if ( f.open_for_write(s.text()) )
    {
    for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
        f.writeline( ultoascii( wire[i]) );
    f.close();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                      FTII_LIMIT_SWITCH_WIRE_CLASS                    *
*                                  []                                  *
***********************************************************************/
unsigned & FTII_LIMIT_SWITCH_WIRE_CLASS::operator[]( int index )
{
static unsigned zero = 0;

if ( index>=0 && index<MAX_FTII_LIMIT_SWITCHES )
    return wire[index];

return zero;
}

/***********************************************************************
*                      FTII_LIMIT_SWITCH_WIRE_CLASS                    *
*                               EMPTY                                  *
***********************************************************************/
void FTII_LIMIT_SWITCH_WIRE_CLASS::empty()
{
int i;

for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
    wire[i] = 0;
}
