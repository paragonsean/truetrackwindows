#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\listbox.h"
#include "..\include\textlist.h"
#include "..\include\subs.h"
#include "..\include\v5help.h"

#include "rtmlist.h"
#include "resource.h"
#include "extern.h"

static TCHAR NullChar   = TEXT( '\0' );
static TCHAR TabChar    = TEXT( '\t' );
static TCHAR XStr[]     = TEXT( "X" );
static TCHAR SpaceStr[] = TEXT( " " );

static HWND MyWindow = 0;
static WNDPROC OldLbProc;

static const TCHAR RT_MACHINE_LIST_FILE[] = TEXT( "rtmach.lst" );

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
static void gethelp( UINT helptype, DWORD context )
{
TCHAR fname[MAX_PATH+1];

if ( get_exe_directory(fname) )
    {
    lstrcat( fname, TEXT("v5help.hlp") );
    WinHelp( MyWindow, fname, helptype, context );
    }

}

/***********************************************************************
*                                 LOAD                                 *
***********************************************************************/
BOOLEAN REALTIME_MACHINE_LIST_CLASS::load( void )
{

TCHAR  path[MAX_PATH+1];
TCHAR  buf[MACHINE_NAME_LEN+1];
DWORD  access_mode;
DWORD  share_mode;
HANDLE fh;

empty();

access_mode = GENERIC_READ;
share_mode  = FILE_SHARE_READ | FILE_SHARE_WRITE;

get_exe_directory( path );
lstrcat( path, RT_MACHINE_LIST_FILE );

fh = CreateFile( path, access_mode, share_mode, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
if ( fh == INVALID_HANDLE_VALUE )
    return FALSE;

while ( readline(buf, fh, MACHINE_NAME_LEN) > 0 )
    append( buf );

CloseHandle( fh );
return TRUE;
}

/***********************************************************************
*                                 SAVE                                 *
***********************************************************************/
BOOLEAN REALTIME_MACHINE_LIST_CLASS::save( void )
{
TCHAR  path[MAX_PATH+1];
DWORD  access_mode;
DWORD  share_mode;
HANDLE fh;

access_mode = GENERIC_WRITE;
share_mode  = 0;

get_exe_directory( path );
lstrcat( path, RT_MACHINE_LIST_FILE );

fh = CreateFile( path, access_mode, share_mode, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, 0 );

if ( fh == INVALID_HANDLE_VALUE )
    return FALSE;

rewind();
while ( next() )
    writeline( fh, text() );

CloseHandle( fh );
return TRUE;
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static void save_changes( void )
{
LISTBOX_CLASS lb;
TCHAR       * cp;
TCHAR         buf[MACHINE_NAME_LEN + 3];
int32         slen;
int32         i;
int32         n;

RealtimeMachineList.empty();

lb.init( MyWindow, RT_MACHINE_LISTBOX );

n = lb.count();

for ( i=0; i<n; i++ )
    {
    lstrcpy( buf, lb.item_text(i) );
    slen = lstrlen( buf );
    if ( slen )
        {
        cp = buf + slen;
        cp--;
        if ( *cp == *XStr )
            {
            cp--;
            *cp = NullChar;
            RealtimeMachineList.append( buf );
            }
        }
    }

RealtimeMachineList.save();
}

/***********************************************************************
*                         TOGGLE_LISTBOX_STRING                        *
***********************************************************************/
static void toggle_listbox_string( void )
{
LISTBOX_CLASS lb;
TCHAR       * cp;
TCHAR         buf[MACHINE_NAME_LEN + 3];
int32         slen;

lb.init( MyWindow, RT_MACHINE_LISTBOX );

lstrcpy( buf, lb.selected_text() );
slen = lstrlen( buf );
if ( slen )
    {
    cp = buf + slen;
    cp--;
    if ( *cp == *XStr )
        *cp = *SpaceStr;
    else
        *cp = *XStr;

    lb.replace( buf );
    }

lb.setcursel( NO_INDEX );
}

/***********************************************************************
*                        MACHINE_LISTBOX_STRING                        *
***********************************************************************/
static TCHAR * machine_listbox_string( TCHAR * machine, TCHAR default_char )
{
static TCHAR buf[MACHINE_NAME_LEN + 3];
TCHAR * cp;

cp = copy_w_char( buf, machine, TabChar );
if ( default_char == NullChar )
    {
    if ( RealtimeMachineList.find(machine)  )
        lstrcpy( cp, XStr );
    else
        lstrcpy( cp, SpaceStr );
    }
else
    {
    *cp = default_char;
    cp++;
    *cp = NullChar;
    }

return buf;
}

/***********************************************************************
*                             FILL_LISTBOX                             *
***********************************************************************/
static void fill_listbox( TCHAR default_char )
{
LISTBOX_CLASS lb;
BOOLEAN monitoring;
COMPUTER_CLASS c;

monitoring = FALSE;

static TCHAR MonitorClassName[] = MONITOR_CLASS_NAME;

if ( FindWindow(MonitorClassName, 0) )
    monitoring = TRUE;

lb.init( MyWindow, RT_MACHINE_LISTBOX );
lb.empty();

MachList.rewind();

while( MachList.next() )
    {
    if ( monitoring )
        if ( lstrcmp(MachList.computer_name(), c.whoami()) != 0 )
            continue;
    lb.add( machine_listbox_string(MachList.name(), default_char) );
    }

lb.setcursel( NO_INDEX );
}

/***********************************************************************
*                              SUB_PROC                                *
***********************************************************************/
LRESULT CALLBACK sub_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
INT i;
LISTBOX_CLASS lb;

if ( msg == WM_KEYDOWN )
    {
    if ( wParam == VK_UP || wParam == VK_DOWN )
        {
        lb.init( MyWindow, RT_MACHINE_LISTBOX );
        i = lb.current_index();
        if ( wParam == VK_UP && i > 0 )
            i--;
        else if ( wParam == VK_DOWN && i < (lb.count() - 1) )
            i++;
        else
            return 0;

        lb.set_current_index( i );
        SetFocus( w );
        return 0;
        }
    }

#ifdef __BORLANDC__
return CallWindowProc( (FARPROC) OldLbProc, w, msg, wParam, lParam );
#else
return CallWindowProc( OldLbProc, w, msg, wParam, lParam );
#endif
}

#ifdef __BORLANDC__
#pragma argsused
#endif
/***********************************************************************
*                       REALTIME_MACHINE_LIST_PROC                     *
***********************************************************************/
BOOL CALLBACK realtime_machine_list_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
int  cmd;
HWND w;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        MyWindow = hWnd;
        w = GetDlgItem( MyWindow, RT_MACHINE_LISTBOX );
        OldLbProc = (WNDPROC) SetWindowLong( w, GWL_WNDPROC, (LONG) sub_proc );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        fill_listbox( NullChar );
        SetFocus( GetDlgItem(MyWindow, RT_MACHINE_LISTBOX) );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, RTML_MENU_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case RT_MACHINE_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    toggle_listbox_string();
                return TRUE;

            case RT_DISPLAY_ALL_BUTTON:
                fill_listbox( *XStr );
                w = GetDlgItem( MyWindow, RT_MACHINE_LISTBOX );
                SetFocus( w );
                return TRUE;

            case RT_DISPLAY_NONE_BUTTON:
                fill_listbox( *SpaceStr );
                w = GetDlgItem( MyWindow, RT_MACHINE_LISTBOX );
                SetFocus( w );
                return TRUE;

            case IDOK:
                save_changes();

            case IDCANCEL:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case IDCANCEL:
    case WM_CLOSE:
        DialogIsActive = FALSE;
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}