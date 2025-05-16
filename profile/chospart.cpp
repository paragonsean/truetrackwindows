#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\names.h"
#include "..\include\subs.h"

#include "extern.h"

#define MACHINE_LISTBOX 112
#define PART_LISTBOX    121
#define OK_BUTTON         1

const TCHAR TabChar = TEXT( '\t' );

extern HWND MainWindow;
static HWND MyWindow;

void fill_machines( HWND w, UINT listbox_id );

/***********************************************************************
*                         CURRENT_MACHINE_NAME                         *
***********************************************************************/
static TCHAR * current_machine_name( void )
{
static TCHAR machine_name[MACHINE_NAME_LEN+1];
LRESULT x;

lstrcpy( machine_name, MachineName );

x = SendDlgItemMessage( MyWindow, MACHINE_LISTBOX, LB_GETCURSEL, 0, 0L );
if ( x != LB_ERR )
    SendDlgItemMessage( MyWindow, MACHINE_LISTBOX, LB_GETTEXT, (WPARAM) x, (LPARAM) ((LPSTR) machine_name) );

return machine_name;
}

/***********************************************************************
*                              FILL_PARTS                              *
***********************************************************************/
static void fill_parts( void )
{
if ( MachList.find(current_machine_name()) )
    fill_parts_and_counts_listbox( MyWindow, PART_LISTBOX, MachList.computer_name(), MachList.name(), TabChar );
}

/***********************************************************************
*                      COPY_SELECTIONS_TO_EXTERN                       *
***********************************************************************/
static void copy_selections_to_extern()
{
TCHAR * s;
LISTBOX_CLASS lb;

lb.init( MyWindow, PART_LISTBOX );

if ( MachList.find(current_machine_name()) )
    {
    s = lb.selected_text();
    if ( lstrlen(s) > 0 )
        {
        lstrcpyn( ComputerName, MachList.computer_name(), COMPUTER_NAME_LEN+1 );
        lstrcpyn( MachineName,  MachList.name(),          MACHINE_NAME_LEN+1 );
        lstrcpy( PartName, part_name_copy(s) );
        }
    }
}

/***********************************************************************
*                           CHOOSE_THIS_PART                           *
***********************************************************************/
static void choose_this_part( void )
{
copy_selections_to_extern();
SendMessage( MainWindow, WM_NEWPART, 0, 0L );
SendMessage( MyWindow,   WM_CLOSE,   0, 0L );
}

/***********************************************************************
*                            CHOSPART_PROC                             *
***********************************************************************/
BOOL CALLBACK chospart_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int tabs[2] = { 42, 75 };
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_DBINIT:
        fill_machines( MyWindow, MACHINE_LISTBOX );
        fill_parts();
        SetFocus( GetDlgItem(MyWindow, MACHINE_LISTBOX) );
        return TRUE;

    case WM_INITDIALOG:
        MyWindow = hWnd;
        if ( PRIMARYLANGID(GetUserDefaultLangID()) == LANG_CHINESE )
            tabs[0] = 60;
        SendDlgItemMessage( hWnd, PART_LISTBOX, LB_SETTABSTOPS, (WPARAM) 2, (LPARAM) tabs );
        return TRUE;

    case WM_CLOSE:
        ShowWindow( hWnd, SW_HIDE );
        return TRUE;

    case WM_SETFOCUS:
        SetFocus( GetDlgItem(MyWindow, MACHINE_LISTBOX) );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case PART_LISTBOX:
                if ( cmd == LBN_DBLCLK )
                    choose_this_part();
                return TRUE;

            case MACHINE_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    fill_parts();
                return TRUE;

            case OK_BUTTON:
                choose_this_part();
                return TRUE;
            }
    }

return FALSE;
}
