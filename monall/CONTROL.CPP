#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\ft.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

#include "extern.h"
#include "boards.h"
#include "resource.h"

#define CANCEL_BUTTON             2

#define MONITORING_TEXT         102
#define PARTS_TEXT              103
#define SET_COLLECT_MODE_BUTTON 105
#define SET_MONITOR_MODE_BUTTON 123
#define SEND_DATA_BUTTON        126
#define READ_DATA_BUTTON        124
#define COMMAND_COMBO_BOX       125
#define HEX_DATA_EDIT_BOX       103
#define BOARD_ONE_ADDRESS_BOX   140
#define BOARD_ONE_RADIO_BUTTON  130

#define MAX_FT_COMMAND_LEN       30

BOARD_CLASS B;

static const TCHAR EmptyString[] = TEXT( "" );
static const TCHAR CommandsIniFile[] = TEXT( "commands.ini" );

TCHAR * resource_string( UINT resource_id );
void resource_message_box( UINT msg_id, UINT title_id );

/***********************************************************************
*                            SET_BOARD_ADDRESS                         *
***********************************************************************/
static BOOLEAN set_board_address( void )
{
if ( B.address() == NO_BOARD_ADDRESS )
    {
    MessageBox( ManualControlWindow, TEXT("No Address for this Board"), TEXT("Board Error"), MB_OK );
    return FALSE;
    }

B.set_port_addresses();
return TRUE;
}

/***********************************************************************
*                           INIT_BOARD_CONTROLS                        *
***********************************************************************/
void init_board_controls( void )
{

short i;
BOOLEAN firstbutton;
TCHAR   s[MAX_HEX_LEN+1];

firstbutton = TRUE;

for ( i=0; i<MAX_BOARDS; i++ )
    {
    if ( B.find(i+1) )
        {
        if ( firstbutton )
            {
            firstbutton = FALSE;
            SendDlgItemMessage( ManualControlWindow, BOARD_ONE_RADIO_BUTTON+i, BM_CLICK, 0, 0L );
            }

        int32toasc( s, (int32) B.address(), HEX_RADIX );
        SetDlgItemText( ManualControlWindow, BOARD_ONE_ADDRESS_BOX+i, s );
        }
    else
        {
        SetDlgItemText( ManualControlWindow, BOARD_ONE_ADDRESS_BOX+i, EmptyString );
        }
    }
}

/***********************************************************************
*                        SEND_COMMAND_TO_BOARD                         *
***********************************************************************/
static void send_command_to_board( void )
{

LRESULT x;
TCHAR   fname[MAX_PATH+1];
TCHAR   command[MAX_FT_COMMAND_LEN+1];
TCHAR   full_command[MAX_FT_COMMAND_LEN+1];
BOARD_DATA hex_command;

WaitForSingleObject( FasTrakMutex, INFINITE );

if ( set_board_address() )
    {
    x = SendDlgItemMessage( ManualControlWindow, COMMAND_COMBO_BOX, CB_GETCURSEL, 0, 0L);
    if ( x != CB_ERR )
        {

        SendDlgItemMessage( ManualControlWindow, COMMAND_COMBO_BOX, CB_GETLBTEXT, UINT(x), (LPARAM)((LPSTR) command) );

        get_exe_directory( fname );
        lstrcat( fname, CommandsIniFile );

        GetPrivateProfileString( TEXT("Commands"), command, TEXT("NO_COMMAND"), full_command, MAX_FT_COMMAND_LEN, fname );
        if ( !IsCharAlpha(*full_command) )
            {
            hex_command = BOARD_DATA( extlong( full_command, 4, HEX_RADIX) );
            ft_send_cmd_wo_check( hex_command );
            }
        }
    }

ReleaseMutex( FasTrakMutex );
}

/***********************************************************************
*                          SEND_DATA_TO_BOARD                          *
***********************************************************************/
static void send_data_to_board( void )
{
BOARD_DATA x;
TCHAR s[11];

WaitForSingleObject( FasTrakMutex, INFINITE );

if ( set_board_address() )
    {
    GetDlgItemText( ManualControlWindow, HEX_DATA_EDIT_BOX, s, 10 );
    *(s+10) = TEXT('\0');

    x = BOARD_DATA( extlong(s, lstrlen(s), HEX_RADIX) );
    ft_send_data( x );
    }

ReleaseMutex( FasTrakMutex );
}

