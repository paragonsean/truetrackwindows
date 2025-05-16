#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\iniclass.h"
#include "..\include\names.h"
#include "..\include\subs.h"

#include "resource.h"

#include "backupcl.h"
#include "extern.h"

static HWND  MyWindow   = 0;
static HWND  Machine_Based_Xbox = 0;
static LISTBOX_CLASS MachineListbox;

static DS_BACKUP_CLASS MyList;

static BOOLEAN Net_Shot_Parm_Settings_Have_Changed = FALSE;
static BOOLEAN Zip_File_Paths_Have_Changed         = FALSE;

static STRING_CLASS CurrentMachine;
static STRING_CLASS Default_machine_zip_path;
static STRING_CLASS Default_part_zip_path;
static TCHAR DefaultNetRecordFormat[] = TEXT("{m},{p},{dd}-{mm}-{yyyy} {hh}:{mi}:{ss},{n},{6}" );
void get_zip_file_path( STRING_CLASS & dest, const TCHAR * section, int path_type );

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static void save_changes()
{
INI_CLASS ini;
STRING_CLASS s;

s = exe_directory();
s.cat_path( IniFileName );
ini.set_file( s.text() );

if ( Net_Shot_Parm_Settings_Have_Changed )
    {
    ini.set_section( ShotparmSection );

    s.get_text( MyWindow, NET_SHOT_PARAM_FORMAT_EBOX );
    ini.put_string( RecordFormatKey, s.text() );

    s.get_text( MyWindow, NET_SHOT_PARAM_FILE_EBOX );
    ini.put_string( FileNameKey, s.text() );

    Net_Shot_Parm_Settings_Have_Changed = FALSE;
    }

ini.set_section( DefaultSection );

Using_Machine_Based_Zip_Paths = is_checked( Machine_Based_Xbox );
ini.put_boolean( MachineBasedZipPathsKey, Using_Machine_Based_Zip_Paths );

ini.put_string( BackupPartPathString,    Default_part_zip_path.text()    );
ini.put_string( BackupMachinePathString, Default_machine_zip_path.text() );

/*
------------------------------------
Copy any changes to the global names
------------------------------------ */
DefaultMachineZipPath = Default_machine_zip_path;
DefaultPartZipPath    = Default_part_zip_path;

MyList.rewind();
while ( MyList.next() )
    MyList.current_entry->write_zip_paths();

/*
---------------------------------------------
Tell the mailslots thread to reload the paths
--------------------------------------------- */
NeedToReloadZipPaths = TRUE;
}

/***********************************************************************
*                        COPY_ZIP_PATH_CHANGES                         *
***********************************************************************/
static void copy_zip_path_changes()
{

if ( !Zip_File_Paths_Have_Changed )
    return;

if ( CurrentMachine == NO_MACHINE )
    {
    Default_part_zip_path.get_text(    MyWindow, PART_PATH_EBOX );
    Default_machine_zip_path.get_text( MyWindow, MACHINE_PATH_EBOX );
    }
else
    {
    if ( MyList.find(CurrentMachine.text()) )
        {
        MyList.current_entry->part_zip_file_path.get_text( MyWindow, PART_PATH_EBOX );
        MyList.current_entry->machine_zip_file_path.get_text( MyWindow, MACHINE_PATH_EBOX );
        }
    }

Zip_File_Paths_Have_Changed = FALSE;
}

/***********************************************************************
*                     LOAD_MACHINE_BASED_ZIP_PATHS                     *
***********************************************************************/
static void load_machine_based_zip_paths()
{
copy_zip_path_changes();

CurrentMachine = MachineListbox.selected_text();
if ( CurrentMachine.isempty() )
    {
    CurrentMachine = NO_MACHINE;
    }
else if ( MyList.find(CurrentMachine.text()) )
    {
    MyList.current_entry->part_zip_file_path.set_text(    MyWindow, PART_PATH_EBOX    );
    MyList.current_entry->machine_zip_file_path.set_text( MyWindow, MACHINE_PATH_EBOX );
    }
}

