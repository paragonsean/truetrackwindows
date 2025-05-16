#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"


/***********************************************************************
*                               DESTROY                                *
***********************************************************************/
void WINDOW_CLASS::destroy( void )
{
if ( w )
    {
    DestroyWindow( w );
    w = 0;
    }
}

/***********************************************************************
*                                 SHOW                                 *
***********************************************************************/
void WINDOW_CLASS::show( int cmd )
{
if ( w )
    ShowWindow( w, cmd );
}

/***********************************************************************
*                                ENABLE                                *
***********************************************************************/
void WINDOW_CLASS::enable( bool is_enabled )
{
BOOL state;
if ( is_enabled )
    state = TRUE;
else
    state = FALSE;
if ( w )
    EnableWindow( w, state );
}

/***********************************************************************
*                                ENABLE                                *
***********************************************************************/
void WINDOW_CLASS::enable( BOOLEAN is_enabled )
{
BOOL state;
if ( is_enabled )
    state = TRUE;
else
    state = FALSE;
if ( w )
    EnableWindow( w, state );
}


/***********************************************************************
*                                 SEND                                 *
***********************************************************************/
void WINDOW_CLASS::send( UINT msg, WPARAM wParam, LPARAM lParam )
{
if ( w )
    SendMessage( w, msg, wParam, lParam );
}

/***********************************************************************
*                                 SEND                                 *
***********************************************************************/
void WINDOW_CLASS::send( UINT msg )
{
if ( w )
    SendMessage( w, msg, 0, 0L );
}

/***********************************************************************
*                                 POST                                 *
***********************************************************************/
void WINDOW_CLASS::post( UINT msg, WPARAM wParam, LPARAM lParam )
{
if ( w )
    PostMessage( w, msg, wParam, lParam );
}

/***********************************************************************
*                                 POST                                 *
***********************************************************************/
void WINDOW_CLASS::post( UINT msg )
{
if ( w )
    PostMessage( w, msg, 0, 0L );
}

/***********************************************************************
*                                WRAP                                  *
***********************************************************************/
void WINDOW_CLASS::shrinkwrap( HWND sorc )
{
::shrinkwrap( w, sorc );
}

/***********************************************************************
*                                 MOVE                                 *
***********************************************************************/
void WINDOW_CLASS::move( int x, int y, int width, int height, BOOL need_repaint )
{
if ( w )
    MoveWindow( w, x, y, width, height, need_repaint );

}

/***********************************************************************
*                                GETRECT                               *
***********************************************************************/
BOOL WINDOW_CLASS::getrect( RECT & r )
{

if ( w )
    if ( GetWindowRect(w, &r) )
        return TRUE;

return FALSE;
}