/***********************************************************************
*                             LOAD_COMMANDS                            *
***********************************************************************/
static void load_commands( void )
{

HANDLE  fh;
TCHAR   fname[MAX_PATH+1];
TCHAR   command[MAX_FT_COMMAND_LEN+1];
short   slen;
TCHAR * cp;

get_exe_directory( fname );
lstrcat( fname, CommandsIniFile );

fh = CreateFile( fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
if ( !fh )
    return;

while ( TRUE )
    {
    slen = readline( command, fh, MAX_FT_COMMAND_LEN );
    if ( slen )
        {
        cp = findchar( TEXT('='), command, slen );
        if ( cp )
            {
            *cp = TEXT('\0');
            SendDlgItemMessage(ManualControlWindow, COMMAND_COMBO_BOX, CB_ADDSTRING, 0,(LPARAM) ((LPSTR) command) );
            }
        }
    else
        break;
    }

SendDlgItemMessage(ManualControlWindow, COMMAND_COMBO_BOX, CB_SETCURSEL, 0, 0L );
CloseHandle( fh );
}

/***********************************************************************
*                         SHOW_MONITORING_STATE                        *
***********************************************************************/
void show_monitoring_state( void )
{
UINT id;

if ( B.monitoring() )
    id = MONITORING_STRING;
else
    id = NOT_MONITORING_STRING;

set_text( ManualControlWindow, MONITORING_TEXT, resource_string(id) );
}

/***********************************************************************
*                          TOGGLE_MONITORING                           *
***********************************************************************/
static void toggle_monitoring( void )
{
BOOLEAN new_monitoring_state;

if ( B.monitoring() )
    new_monitoring_state = FALSE;
else
    new_monitoring_state = TRUE;

B.set_monitoring( new_monitoring_state );
show_monitoring_state();
}

/***********************************************************************
*                       POSITION_MANUAL_CONTROL                        *
***********************************************************************/
void position_manual_control( void )
{

RECT  r;
long  x;
long  y;

GetWindowRect( MainWindow, &r );
x = r.right;
y = r.top;

GetWindowRect( ManualControlWindow, &r );
MoveWindow( ManualControlWindow, x, y, r.right-r.left, r.bottom-r.top, TRUE );
}

/***********************************************************************
*                         READ_DATA_FROM_BOARD                         *
***********************************************************************/
static void read_data_from_board( void )
{

BOARD_DATA x;
TCHAR s[5];

WaitForSingleObject( FasTrakMutex, INFINITE );

if ( set_board_address() )
    {
    x = ft_read_data();
    insalph( s, x, 4, TEXT('0'), HEX_RADIX );
    *(s+4) = TEXT('\0');

    SetDlgItemText( ManualControlWindow, HEX_DATA_EDIT_BOX, s );
    }

ReleaseMutex( FasTrakMutex );
}

/***********************************************************************
*                             NEW_BOARD_CHOSEN                         *
***********************************************************************/
void new_board_chosen( int id )
{
short new_board_number;
short old_board_index;

old_board_index = B.number() - 1;
new_board_number = id - BOARD_ONE_RADIO_BUTTON + 1;

if ( !B.find(new_board_number) )
    {
    if ( old_board_index >= 0 )
        SendDlgItemMessage( ManualControlWindow, BOARD_ONE_RADIO_BUTTON+old_board_index, BM_CLICK, 0, 0L );
    }

show_monitoring_state();
}

/***********************************************************************
*                          MANUALCONTROLPROC                           *
***********************************************************************/
BOOL CALLBACK ManualControlProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int        id;
BOARD_DATA status;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
		PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

	case WM_DBINIT:
        init_board_controls();
        load_commands();
        return TRUE;

    case WM_COMMAND:
        switch (id)
            {
            case CANCEL_BUTTON:
               ShowWindow( ManualControlWindow, SW_HIDE );
               return TRUE;

            case SEND_DATA_BUTTON:
                send_data_to_board();
                return TRUE;

            case READ_DATA_BUTTON:
                read_data_from_board();
                return TRUE;

            case SET_COLLECT_MODE_BUTTON:
                WaitForSingleObject( FasTrakMutex, INFINITE );
                set_board_address();
                status = ft_set_data_coll_mode();
                ReleaseMutex( FasTrakMutex );
                if ( status != FT_OK )
                    resource_message_box( ERROR_STRING, SET_DATA_COL_MODE_STRING );
                return TRUE;

            case SET_COMMAND_MODE_BUTTON:
                WaitForSingleObject( FasTrakMutex, INFINITE );
                set_board_address();
                status = ft_set_command_mode();
                ReleaseMutex( FasTrakMutex );
                if ( status != FT_OK )
                    resource_message_box( ERROR_STRING, SET_COMMAND_MODE_STRING );
                return TRUE;

            case SET_MONITOR_MODE_BUTTON:
                toggle_monitoring();
                return TRUE;

            case SEND_COMMAND_BUTTON:
                send_command_to_board();
                return TRUE;

            case BOARD_ONE_RADIO_BUTTON:
            case BOARD_ONE_RADIO_BUTTON+1:
            case BOARD_ONE_RADIO_BUTTON+2:
            case BOARD_ONE_RADIO_BUTTON+3:
            case BOARD_ONE_RADIO_BUTTON+4:
            case BOARD_ONE_RADIO_BUTTON+5:
            case BOARD_ONE_RADIO_BUTTON+6:
            case BOARD_ONE_RADIO_BUTTON+7:
                new_board_chosen(id);
                return TRUE;
            }
        break;
    }

return FALSE;
}

