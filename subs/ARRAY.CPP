#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\array.h"
#include "..\include\fileclas.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"

static const TCHAR CommaChar = TEXT( ',' );
static const TCHAR NullChar  = TEXT( '\0' );

/***********************************************************************
*                           FLOAT_ARRAY_CLASS                          *
*                                 NEXT                                 *
***********************************************************************/
BOOLEAN FLOAT_ARRAY_CLASS::next( void )
{
if ( rewound )
    rewound = FALSE;
else if ( x < (n-1) )
    x++;
else
    return FALSE;

return TRUE;
}

/***********************************************************************
*                           FLOAT_ARRAY_CLASS                          *
*                               REWIND_TO                              *
*                                                                      *
*   Note, this sets rewound = TRUE so you can do the following;        *
*   FLOAT_ARRAY_CLASS f;                                               *
*   ...                                                                *
*                                                                      *
*   if ( f.rewind_to(10) )                                             *
*       while ( f.next() )                                             *
*           foobar( f.value() );                                       *
*                                                                      *
***********************************************************************/
BOOLEAN FLOAT_ARRAY_CLASS::rewind_to( int32 new_index )
{
if ( p && new_index >= 0 && new_index < n )
    {
    rewound = TRUE;
    x = new_index;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                           FLOAT_ARRAY_CLASS                          *
*                              CLEANUP                                 *
***********************************************************************/
void FLOAT_ARRAY_CLASS::cleanup( void )
{
if ( p )
    delete[] p;
p = 0;
n = 0;
x = 0;
rewound = FALSE;
}

/***********************************************************************
*                           FLOAT_ARRAY_CLASS                          *
*                                 INIT                                 *
***********************************************************************/
BOOLEAN FLOAT_ARRAY_CLASS::init( int32 nof_floats )
{
cleanup();

if ( nof_floats > 0 )
    {
    n = nof_floats;
    p = new float[n];
    if ( p )
        {
        for ( x=0; x<n; x++ )
            p[x] = 0.0;
        x = 0;
        rewound = TRUE;
        return TRUE;
        }
    else
        {
        cleanup();
        }
    }

return FALSE;
}

/***********************************************************************
*                           ~FLOAT_ARRAY_CLASS                         *
***********************************************************************/
FLOAT_ARRAY_CLASS::~FLOAT_ARRAY_CLASS()
{
cleanup();
}

/***********************************************************************
*                          DOUBLE_ARRAY_CLASS                          *
*                              CLEANUP                                 *
***********************************************************************/
void DOUBLE_ARRAY_CLASS::cleanup( void )
{
if ( p )
    delete[] p;
p = 0;
n = 0;
}

/***********************************************************************
*                          DOUBLE_ARRAY_CLASS                          *
*                                 INIT                                 *
***********************************************************************/
BOOLEAN DOUBLE_ARRAY_CLASS::init( int32 nof_double )
{
int32 i;

cleanup();

if ( nof_double > 0 )
    {
    p = new double[nof_double];
    if ( p )
        {
        n = nof_double;
        for ( i=0; i<n; i++ )
            p[i] = 0.0;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                          DOUBLE_ARRAY_CLASS                          *
*                                 INIT                                 *
***********************************************************************/
BOOLEAN DOUBLE_ARRAY_CLASS::init( int32 sorc_n, double * sorc )
{
cleanup();

if ( sorc_n > 0 )
    {
    p = sorc;
    n = sorc_n;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                          DOUBLE_ARRAY_CLASS                          *
*                                  =                                   *
***********************************************************************/
void DOUBLE_ARRAY_CLASS::operator=(const DOUBLE_ARRAY_CLASS & sorc )
{
int i;

if ( sorc.n > 0 )
    {
    if ( n != sorc.n )
        init( sorc.n );
    }

if ( n == sorc.n )
    {
    for ( i=0; i<n; i++ )
        p[i] = sorc.p[i];
    }
}

/***********************************************************************
*                          DOUBLE_ARRAY_CLASS                          *
*                                 move                                 *
          This transfers the data from one array to another.
***********************************************************************/
void  DOUBLE_ARRAY_CLASS::move( DOUBLE_ARRAY_CLASS & sorc )
{
cleanup();
n = sorc.n;
p = sorc.p;
sorc.n = 0;
sorc.p = 0;
}

/***********************************************************************
*                          ~DOUBLE_ARRAY_CLASS                         *
***********************************************************************/
DOUBLE_ARRAY_CLASS::~DOUBLE_ARRAY_CLASS()
{
cleanup();
}

/***********************************************************************
*                           DOUBLE_ARRAY_CLASS                         *
*                                  []                                  *
***********************************************************************/
double & DOUBLE_ARRAY_CLASS::operator[]( int32 i )
{
static double empty_double = 0;

if ( p && i>=0 && i< n )
    return p[i];

return empty_double;
}

/***********************************************************************
*                            INT32_ARRAY_CLASS                         *
*                                 NEXT                                 *
***********************************************************************/
BOOLEAN INT32_ARRAY_CLASS::next( void )
{
if ( rewound )
    rewound = FALSE;
else if ( x < (n-1) )
    x++;
else
    return FALSE;

return TRUE;
}

/***********************************************************************
*                            INT32_ARRAY_CLASS                         *
*                               REWIND_TO                              *
*                                                                      *
*   Note, this sets rewound = TRUE so you can do the following;        *
*   INT32_ARRAY_CLASS a;                                               *
*   ...                                                                *
*                                                                      *
*   if ( a.rewind_to(10) )                                             *
*       while ( a.next() )                                             *
*           foobar( a.value() );                                       *
*                                                                      *
***********************************************************************/
BOOLEAN INT32_ARRAY_CLASS::rewind_to( int32 new_index )
{
if ( p && new_index >= 0 && new_index < n )
    {
    rewound = TRUE;
    x = new_index;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                            INT32_ARRAY_CLASS                         *
*                                 CLEANUP                              *
***********************************************************************/
void INT32_ARRAY_CLASS::cleanup( void )
{
if ( p )
    delete[] p;
p    = 0;
n    = 0;
nset = 0;
x    = 0;
rewound = FALSE;
}

/***********************************************************************
*                            INT32_ARRAY_CLASS                         *
*                                 INIT                                 *
***********************************************************************/
BOOLEAN INT32_ARRAY_CLASS::init( int32 nof_ints )
{
cleanup();

if ( nof_ints > 0 )
    {
    n = nof_ints;
    p = new int32[n];
    if ( p )
        {
        for ( x=0; x<n; x++ )
            p[x] = 0;
        x    = 0;
        rewound = TRUE;
        return TRUE;
        }
    else
        {
        cleanup();
        }
    }

return FALSE;
}

/***********************************************************************
*                            INT32_ARRAY_CLASS                         *
*                                LEFT_SHIFT                            *
***********************************************************************/
void INT32_ARRAY_CLASS::left_shift( int32 new_value )
{
int32 i;

if ( !p )
    return;

if ( n <= 0 )
    return;

/*
--------------------------------------------------------------------
If all the elements have not been set, set the next, else left shift
-------------------------------------------------------------------- */
if ( nset < n )
    {
    p[nset] = new_value;
    nset++;
    }
else
    {
    for ( i=0; i<(n-1); i++ )
        p[i] = p[i+1];

    p[n-1] = new_value;
    }
}

/***********************************************************************
*                            INT32_ARRAY_CLASS                         *
*                                MAX_VALUE                             *
***********************************************************************/
int32 INT32_ARRAY_CLASS::max_value( void )
{
int32 i;
int32 x;

if ( !p )
    return 0;

x = p[0];

for ( i=1; i<nset; i++ )
    if ( p[i] > x )
        x = p[i];

return x;
}

/***********************************************************************
*                            INT32_ARRAY_CLASS                         *
*                                MIN_VALUE                             *
***********************************************************************/
int32 INT32_ARRAY_CLASS::min_value( void )
{
int32 i;
int32 x;

if ( !p )
    return 0;

x = p[0];

for ( i=1; i<nset; i++ )
    if ( p[i] < x )
        x = p[i];

return x;
}

/***********************************************************************
*                           ~INT32_ARRAY_CLASS                         *
***********************************************************************/
INT32_ARRAY_CLASS::~INT32_ARRAY_CLASS()
{
cleanup();
}
/***********************************************************************
*                            INT32_ARRAY_CLASS                         *
*                                   =                                  *
***********************************************************************/
void INT32_ARRAY_CLASS::operator=(const INT32_ARRAY_CLASS & sorc )
{
int32 i;

if (  sorc.p )
    {
    if ( init(sorc.n) )
        {
        for ( i=0; i<n; i++ )
            p[i] = sorc.p[i];
        }
    }
else
    {
    cleanup();
    }
}

/***********************************************************************
*                            INT32_ARRAY_CLASS                         *
*                                  ==                                  *
***********************************************************************/
BOOLEAN INT32_ARRAY_CLASS::operator==(const INT32_ARRAY_CLASS & sorc )
{
int32 i;

if ( !p || !sorc.p )
    return FALSE;

if ( n != sorc.n )
    return FALSE;

for ( i=0; i<n; i++ )
    if ( p[i] != sorc.p[i] )
        return FALSE;

return TRUE;
}

/***********************************************************************
*                            INT32_ARRAY_CLASS                         *
*                                  !=                                  *
***********************************************************************/
BOOLEAN INT32_ARRAY_CLASS::operator!=(const INT32_ARRAY_CLASS & sorc )
{
if ( operator==(sorc) )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                            INT32_ARRAY_CLASS                         *
*                                  []                                  *
***********************************************************************/
int32 & INT32_ARRAY_CLASS::operator[]( int32 i )
{
static int32 empty_int = 0;

if ( p && i>=0 && i< n )
    return p[i];

return empty_int;
}

/***********************************************************************
*                            INT32_ARRAY_CLASS                         *
*                                  READ                                *
***********************************************************************/
BOOLEAN INT32_ARRAY_CLASS::read( TCHAR * fname )
{
int32        nof_ints;
FILE_CLASS   f;
STRING_CLASS s;
TCHAR      * cp;

cleanup();

f.open_for_read( fname );
s = f.readline();
f.close();

if ( s.isempty() )
    return FALSE;

/*
------------------------------------------------------
Count the number of parameters by counting the comma's
------------------------------------------------------ */
nof_ints = 0;
cp = s.text();
while ( *cp )
    {
    if ( *cp == CommaChar )
        nof_ints++;
    cp++;
    }

/*
-------------------------------------
There is one on the end with no comma
------------------------------------- */
nof_ints++;

cp = s.text();
if ( init(nof_ints) )
    {
    while ( next() )
        {
        replace_char_with_null( cp, CommaChar );
        set( asctoint32(cp) );
        cp = nextstring( cp );
        }

    rewind();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                                WRITE                                 *
***********************************************************************/
BOOLEAN INT32_ARRAY_CLASS::write( TCHAR * fname )
{
int32 slen;
FILE_CLASS   f;
TCHAR      * cp;
TCHAR      * s;

if ( n == 0 )
    return FALSE;

slen = n * (MAX_INTEGER_LEN+1);
s    = maketext( slen );
if ( !s )
    return FALSE;

cp = s;
rewind();

while ( next() )
    cp = copy_w_char( cp, int32toasc(value()), CommaChar );

rewind();

/*
----------------------
Remove the final comma
---------------------- */
cp--;
*cp = NullChar;

f.open_for_write( fname );
f.writeline( s );
f.close();

delete[] s;
return TRUE;
}

/***********************************************************************
*                             INT_ARRAY_CLASS                          *
*                                 CLEANUP                              *
***********************************************************************/
void INT_ARRAY_CLASS::cleanup( void )
{
if ( p )
    delete[] p;
p = 0;
n = 0;
}

/***********************************************************************
*                            ~INT_ARRAY_CLASS                          *
***********************************************************************/
INT_ARRAY_CLASS::~INT_ARRAY_CLASS()
{
cleanup();
}

/***********************************************************************
*                             INT_ARRAY_CLASS                          *
*                                 CLEANUP                              *
***********************************************************************/
bool INT_ARRAY_CLASS::contains( int sorc )
{
int i;

for ( i=0; i<n; i++ )
    {
    if ( p[i] == sorc )
        return true;
    }

return false;
}

/***********************************************************************
*                             INT_ARRAY_CLASS                          *
*                                   []                                 *
***********************************************************************/
int & INT_ARRAY_CLASS::operator[]( int i )
{
static int empty_int = 0;
int * newp;
int   j;

if ( i >= n )
    {
    newp = new int[i+1];
    if ( newp )
        {
        for ( j=0; j<=i; j++ )
            {
            if ( j < n )
                newp[j] = p[j];
            else
                newp[j] = 0;
            }
        cleanup();
        p = newp;
        n = i+1;
        }
    }

if ( p && i>=0 && i<n )
    return p[i];

return empty_int;
}

