#include <windows.h>
#include <ddeml.h>
#include <commctrl.h>

#define _MAIN_

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\genlist.h"
#include "..\include\events.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"

#include "resource.h"
#include "extern.h"

#define NEW_AUTO_SHOT_INDEX 0
#define SHOT_NUMBER_INDEX   1

#define VIEW_DDE_CHOICE     203
#define VIEW_VISI_CHOICE    205
#define CLEAR_EVENTS_CHOICE 204
#define SHOW_STATUS_CHOICE  207
#define MINIMIZE_CHOICE     107
#define EXIT_CHOICE         108
#define SHOT_NUMBER_EDITBOX 102
#define MESSAGE_LISTBOX_ID  101
#define WM_USER_INITIATE    WM_USER+0x101

/*
---------------------------------------------------
Data source types for the add_new_computer function
--------------------------------------------------- */
const int POKED_DATA    = 1;
const int MAILSLOT_DATA = 2;

HWND      MainDialogWindow = 0;
HWND      MessageListbox   = 0;

TCHAR        MyClassName[] = VISITRAK_EVENT_MANAGER;
STRING_CLASS MyWindowTitle;

TCHAR     ServerName[] = VISITRAK_EVENT_MANAGER;
HSZ       ServerNameHandle;

UINT      ViewDdeState  = MF_UNCHECKED;
UINT      ViewVisiState = MF_UNCHECKED;
BOOLEAN   DisplayingMessages = FALSE;

static  DWORD  DdeId;
static  int    CodePageId    = CP_WINANSI;
static  UINT   DdeClipFormat = CF_TEXT;

/*
-----------------------------------------------------------------
The data list has pairs of topics and items for which I have data
----------------------------------------------------------------- */
struct DATA_ENTRY
    {
    TCHAR   topic[MAX_DDE_TOPIC_LEN+1];
    TCHAR * current_value;
    short   item;
    };

GENERIC_LIST_CLASS DataList;

TCHAR VisiTrakIniFile[] = TEXT( "visitrak.ini" );
TCHAR ConfigSection[]   = TEXT( "Config" );
TCHAR DataServerKey[]   = TEXT( "DataServer" );
const static TCHAR NoNetworkKey[]    = TEXT( "NoNetwork" );

const static TCHAR XtypConnect[]        = TEXT( "XTYP_CONNECT" );
const static TCHAR XtypDisconnect[]     = TEXT( "XTYP_DISCONNECT" );
const static TCHAR XtypConnectConfirm[] = TEXT( "XTYP_CONNECT_CONFIRM" );
const static TCHAR XtypAdvstart[]       = TEXT( "XTYP_ADVSTART" );
const static TCHAR XtypAdvstop[]        = TEXT( "XTYP_ADVSTOP" );
const static TCHAR XtypRequest[]        = TEXT( "XTYP_REQUEST" );
const static TCHAR XtypAdvReq[]         = TEXT( "XTYP_ADVREQ" );
const static TCHAR XtypRegister[]       = TEXT( "XTYP_REGISTER" );
const static TCHAR XtypPoke[]           = TEXT( "XTYP_POKE" );

const static TCHAR BackSlashChar = TEXT( '\\' );
const static TCHAR ColonChar     = TEXT( ':' );
const static TCHAR CommaChar     = TEXT( ',' );
const static TCHAR CrChar        = TEXT( '\015' );
const static TCHAR LfChar        = TEXT( '\012' );
const static TCHAR NullChar      = TEXT( '\0' );
const static TCHAR CommaString[] = TEXT( "," );
const static TCHAR SpaceString[] = TEXT( " " );

void    send_mailslot_event( TCHAR * topic, TCHAR * item, TCHAR * data );
void    shutdown_mailslots();
BOOLEAN start_mailslots();
void    to_mailslot( TCHAR * dest, TCHAR * sorc );

/***********************************************************************
*                            RESOURCE_STRING                           *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                             SHOW_MESSAGE                             *
***********************************************************************/
void show_message( const TCHAR * sorc )
{
if ( MessageListbox )
    SendMessage( MessageListbox, LB_ADDSTRING, 0, (LPARAM) sorc );
}

/***********************************************************************
*                        TOGGLE_MENU_VIEW_STATE                        *
***********************************************************************/
void toggle_menu_view_state( UINT & state, UINT menu_choice_id )
{
HMENU menu_handle;
if ( state == MF_CHECKED )
    state = MF_UNCHECKED;
else
    state = MF_CHECKED;

menu_handle = GetMenu( MainWindow );
CheckMenuItem( menu_handle, menu_choice_id, state );
}

