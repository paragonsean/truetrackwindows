#include <windows.h>

#include "windowlist.h"
#include "..\include\wclass.h"

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                                empty                                 *
***********************************************************************/
void WINDOW_LIST_CLASS::empty()
{
if ( list )
    {
    delete[] list;
    list = 0;
    n = 0;
    max_n = 0;
    x = -1;
    }
}

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                                destroy                               *
*           Like remove_all but destroys the window as well.           *
***********************************************************************/
void WINDOW_LIST_CLASS::destroy()
{
int i;

for ( i=0; i<n; i++ )
    {
    if ( list[i] )
        {
        DestroyWindow( list[i] );
        list[i] = 0;
        }
    }

n = 0;
x = -1;
}

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                              remove_all                              *
*                Like empty but don't delete the array.                *
***********************************************************************/
void WINDOW_LIST_CLASS::remove_all()
{
int i;
if ( list )
    {
    n = 0;
    for ( i=0; i<max_n; i++ )
        list[i] = 0;
    x = -1;
    }
}

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                         ~WINDOW_LIST_CLASS                           *
***********************************************************************/
WINDOW_LIST_CLASS::~WINDOW_LIST_CLASS()
{
empty();
}

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                               setsize                                *
*                                                                      *
* This will only increase the size of the array. If the new size       *
* is less than n, n will be decreased but max_n will not.              *
*                                                                      *
***********************************************************************/
bool WINDOW_LIST_CLASS::setsize( int new_max_n )
{
HWND * p;
HWND * pcopy;
int    i;

pcopy = 0;

if ( new_max_n > max_n )
    {
    p = new HWND[new_max_n];

    if ( !p )
        return false;

    /*
    --------------------------------------
    Copy an existing list to the new array
    -------------------------------------- */
    if ( list )
        {
        for ( i=0; i<n; i++ )
            p[i] = list[i];

        pcopy = list;
        }

    /*
    --------------------
    Zero the new entries
    -------------------- */
    for ( i=n; i<new_max_n; i++ )
        p[i] = 0;

    /*
    -------------------------
    Put the new list in place
    ------------------------- */
    list = p;
    max_n = new_max_n;

    /*
    -------------------
    Delete the old list
    ------------------- */
    if ( pcopy )
        delete[] pcopy;
    }
else if ( n > new_max_n )
    {
    n = new_max_n;
    for ( i=n; i<max_n; i++ )
        list[i] = 0;

    if ( x >= n )
        x = n-1;
    }

return true;
}

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                               contains                               *
***********************************************************************/
bool WINDOW_LIST_CLASS::contains( HWND w )
{
int i;

for ( i=0; i<n; i++ )
    {
    if ( list[i] == w )
        return true;
    }
return false;
}

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                                 hide                                 *
***********************************************************************/
void WINDOW_LIST_CLASS::hide()
{
int i;

for ( i=0; i<n; i++ )
    {
    if ( list[i] )
        ShowWindow( list[i], SW_HIDE );
    }
}

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                                enable                                *
***********************************************************************/
void WINDOW_LIST_CLASS::enable( BOOL is_enabled )
{
int i;

for ( i=0; i<n; i++ )
    {
    if ( list[i] )
        EnableWindow( list[i], is_enabled );
    }
}

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                               refresh                                *
***********************************************************************/
void WINDOW_LIST_CLASS::refresh()
{
int i;
HWND w;

for ( i=0; i<n; i++ )
    {
    w = list[i];
    if ( w )
        {
        if ( IsWindowVisible(w) )
            InvalidateRect( w, 0, TRUE );
        }
    }
}

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                                 show                                 *
***********************************************************************/
void WINDOW_LIST_CLASS::show()
{
int i;

for ( i=0; i<n; i++ )
    {
    if ( list[i] )
        ShowWindow( list[i], SW_SHOWNA );
    }
}


/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                               offset                                 *
***********************************************************************/
void WINDOW_LIST_CLASS::offset( POINT p )
{
int i;
WINDOW_CLASS w;

for ( i=0; i<n; i++ )
    {
    if ( list[i] )
        {
        w = list[i];
        w.offset( p );
        }
    }
}

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                                 find                                 *
*                                                                      *
*    Find is like contains except it sets the current index.           *
*                                                                      *
***********************************************************************/
bool WINDOW_LIST_CLASS::find( HWND w )
{
int i;

for ( i=0; i<n; i++ )
    {
    if ( list[i] == w )
        {
        x = i;
        return true;
        }
    }

return false;
}

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                                 copy                                 *
***********************************************************************/
bool WINDOW_LIST_CLASS::copy( const WINDOW_LIST_CLASS & sorc )
{
int i;

if ( sorc.n > max_n )
    {
    if ( !setsize( sorc.n ) )
        return false;
    }

for ( i=0; i<sorc.n; i++ )
    list[i] = sorc.list[i];

n = sorc.n;
x = -1;

return true;
}

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                               handle                                 *
***********************************************************************/
HWND WINDOW_LIST_CLASS::handle()
{
if ( x >= 0 )
    return list[x];

return 0;
}

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                                 []                                   *
***********************************************************************/
HWND & WINDOW_LIST_CLASS::operator[]( int i )
{
static HWND empty_hwnd = 0;

if ( i>=0 && i< max_n )
    return list[i];

return empty_hwnd;
}

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                                next                                  *
***********************************************************************/
HWND WINDOW_LIST_CLASS::next()
{
if ( x < (n-1) )
    {
    x++;
    return handle();
    }

return 0;
}

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                               append                                 *
***********************************************************************/
bool WINDOW_LIST_CLASS::append( const WINDOW_LIST_CLASS & sorc )
{
int i;

if ( !setsize(n + sorc.n) )
    return false;

for ( i=0; i<sorc.n; i++ )
    {
    list[n] = sorc.list[i];
    n++;
    }

return true;
}

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                               append                                 *
***********************************************************************/
bool WINDOW_LIST_CLASS::append( HWND w )
{
if ( !setsize(n + 1) )
    return false;

list[n] = w;
n++;

return true;
}

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                               remove                                 *
***********************************************************************/
bool WINDOW_LIST_CLASS::remove( const WINDOW_LIST_CLASS & sorc )
{
int di;
int i;
int si;

for ( i=0; i<n; i++ )
    {
    if ( list[i] == sorc.list[0] )
        {
        if ( n >= i+sorc.n )
            {
            si = sorc.n - 1;
            if ( list[i+si] == sorc.list[si] )
                {
                di = i;
                si += i + 1;
                while ( si < n )
                    list[di++] = list[si++];
                n = di;
                return true;
                }
            }
        }
    }

return false;
}

/***********************************************************************
*                          WINDOW_LIST_CLASS                           *
*                               remove                                 *
***********************************************************************/
bool WINDOW_LIST_CLASS::remove( HWND dead_window )
{
int i;
int si;

for ( i=0; i<n; i++ )
    {
    if ( list[i] == dead_window )
        {
        si = i + 1;
        while ( si < n )
            list[i++] = list[si++];
        n--;
        return true;
        }
    }

return false;
}

