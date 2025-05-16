#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\stringcl.h"
#include "..\include\ringcl.h"
#include "..\include\subs.h"
 
/***********************************************************************
*                              CLEAR                                   *
***********************************************************************/
void RING_CLASS::clear( void )
{
int32 i;
 
if ( s )
    {
    for ( i=0; i<n; i++ )
        s[i].empty();
 
    delete[] s;
    }
 
s      = 0;
index  = 0;
outdex = 0;
n      = 0;
}
 
/***********************************************************************
*                            ~RING_CLASS                               *
***********************************************************************/
RING_CLASS::~RING_CLASS()
{
clear();
}
 
/***********************************************************************
*                             RING_CLASS                               *
***********************************************************************/
RING_CLASS::RING_CLASS()
{
 
s      = 0;
index  = 0;
outdex = 0;
n      = 0;
 
}
 
/***********************************************************************
*                               INIT                                   *
***********************************************************************/
BOOLEAN RING_CLASS::init( int32 max_n )
{
clear();
 
if ( max_n < 2 )
    return FALSE;
 
s = new STRING_CLASS[max_n];
if ( s )
    {
    n      = max_n;
    index  = 0;
    outdex = 0;
    return TRUE;
    }

return FALSE;
}
 
/***********************************************************************
*                               PUSH                                   *
***********************************************************************/
BOOLEAN RING_CLASS::push( TCHAR * sorc )
{
int32 i;
i = index;
i++;
if ( i >= n )
    i = 0;
 
if ( i == outdex )
    return FALSE;
 
s[i]  = sorc;
index = i;
return TRUE;
}
 
/***********************************************************************
*                                POP                                   *
***********************************************************************/
TCHAR * RING_CLASS::pop( void )
{
if ( !s )
    return 0;
 
/*
-----------------------------------------------
If I have cought the gazinta string, I am empty
----------------------------------------------- */
if ( outdex == index )
    return 0;
 
/*
------------------------------------
I am finished with the current entry
------------------------------------ */
s[outdex].empty();
 
if ( outdex == n-1 )
    outdex = 0;
else
    outdex++;
 
if ( s[outdex].isempty() )
    return 0;
 
return s[outdex].text();
}
 