/***********************************************************************
*                              ITEM_INDEX                              *
***********************************************************************/
static int item_index( HSZ item_handle )
{
int i;

for ( i=0; i<NofItems; i++ )
    {
    if ( DdeCmpStringHandles(item_handle, ItemList[i].handle ) == 0 )
        return i;
    }
return NO_INDEX;
}

/***********************************************************************
*                            APPEND_INTEGER                            *
***********************************************************************/
static void append_integer( TCHAR * dest, int sorc )
{
TCHAR buf[MAX_INTEGER_LEN+1];

lstrcat( dest, TEXT(" $") );
int32toasc( buf, sorc, HEX_RADIX );
lstrcat( dest, buf );
}

/***********************************************************************
*                          APPEND_STRING_HANDLE                        *
***********************************************************************/
static void append_string_handle( TCHAR * dest, HSZ h )
{
const int SLEN = MAX_DDE_ITEM_LEN + MAX_DDE_TOPIC_LEN;

TCHAR buf[SLEN+1];

lstrcat( dest, SpaceString );
if ( DdeQueryString(DdeId, h, buf, SLEN, CodePageId) )
    {
    lstrcat( dest, buf );
    lstrcat( dest, SpaceString );
    }
}

/***********************************************************************
*                           SHOW_LAST_ERROR                            *
***********************************************************************/
void show_last_error( void )
{
UINT    status;
TCHAR   buf[81];
TCHAR * cp;

status = DdeGetLastError( DdeId );
cp = copystring( buf, TEXT("LastError = $") );
int32toasc( cp, status, HEX_RADIX );

show_message( buf );
}

/***********************************************************************
*                              FIND_DATA                               *
***********************************************************************/
DATA_ENTRY * find_data( TCHAR * topic, short item )
{
DATA_ENTRY * d;

DataList.rewind();
while ( TRUE )
    {
    d = (DATA_ENTRY *) DataList.next();
    if ( !d )
        break;
    if ( compare(topic, d->topic, MAX_DDE_TOPIC_LEN) == 0 )
        if ( item == d->item )
            break;
    }
return d;
}

/***********************************************************************
*                              FIND_DATA                               *
***********************************************************************/
DATA_ENTRY * find_data( HSZ topic_handle, HSZ item_handle )
{
TCHAR topic[MAX_DDE_TOPIC_LEN+1];

DdeQueryString( DdeId, topic_handle, topic, MAX_DDE_TOPIC_LEN, CodePageId );

return find_data( topic, item_index(item_handle) );
}

/***********************************************************************
*                           SHOW_VISI_EVENT                            *
***********************************************************************/
static void show_visi_event( DATA_ENTRY * d )
{
const int LBLINE_LEN = 120;
const TCHAR ELIPSIS[] = TEXT( "..." );
const int ELIPSIS_LEN = sizeof( ELIPSIS );

TCHAR buf[MAX_DDE_TOPIC_LEN + MAX_DDE_ITEM_LEN + LBLINE_LEN + ELIPSIS_LEN + 4];
TCHAR * cp;

if ( !d )
    return;

if ( ViewVisiState == MF_UNCHECKED )
    return;

lstrcpy( buf, ItemList[d->item].name );
lstrcat( buf, SpaceString );
lstrcat( buf, d->topic );
lstrcat( buf, SpaceString );
cp = buf + lstrlen( buf );
lstrcpyn( cp, d->current_value, LBLINE_LEN+1 );
if ( LBLINE_LEN < lstrlen(d->current_value) )
    lstrcat( buf, ELIPSIS );

show_message( buf );
}