/***********************************************************************
*                   KILL_GLOBAL_FILE_SETTINGS_DIALOG                   *
***********************************************************************/
static void kill_global_file_settings_dialog()
{
HWND w;

if ( MyWindow )
    {
    w = MyWindow;
    MyWindow = 0;
    EndDialog( w, 0 );
    }

MyList.empty();

Machine_Based_Xbox = 0;
MachineListbox.init(0);

CurrentMachine.empty();
Default_machine_zip_path.empty();
Default_part_zip_path.empty();
}

/***********************************************************************
*                            INIT                                      *
***********************************************************************/
static void init()
{
BOOLEAN b;
INI_CLASS ini;
STRING_CLASS s;

/*
-------------------
Make Global Handles
------------------- */
MyList = BList;
Machine_Based_Xbox = GetDlgItem( MyWindow, MACHINE_BASED_ZIP_XBOX );
MachineListbox.init( MyWindow, MACHINE_LISTBOX );

MyList.rewind();
while ( MyList.next() )
    MachineListbox.add( MyList.current_entry->name.machine );

/*
-------------------------------
Network Shot Parameter Settings
------------------------------- */
s = exe_directory();
s.cat_path( IniFileName );
ini.set_file( s.text() );
ini.set_section( ShotparmSection );
s = ini.get_string( RecordFormatKey );
if ( unknown(s.text()) )
    s = DefaultNetRecordFormat;
s.set_text( MyWindow, NET_SHOT_PARAM_FORMAT_EBOX );
s = ini.get_string( FileNameKey );
s.set_text( MyWindow, NET_SHOT_PARAM_FILE_EBOX );

Net_Shot_Parm_Settings_Have_Changed = FALSE;
Zip_File_Paths_Have_Changed         = FALSE;

if ( Using_Machine_Based_Zip_Paths )
    b = TRUE;
else
    b = FALSE;

set_checkbox( Machine_Based_Xbox, b );

get_zip_file_path( Default_machine_zip_path, DefaultSection, MACHINE_PATH );
get_zip_file_path( Default_part_zip_path,    DefaultSection, PART_PATH    );
}

/***********************************************************************
*                    PROCESS_MACHINE_BASED_ZIP_XBOX                    *
***********************************************************************/
static void process_machine_based_zip_xbox()
{
BOOLEAN b;
INI_CLASS ini;

copy_zip_path_changes();

b = is_checked( MyWindow, MACHINE_BASED_ZIP_XBOX );
if ( !b )
    MachineListbox.unselect_all();

EnableWindow( MachineListbox.handle(), (BOOL) b );

if ( !b )
    {
    CurrentMachine = NO_MACHINE;
    Default_machine_zip_path.set_text( MyWindow, MACHINE_PATH_EBOX );
    Default_part_zip_path.set_text(    MyWindow, PART_PATH_EBOX    );
    }
}

/***********************************************************************
*                       GLOBAL_FILE_SETTINGS_DIALOG                    *
***********************************************************************/
BOOL CALLBACK global_file_settings_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int id;
static int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        MyWindow = hWnd;
        init();
        SetFocus( GetDlgItem(hWnd, NET_SHOT_PARAM_FILE_EBOX) );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_DBINIT :
        process_machine_based_zip_xbox();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case MACHINE_BASED_ZIP_XBOX:
                process_machine_based_zip_xbox();
                return TRUE;

            case NET_SHOT_PARAM_FILE_EBOX:
            case NET_SHOT_PARAM_FORMAT_EBOX:
                if ( cmd == EN_CHANGE )
                    Net_Shot_Parm_Settings_Have_Changed = TRUE;
                return TRUE;

            case PART_PATH_EBOX:
            case MACHINE_PATH_EBOX:
                if ( cmd == EN_CHANGE )
                    Zip_File_Paths_Have_Changed = TRUE;
                return TRUE;

            case MACHINE_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    load_machine_based_zip_paths();
                return TRUE;

            case IDOK:
                copy_zip_path_changes();
                save_changes();
                kill_global_file_settings_dialog();
                return TRUE;

            case IDCANCEL:
                kill_global_file_settings_dialog();
                return TRUE;
            }
        break;
    }

return FALSE;
}
