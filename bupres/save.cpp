#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"

#include "extern.h"
#include "resource.h"

#define _MAIN_
#include "save.h"

HWND    ProgressWindow = 0;
BOOLEAN HaveProgressCancel = FALSE;
BOOLEAN UseBackupListFile  = FALSE;

static TCHAR EmptyString[] = TEXT( "" );

void backup_data( void );
void restore_data( void );
void delete_data(  void );
void renumber_data(  void );
void fill_machine_list( void );

/***********************************************************************
*                        READ_BACKUP_LIST_FILE                         *
*                                                                      *
* The backup list file came after the SaveList. In order to keep       *
* things simple I will make a SaveList even though the data is         *
* also in the backup list. The only change, then, will be to tell      *
* the backup_part program to use the shots in BackupList.              *
*                                                                      *
***********************************************************************/
static void read_backup_list_file()
{
DB_TABLE           t;
STRING_CLASS s;
STRING_CLASS pn;
int          i;
int          n;

s = backup_list_dbname();
if ( !file_exists(s.text()) )
    return;

t.open( s.text(), BACKUP_LIST_RECLEN, PFL );
n = t.nof_recs();
if ( n > 0 )
    {
    BackupList = new PROFILE_NAME_ENTRY[n];
    if ( BackupList )
        {
        i = 0;
        t.rewind();
        while ( t.get_next_record(NO_LOCK) )
            {
            t.get_alpha( BackupList[i].computer, BACKUP_LIST_CN_OFFSET, COMPUTER_NAME_LEN );
            t.get_alpha( BackupList[i].machine,  BACKUP_LIST_MN_OFFSET, MACHINE_NAME_LEN );
            t.get_alpha( BackupList[i].part,     BACKUP_LIST_PN_OFFSET, PART_NAME_LEN );
            t.get_alpha( BackupList[i].shot,     BACKUP_LIST_SN_OFFSET, SHOT_LEN );
            i++;
            }
        BackupListCount = i;
        t.close();
        }
    }

n = 0;
s = EmptyString;
for ( i=0; i<BackupListCount; i++ )
    {
    if ( s != BackupList[i].machine )
        {
        s = BackupList[i].machine;
        if ( !s.isempty() )
            n++;
        }
    }


s = EmptyString;
if ( n > 0 )
    {
    NtoSave = -1;
    SaveList = new SAVE_LIST_ENTRY[n];
    if ( SaveList )
        {
        for ( i=0; i<BackupListCount; i++ )
            {
            if ( s != BackupList[i].machine )
                {
                if ( !pn.isempty() )
                    pn = EmptyString;
                s = BackupList[i].machine;
                if ( !s.isempty() )
                    {
                    NtoSave++;
                    SaveList[NtoSave].name.set( BackupList[i].computer, BackupList[i].machine );
                    }
                }
            if ( NtoSave >= 0 )
                {
                if ( pn != BackupList[i].part )
                    {
                    pn = BackupList[i].part;
                    SaveList[NtoSave].part.append( pn.text() );
                    }
                }
            }
        }
    /*
    -----------------------------------
    Convert NtoSave from index to count
    ----------------------------------- */
    NtoSave++;
    }
}

/***********************************************************************
*                             THREAD_FUNC                              *
***********************************************************************/
DWORD thread_func( PVOID dummy  )
{
INT   * ip;
INT     i;
INT     nof_parts;
TCHAR   part_name[PART_NAME_LEN+1];
STRING_CLASS s;
DB_TABLE t;

SaveList = 0;

if ( BackupAllTextFiles )
    {
    backup_data();
    BackupAllTextFiles = FALSE;
    }
else if ( UseBackupListFile )
    {
    read_backup_list_file();
    if ( BackupList && SaveList )
        {
        set_text( ProgressWindow, resource_string(MainInstance, BACKING_UP_STRING) );
        backup_data();
        }
    UseBackupListFile  = FALSE;
    if ( BackupList )
        {
        delete[] BackupList;
        BackupList = 0;
        }
    BackupListCount = 0;
    }
else
    {
    NtoSave = MachineBox.get_select_list( &ip );
    if ( (NtoSave > 0) && (BackupType & WITH_PURGE) )
        {
        i = NtoSave;
        if ( i == 1 )
            i = PartBox.select_count();
        if ( i > 1 )
            {
            if ( resource_message_box(MainInstance, PURGE_N_PARTS_STRING, PURGE_SHOT_DATA_STRING, MB_YESNO | MB_APPLMODAL) == IDNO )
                NtoSave = 0;
            }
        }

    if ( NtoSave > 0 )
        {
        SaveList = new SAVE_LIST_ENTRY[NtoSave];
        if ( !SaveList )
            {
            resource_message_box( MainInstance, NO_MEMORY_STRING, SAVELIST_STRING );
            return 0;
            }

        i = 0;
        while ( i < NtoSave )
            {
            s = MachineBox.item_text( ip[i] );

            if ( BackupType & DO_RESTORE )
                {
                SaveList[i].name.set( BACKUP_COMPUTER, s.text() );
                i++;
                }
            else if ( MachList.find(s.text()) )
                {
                SaveList[i].name.set( MachList.computer_name(), s.text() );
                i++;
                }
            else
                {
                MessageBox( NULL, resource_string(MainInstance, NO_MACHINE_STRING), s.text(), MB_OK | MB_SYSTEMMODAL );
                NtoSave--;
                }
            }

        delete[] ip;
        ip = 0;
        }

    if ( NtoSave > 0 )
        {
        if ( NtoSave == 1 )
            {
            nof_parts = PartBox.get_select_list( &ip );
            if ( nof_parts > 0 )
                {
                for ( i=0; i<nof_parts; i++ )
                    {
                    if ( HaveProgressCancel )
                        break;

                    s = PartBox.item_text( ip[i] );

                    SaveList[0].part.append( part_name_copy(s.text()) );
                    }
                delete[] ip;
                ip = 0;
                }
            }
        else
            {
            for ( i=0; i<NtoSave; i++ )
                {
                /*
                ------------------
                Copy all the parts
                ------------------ */
                t.open( parts_dbname(SaveList[i].name.computer,SaveList[i].name.machine), PARTS_RECLEN, PFL );
                while ( t.get_next_record(NO_LOCK) )
                    {
                    if ( HaveProgressCancel )
                        break;

                    if (t.get_alpha(part_name, PARTS_PART_NAME_OFFSET, PART_NAME_LEN) )
                        SaveList[i].part.append( part_name );
                    }
                t.close();
                }
            }

        if ( BackupType & DO_BACKUP )
            {
            set_text( ProgressWindow, resource_string(MainInstance, BACKING_UP_STRING) );
            backup_data();
            }

        if ( BackupType & DO_RESTORE )
            {
            set_text( ProgressWindow, resource_string(MainInstance, RESTORING_STRING) );
            restore_data();
            }

        if ( BackupType & DO_DELETE )
            {
            set_text( ProgressWindow, resource_string(MainInstance, DELETING_STRING) );
            delete_data();
            }

        if ( BackupType & DO_RENUMBER )
            {
            set_text( ProgressWindow, resource_string(MainInstance, RENUMBERING_STRING) );
            renumber_data();
            }
        }
    }

if ( SaveList )
    {
    for ( i=0; i<NtoSave; i++ )
        SaveList[i].part.empty();
    delete[] SaveList;
    SaveList = 0;
    NtoSave  = 0;
    }

if ( ProgressWindow )
    SendMessage( ProgressWindow, WM_CLOSE, 0, 0L );

return 0;
}

