#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\machine.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

#include "extern.h"
#include "resource.h"
#include "save.h"

#define PART_LISTBOX_LEN (PART_NAME_LEN + SHOT_LEN + SHOT_LEN + 3)
#define MAX_ZIP_CMD_LEN 150

static TCHAR     BackupComputerName[] = BACKUP_COMPUTER;
static TCHAR     CmdBuf[MAX_PATH+MAX_ZIP_CMD_LEN+1];
static TCHAR     EmptyString[] = TEXT("");
static TCHAR     NullChar = TEXT( '\0' );

extern HWND      ProgressWindow;
extern BOOLEAN   HaveProgressCancel;

/***********************************************************************
*                           RESOURCE_STRING                            *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                         COPY_PLOOKUP_RECORD                          *
***********************************************************************/
static BOOLEAN copy_plookup_record( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )
{

DB_TABLE   t;
int32      shot_number;
SYSTEMTIME shot_time;
BOOLEAN    have_rec;

/*
------------------------------------
Get the plookup record for this part
------------------------------------ */
t.open( plookup_dbname(computer_name, machine_name), PLOOKUP_RECLEN, PFL );
t.put_alpha( PLOOKUP_PART_NAME_OFFSET, part_name, PART_NAME_LEN );
t.reset_search_mode();
have_rec = t.get_next_key_match(1, NO_LOCK);
if ( have_rec )
    {
    shot_number = t.get_long( PLOOKUP_LAST_SHOT_OFFSET );
    t.get_date( shot_time, PLOOKUP_DATE_OFFSET );
    t.get_time( shot_time, PLOOKUP_TIME_OFFSET );
    }
t.close();

/*
------------------------------------------
Copy it over the record in the backup part
------------------------------------------ */
if ( have_rec )
    {
    t.open( plookup_dbname(BackupComputerName, machine_name), PLOOKUP_RECLEN, PWL );

    t.put_alpha( PLOOKUP_PART_NAME_OFFSET, part_name, PART_NAME_LEN );
    t.reset_search_mode();
    have_rec = t.get_next_key_match( 1, WITH_LOCK );

    t.put_long( PLOOKUP_LAST_SHOT_OFFSET, shot_number, SHOT_LEN );
    t.put_date( PLOOKUP_DATE_OFFSET, shot_time );
    t.put_time( PLOOKUP_TIME_OFFSET, shot_time );

    if ( have_rec )
        {
        t.rec_update();
        t.unlock_record();
        }
    else
        {
        t.rec_append();
        }

    t.close();
    if ( t.get_global_error() == VS_SUCCESS )
        return TRUE;
    }


return FALSE;
}

/***********************************************************************
*                             BACKUP_PART                              *
***********************************************************************/
static BOOLEAN backup_part( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name )
{
static TCHAR profile_suffix[] = TEXT("*") PROFILE_BOTH_SUFFIXES;
TCHAR        dest[MAX_PATH+1];
TCHAR        sorc[MAX_PATH+1];
DB_TABLE     t;
DB_TABLE     d;
int32        i;
int32        n;
int32        ntocopy;
STRING_CLASS dest_profile;
STRING_CLASS sorc_profile;
DWORD        last_tick_count;
DWORD        this_tick_count;
BOOLEAN      status;

status = TRUE;

set_text( ProgressWindow, SAVING_PART_TEXT_BOX, part_name );

/*
----------------------
Always save the setups
---------------------- */
copypart( BackupComputerName, machine_name, part_name, computer_name, machine_name, part_name, COPY_PART_ALL );

if ( BackupType & PARAMETER_DATA )
    {
    SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, SHOTPARM_DB );

    lstrcpy( sorc, shotparm_dbname(computer_name,      machine_name, part_name) );
    lstrcpy( dest, shotparm_dbname(BackupComputerName, machine_name, part_name) );

    if ( file_exists(sorc) )
        {
        if ( t.open(sorc, SHOTPARM_RECLEN, PFL) )
            {
            n = t.nof_recs();
            if ( ProfilesToCopy > 0 && ProfilesToCopy<n )
                {
                n -= ProfilesToCopy;
                n--;
                t.goto_record_number( n );
                t.get_current_record( FALSE );
                d.open( dest, SHOTPARM_RECLEN, WL  );
                while ( t.get_next_record(FALSE) )
                    {
                    d.copy_rec( t );
                    d.rec_append();
                    }
                d.close();
                }
            else
                {
                CopyFile( sorc, dest, OVERWRITE_EXISTING );
                }
            t.close();
            }
        }

    /*
    -------------------------------------------------------
    If I can't copy anything then just make an empty table.
    ------------------------------------------------------- */
    if ( !file_exists(dest) )
        t.create( dest );
    }

