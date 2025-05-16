#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\listbox.h"
#include "..\include\subs.h"

const int LISTBOX_TYPE_INDEX  = 0;
const int COMBOBOX_TYPE_INDEX = 1;

const UINT ResetCmd[2]      = { LB_RESETCONTENT,    CB_RESETCONTENT    };
const UINT AddCmd[2]        = { LB_ADDSTRING,       CB_ADDSTRING       };
const UINT DeleteCmd[2]     = { LB_DELETESTRING,    CB_DELETESTRING    };
const UINT GetCurSelCmd[2]  = { LB_GETCURSEL,       CB_GETCURSEL       };
const UINT SetCurSelCmd[2]  = { LB_SETCURSEL,       CB_SETCURSEL       };
const UINT FindStringCmd[2] = { LB_FINDSTRINGEXACT, CB_FINDSTRINGEXACT };
const UINT FindPrefixCmd[2] = { LB_FINDSTRING,      CB_FINDSTRING      };
const UINT GetCountCmd[2]   = { LB_GETCOUNT,        CB_GETCOUNT        };
const UINT GetTextCmd[2]    = { LB_GETTEXT,         CB_GETLBTEXT       };
const UINT GetTextLenCmd[2] = { LB_GETTEXTLEN,      CB_GETLBTEXTLEN    };
const UINT InsertCmd[2]     = { LB_INSERTSTRING,    CB_INSERTSTRING    };
const LRESULT CmdError[2]   = { LB_ERR,             CB_ERR             };

const TCHAR NullChar       = TEXT( '\0' );
static TCHAR EmptyString[] = TEXT( "" );

