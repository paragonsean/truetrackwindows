#include <windows.h>

typedef long int int32;
static const int MAX_DOUBLE_LEN =  40;
static const int MAX_STRING_LEN = 255;  /* String Table Entries */
static const TCHAR NullChar  = TEXT( '\0' );
static const TCHAR Unknown[] = TEXT( "UNKNOWN" );

double    extdouble( const TCHAR * sorc, short nof_chars );

/***********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( HINSTANCE instance, UINT resource_id )
{
static TCHAR s[MAX_STRING_LEN+1];

if ( LoadString(instance, resource_id, s, MAX_STRING_LEN+1) == 0 )
    lstrcpy( s, Unknown );

return s;
}

/***********************************************************************
*                             GET_LB_COUNT                             *
***********************************************************************/
short get_lb_count( HWND w )
{
LRESULT n;

n = SendMessage( w, LB_GETCOUNT, 0, 0L);
if ( n == LB_ERR )
    n = 0;

return short( n );
}

/***********************************************************************
*                             GET_LB_COUNT                             *
***********************************************************************/
short get_lb_count( HWND w, int box )
{
return get_lb_count( GetDlgItem(w, box) );
}

/***********************************************************************
*                         GET_LB_SELECT_COUNT                          *
***********************************************************************/
short get_lb_select_count( HWND w )
{
LRESULT n;

n = SendMessage( w, LB_GETSELCOUNT, 0, 0L );
if ( n == LB_ERR )
    n = 0;

return short( n );
}

/***********************************************************************
*                         GET_LB_SELECT_COUNT                          *
***********************************************************************/
short get_lb_select_count( HWND w, int box )
{
return get_lb_select_count( GetDlgItem(w, box) );
}

/***********************************************************************
*                            GET_LB_SELECT                             *
***********************************************************************/
BOOLEAN get_lb_select( HWND w, TCHAR * dest )
{
LRESULT x;

x = SendMessage( w, LB_GETCURSEL, 0, 0L);
if ( x != LB_ERR )
    x = SendMessage( w, LB_GETTEXT, x, (LPARAM) dest );

if ( x == LB_ERR )
    {
    *dest = NullChar;
    return FALSE;
    }

return TRUE;
}

/***********************************************************************
*                            GET_LB_SELECT                             *
***********************************************************************/
BOOLEAN get_lb_select( HWND w, int box, TCHAR * dest )
{
return get_lb_select( GetDlgItem(w, box), dest );
}

/***********************************************************************
*                            GET_CB_SELECT                             *
***********************************************************************/
BOOLEAN get_cb_select( HWND w, TCHAR * dest )
{
LRESULT x;

x = SendMessage( w, CB_GETCURSEL, 0, 0L );

if ( x != CB_ERR )
    x = SendMessage( w, CB_GETLBTEXT, x, (LPARAM) dest );

if ( x == CB_ERR )
    {
    *dest = NullChar;
    return FALSE;
    }

return TRUE;
}

/***********************************************************************
*                            GET_CB_SELECT                             *
***********************************************************************/
BOOLEAN get_cb_select( HWND w, int box, TCHAR * dest )
{
return get_cb_select( GetDlgItem(w, box), dest );
}