if ( BackupType & PROFILE_DATA )
    {
    SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, GRAPHLST_DB );

    /*
    --------------------------
    Copy the graphlst database
    -------------------------- */
    lstrcpy( sorc, graphlst_dbname(computer_name,      machine_name, part_name) );
    lstrcpy( dest, graphlst_dbname(BackupComputerName, machine_name, part_name) );

    if ( file_exists(sorc) )
        {
        last_tick_count = 0;
        if ( t.open(sorc, GRAPHLST_RECLEN, PFL) )
            {
            n = t.nof_recs();

            if ( BackupListCount > 0 )
                {
                d.open( dest, GRAPHLST_RECLEN, WL  );
                for ( i=0; i<BackupListCount; i++ )
                    {
                    if ( strings_are_equal(BackupList[i].machine, machine_name) )
                        {
                        if ( strings_are_equal(BackupList[i].part, part_name) )
                            {
                            n = asctoint32( BackupList[i].shot );
                            t.put_long(  GRAPHLST_SHOT_NUMBER_OFFSET, n, SHOT_LEN );
                            if ( t.get_next_key_match(1, NO_LOCK) )
                                {
                                d.copy_rec( t );
                                d.rec_append();

                                sorc_profile = profile_name( computer_name, machine_name, part_name, n );
                                if ( file_exists(sorc_profile.text()) )
                                    {
                                    dest_profile = profile_name( BackupComputerName, machine_name, part_name, n );
                                    }
                                else
                                    {
                                    sorc_profile = profile_ii_name(      computer_name, machine_name, part_name, n );
                                    dest_profile = profile_ii_name( BackupComputerName, machine_name, part_name, n );
                                    }
                                this_tick_count = GetTickCount();
                                if ( last_tick_count>this_tick_count || (this_tick_count-last_tick_count) > 4 )
                                   {
                                   last_tick_count = this_tick_count;
                                   SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, filename_from_path(sorc_profile.text()) );
                                   }
                                CopyFile( sorc_profile.text(), dest_profile.text(), OVERWRITE_EXISTING );
                                }
                            }
                        }
                    if ( HaveProgressCancel )
                        break;
                    }
                d.close();
                }
            else if ( ProfilesToCopy >= 0 )
                {
                ntocopy = ProfilesToCopy;
                if ( ntocopy > n )
                    ntocopy = n;
                /*
                ------------------------------------
                Copy the last ProfilesToCopy records
                ------------------------------------ */
                if ( ntocopy>0 && ntocopy<n )
                    {
                    n -= ntocopy;
                    n--;
                    t.goto_record_number( n );
                    t.get_current_record( FALSE );
                    }

                d.open( dest, GRAPHLST_RECLEN, WL  );
                while ( t.get_next_record(FALSE) )
                    {
                    /*
                    ------------------
                    Copy the .pro file
                    ------------------ */
                    n = t.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
                    sorc_profile = profile_name( computer_name, machine_name, part_name, n );
                    if ( file_exists(sorc_profile.text()) )
                        {
                        dest_profile = profile_name( BackupComputerName, machine_name, part_name, n );
                        }
                    else
                        {
                        sorc_profile = profile_ii_name(      computer_name, machine_name, part_name, n );
                        dest_profile = profile_ii_name( BackupComputerName, machine_name, part_name, n );
                        }

                     this_tick_count = GetTickCount();
                     if ( last_tick_count>this_tick_count || (this_tick_count-last_tick_count) > 4 )
                        {
                        last_tick_count = this_tick_count;
                        SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, filename_from_path(sorc_profile.text()) );
                        }

                    CopyFile( sorc_profile.text(), dest_profile.text(), OVERWRITE_EXISTING );

                    /*
                    ------------------------
                    Copy the graphlst record
                    ------------------------ */
                    d.copy_rec( t );
                    d.rec_append();

                    if ( HaveProgressCancel )
                        break;
                    }
                d.close();
                }
            t.close();
            }
        }
    if ( !file_exists(dest) )
        t.create( dest );

    /*
    -----------------------
    Copy the plookup record
    ----------------------- */
    copy_plookup_record( computer_name, machine_name, part_name );
    }