/***********************************************************************
*                              GET_MOVE_RECT                           *
* The move rect is the rectangle that can be used to move me to        *
* my current position. If I am a child window, this is in client       *
* coordinates of the parent, otherwise it is in screen coordinates.    *
***********************************************************************/
BOOL WINDOW_CLASS::get_move_rect( RECT & r )
{
LONG  my_window_style;
POINT p;
HWND  parent;

if ( w )
    {
    if ( GetWindowRect(w, &r) )
        {
        my_window_style = GetWindowLong( w, GWL_STYLE );
        if ( my_window_style & WS_CHILD )
            {
            parent = GetParent( w );
            if ( parent )
                {
                p.x = r.left;
                p.y = r.top;
                ScreenToClient( parent, &p );
                r.left = p.x;
                r.top  = p.y;

                p.x = r.right;
                p.y = r.bottom;
                ScreenToClient( parent, &p );
                r.right  = p.x;
                r.bottom = p.y;
                }
            }
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                             REMOVE_STYLE                             *
***********************************************************************/
BOOL WINDOW_CLASS::remove_style( LONG style_bits_to_remove )
{
LONG  my_window_style;

my_window_style = GetWindowLong( w, GWL_STYLE );
my_window_style &= ~style_bits_to_remove;

SetWindowLong( w, GWL_STYLE, my_window_style );
return TRUE;
}

/***********************************************************************
*                               ADD_STYLE                              *
***********************************************************************/
BOOL WINDOW_CLASS::add_style( LONG style_bits_to_add )
{
LONG  my_window_style;

my_window_style = GetWindowLong( w, GWL_STYLE );
my_window_style |= style_bits_to_add;

SetWindowLong( w, GWL_STYLE, my_window_style );
return TRUE;
}

/***********************************************************************
*                                 STYLE                                *
***********************************************************************/
LONG WINDOW_CLASS::style()
{
LONG x;

x = 0;
if ( w )
    x = GetWindowLong( w, GWL_STYLE );

return x;
}

/***********************************************************************
*                               USETDATA                               *
***********************************************************************/
LONG WINDOW_CLASS::userdata()
{
LONG x;

x = 0;
if ( w )
    x = GetWindowLong( w, GWL_USERDATA );

return x;
}

/***********************************************************************
*                                OFFSET                                *
***********************************************************************/
void WINDOW_CLASS::offset( long dx, long dy )
{
RECT  r;

if ( get_move_rect(r) )
    {
    r.left   += dx;
    r.right  += dx;
    r.top    += dy;
    r.bottom += dy;
    }

move( r );
}

/***********************************************************************
*                                 MOVE                                 *
***********************************************************************/
void WINDOW_CLASS::move( int x, int y, BOOL need_repaint )
{
int   height;
int   width;
RECT  r;

if ( getrect(r) )
    {
    height = r.bottom - r.top;
    width  = r.right  - r.left;
    MoveWindow( w, x, y, width, height, need_repaint );
    }

}

/***********************************************************************
*                                 MOVE                                 *
***********************************************************************/
void WINDOW_CLASS::move( RECT & r )
{
if ( w )
    MoveWindow( w, r.left, r.top, r.right-r.left, r.bottom-r.top, TRUE );

}

/***********************************************************************
*                              IS_ENABLED                              *
***********************************************************************/
BOOL WINDOW_CLASS::is_enabled()
{
if ( w )
    return IsWindowEnabled(w);

return FALSE;
}

/***********************************************************************
*                              IS_VISIBLE                              *
***********************************************************************/
BOOL WINDOW_CLASS::is_visible( void )
{
if ( w )
    return IsWindowVisible(w);

return FALSE;
}

/***********************************************************************
*                            IS_MINIMIZED                              *
***********************************************************************/
BOOL WINDOW_CLASS::is_minimized( void )
{
if ( w )
    return IsIconic(w);

return FALSE;
}

/***********************************************************************
*                               REFRESH                                *
***********************************************************************/
void WINDOW_CLASS::refresh( void )
{
if ( IsWindowVisible(w) )
    InvalidateRect( w, 0, TRUE );
}

/***********************************************************************
*                               SET_TITLE                              *
***********************************************************************/
void WINDOW_CLASS::set_title( TCHAR * new_title )
{
if ( !new_title )
    return;

if ( w )
   SendMessage( w, WM_SETTEXT, 0, (LPARAM) new_title );

}

/***********************************************************************
*                          WINDOW_ARRAY_CLASS                           *
*                                empty                                 *
***********************************************************************/
void WINDOW_ARRAY_CLASS::empty()
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
*                         WINDOW_ARRAY_CLASS                           *
*                              remove_all                              *
*                Like empty but don't delete the array.                *
***********************************************************************/
void WINDOW_ARRAY_CLASS::remove_all()
{
int i;
if ( list )
    {
    n = 0;
    for ( i=0; i<max_n; i++ )
        list[i] = (HWND) 0;
    x = -1;
    }
}

/***********************************************************************
*                         WINDOW_ARRAY_CLASS                           *
*                        ~WINDOW_ARRAY_CLASS                           *
***********************************************************************/
WINDOW_ARRAY_CLASS::~WINDOW_ARRAY_CLASS()
{
empty();
}

/***********************************************************************
*                         WINDOW_ARRAY_CLASS                           *
*                               setsize                                *
*                                                                      *
* This will only increase the size of the array. If the new size       *
* is less than n, n will be decreased but max_n will not.              *
*                                                                      *
***********************************************************************/
bool WINDOW_ARRAY_CLASS::setsize( int new_max_n )
{
WINDOW_CLASS * p;
WINDOW_CLASS * pcopy;
int    i;

pcopy = 0;

if ( new_max_n > max_n )
    {
    p = new WINDOW_CLASS[new_max_n];

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
        list[i] = (HWND) 0;

    if ( x >= n )
        x = n-1;
    }

return true;
}

/***********************************************************************
*                         WINDOW_ARRAY_CLASS                           *
*                               contains                               *
***********************************************************************/
bool WINDOW_ARRAY_CLASS::contains( HWND w )
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
*                         WINDOW_ARRAY_CLASS                           *
*                                next                                  *
***********************************************************************/
bool WINDOW_ARRAY_CLASS::next()
{
if ( x < (n-1) )
    {
    x++;
    return true;
    }

return false;
}

/***********************************************************************
*                         WINDOW_ARRAY_CLASS                           *
*                                 add                                  *
***********************************************************************/
bool WINDOW_ARRAY_CLASS::add( HWND w )
{
if ( contains(w) )
    return true;

if ( !setsize(n + 1) )
    return false;

list[n] = w;
n++;

return true;
}

/***********************************************************************
*                         WINDOW_ARRAY_CLASS                           *
*                               remove                                 *
***********************************************************************/
bool WINDOW_ARRAY_CLASS::remove( HWND dead_window )
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

/***********************************************************************
*                         WINDOW_ARRAY_CLASS                           *
*                                 []                                   *
***********************************************************************/
WINDOW_CLASS & WINDOW_ARRAY_CLASS::operator[]( int i )
{
static WINDOW_CLASS empty_wc;

if ( i>=0 && i< max_n )
    return list[i];

return empty_wc;
}