/***********************************************************************
*                           SET_TOPIC_VALUE                            *
***********************************************************************/
void set_topic_value( TCHAR * topic, short item, TCHAR * new_value )
{
DATA_ENTRY * d;
HSZ          h;
TCHAR      * old_value;
TCHAR      * cp;
int          slen;

slen = lstrlen( new_value );
if ( slen <= 0 )
    return;

cp = maketext( slen );
if ( !cp )
    {
    show_message( TEXT("No Memory for new event string") );
    return;
    }

lstrcpy( cp, new_value );
d = find_data( topic, item );

if ( d )
    {
    old_value = d->current_value;
    d->current_value = cp;
    if ( old_value )
        delete[] old_value;
    }
else
    {
    d = new DATA_ENTRY;
    if ( d )
        {
        lstrcpy( d->topic, topic );
        d->item          = item;
        d->current_value = cp;
        if ( !DataList.append(d) )
            {
            delete d;
            d = 0;
            }
        }
    else
        {
        show_message( TEXT("No Memory for new DATA_ENTRY") );
        }
    }

if ( d )
    {
    show_visi_event( d );

    h = DdeCreateStringHandle( DdeId, d->topic, CodePageId );
    DdePostAdvise( DdeId, h, ItemList[d->item].handle );
    DdeFreeStringHandle( DdeId, h );
    }
else
    {
    delete[] cp;
    }
}

/***********************************************************************
*                              SEND_DATA                               *
***********************************************************************/
static HDDEDATA send_data( HCONV conversation_handle, HSZ topic_handle, HSZ item_handle, UINT clipformat )
{
TCHAR * value;
int i;
DATA_ENTRY * d;
DWORD        slen;
if ( clipformat != DdeClipFormat )
    {
    return 0;
    }

i = item_index( item_handle );
if ( i == NO_INDEX )
    {
    show_message( TEXT("No Item") );
    return 0;
    }

value = ItemList[i].default_value;
d = find_data( topic_handle, item_handle );
if ( d )
    value = d->current_value;

slen = lstrlen(value) * sizeof(TCHAR);

return DdeCreateDataHandle( DdeId, (LPBYTE) value, slen, 0, item_handle, clipformat, 0 );
}

/***********************************************************************
*                            GET_POKE_DATA                             *
*               Poked data has the topic at the beginning.             *
*                           "M01,3\015\010"                            *
***********************************************************************/
void get_poke_data( HSZ item_handle, HDDEDATA hdata )
{
const short slen = MAX_DDE_TOPIC_LEN+MAX_DDE_DATA_LEN+2;
TCHAR buf[slen];
TCHAR * cp;
int     i;

DdeGetData( hdata, (LPBYTE) buf, slen*sizeof(TCHAR), 0 );
if ( compare( DDE_EXIT_TOPIC,  buf, 4 ) == 0 )
    {
    PostMessage( MainWindow, WM_CLOSE, 0, 0L );
    return;
    }

cp = findchar( TEXT(','), buf, slen );

if ( cp )
    {
    *cp = NullChar;
    cp++;
    i = item_index( item_handle );

    set_topic_value( buf, i, cp );

    if ( HaveMailslots && i != PW_LEVEL_INDEX )
        send_mailslot_event( buf, ItemList[i].name, cp );
    }
}

/***********************************************************************
*                             SAVE_NEWMAIL                             *
***********************************************************************/
void save_newmail( TCHAR * s )
{
TCHAR * tp;
TCHAR * ip;
TCHAR * dp;
int     i;

if ( !s )
    return;

tp = s;
ip = findchar( CommaChar, tp, lstrlen(tp) );
if ( ip )
    {
    *(ip) = NullChar;
    ip++;
    dp = findchar( CommaChar, ip, lstrlen(ip) );
    if ( dp )
        {
        *(dp) = NullChar;
        dp++;
        for ( i=0; i<NofItems; i++ )
            {
            if (lstrcmp( ItemList[i].name, ip) == 0 )
                {
                /*
                -----------------------------------------------------------------------
                Both NEW_COMPUTER_INDEX and NEW_DS_INDEX are messages for eventman only
                ----------------------------------------------------------------------- */
                if ( i == COMPUTER_SETUP_INDEX )
                    {
                    to_mailslot( ReloadRemoteComputers, ReloadRemoteComputers );
                    break;
                    }
                else
                    {
                    set_topic_value( tp, i, dp );
                    }
                }
            }
        }
    }

/*
-----------------------------------------------
I am responsible for deleting the passed string
----------------------------------------------- */
delete[] s;
}