if ( BackupType & ALARM_DATA )
    {
    SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, ALARMSUM_DB );

    lstrcpy( sorc, alarmsum_dbname(computer_name,      machine_name, part_name) );
    lstrcpy( dest, alarmsum_dbname(BackupComputerName, machine_name, part_name) );

    if ( file_exists(sorc) )
        {
        if ( t.open(sorc, ALARMSUM_RECLEN, PFL) )
            {
            CopyFile( sorc, dest, OVERWRITE_EXISTING );
            t.close();
            }
        }
    else
        {
        t.create( dest );
        }
    }

SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, EmptyString );
return status;
}

/***********************************************************************
*                            BACKUP_DOWNTIME                           *
***********************************************************************/
static void backup_downtime( TCHAR * computer_name, TCHAR * machine_name )
{
TCHAR dest[MAX_PATH+1];
TCHAR sorc[MAX_PATH+1];

lstrcpy( sorc, downtime_dbname(computer_name,      machine_name) );
lstrcpy( dest, downtime_dbname(BackupComputerName, machine_name) );

if ( file_exists(sorc) )
    CopyFile( sorc, dest, OVERWRITE_EXISTING );

}

/***********************************************************************
*                            PURGE_DOWNTIME                            *
***********************************************************************/
static void purge_downtime( TCHAR * computer_name, TCHAR * machine_name )
{
DB_TABLE t;
int32    n;
STRING_CLASS s;

if ( BackupType & PURGE_DOWNTIME )
    {
    s = downtime_dbname( computer_name, machine_name );
    if ( file_exists(s.text()) )
        {
        if ( t.open( s.text(), DOWNTIME_RECLEN, FL) )
            {
            n = t.nof_recs();
            if ( n > 0 )
                {
                t.goto_record_number( n-1 );
                t.get_current_record( NO_LOCK );
                }
            t.empty();
            if ( n > 0 )
                t.rec_append();
            t.close();
            }
        }
    }
}

/***********************************************************************
*                       BACKUP_SURETRAK_GLOBALS                        *
*       Copy the suretrak global setups to the MACHINE directory       *
***********************************************************************/
static void backup_suretrak_globals( TCHAR * computer_name, TCHAR * machine_name )
{
TCHAR dest[MAX_PATH+1];
TCHAR sorc[MAX_PATH+1];
TCHAR * dest_file_name;

lstrcpy( dest, machine_directory_name(BackupComputerName, machine_name) );
append_backslash_to_path( dest );
dest_file_name = dest + lstrlen(dest);

/*
--------------------
Control message file
-------------------- */
lstrcpy( sorc, control_message_name(computer_name) );
lstrcpy( dest_file_name, CONTROL_MESSAGE_FILE );
if ( file_exists(sorc) )
    CopyFile( sorc, dest, FALSE );

/*
--------------------
Control program file
-------------------- */
lstrcpy( sorc, control_program_name(computer_name) );
lstrcpy( dest_file_name, CONTROL_PROGRAM_FILE );
CopyFile( sorc, dest, FALSE );


/*
--------------------------
Global suretrak parameters
-------------------------- */
lstrcpy( sorc, stparam_filename(computer_name) );
lstrcpy( dest_file_name, STPARAM_FILE );
CopyFile( sorc, dest, FALSE );

/*
------------------
Valve test profile
------------------ */
lstrcpy( sorc, valve_test_master_filename(computer_name) );
lstrcpy( dest_file_name, VTMASTER_FILE );
if ( file_exists(sorc) )
    CopyFile( sorc, dest, FALSE );

/*
-------------------------------------
Copy the FasTrak2 files if they exist
------------------------------------- */
lstrcpy( sorc, ftii_editor_settings_name(computer_name, machine_name) );
lstrcpy( dest_file_name, FTII_EDITOR_SETTINGS_FILE );
if ( file_exists(sorc) )
    CopyFile( sorc, dest, FALSE );

lstrcpy( sorc, ftii_global_settings_name(computer_name, machine_name) );
lstrcpy( dest_file_name, FTII_GLOBAL_SETTINGS_FILE );
if ( file_exists(sorc) )
    CopyFile( sorc, dest, FALSE );

lstrcpy( sorc, ftii_ctrl_prog_steps_name(computer_name, machine_name) );
lstrcpy( dest_file_name, FTII_CTRL_PROG_STEPS_FILE );
if ( file_exists(sorc) )
    CopyFile( sorc, dest, FALSE );

copystring( sorc, ftii_machine_settings_name(computer_name, machine_name) );
lstrcpy( dest_file_name, FTII_MACHINE_SETTINGS_FILE );
if ( file_exists(sorc) )
    CopyFile( sorc, dest, FALSE );
}