/***********************************************************************
*                         SET_CURRENT_LB_ITEM                          *
***********************************************************************/
BOOLEAN set_current_lb_item( HWND w, TCHAR * item_to_find )
{
LRESULT x;

x = SendMessage( w, LB_FINDSTRINGEXACT, -1, (LPARAM) item_to_find );
if ( x != LB_ERR )
    {
    SendMessage( w, LB_SETCURSEL, x, 0L );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         SET_CURRENT_LB_ITEM                          *
***********************************************************************/
BOOLEAN set_current_lb_item( HWND w, int box, TCHAR * item_to_find )
{
return set_current_lb_item( GetDlgItem(w, box), item_to_find );
}

/***********************************************************************
*                      SET_CURRENT_CB_ITEM                             *
***********************************************************************/
BOOLEAN set_current_cb_item( HWND w, TCHAR * item_to_find )
{
LRESULT x;

x = SendMessage( w, CB_FINDSTRINGEXACT, -1, (LPARAM) item_to_find );
if ( x != LB_ERR )
    {
    SendMessage( w, CB_SETCURSEL, x, 0L );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         SET_CURRENT_CB_ITEM                          *
***********************************************************************/
BOOLEAN set_current_cb_item( HWND w, int box, TCHAR * item_to_find )
{
return set_current_cb_item( GetDlgItem(w, box), item_to_find );
}

/***********************************************************************
*                            LB_SELECT_ALL                             *
***********************************************************************/
void lb_select_all( HWND w )
{
short i;
short n;

n = get_lb_count( w );
for ( i=0; i<n; i++ )
    SendMessage( w, LB_SETSEL, TRUE, (LPARAM)(UINT) i );
}

/***********************************************************************
*                            LB_SELECT_ALL                             *
***********************************************************************/
void lb_select_all( HWND w, int box )
{
lb_select_all( GetDlgItem(w, box) );
}

/***********************************************************************
*                        DESKTOP_CLIENT_RECT                           *
***********************************************************************/
RECT desktop_client_rect( void )
{
HWND desktop_window;
RECT r;
int  w;
int  h;
APPBARDATA ab;

r.top = r.bottom = r.right = r.left = 0;

desktop_window = GetDesktopWindow();
if ( desktop_window )
    {
    GetClientRect( desktop_window, &r );

    ab.cbSize = sizeof( APPBARDATA );
    ab.hWnd   = 0;

    if ( (BOOL) SHAppBarMessage(ABM_GETTASKBARPOS, &ab) )
        {
        /*
        -----------------------------------------------------------
        The above call doesn't tell me which edge the taskbar is on
        ----------------------------------------------------------- */
        h = ab.rc.bottom - ab.rc.top;
        w = ab.rc.right  - ab.rc.left;
        if ( h > w )
            {
            if ( ab.rc.right >= r.right )
                r.right = ab.rc.left;         /* ABE_RIGHT  */
            else
                r.left = ab.rc.right;         /* ABE_LEFT   */
            }
        else
            {
            if ( ab.rc.bottom >= r.bottom )
                r.bottom = ab.rc.top;         /* ABE_BOTTOM */
            else
                r.top = ab.rc.bottom;         /* ABE_TOP    */
            }
        }
    }

return r;
}

/***********************************************************************
*                         IS_PREVIOUS_INSTANCE                         *
***********************************************************************/
BOOLEAN is_previous_instance( TCHAR * class_name, TCHAR * window_title )
{
HWND w;

w = FindWindow( class_name, window_title );
if ( w )
    {
    ShowWindow( w, SW_SHOWNORMAL );
    SetForegroundWindow( w );
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                          SET_MENU_VIEW_STATE                         *
***********************************************************************/
void set_menu_view_state( BOOLEAN & state, HWND w, UINT menu_choice_id )
{
HMENU menu_handle;
UINT  menu_state;

if ( state )
    menu_state = MF_CHECKED;
else
    menu_state = MF_UNCHECKED;

menu_handle = GetMenu( w );
CheckMenuItem( menu_handle, menu_choice_id, menu_state );
}

/***********************************************************************
*                        TOGGLE_MENU_VIEW_STATE                        *
***********************************************************************/
void toggle_menu_view_state( BOOLEAN & state, HWND w, UINT menu_choice_id )
{
if ( state )
    state = FALSE;
else
    state = TRUE;

set_menu_view_state( state, w, menu_choice_id );
}

/***********************************************************************
*                           GET_LB_ITEM_TEXT                           *
***********************************************************************/
BOOLEAN get_lb_item_text( HWND w, TCHAR * dest, int i, int max_len )
{
LRESULT x;

*dest = NullChar;

x = SendMessage( w, LB_GETTEXTLEN, i, 0L );
if ( x == LB_ERR )
    return FALSE;

if ( int(x) > max_len )
    return FALSE;

x = SendMessage( w, LB_GETTEXT, i, (LPARAM) dest );
if ( x == LB_ERR )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                           GET_LB_ITEM_TEXT                           *
***********************************************************************/
BOOLEAN get_lb_item_text( HWND w, int box, TCHAR * dest, int i, int max_len )
{
return get_lb_item_text( GetDlgItem(w, box), dest, i, max_len );
}

/***********************************************************************
*                               GET_TEXT                               *
* TCHAR s[MAXLINE+1];                                                  *
* get_text( s, w, MAXLINE );                                           *
***********************************************************************/
BOOLEAN get_text( TCHAR * dest, HWND w, int max_len )
{
LRESULT chars_copied;

chars_copied = SendMessage( w, WM_GETTEXT, max_len+1, (LPARAM) dest );
if ( chars_copied > 0 )
    return TRUE;

*dest = NullChar;
return FALSE;
}

/***********************************************************************
*                               GET_TEXT                               *
***********************************************************************/
BOOLEAN get_text( TCHAR * dest, HWND w, int box, int max_len )
{
return get_text( dest, GetDlgItem(w, box), max_len );
}


/***********************************************************************
*                           GET_DOUBLE_FROM_EBOX                       *
***********************************************************************/
double get_double_from_ebox( HWND w )
{
double x;
TCHAR   s[MAX_DOUBLE_LEN+1];

x = 0.0;

if ( get_text(s, w, MAX_DOUBLE_LEN) )
    x = extdouble( s, lstrlen(s) );

return x;
}

/***********************************************************************
*                           GET_DOUBLE_FROM_EBOX                       *
***********************************************************************/
double get_double_from_ebox( HWND w, int box )
{
return get_double_from_ebox( GetDlgItem(w, box) );
}

/***********************************************************************
*                               SET_TEXT                               *
* TCHAR s[MAXLINE+1];                                                  *
* get_text( w, box, s, MAXLINE );                                      *
***********************************************************************/
BOOLEAN set_text( HWND w, const TCHAR * sorc )
{
BOOLEAN status;

status = FALSE;

if ( w )
    {
    if ( SendMessage(w, WM_SETTEXT,  0, (LPARAM) sorc) )
        status = TRUE;
    }

return status;
}

/***********************************************************************
*                               SET_TEXT                               *
***********************************************************************/
BOOLEAN set_text( HWND w, int box, const TCHAR * sorc )
{
return set_text( GetDlgItem(w, box), sorc );
}

/***********************************************************************
*                              LIMIT_TEXT                              *
* Limit the number of characters that can be entered into an editbox.  *
***********************************************************************/
void limit_text( HWND w, int nof_chars )
{
SendMessage( w, EM_LIMITTEXT, (WPARAM) nof_chars, 0 );
}

/***********************************************************************
*                              LIMIT_TEXT                              *
***********************************************************************/
void limit_text( HWND w, int box, int nof_chars )
{
limit_text( GetDlgItem(w, box), nof_chars );
}

/***********************************************************************
*                            EB_SELECT_ALL                             *
***********************************************************************/
void eb_select_all( HWND w )
{
SendMessage( w, EM_SETSEL, 0, -1 );
}

/***********************************************************************
*                            EB_SELECT_ALL                             *
***********************************************************************/
void eb_select_all( HWND w, int id )
{
eb_select_all( GetDlgItem(w, id) );
}

/***********************************************************************
*                              IS_CHECKED                              *
***********************************************************************/
BOOLEAN is_checked( HWND w, int button_id )
{
if ( IsDlgButtonChecked(w, button_id) == BST_CHECKED )
    return TRUE;
return FALSE;
}

/***********************************************************************
*                              IS_CHECKED                              *
***********************************************************************/
BOOLEAN is_checked( HWND w )
{
if ( SendMessage(w, BM_GETCHECK, 0, 0L) == BST_CHECKED )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                              IS_CHANGED                              *
*    Check to see if the contents of an edit box have changed.         *
***********************************************************************/
BOOLEAN is_changed( HWND w )
{
if ( SendMessage(w, EM_GETMODIFY, 0, 0L) )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                              IS_CHANGED                              *
***********************************************************************/
BOOLEAN is_changed( HWND w, int id )
{
if ( SendMessage(GetDlgItem(w, id), EM_GETMODIFY, 0, 0L) )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                             SET_CHECKBOX                             *
***********************************************************************/
void set_checkbox( HWND w, BOOLEAN need_check )
{
WPARAM state;
if ( need_check )
    state = BST_CHECKED;
else
    state = BST_UNCHECKED;

SendMessage( w, BM_SETCHECK, state, 0L );
}

/***********************************************************************
*                             SET_CHECKBOX                             *
***********************************************************************/
void set_checkbox( HWND w, int id, BOOLEAN need_check )
{
set_checkbox( GetDlgItem(w, id), need_check );
}

/***********************************************************************
*                          GET_LB_SELECT_LIST                          *
*                                                                      *
* This routine will create an array of indexes of the selected         *
* listbox entries. If n > 0 then the calling program is responsible    *
* for deleting the array when finished with it.                        *
***********************************************************************/
short get_lb_select_list( INT ** dest, HWND w )
{
short   n;
INT   * ip;
LRESULT x;

n = get_lb_select_count( w );
if ( n <= 0 )
    return 0;

ip = new INT[n];
if ( !ip )
    return 0;

x = SendMessage( w, LB_GETSELITEMS, n, (LPARAM) ip );
if ( x != LB_ERR && x > 0 )
    {
    *dest = ip;
    return short(x);
    }

delete[] ip;
return 0;
}

/***********************************************************************
*                          GET_LB_SELECT_LIST                          *
***********************************************************************/
short get_lb_select_list( INT ** dest, HWND w, int box )
{
return get_lb_select_list( dest, GetDlgItem(w, box) );
}

/***********************************************************************
*                         RESOURCE_MESSAGE_BOX                         *
***********************************************************************/
int resource_message_box( HWND w, HINSTANCE instance, UINT msg_id, UINT title_id, UINT boxtype )
{
TCHAR s[MAX_STRING_LEN+1];
TCHAR t[MAX_STRING_LEN+1];

lstrcpy( s, resource_string(instance, msg_id)   );
lstrcpy( t, resource_string(instance, title_id) );

boxtype |= MB_SETFOREGROUND;

return MessageBox( w, s, t, boxtype );
}

/***********************************************************************
*                         RESOURCE_MESSAGE_BOX                         *
***********************************************************************/
int resource_message_box( HINSTANCE instance, UINT msg_id, UINT title_id, UINT boxtype )
{
return resource_message_box( NULL, instance, msg_id, title_id, boxtype );
}

/***********************************************************************
*                         RESOURCE_MESSAGE_BOX                         *
***********************************************************************/
void resource_message_box( HINSTANCE instance, UINT msg_id, UINT title_id )
{
resource_message_box( NULL, instance, msg_id, title_id, MB_OK | MB_SYSTEMMODAL );
}

/***********************************************************************
*                              SHRINKWRAP                              *
*   This routine shrink-wraps the main window around a dialog box.     *
***********************************************************************/
void shrinkwrap( HWND main_window, HWND dialog_window )
{
RECT dr;
RECT mr;
int  dh;
int  dw;

GetClientRect( main_window, &mr );
GetWindowRect( dialog_window, &dr );

/*
------------------------------------------------------------
Calculate how much bigger the dialog is than the client area
------------------------------------------------------------ */
dw = dr.right - dr.left - (mr.right - mr.left);
dh = dr.bottom - dr.top - (mr.bottom - mr.top);

GetWindowRect( main_window, &mr );

MoveWindow( main_window, mr.left, mr.top, mr.right-mr.left+dw, mr.bottom-mr.top+dh, TRUE );
}

/***********************************************************************
*                             IS_A_LISTBOX                             *
***********************************************************************/
BOOLEAN is_a_listbox( HWND w )
{
const TCHAR listbox_class_name[] = TEXT( "ListBox" );
const int   COMBOBOX_NAME_LEN = 8;
TCHAR s[COMBOBOX_NAME_LEN+1];
int   n;

n = GetClassName( w, s, sizeof(s) );
if ( n )
    {
    s[n] = NullChar;
    if ( lstrcmp(listbox_class_name, s) == 0 )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                            IS_AN_EDITBOX                             *
***********************************************************************/
BOOLEAN is_an_editbox( HWND w )
{
const TCHAR editbox_class_name[] = TEXT( "Edit" );
const int   EDITBOX_NAME_LEN = 4;
TCHAR s[EDITBOX_NAME_LEN+3];
int   n;

n = GetClassName( w, s, sizeof(s) );
if ( n )
    {
    s[n] = NullChar;
    if ( lstrcmp(editbox_class_name, s) == 0 )
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                           CHARACTER_HEIGHT                           *
***********************************************************************/
int32 character_height( HDC dc )
{
TEXTMETRIC tm;
GetTextMetrics( dc, &tm );
return (int32) tm.tmHeight;
}

/***********************************************************************
*                        AVERAGE_CHARACTER_WIDTH                       *
***********************************************************************/
int32 average_character_width( HDC dc )
{
TEXTMETRIC tm;
GetTextMetrics( dc, &tm );
return (int32) tm.tmAveCharWidth;
}

/***********************************************************************
*                            DRAW_RECTANGLE                            *
***********************************************************************/
void draw_rectangle( HDC dc, RECT & r, COLORREF color )
{
HBRUSH       oldbrush;
HBRUSH       mybrush;

mybrush  = CreateSolidBrush( color );
oldbrush = (HBRUSH) SelectObject( dc, mybrush );
FrameRect( dc, &r, mybrush );
SelectObject( dc, oldbrush );
DeleteObject( mybrush );
}

/***********************************************************************
*                            FILL_RECTANGLE                            *
***********************************************************************/
void fill_rectangle( HDC dc, RECT & r, COLORREF color )
{
HBRUSH       oldbrush;
HBRUSH       mybrush;

mybrush  = CreateSolidBrush( color );
oldbrush = (HBRUSH) SelectObject( dc, mybrush );
FillRect( dc, &r, mybrush );
SelectObject( dc, oldbrush );
DeleteObject( mybrush );
}

/***********************************************************************
*                               PIXEL_WIDTH                            *
***********************************************************************/
int32 pixel_width( HDC dc, TCHAR * s )
{
SIZE sz;
int  nof_chars;

nof_chars = lstrlen( s );

GetTextExtentPoint32( dc, s, nof_chars, &sz );

return (int32) sz.cx;
}

/***********************************************************************
*                               PIXEL_WIDTH                            *
***********************************************************************/
int32 pixel_width( HDC dc, TCHAR * s, int32 nof_chars )
{
SIZE sz;

GetTextExtentPoint32( dc, s, nof_chars, &sz );

return (int32) sz.cx;
}

/***********************************************************************
*                            FREE_KILOBYTES                            *
***********************************************************************/
int32 free_kilobytes( TCHAR * root )
{
static TCHAR path[] = TEXT( "C:\\" );

DWORD free_clusters;
DWORD sectors_per_cluster;
DWORD bytes_per_sector;
DWORD total_clusters;

double total_bytes;

*path = *root;

total_bytes = 0;

if ( GetDiskFreeSpace( path, &sectors_per_cluster, &bytes_per_sector, &free_clusters, &total_clusters) )
    {
    total_bytes  = (double) free_clusters;
    total_bytes *= (double) sectors_per_cluster;
    total_bytes *= (double) bytes_per_sector;
    total_bytes /= 1000.0;
    }

return (int32) total_bytes;
}

/***********************************************************************
*                             CREATE_FONT                              *
***********************************************************************/
HFONT create_font( HDC dc, long width, long height, TCHAR * name )
{
POINT      p;
LOGFONT    lf;

p.x = width;
p.y = height;

DPtoLP( dc, &p, 1 );

lf.lfHeight          = p.y;
lf.lfWidth           = p.x;
lf.lfEscapement      = 0;
lf.lfOrientation     = 0;
lf.lfWeight          = FW_DONTCARE;
lf.lfItalic          = FALSE;
lf.lfUnderline       = FALSE;
lf.lfStrikeOut       = FALSE;
lf.lfCharSet         = ANSI_CHARSET;
lf.lfOutPrecision    = 0;
lf.lfClipPrecision   = 0;
lf.lfQuality         = DEFAULT_QUALITY;
lf.lfPitchAndFamily  = 0;
lstrcpy( lf.lfFaceName, name );

if ( PRIMARYLANGID(GetUserDefaultLangID())==LANG_JAPANESE )
    {
    lf.lfCharSet = SHIFTJIS_CHARSET;
    lstrcpy(lf.lfFaceName,TEXT("MS –¾’©"));
    }

return CreateFontIndirect( &lf );
}

/***********************************************************************
*                             IS_WINDOWS_NT                            *
***********************************************************************/
BOOLEAN is_windows_nt( void )
{
OSVERSIONINFO vi;

vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

GetVersionEx( &vi );

if ( vi.dwPlatformId == VER_PLATFORM_WIN32_NT )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                SET_LISTBOX_TABS_FROM_TITLE_POSITIONS                 *
*                                                                      *
* id is an array of static text controls that I want to line up with.  *
*  n = number of ids in id.                                            *
*                                                                      *
***********************************************************************/
void set_listbox_tabs_from_title_positions( HWND parent_window, UINT listbox_id, int * id, int n )
{
int        i;
int        pixels_per_dialog_unit;
int        left;
HWND       w;
RECT       r;
int      * tabs;

if ( !parent_window )
    return;

if ( n <= 0 )
    return;

tabs = new int[n];
if ( !tabs )
    return;

r.bottom = 1;
r.left   = 1;
r.right  = 4;
r.top    = 1;
MapDialogRect( parent_window, &r );
pixels_per_dialog_unit = r.right;

w = GetDlgItem( parent_window, listbox_id );
GetWindowRect( w, &r );
left = r.left;

for ( i=0; i<n; i++ )
    {
    w = GetDlgItem( parent_window, id[i] );
    GetWindowRect( w, &r );
    tabs[i] = r.left - left;
    tabs[i] *= 4;
    tabs[i] /= pixels_per_dialog_unit;
    }

SendDlgItemMessage( parent_window, listbox_id, LB_SETTABSTOPS, (WPARAM) n, (LPARAM) tabs );


delete [] tabs;
}

/***********************************************************************
*                             SHOW_WINDOW                              *
***********************************************************************/
void show_window( HWND w, BOOLEAN is_visible )
{
int sh;

sh = SW_HIDE;
if ( is_visible )
    sh = SW_SHOW;

ShowWindow( w, sh );
}

/***********************************************************************
*                             SHOW_WINDOW                              *
***********************************************************************/
void show_window( HWND w, int id, BOOLEAN is_visible )
{
int sh;

sh = SW_HIDE;
if ( is_visible )
    sh = SW_SHOW;

ShowWindow( GetDlgItem(w, id), sh );
}