/***********************************************************************
*                           DDE_CALLBACK                               *
***********************************************************************/
HDDEDATA CALLBACK dde_callback( UINT type, UINT clip_format, HCONV conversation_handle,
        HSZ hs1, HSZ hs2, HDDEDATA hdata, DWORD d1, DWORD d2 )
{
TCHAR    buf[81];
HDDEDATA retval;

/*
------------------------------------------
Display a message showing the message type
------------------------------------------ */
if ( ViewDdeState == MF_CHECKED )
    {
    switch ( type )
        {
        case XTYP_CONNECT:
            lstrcpy( buf, XtypConnect );
            break;
        case XTYP_DISCONNECT:
            lstrcpy( buf, XtypDisconnect );
            break;

        case XTYP_CONNECT_CONFIRM:
            lstrcpy( buf, XtypConnectConfirm );
            break;

        case XTYP_ADVSTART:
            lstrcpy( buf, XtypAdvstart );
            break;

        case XTYP_ADVSTOP:
            lstrcpy( buf, XtypAdvstop );
            break;

        case XTYP_REQUEST:
            lstrcpy( buf, XtypRequest );
            break;

        case XTYP_REGISTER:
            lstrcpy( buf, XtypRegister );
            break;

        case XTYP_ADVREQ:
            lstrcpy( buf, XtypAdvReq );
            if ( LOWORD(d1) == CADV_LATEACK )
                lstrcat( buf, TEXT("LATE ACK") );
            break;

        case XTYP_POKE:
            lstrcpy( buf, XtypPoke );
            break;

        default:
            show_last_error();
            lstrcpy( buf, TEXT("Unknown =") );
            append_integer( buf, (int) type );
            append_integer( buf, (int) d1 );
            break;

        }

    append_integer( buf, (int) clip_format );
    append_string_handle( buf, hs1 );
    append_string_handle( buf, hs2 );
    show_message( buf );
    }

switch ( type )
    {
    case XTYP_CONNECT:
        retval = (HDDEDATA) TRUE;
        break;

    case XTYP_DISCONNECT:
        retval = (HDDEDATA) NULL;
        break;

    case XTYP_CONNECT_CONFIRM:
        retval = (HDDEDATA) NULL;
        break;

    case XTYP_ADVSTART:
        retval = (HDDEDATA) FALSE;
        if ( clip_format == DdeClipFormat )
            {
            if ( item_index(hs2) != NO_INDEX )
                retval = (HDDEDATA) TRUE;
            }
        break;

    case XTYP_ADVSTOP:
        retval = (HDDEDATA) NULL;
        break;

    case XTYP_ADVREQ:
        clip_format = DdeClipFormat;

    case XTYP_REQUEST:
        retval = (HDDEDATA) send_data( conversation_handle, hs1, hs2, clip_format );
        break;

    case XTYP_REGISTER:
        retval = (HDDEDATA) NULL;
        break;

    case XTYP_POKE:
        get_poke_data( hs2, hdata );
        retval = (HDDEDATA) DDE_FACK;
        break;

    default:
        retval = (HDDEDATA) NULL;
        break;
    }

return retval;
}

/***********************************************************************
*                           SIZE_MAIN_DIALOG                           *
***********************************************************************/
void size_main_dialog()
{
RECT  r;

GetClientRect( MainWindow, &r);

MoveWindow( MainDialogWindow, r.left, r.top, r.right-r.left, r.bottom-r.top, TRUE );
GetClientRect( MainDialogWindow, &r);
MoveWindow( GetDlgItem(MainDialogWindow,MESSAGE_LISTBOX_ID), r.left, r.top, r.right-r.left, r.bottom-r.top, TRUE );
}