/***********************************************************************
*                             BACKUP_DATA                              *
***********************************************************************/
void backup_data( void )
{
const BITSETYPE backup_mask = SETUP_DATA | PROFILE_DATA | DOWNTIME_DATA | PARAMETER_DATA | ALARM_DATA;
const DWORD TEN_MINUTES = 600000L;
int     i;

TCHAR   dest[MAX_PATH+1];
TCHAR   sorc[MAX_PATH+1];

const TCHAR * cp;
TCHAR * dp;

TCHAR   computer_name[COMPUTER_NAME_LEN+1];
TCHAR   machine_name[MACHINE_NAME_LEN+1];
TCHAR   part_name[PART_NAME_LEN+1];

COMPUTER_CLASS c;
MACHINE_CLASS  m;
DB_TABLE       t;
BOOLEAN        backing_up;
SAVE_LIST_ENTRY * s;
BOOLEAN        backup_ok;


if ( BackupAllTextFiles )
    {
    if ( file_exists(BackupZipFileName) )
        DeleteFile( BackupZipFileName );

    get_exe_directory( dest );
    dp = findstring( TEXT("Exes"), dest );
    if ( dp )
        {
        dp--;
        *dp = NullChar;
        SetCurrentDirectory( dest );

        get_exe_directory( CmdBuf );
        lstrcat( CmdBuf, TEXT("pkzip25") );

        lstrcat( CmdBuf, TEXT(" -add -dir -lev=9 -excl=graphlst.txt -excl=shotparm.txt -excl=alarmsum.txt -excl=downtime.txt ") );

        lstrcat( CmdBuf, BackupZipFileName );
        lstrcat( CmdBuf, TEXT(" data\\*.txt data\\*.dat data\\*.csv data\\*.msg data\\*.prg exes\\*.ini ") );

        execute_and_wait( CmdBuf, NULL, TEN_MINUTES );
        if ( get_exe_directory(dest) )
            {
            remove_backslash_from_path( dest );
            SetCurrentDirectory( dest );
            }
        }
    else
        {
        resource_message_box( MainInstance, NO_EXES_DIR_FOUND_STRING, CANT_DO_THAT_STRING );
        }
    return;
    }

if ( NtoSave < 0 )
    return;

/*
------------------------------------------------
See if I am backing something up or just purging
------------------------------------------------ */
backing_up = TRUE;
backup_ok = TRUE;
if ( (BackupType & backup_mask) == 0 )
    backing_up = FALSE;

if ( backing_up )
    {
    while ( !disk_is_in_drive(BackupZipFileName) )
        {
        i = resource_message_box(MainInstance, TRY_AGAIN_STRING, NO_DISK_IN_DRIVE_STRING, MB_YESNO );
        if ( i != IDYES )
            return;
        }

    copystring( dest, BackupZipFileName );
    if ( dir_from_filename(dest) )
        {
        if ( !directory_exists(dest) )
            {
            if ( !create_directory(dest) )
                {
                copystring( sorc, resource_string(NO_ZIP_DIR_STRING) );
                MessageBox( 0, BackupZipFileName, sorc, MB_OK | MB_SYSTEMMODAL );
                return;
                }
            }
        }
    }

/*
-----------------------------------------------------
Create the root directory if it doesn't exist already
----------------------------------------------------- */
if ( backing_up && !path_is_drive_only(BackupDir) )
    {
    lstrcpy( dest, BackupDir );
    remove_backslash_from_path( dest );

    /*
    --------------------------------------------
    Empty the destination directory if it exists
    -------------------------------------------- */
    if ( directory_exists(dest) )
        kill_directory( dest );

    if ( !CreateDirectory( dest, NULL ) )
        {
        copystring( sorc, resource_string(DIR_CREATE_FAIL) );
        MessageBox( 0, dest, sorc, MB_OK );
        return;
        }
    }

/*
--------------------------------------------------------
Create the backup computer directory if it doesn't exist
-------------------------------------------------------- */
if ( backing_up )
    {
    lstrcpy( dest, BackupDir );
    append_directory_to_path( dest, BackupComputerName );
    c.add( BackupComputerName, dest );
    if ( !directory_exists(dest) )
        {
        if ( !CreateDirectory(dest, NULL) )
            {
            resource_message_box( MainInstance, COMPUTER_DIR_CREATE_FAIL, CANT_DO_THAT_STRING );
            return;
            }
        }

    /*
    ------------------------------------------------
    Make an empty machines table if I can't find one
    ------------------------------------------------ */
    lstrcpy( sorc, machset_dbname(BackupComputerName) );
    if ( !file_exists(sorc) )
        t.create( sorc );
    }

for ( i=0; i<NtoSave; i++ )
    {
    if ( HaveProgressCancel )
        break;

    s = SaveList+i;

    lstrcpy( computer_name, s->name.computer );
    lstrcpy( machine_name,  s->name.machine  );

    if ( lstrcmp(machine_name, NO_MACHINE) != 0 )
        {
        SetDlgItemText( ProgressWindow, SAVING_MACHINE_TEXT_BOX, machine_name );
        SetDlgItemText( ProgressWindow, SAVING_PART_TEXT_BOX, EmptyString );
        SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, EmptyString );

        if ( HaveProgressCancel )
            break;

        if ( m.find(computer_name, machine_name) )
            {

            if ( !waitfor_shotsave_semaphor(computer_name, machine_name) )
                {
                resource_message_box( MainInstance, CANT_DO_THAT_STRING, SEMAPHOR_FAIL_STRING );
                continue;
                }

            if ( backing_up )
                {
                lstrcpy( m.computer, BackupComputerName );
                if ( !m.save() )
                    backup_ok = FALSE;

                /*
                ------------------------------------------------------------------------------------------
                Make a copy of the boards and analogs tables in the machine folder since everything is
                being saved as one backup computer and the machines could all be from different computers.
                ------------------------------------------------------------------------------------------ */
                lstrcpy( sorc, boards_dbname(computer_name) );
                lstrcpy( dest, madir_name(BackupComputerName, machine_name, BOARDS_DB) );
                if ( file_exists(sorc) )
                    CopyFile( sorc, dest, FALSE );

                lstrcpy( sorc, ftii_boards_dbname(computer_name) );
                lstrcpy( dest, madir_name(BackupComputerName, machine_name, FTII_BOARDS_DB) );
                if ( file_exists(sorc) )
                    CopyFile( sorc, dest, FALSE );

                /*
                ---------------------------------------------------------------------
                The analog sensor table is in the root directory of the sorc computer
                --------------------------------------------------------------------- */
                cp = computer_root_directory( computer_name );
                if ( cp )
                    {
                    lstrcpy( sorc, cp );
                    append_filename_to_path( sorc, ASENSOR_DB );
                    lstrcpy( dest, madir_name(BackupComputerName, machine_name, ASENSOR_DB) );
                    if ( file_exists(sorc) )
                        CopyFile( sorc, dest, FALSE );
                    }

                /*
                --------
                Downtime
                -------- */
                if ( BackupType & DOWNTIME_DATA )
                    backup_downtime( computer_name, machine_name );

                /*
                --------------------------------------------------------------------------
                I used to do this only with suretrak machines but now I do it for everyone
                as the ftii files span both types.
                -------------------------------------------------------------------------- */
                if ( BackupType & SETUP_DATA )
                    backup_suretrak_globals( computer_name, machine_name );
                }


            s->part.rewind();
            while ( s->part.next() )
                {
                if ( HaveProgressCancel )
                    break;

                lstrcpy( part_name, s->part.text() );

                if ( backing_up )
                    {
                    if ( !backup_part(computer_name, machine_name, part_name) )
                        backup_ok = FALSE;
                    }

                }

            free_shotsave_semaphore();
            }
        }
    }