/***********************************************************************
*                         START_BACKUP_THREAD                          *
***********************************************************************/
static void start_backup_thread( void )
{
DWORD  dwThreadId;
HANDLE hThread;

hThread = CreateThread(
    NULL,                                 /* no security attributes        */
    0,                                    /* use default stack size        */
    (LPTHREAD_START_ROUTINE) thread_func, /* thread function               */
    0,                                    /* argument to thread function   */
    0,                                    /* use default creation flags    */
    &dwThreadId );                        /* returns the thread identifier */

if ( !hThread )
    resource_message_box( MainInstance, THREAD_CREATE_FAIL, CANT_DO_THAT_STRING );
}

/***********************************************************************
*                       SAVING PROGRESS PROC                           *
***********************************************************************/
BOOL CALLBACK SavingProgressProc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        ProgressWindow = w;
        HaveProgressCancel = FALSE;
        start_backup_thread();
        return TRUE;

    case WM_CLOSE:
        if ( BackupWindow )
            SendMessage( BackupWindow, WM_DB_SAVE_DONE, 0, 0L );
        ProgressWindow = 0;
        EndDialog( w, 0 );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case SAVING_CANCEL_BUTTON:
                HaveProgressCancel = TRUE;
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                                 SAVE                                 *
***********************************************************************/
void save( BITSETYPE backup_type, HWND w, int machine_listbox_id, int part_listbox_id )
{
static TCHAR dotzip[] = TEXT(".zip" );
BackupType   = backup_type;
BackupWindow = w;

/*
-----------------------------------------------
Make sure the zip file name has .zip on the end
----------------------------------------------- */
if ( !findstring(dotzip, BackupZipFileName) )
    catstring( BackupZipFileName, dotzip );

if ( w )
    {
    MachineBox.init( w, machine_listbox_id );
    PartBox.init(    w, part_listbox_id    );
    }

DialogBox( MainInstance, TEXT("SAVING_PROGRESS"), w, (DLGPROC) SavingProgressProc );
}

/***********************************************************************
*                                 SAVE                                 *
***********************************************************************/
void save( BITSETYPE backup_type, HWND w, int machine_listbox_id, int part_listbox_id, int shots_listbox_id, int32 renumber_start )
{
ShotBox.init( w, shots_listbox_id );
RenumberStartShot = renumber_start;
save( backup_type, w, machine_listbox_id, part_listbox_id );
}

/***********************************************************************
*                                 SAVE                                 *
*                                                                      *
*                    This is a backup to a zip file.                   *
*                                                                      *
***********************************************************************/
void save( TCHAR * zip_file_name, BITSETYPE backup_type, HWND w, int machine_listbox_id, int part_listbox_id )
{

lstrcpy( BackupZipFileName, zip_file_name );

save( backup_type, w, machine_listbox_id, part_listbox_id );

}

/***********************************************************************
*                           SAVE_BACKUP_LIST                           *
***********************************************************************/
void save_backup_list( TCHAR * zip_file_name, BITSETYPE backup_type )
{
UseBackupListFile  = TRUE;
lstrcpy( BackupZipFileName, zip_file_name );
save( backup_type, 0, 0, 0 );
}

/***********************************************************************
*                          SAVE_ALL_TEXT_FILES                         *
***********************************************************************/
void save_all_text_files( TCHAR * zip_file_name  )
{
const BITSETYPE MY_BACKUP_TYPE = 0;

BackupAllTextFiles = TRUE;
lstrcpy( BackupZipFileName, zip_file_name );
save( MY_BACKUP_TYPE, 0, 0, 0 );
}
