#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\names.h"
#include "..\include\subs.h"

#include "resource.h"
#include "extern.h"
#include "fill.h"
#include "save.h"

static TCHAR backup_computer_name[] = BACKUP_COMPUTER;
static MACHINE_NAME_LIST_CLASS BupMachList;

void load_zip_file( void );

/***********************************************************************
*                             BACKUP_BITS                              *
*                                                                      *
*    If the return value is DO_RESTORE, no boxes were checked.         *
*                                                                      *
***********************************************************************/
static BITSETYPE backup_bits( HWND w )
{
int i;
int n;
BITSETYPE mask;

static int listbox_id[] = { TR_SETUPS_CHECKBOX,  TR_PROFILE_CHECKBOX,
                            TR_PARAM_CHECKBOX,   TR_SCRAP_DOWN_CHECKBOX,
                            TR_ALARM_CHECKBOX,   TR_SURETRAK_CHECKBOX,
                            TR_ANALOGS_CHECKBOX, TR_BOARDS_CHECKBOX };

static BITSETYPE bitmask[] = { SETUP_DATA,         PROFILE_DATA,
                               PARAMETER_DATA,     DOWNTIME_DATA,
                               ALARM_DATA,         SURETRAK_GLOBAL_DATA,
                               ANALOG_SENSOR_DATA, FASTRAK_BOARDS_DATA };


n = sizeof(listbox_id) / sizeof(int);

mask = DO_RESTORE;

for ( i=0; i<n; i++ )
    if ( is_checked(w, listbox_id[i]) )
        mask |= bitmask[i];

return mask;
}

/***********************************************************************
*                        CHECK_FOR_DEST_MACHINE                        *
***********************************************************************/
void check_for_dest_machine( HWND w )
{
LISTBOX_CLASS lb;

lb.init( w, TR_MACHINE_LISTBOX );
if ( lb.select_count() == 1 )
    {
    lb.init( w, TR_DEST_LISTBOX );
    if ( lb.selected_index() != NO_INDEX )
        {
        if ( MachList.find(lb.selected_text()) )
            {
            DestMachine.set( MachList.computer_name(), MachList.name() );
            return;
            }
        }
    }

DestMachine.empty();
}

/***********************************************************************
*                         SORC_MACHINE_CHANGE                          *
***********************************************************************/
void sorc_machine_change( HWND w )
{
LISTBOX_CLASS sorc;
LISTBOX_CLASS dest;
BOOL is_enabled;

sorc.init( w, TR_MACHINE_LISTBOX );
dest.init( w, TR_DEST_LISTBOX    );

if ( sorc.select_count() == 1 )
    {
    is_enabled = TRUE;
    }
else
    {
    dest.unselect_all();
    is_enabled = FALSE;
    }

EnableWindow( dest.handle(), is_enabled );
if ( is_enabled )
    dest.setcursel( sorc.selected_text() );
}

/***********************************************************************
*                            INIT_LISTBOXES                            *
***********************************************************************/
static void init_listboxes( HWND w )
{
COMPUTER_CLASS c;
TCHAR          destpath[MAX_PATH+1];

lstrcpy( destpath, BackupDir );
append_directory_to_path( destpath, backup_computer_name );
c.add( backup_computer_name, destpath );
BupMachList.empty();
fill_startup( w, TR_MACHINE_LISTBOX, TR_PART_LISTBOX );

/*
-------------------------------------------------------------
Fill the destination listbox with a list of existing machines
------------------------------------------------------------- */
fill_machine_listbox( w, TR_DEST_LISTBOX, MachList );

if ( directory_exists(BackupDir) )
    {
    if ( file_exists(machset_dbname(backup_computer_name)) )
        {
        BupMachList.add_computer( backup_computer_name );
        fill_machines( BupMachList );
        fill_parts( BupMachList, DO_RESTORE );
        }
    }

sorc_machine_change( w );
}

/***********************************************************************
*                             RESTORE_PROC                             *
***********************************************************************/
BOOL CALLBACK restore_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int cmd;
int id;
BITSETYPE b;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        DestMachine.empty();
        return TRUE;

    case WM_DBINIT:
        BOOL is_enabled;
        BupMachList.empty();
        init_listboxes( w );
        if ( HaveSureTrakControl )
            is_enabled = TRUE;
        else
            is_enabled = FALSE;
        EnableWindow( GetDlgItem(w, TR_SURETRAK_CHECKBOX), is_enabled );
        return TRUE;

    case WM_DB_SAVE_DONE:
        /*
        ----------------------------------------------------------------
        Refill the destination listbox in case a new machine was created
        ---------------------------------------------------------------- */
        fill_machine_listbox( w, TR_DEST_LISTBOX, MachList );
        sorc_machine_change( w );
        return TRUE;

    case WM_DBCLOSE:
        SendDlgItemMessage( w, TR_MACHINE_LISTBOX, LB_RESETCONTENT, TRUE, 0L );
        SendDlgItemMessage( w, TR_PART_LISTBOX,    LB_RESETCONTENT, TRUE, 0L );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case TR_MACHINE_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    sorc_machine_change( w );
                    fill_parts( BupMachList, DO_RESTORE );
                    }
                return TRUE;

            case TR_PART_LISTBOX:
                return TRUE;

            case TR_ALL_MACHINES_BUTTON:
                lb_select_all( w, TR_MACHINE_LISTBOX );
                sorc_machine_change( w );
                fill_parts( BupMachList, DO_RESTORE );
                return TRUE;

            case TR_LOAD_ZIP_FILE_BUTTON:
                load_zip_file();
                PostMessage( w, WM_DBINIT, 0, 0 );
                return TRUE;

            case TR_RESTORE_ALL_BUTTON:
                CheckDlgButton( w, TR_SETUPS_CHECKBOX,     BST_CHECKED );
                CheckDlgButton( w, TR_PROFILE_CHECKBOX,    BST_CHECKED );
                CheckDlgButton( w, TR_PARAM_CHECKBOX,      BST_CHECKED );
                CheckDlgButton( w, TR_SCRAP_DOWN_CHECKBOX, BST_CHECKED );
                CheckDlgButton( w, TR_ALARM_CHECKBOX,      BST_CHECKED );
                CheckDlgButton( w, TR_SURETRAK_CHECKBOX,   BST_CHECKED );
                CheckDlgButton( w, TR_ANALOGS_CHECKBOX,    BST_CHECKED );
                CheckDlgButton( w, TR_BOARDS_CHECKBOX,     BST_CHECKED );
                return TRUE;

            case IDOK:
                b = backup_bits( w );
                if ( b == DO_RESTORE )
                    {
                    resource_message_box( MainInstance, NOTHING_TO_DO_STRING, NOTE_STRING );
                    SetFocus( GetDlgItem(w, TR_SETUPS_CHECKBOX) );
                    }
                else
                    {
                    check_for_dest_machine( w );
                    save( b, w, TR_MACHINE_LISTBOX, TR_PART_LISTBOX );
                    }
                return TRUE;
            }
        break;
    }

return FALSE;
}