if ( HaveProgressCancel )
    return;

if ( backing_up )
    {
    lstrcpy( dest, BackupZipFileName );
    if ( dir_from_filename(dest) )
        {
        if ( !directory_exists(dest) )
            {
            if ( !create_directory(dest) )
                {
                resource_message_box(MainInstance, NO_ZIP_DIR_STRING, CANT_DO_THAT_STRING );
                return;
                }
            }
        }

    if ( file_exists(BackupZipFileName) )
        DeleteFile( BackupZipFileName );

    lstrcpy( dest, BackupDir );
    remove_backslash_from_path( dest );
    SetCurrentDirectory( dest );

    get_exe_directory( CmdBuf );
    lstrcat( CmdBuf, TEXT("pkzip25") );

    lstrcat( CmdBuf, TEXT(" -add -dir ") );

    lstrcat( CmdBuf, BackupZipFileName );
    lstrcat( CmdBuf, TEXT(" *.* ") );

    if ( !execute_and_wait(CmdBuf, NULL, TEN_MINUTES) )
        {
        copystring( dest, resource_string(UNABLE_TO_BACKUP_STRING) );
        MessageBox( 0, dest, BackupZipFileName, MB_OK | MB_SYSTEMMODAL );
        backup_ok = FALSE;
        }

    if ( get_exe_directory(dest) )
        {
        remove_backslash_from_path( dest );
        SetCurrentDirectory( dest );
        }

    if ( !file_exists(BackupZipFileName) )
        {
        copystring( dest, resource_string(NO_ZIP_FILE_STRING) );
        MessageBox( 0, dest, BackupZipFileName, MB_OK | MB_SYSTEMMODAL );
        backup_ok = FALSE;
        }
    }