/***********************************************************************
*                          ~LISTBOX_CLASS                              *
***********************************************************************/
LISTBOX_CLASS::~LISTBOX_CLASS( void )
{
if ( buf )
    delete[] buf;
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
BOOLEAN LISTBOX_CLASS::init( HWND hWnd )
{
is_extendedsel = FALSE;

w = hWnd;

if ( w )
    {
    if ( is_a_listbox(w) )
        {
        c = LISTBOX_TYPE_INDEX;
        if ( (GetWindowLong(w, GWL_STYLE) & (LBS_EXTENDEDSEL | LBS_MULTIPLESEL) ) != 0 )
            is_extendedsel = TRUE;
        }
    else
        {
        c = COMBOBOX_TYPE_INDEX;
        }

    return TRUE;
    }
else
    {
    c = 0;
    if ( buf )
        {
        delete[] buf;
        buf = 0;
        }
    buflen = 0;
    }

return FALSE;
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
BOOLEAN LISTBOX_CLASS::init( HWND dialog_box, INT id )
{
return init( GetDlgItem(dialog_box, id) );
}

/***********************************************************************
*                                EMPTY                                 *
***********************************************************************/
void LISTBOX_CLASS::empty( void )
{
SendMessage( w, ResetCmd[c], TRUE, 0L );
}

/***********************************************************************
*                    LISTBOX_CLASS::CHECK_BUF_SIZE                     *
***********************************************************************/
BOOLEAN LISTBOX_CLASS::check_buf_size( int slen )
{

if ( slen > buflen )
    {
    if ( buf )
        delete[] buf;

    buf = new TCHAR[slen+1];
    if ( buf )
        {
        buflen = slen;
        }
    else
        {
        buflen = 0;
        return FALSE;
        }
    }

return TRUE;
}

/***********************************************************************
*                    LISTBOX_CLASS::CHECK_BUF_SIZE                     *
***********************************************************************/
BOOLEAN LISTBOX_CLASS::check_buf_size( TCHAR * s )
{
int     slen;

slen = lstrlen( s );

return check_buf_size( slen );
}

/***********************************************************************
*                           CHECK_ITEM_SIZE                            *
***********************************************************************/
BOOLEAN LISTBOX_CLASS::check_item_size( INT i )
{
LRESULT x;
int     slen;

x = SendMessage( w, GetTextLenCmd[c], i, 0L );
if ( x != CmdError[c] )
    {
    slen = int( x );
    return check_buf_size( slen );
    }

return FALSE;
}

/***********************************************************************
*                                 ADD                                  *
***********************************************************************/
INT LISTBOX_CLASS::add( TCHAR * s )
{
LRESULT x;

if ( !check_buf_size(s) )
    return NO_INDEX;

x = SendMessage( w, AddCmd[c], 0, (LPARAM) s );
if ( x == CmdError[c] )
    return NO_INDEX;

return (INT) x;
}

/***********************************************************************
*                               INSERT                                 *
***********************************************************************/
INT LISTBOX_CLASS::insert( INT i, TCHAR * s )
{
LRESULT x;

if ( !check_buf_size(s) )
    return NO_INDEX;

x = SendMessage( w, InsertCmd[c], i, (LPARAM) s );
if ( x == CmdError[c] )
    return NO_INDEX;

return (INT) x;
}

/***********************************************************************
*                              SETCURSEL                               *
***********************************************************************/
BOOLEAN LISTBOX_CLASS::setcursel( INT i )
{
LRESULT x;

if ( is_extendedsel )
    x = SendMessage( w, LB_SETSEL, TRUE, i );
else
    x = SendMessage( w, SetCurSelCmd[c], i, 0L );

if ( x == CmdError[c] )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                              SETCURSEL                               *
***********************************************************************/
BOOLEAN LISTBOX_CLASS::setcursel( TCHAR * s )
{
LRESULT x;

if ( s )
    {
    x = SendMessage( w, FindStringCmd[c], -1, (LPARAM) s );
    if ( x != CmdError[c] )
        {
        setcursel( (INT) x );
        return TRUE;
        }
    else
        {
        setcursel( (INT) -1 );
        }
    }

return FALSE;
}

/***********************************************************************
*                              FINDPREFIX                              *
***********************************************************************/
BOOLEAN LISTBOX_CLASS::findprefix( TCHAR * s )
{
LRESULT x;

x = SendMessage( w, FindPrefixCmd[c], -1, (LPARAM) s );
if ( x != CmdError[c] )
    {
    SendMessage( w, SetCurSelCmd[c], x, 0L );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                              REDRAW_OFF                              *
***********************************************************************/
void LISTBOX_CLASS::redraw_off( void )
{
SendMessage( w, WM_SETREDRAW, 0,  0l );
}

/***********************************************************************
*                              REDRAW_ON                               *
***********************************************************************/
void LISTBOX_CLASS::redraw_on( void )
{
SendMessage( w, WM_SETREDRAW, 1,  0l );
InvalidateRect( w, NULL, TRUE );
}


/***********************************************************************
*                                COUNT                                 *
***********************************************************************/
INT LISTBOX_CLASS::count( void )
{
LRESULT n;

n = SendMessage( w, GetCountCmd[c], 0, 0L );
if ( n == CmdError[c] )
    n = 0;

return INT(n);
}

/***********************************************************************
*                             CURRENT_INDEX                            *
***********************************************************************/
INT LISTBOX_CLASS::current_index( void )
{
LRESULT n;

n = 0;
if ( c == LISTBOX_TYPE_INDEX )
    {
    n = SendMessage( w, LB_GETCARETINDEX, 0, 0L );
    if ( n == LB_ERR )
        n = 0;
    }

return INT(n);
}

/***********************************************************************
*                           SET_CURRENT_INDEX                          *
***********************************************************************/
BOOLEAN LISTBOX_CLASS::set_current_index( INT new_index  )
{
LRESULT n;

if ( c == LISTBOX_TYPE_INDEX )
    {
    n = SendMessage( w, LB_SETCARETINDEX, new_index, MAKELPARAM(FALSE, 0) );
    if ( n == LB_ERR )
        return FALSE;
    }

return TRUE;
}

/***********************************************************************
*                              GET_ITEM_DATA                           *
***********************************************************************/
int LISTBOX_CLASS::get_item_data( INT item_index )
{
LRESULT n;

n = SendMessage( w, LB_GETITEMDATA, (WPARAM) item_index, (LPARAM) 0 );

return (int) n;
}

/***********************************************************************
*                              GET_ITEM_DATA                           *
***********************************************************************/
int LISTBOX_CLASS::get_item_data()
{
INT     item_index;
LRESULT n;

item_index = current_index();
n = SendMessage( w, LB_GETITEMDATA, (WPARAM) item_index, (LPARAM) 0 );

return (int) n;
}

/***********************************************************************
*                              SET_ITEM_DATA                           *
***********************************************************************/
bool LISTBOX_CLASS::set_item_data( INT item_index, int value )
{
LRESULT n;

n = SendMessage( w, LB_SETITEMDATA, (WPARAM) item_index, (LPARAM) value );
if ( n == LB_ERR )
    return false;

return true;
}

/***********************************************************************
*                              SET_ITEM_DATA                           *
***********************************************************************/
bool LISTBOX_CLASS::set_item_data( int value )
{
INT     item_index;
LRESULT n;

item_index = current_index();

n = SendMessage( w, LB_SETITEMDATA, (WPARAM) item_index, (LPARAM) value );
if ( n == LB_ERR )
    return false;

return true;
}

/***********************************************************************
*                                 TOP_INDEX                            *
***********************************************************************/
INT LISTBOX_CLASS::top_index( void )
{
LRESULT n;

n = 0;
if ( c == LISTBOX_TYPE_INDEX )
    {
    n = SendMessage( w, LB_GETTOPINDEX, 0, 0L );
    if ( n == LB_ERR )
        n = 0;
    }

return INT(n);
}

/***********************************************************************
*                             SET_TOP_INDEX                            *
***********************************************************************/
BOOLEAN LISTBOX_CLASS::set_top_index( INT new_index  )
{
LRESULT n;

if ( c == LISTBOX_TYPE_INDEX )
    {
    n = SendMessage( w, LB_SETTOPINDEX, new_index, 0 );
    if ( n == LB_ERR )
        return FALSE;
    }

return TRUE;
}

/***********************************************************************
*                               REMOVE                                 *
***********************************************************************/
BOOLEAN LISTBOX_CLASS::remove( INT x )
{
LRESULT status;

status = SendMessage( w, DeleteCmd[c], x, 0L );
if ( status == CmdError[c] )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                             IS_SELECTED                              *
***********************************************************************/
BOOLEAN LISTBOX_CLASS::is_selected( INT item_index )
{

LRESULT x;

x = SendMessage( w, LB_GETSEL, item_index, 0L );
if ( x != LB_ERR && x != 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                        LISTBOX_CLASS::REPLACE                        *
*                   Assumes the current selected row                   *
***********************************************************************/
BOOLEAN LISTBOX_CLASS::replace( TCHAR * s )
{
INT x;
BOOLEAN was_selected;

if ( c != LISTBOX_TYPE_INDEX )
    return FALSE;

x = current_index();
was_selected = is_selected( x );
if ( remove(x) )
    {
    x = insert( x, s );
    if ( x != NO_INDEX )
        {
        if ( was_selected )
            setcursel( x );
        else
            set_current_index( x );
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                      LISTBOX_CLASS::SET_SELECT                       *
***********************************************************************/
void LISTBOX_CLASS::set_select( INT item_index, BOOLEAN is_selected )
{

SendMessage( w, LB_SETSEL, is_selected, item_index );

}

/***********************************************************************
*                              SELECT_ALL                              *
***********************************************************************/
void LISTBOX_CLASS::select_all( WPARAM is_set )
{
const LPARAM i = -1;

redraw_off();

if ( !is_extendedsel && !is_set )
    {
    SendMessage( w, SetCurSelCmd[c], -1, 0L );
    }
else
    {
    if ( c == LISTBOX_TYPE_INDEX )
        SendMessage( w, LB_SETSEL, is_set, i );
    }

redraw_on();
}

/***********************************************************************
*                             SELECT_COUNT                             *
***********************************************************************/
INT LISTBOX_CLASS::select_count( void )
{
LRESULT n;

n = 0;

if ( c == LISTBOX_TYPE_INDEX )
    {
    n = SendMessage(w, LB_GETSELCOUNT, 0, 0L);
    if ( n == CmdError[c] )
        n = 0;
    }

return INT( n );
}

/***********************************************************************
*                              ITEM_TEXT                               *
***********************************************************************/
TCHAR * LISTBOX_CLASS::item_text( INT i )
{
LRESULT x;

if ( !check_item_size(i) )
    return EmptyString;

x = SendMessage( w, GetTextCmd[c], i, (LPARAM) buf );
if ( x == CmdError[c] )
    return EmptyString;

return buf;
}

/***********************************************************************
*                           SELECTED_INDEX                             *
***********************************************************************/
INT LISTBOX_CLASS::selected_index( void )
{
LRESULT x;

x = SendMessage( w, GetCurSelCmd[c], 0, 0L );
if ( x == CmdError[c] )
    return NO_INDEX;

return (INT) x;
}

/***********************************************************************
*                           SELECTED_TEXT                              *
***********************************************************************/
TCHAR * LISTBOX_CLASS::selected_text( void )
{
INT x;

x = selected_index();
if ( x == NO_INDEX )
    return EmptyString;

return item_text( x );
}

/***********************************************************************
*                            GET_SELECT_LIST                           *
*                                                                      *
* This routine will create an array of indexes of the selected         *
* listbox entries. If n > 0 then the calling program is responsible    *
* for deleting the array when finished with it.                        *
***********************************************************************/
INT LISTBOX_CLASS::get_select_list( INT ** dest )
{
INT     n;
INT   * ip;
LRESULT x;

if ( c != LISTBOX_TYPE_INDEX )
    return 0;

n = select_count();
if ( n <= 0 )
    return 0;

ip = new INT[n];
if ( !ip )
    return 0;

x = SendMessage( w, LB_GETSELITEMS, n, (LPARAM) ip );
if ( x != LB_ERR && x > 0 )
    {
    *dest = ip;
    return INT(x);
    }

delete[] ip;
return 0;
}

/***********************************************************************
*                           VISIBLE_ROW_COUNT                          *
***********************************************************************/
INT LISTBOX_CLASS::visible_row_count( void )
{
RECT    r;
LRESULT x;
long    n;

if ( c != LISTBOX_TYPE_INDEX )
    return 1;

GetClientRect( w, &r );

n = 1;
x = SendMessage( w, LB_GETITEMHEIGHT, 0, 0L );
if ( x != LB_ERR && x > 0 )
    {
    n = r.bottom - r.top;
    n /= (long) x;
    }

return (INT) n;
}

/***********************************************************************
*                             LISTBOX_CLASS                            *
*                   CONVERT_TAB_ARRAY_TO_DIALOG_UNITS                  *
* The array is assumed to be in pixels. It will be converted to        *
* the dialog units of the listbox's parent dialog box.                 *
***********************************************************************/
bool LISTBOX_CLASS::convert_tab_array_to_dialog_units( INT * tabs, int n )
{
HWND  parent;
RECT  r;
INT   horizontal_base_units;

if ( !tabs )
    return false;

parent = GetParent( w );
if ( !parent )
    return false;

r.bottom = 1;
r.left   = 1;
r.right  = 4;
r.top    = 1;
MapDialogRect( parent, &r );
horizontal_base_units = r.right;
if ( horizontal_base_units < 1 )
    horizontal_base_units = 1;

while ( n > 0 )
    {
    (*tabs) *= 4;
    (*tabs) /= horizontal_base_units;
    tabs++;
    n--;
    }

return true;
}

/***********************************************************************
*                           SET_PIXEL_TABS                             *
* Set the tabs from a pixel array. The array entries will be converted *
* to dialog units and sent to the listbox.                             *
***********************************************************************/
bool LISTBOX_CLASS::set_pixel_tabs( INT * tabs, int n )
{
if ( n < 0 || (n>0 && !tabs) )
    return false;

if ( n > 0 )
    {
    convert_tab_array_to_dialog_units( tabs, n );
    SendMessage( w, LB_SETTABSTOPS, (WPARAM) n, (LPARAM) tabs );
    return true;
    }

return false;
}

/***********************************************************************
*                                SET_TABS                              *
* Sets the tabs of the listbox to correspond to the items in a header  *
* control.                                                             *
***********************************************************************/
bool LISTBOX_CLASS::set_tabs( HWND header_control_handle )
{
int   i;
int   n;
INT * tabs;
HD_ITEM hdi;
int   width;
bool  status;

if ( !w )
    return false;

n = SendMessage( header_control_handle, HDM_GETITEMCOUNT, 0, 0L );
if ( n < 2 )
    return true;

tabs = new INT[n];
if ( !tabs )
    return false;

width = 0;
for ( i=0; i<n; i++ )
    {
    hdi.mask = HDI_WIDTH;
    SendMessage( header_control_handle, HDM_GETITEM, (WPARAM) i, (LPARAM) &hdi );
    width += hdi.cxy;
    tabs[i] = width;
    }

status = set_pixel_tabs( tabs, n );
delete[] tabs;
return status;
}

/***********************************************************************
*                                SET_TABS                              *
*  The static controls must be numbered consecutively for this to work *
***********************************************************************/
bool LISTBOX_CLASS::set_tabs( INT first_static_control, INT last_static_control )
{

int   i;
int   id;
int   n;
int   left;
INT * tabs;
HWND  parent;
RECT  r;

if ( !w )
    return false;

parent = GetParent( w );
if ( !parent )
    return false;

n = 1 + last_static_control - first_static_control;
if ( n <= 0 )
    return false;

tabs = new INT[n];
if ( !tabs )
    return false;

GetWindowRect( w, &r );
left = r.left;

id = first_static_control;
for ( i=0; i<n; i++ )
    {
    GetWindowRect( GetDlgItem(parent, id), &r );
    tabs[i] = r.left - left;
    id++;
    }

set_pixel_tabs( tabs, n );
delete[] tabs;

return true;
}

/***********************************************************************
*                               REFRESH                                *
***********************************************************************/
void LISTBOX_CLASS::refresh()
{
if ( !w )
    return;

if ( IsWindowVisible(w) )
    InvalidateRect( w, 0, TRUE );
}

