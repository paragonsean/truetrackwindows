#include <windows.h>
#include <conio.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

const TCHAR NullChar = '\0';

class POINT_ARRAY_CLASS
    {
    private:

    POINT * p;
    int     n;
    int     i;
    
    public:

    POINT_ARRAY_CLASS() { p=0; n=0; i=-1; }
    POINT_ARRAY_CLASS( const POINT_ARRAY_CLASS & sorc );
    ~POINT_ARRAY_CLASS();
    int  count( void ) { return n; }
    int  index( void ) { return i; }
    void rewind( void ) { i=-1; }
    void cleanup( void );
    bool next( void );
    bool set( long new_x, long new_y );
    POINT * release_array( void ); 
    bool init( int new_n );
    POINT & operator[]( int i );
    POINT_ARRAY_CLASS & operator=( const POINT_ARRAY_CLASS & sorc );
    POINT & value( void ) { return operator[]; }
    };
 
/***********************************************************************
*                           POINT_ARRAY_CLASS                          *
***********************************************************************/
POINT_ARRAY_CLASS::~POINT_ARRAY_CLASS()
{
cleanup();
}

/***********************************************************************
*                           POINT_ARRAY_CLASS                          *
***********************************************************************/
POINT_ARRAY_CLASS::POINT_ARRAY_CLASS( const POINT_ARRAY_CLASS & sorc )
{
p = 0;
n = 0;
i = -1;
*this = sorc;
}

/***********************************************************************
*                           POINT_ARRAY_CLASS                          *
*                             RELEASE_ARRAY                            *
* I am going to pass the array to someone who will then delete it.     *
***********************************************************************/
POINT * POINT_ARRAY_CLASS::release_array( void )
{
POINT * copy;

copy = p;
p    = 0;
n    = 0;

return copy;
}

/***********************************************************************
*                           POINT_ARRAY_CLASS                          *
*                                CLEANUP                               *
***********************************************************************/
void POINT_ARRAY_CLASS::cleanup( void )
{
if ( p )
    {
    delete[] p;
    p = 0;
    }
n = 0;
i = -1;
}

/***********************************************************************
*                           POINT_ARRAY_CLASS                          *
*                                   =                                  *
***********************************************************************/
POINT_ARRAY_CLASS & POINT_ARRAY_CLASS::operator=( const POINT_ARRAY_CLASS & sorc )
{
cleanup();
if ( sorc.n > 0 )
    {
    init( sorc.n );
    if ( n > 0 )
        {
        int j;
        for ( j=0; j<n; j++ )
            p[j] = sorc.p[j];
        i = sorc.i;
        }
    }

return *this;
}

/***********************************************************************
*                           POINT_ARRAY_CLASS                          *
*                                 NEXT                                 *
*               Next does NOT add a point to the array.                *
***********************************************************************/
bool POINT_ARRAY_CLASS::next( void )
{
if ( i < (n-1) )
    {
    i++;
    return true;
    }

return false;
}

/***********************************************************************
*                           POINT_ARRAY_CLASS                          *
*                                  SET                                 *
***********************************************************************/
bool POINT_ARRAY_CLASS::set( long new_x, long new_y )
{
if ( i != -1 )
    {
    p[i].x = new_x;
    p[i].y = new_y;
    return true;
    }

return false;
}

/***********************************************************************
*                           POINT_ARRAY_CLASS                          *
*                                  INIT                                *
***********************************************************************/
bool POINT_ARRAY_CLASS::init( int new_n )
{
POINT * newp;
int j;
 
if ( new_n > 0 )
    {
    newp = new POINT[new_n];
    if ( !newp )
        return false;
    }
else
    {
    newp  = 0;
    new_n = 0;
    }
 
if ( n > 0 )
    {
    minint( n, new_n );
    for( j=0; j<n; j++ )
        newp[j] = p[j];
    delete[] p;
    }
 
p = newp;
 
/*
-------------------------
Initialize any new points
------------------------- */
while ( n < new_n )
    {
    p[n].x = 0;
    p[n].y = 0;
    n++;
    }

/*
---------------------------------------------------- 
Make sure the current index is within the new bounds
---------------------------------------------------- */ 
if ( i >= n )
    i = n-1; 

return true;
}
 
/***********************************************************************
*                           POINT_ARRAY_CLASS                          *
*                                  []                                  *
***********************************************************************/
POINT & POINT_ARRAY_CLASS::operator[]( int j )
{
static POINT emptypoint;
 
if ( j < 0 )
    return emptypoint;
 
if ( j >= n )
    init( j+1 );
 
if ( j >= n )
    return emptypoint;
 
return p[j];
}
 

/***********************************************************************
*                               GETLINE                                *
***********************************************************************/
TCHAR * getline( void )
{
static TCHAR buf[103];
buf[0] = 100;

cgets( buf );

return buf+2;
}

/***********************************************************************
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
int i;

POINT_ARRAY_CLASS pa;

pa.init( 10 );
i = 1;
while( pa.next() )
    {
    pa.set( i, i*2 );
    i++;
    }

for ( i=0; i<10; i++ )
    cprintf( "i = %d, x = %d, y = %d ...\n\r", i, pa[i].x, pa[i].y );

POINT_ARRAY_CLASS pb = pa;

for ( i=0; i<10; i++ )
    cprintf( "i = %d, x = %2d, y = %2d ...\n\r", i, pb[i].x, pb[i].y );

return 0;
}