if ( HaveProgressCancel )
    return;

if ( !backup_ok )
    return;

/*
-----
Purge
----- */
if ( !(BackupType & WITH_PURGE) && !(BackupType & PURGE_DOWNTIME) && !(BackupType & PURGE_ALARM_SUMMARY) )
    return;

copystring( sorc, resource_string(PURGING_STRING) );
set_text( ProgressWindow, sorc );

for ( i=0; i<NtoSave; i++ )
    {
    if ( HaveProgressCancel )
        break;

    s = SaveList+i;

    lstrcpy( computer_name, s->name.computer );
    lstrcpy( machine_name,  s->name.machine  );

    if ( lstrcmp(machine_name, NO_MACHINE) != 0 )
        {
        SetDlgItemText( ProgressWindow, SAVING_MACHINE_TEXT_BOX, machine_name );
        SetDlgItemText( ProgressWindow, SAVING_PART_TEXT_BOX, EmptyString );
        SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, EmptyString );

        if ( waitfor_shotsave_semaphor(computer_name, machine_name) )
            {
            if ( !HaveProgressCancel )
                {
                purge_downtime( computer_name, machine_name );

                s->part.rewind();
                while ( s->part.next() )
                    {
                    if ( HaveProgressCancel )
                        break;

                    copystring( part_name,  s->part.text() );

                    purge( computer_name, machine_name, part_name );

                    if ( BackupType & PURGE_ALARM_SUMMARY )
                        {
                        lstrcpy( dest,  alarmsum_dbname(computer_name, machine_name, part_name) );
                        if ( file_exists(dest) )
                            DeleteFile( dest );
                        }
                    }
                }
            free_shotsave_semaphore();
            }
        }
    }
}