/***********************************************************************
*                            MAIN_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK main_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        MessageListbox = GetDlgItem( hWnd, MESSAGE_LISTBOX_ID );
        break;
    }

return FALSE;
}

/***********************************************************************
*                              START_DDE                               *
***********************************************************************/
static void start_dde()
{
int   i;
UINT  status;
DWORD ddeflags;

//show_message( TEXT("Calling DdeInitialize...") );
ddeflags = APPCLASS_STANDARD | APPCMD_FILTERINITS;
DdeId    = 0;

status = DdeInitialize( &DdeId, (PFNCALLBACK) dde_callback, ddeflags, 0 );
if ( status != DMLERR_NO_ERROR )
    MessageBox( MainWindow, TEXT("Failed"), TEXT("DdeInitialize"), MB_OK );

ServerNameHandle = DdeCreateStringHandle( DdeId, ServerName, CodePageId );
if ( !ServerNameHandle )
    MessageBox( MainWindow, TEXT("Failed"), TEXT("DdeCreateStringHandle"), MB_OK );

//show_message( TEXT("Calling DdeNameService...") );
if ( DdeNameService(DdeId, ServerNameHandle, 0, DNS_REGISTER) == 0 )
    MessageBox( MainWindow, TEXT("Failed"), TEXT("DdeNameService"), MB_OK );

/*
-----------------------------------
Create handles for each of my items
----------------------------------- */
for ( i=0; i<NofItems; i++ )
    ItemList[i].handle = DdeCreateStringHandle( DdeId, ItemList[i].name, 0 );
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
BOOL CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_SIZE:
        size_main_dialog();
        return TRUE;

    case WM_CREATE:
        PostMessage( hWnd, WM_USER_INITIATE, 0, 0L );
        return 0;

    case WM_USER_INITIATE:
        start_dde();
        if ( !bool_from_ini(VisiTrakIniFile, ConfigSection, NoNetworkKey) )
            start_mailslots();
        return 0;

    case WM_NEWMAIL :
        if ( lParam )
            save_newmail( (TCHAR *) lParam );
        return 0;

    case WM_COMMAND:

        switch ( id )
            {
            case VIEW_DDE_CHOICE:
                toggle_menu_view_state( ViewDdeState, id );
                return 0;

            case VIEW_VISI_CHOICE:
                toggle_menu_view_state( ViewVisiState, id );
                if ( ViewVisiState == MF_CHECKED )
                    DisplayingMessages = TRUE;
                else
                    DisplayingMessages = FALSE;
                return 0;

            case CLEAR_EVENTS_CHOICE:
                SendDlgItemMessage( MainDialogWindow, MESSAGE_LISTBOX_ID, LB_RESETCONTENT, 0, 0L );
                return 0;

            case SHOW_STATUS_CHOICE:
                to_mailslot( ShowStatusRequest, ShowStatusRequest );
                return 0;

            case MINIMIZE_CHOICE:
                CloseWindow( MainWindow );
                return 0;

            case EXIT_CHOICE:
                PostMessage( MainWindow, WM_CLOSE, 0, 0L );
                return 0;
            }
        break;

    case WM_CLOSE:
        if ( HaveMailslots )
            {
            shutdown_mailslots();
            return 0;
            }
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
BOOL init()
{
WNDCLASS wc;
COMPUTER_CLASS c;

if ( sizeof(TCHAR) > 1 )
    {
    DdeClipFormat = CF_UNICODETEXT;
    CodePageId = CP_WINUNICODE;
    }

names_startup();
c.startup();

InitializeCriticalSection( &ComputersCriticalSection );

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon(MainInstance, MyClassName );
wc.lpszMenuName  = TEXT("MainMenu");
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyWindowTitle.text(),
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    279, 244,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

if ( !MainWindow )
    return FALSE;

MainDialogWindow  = CreateDialog(
    MainInstance,
    TEXT("MainDialog"),
    MainWindow,
    (DLGPROC) main_dialog_proc );

ShowWindow( MainWindow, SW_HIDE );
UpdateWindow( MainWindow );

return TRUE;
}

/***********************************************************************
*                             SHUTDOWN_DDE                             *
***********************************************************************/
void shutdown_dde( void )
{
int i;

if ( DdeNameService(DdeId, ServerNameHandle, 0, DNS_UNREGISTER) == 0 )
    MessageBox( MainWindow, TEXT("Failed"), TEXT("DdeNameService"), MB_OK );

/*
---------------------------------
Free handles for each of my items
--------------------------------- */
for ( i=0; i<NofItems; i++ )
    {
    DdeFreeStringHandle( DdeId, ItemList[i].handle );
    ItemList[i].handle = 0;
    }

DdeUninitialize( DdeId );
DdeId = 0;
}

/***********************************************************************
*                               CLEANUP                                *
***********************************************************************/
void cleanup()
{
DATA_ENTRY * d;
TCHAR      * cp;

shutdown_dde();

DataList.rewind();
while ( TRUE )
    {
    d = (DATA_ENTRY *) DataList.next();
    if ( !d )
        break;
    cp = d->current_value;
    delete d;
    if ( cp )
        delete[] cp;
    }

DataList.remove_all();

DeleteCriticalSection( &ComputersCriticalSection );
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG  msg;
BOOL status;

MainInstance = this_instance;

MyWindowTitle = resource_string( MAIN_WINDOW_TITLE_STRING );

if ( is_previous_instance(MyClassName, 0) )
    return 0;

InitCommonControls();

if ( !init() )
    return FALSE;

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( MainDialogWindow )
        status = IsDialogMessage( MainDialogWindow, &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
        }
    }

cleanup();
return msg.wParam;
}
