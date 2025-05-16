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
#include "..\include\verrors.h"

#include "extern.h"
#include "resource.h"
#include "save.h"

#define SHOTPARM_TABLE_TYPE 1
#define GRAPHLST_TABLE_TYPE 2

static TCHAR   ZeroChar = TEXT('0');
static TCHAR   BackupComputerName[] = BACKUP_COMPUTER;
static TCHAR   EmptyString[] = TEXT( "" );

extern TCHAR   VisiTrakIniFile[];
extern HWND    ProgressWindow;
extern BOOLEAN HaveProgressCancel;

BOOLEAN set_last_shot_number( PART_NAME_ENTRY & pn );
void fill_machine_list( void );

/***********************************************************************
*                            SHOW_FILE_NAME                            *
***********************************************************************/
void show_file_name( TCHAR * sorc )
{
SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, sorc );
}

/***********************************************************************
*                            GET_SHOT_ENTRY                            *
***********************************************************************/
void get_shot_entry( SHOT_ENTRY & se, DB_TABLE & t, short table_type )
{
FIELDOFFSET shot_number_offset;
FIELDOFFSET date_offset;
FIELDOFFSET time_offset;

if ( table_type == SHOTPARM_TABLE_TYPE )
    {
    shot_number_offset = SHOTPARM_SHOT_NUMBER_OFFSET;
    date_offset        = SHOTPARM_TIME_OFFSET;
    time_offset        = SHOTPARM_DATE_OFFSET;
    }
else
    {
    shot_number_offset = GRAPHLST_SHOT_NUMBER_OFFSET;
    date_offset        = GRAPHLST_TIME_OFFSET;
    time_offset        = GRAPHLST_DATE_OFFSET;
    }

se.number = t.get_long( shot_number_offset );
t.get_time( se.time, time_offset );
t.get_date( se.time, date_offset );
}

/***********************************************************************
*                  GET_NEWEST_AND_OLDEST_SHOT_ENTRIES                  *
*                                                                      *
* Return the number of records of this type. 0 => entries not changed. *
***********************************************************************/
int32 get_newest_and_oldest_shot_entries( SHOT_RANGE_ENTRY & shot, PART_NAME_ENTRY & sorc, short table_type )
{
TCHAR * fname;
short   reclen;
int32   n;
DB_TABLE t;

if ( table_type == SHOTPARM_TABLE_TYPE )
    {
    fname  = shotparm_dbname( sorc.computer, sorc.machine, sorc.part );
    reclen = SHOTPARM_RECLEN;
    }
else
    {
    fname  = graphlst_dbname(sorc.computer, sorc.machine, sorc.part);
    reclen = GRAPHLST_RECLEN;
    }

t.open( fname, reclen, PFL );
n = t.nof_recs();
if ( n > 0 )
    {
    t.get_next_record( FALSE );
    get_shot_entry( shot.oldest, t, table_type );

    if ( n > 1 )
        {
        t.goto_record_number(n-1);
        t.get_rec();
        }

    get_shot_entry( shot.newest, t, table_type );
    }
t.close();

return n;
}

/***********************************************************************
*                              SHOT_RANGE                              *
*                                                                      *
*        Return the sum of parameter shots and profile shots           *
***********************************************************************/
int32 shot_range( SHOT_RANGE_ENTRY & dest, PART_NAME_ENTRY & sorc )
{
int32 n;
int32 np;
int32 ng;
SHOT_RANGE_ENTRY p;
SHOT_RANGE_ENTRY g;

np = get_newest_and_oldest_shot_entries( p, sorc, SHOTPARM_TABLE_TYPE );
ng = get_newest_and_oldest_shot_entries( g, sorc, GRAPHLST_TABLE_TYPE );

n = np + ng;
if ( n )
    {
    if ( np == 0 )
        {
        dest = g;
        }
    else if ( ng == 0 )
        {
        dest = p;
        }
    else
        {
        if ( seconds_difference(p.oldest.time, g.oldest.time) > 0 )
            dest.oldest = g.oldest;
        else
            dest.oldest = p.oldest;

        /*
        ---------------------------------
        if newest is greater, it is newer
        --------------------------------- */
        if ( seconds_difference(p.newest.time, g.newest.time) > 0 )
            dest.newest = p.newest;
        else
            dest.newest = g.newest;
        }

    }

return n;
}

/***********************************************************************
*                             RESTORE_DATA                             *
***********************************************************************/
BOOLEAN restore_data( PART_NAME_ENTRY & dest, PART_NAME_ENTRY & sorc )
{
DB_TABLE         d;
DB_TABLE         s;
int32            nd;
int32            ns;
int32            shot_number;
int32            shot_increment;
SHOT_RANGE_ENTRY drange;
SHOT_RANGE_ENTRY srange;
BOOLEAN          have_rec;
TCHAR          * filename;
TCHAR            progress_name[SHOT_LEN+5];

/*
-----------------------
Copy alarm summary data
----------------------- */
if ( BackupType & ALARM_DATA )
    {
    filename = alarmsum_dbname( sorc.computer, sorc.machine, sorc.part );
    if ( file_exists(filename) )
        {
        s.open( filename, ALARMSUM_RECLEN, PFL );

        filename = alarmsum_dbname( dest.computer, dest.machine, dest.part );
        if ( !file_exists(filename) )
            d.create( filename );

        d.open( filename, ALARMSUM_RECLEN, WL  );
        while ( s.get_next_record(FALSE) )
            {
            d.copy_rec( s );
            d.rec_append();
            }

        s.close();
        d.close();
        }
    }

/*
----------------------------------
Get the range of shots to be added
---------------------------------- */
ns = shot_range( srange, sorc );

/*
---------------------------------------------------------
If there are no entries in the source directory I am done
--------------------------------------------------------- */
if ( !ns )
    return TRUE;

/*
---------------------------
Get the existing shot range
--------------------------- */
nd = shot_range( drange, dest );

/*
---------------------------------------------------------------------
If there are no entries in the destination directory I need only copy
--------------------------------------------------------------------- */
shot_increment = 0;

if ( nd > 0 )
    {
    /*
    ----------------------------------------------
    For now I am only going to append the new data
    ---------------------------------------------- */
    if ( srange.oldest.number <= drange.newest.number )
        {
        shot_increment = drange.newest.number + 1 - srange.oldest.number;
        }
    }

lstrcpy( progress_name + SHOT_LEN, PROFILE_NAME_SUFFIX );

/*
-------------
Copy profiles
------------- */
if ( BackupType & PROFILE_DATA )
    {
    s.open( graphlst_dbname(sorc.computer, sorc.machine, sorc.part), GRAPHLST_RECLEN, PFL );
    d.open( graphlst_dbname(dest.computer, dest.machine, dest.part), GRAPHLST_RECLEN, WL );

    while ( s.get_next_record(FALSE) )
        {
        shot_number = s.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
        copy_profile( dest, sorc, shot_number, shot_increment );

        insalph( progress_name, shot_number, SHOT_LEN, ZeroChar, DECIMAL_RADIX );
        show_file_name( progress_name );

        /*
        ------------------------
        Copy the graphlst record
        ------------------------ */
        d.copy_rec( s );
        if ( shot_increment )
            {
            shot_number += shot_increment;
            d.put_long( GRAPHLST_SHOT_NUMBER_OFFSET, shot_number, SHOT_LEN );
            }
        d.rec_append();
        }

    s.close();
    d.close();
    }

/*
---------------
Copy parameters
--------------- */
if ( BackupType & PARAMETER_DATA )
    {
    s.open( shotparm_dbname(sorc.computer, sorc.machine, sorc.part), SHOTPARM_RECLEN, PFL );
    d.open( shotparm_dbname(dest.computer, dest.machine, dest.part), SHOTPARM_RECLEN, WL  );

    while ( s.get_next_record(FALSE) )
        {
        d.copy_rec( s );
        if ( shot_increment )
            {
            shot_number = d.get_long( SHOTPARM_SHOT_NUMBER_OFFSET );
            shot_number += shot_increment;
            d.put_long( SHOTPARM_SHOT_NUMBER_OFFSET, shot_number, SHOT_LEN );
            }
        d.rec_append();
        }

    s.close();
    d.close();
    }

nd = shot_range( drange, dest );
if ( nd <= 0 )
    {
    drange.newest.number = 0L;
    GetSystemTime( &drange.newest.time );
    }

s.open( plookup_dbname(dest.computer, dest.machine), PLOOKUP_RECLEN, PWL );
s.put_alpha(PLOOKUP_PART_NAME_OFFSET, dest.part, PART_NAME_LEN);
s.reset_search_mode();
have_rec = s.get_next_key_match( 1, TRUE );

s.put_long( PLOOKUP_LAST_SHOT_OFFSET, drange.newest.number, SHOT_LEN );
s.put_date( PLOOKUP_DATE_OFFSET, drange.newest.time );
s.put_time( PLOOKUP_TIME_OFFSET, drange.newest.time );

if ( have_rec )
    {
    s.rec_update();
    s.unlock_record();
    }
else
    {
    s.rec_append();
    }

s.close();

return TRUE;
}

/***********************************************************************
*                         MAKE_BACKUP_COMPUTER                         *
***********************************************************************/
void make_backup_computer( void )
{
COMPUTER_CLASS c;
TCHAR          path[MAX_PATH+1];

lstrcpy( path, BackupDir );
append_directory_to_path( path, BackupComputerName );
c.add( BackupComputerName, path );
}

/***********************************************************************
*                            SHOW_PROGRESS                             *
***********************************************************************/
void show_progress( PART_NAME_ENTRY sorc )
{
SetDlgItemText( ProgressWindow, SAVING_MACHINE_TEXT_BOX, sorc.machine );
SetDlgItemText( ProgressWindow, SAVING_PART_TEXT_BOX, sorc.part );
SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, EmptyString );
}

/***********************************************************************
*                           RESTORE_DOWNTIME                           *
***********************************************************************/
static void restore_downtime( TCHAR * computer_name, TCHAR * machine_name )
{

TCHAR dest[MAX_PATH+1];
TCHAR sorc[MAX_PATH+1];

lstrcpy( sorc, downtime_dbname(BackupComputerName, machine_name) );
lstrcpy( dest, downtime_dbname(computer_name,      machine_name) );

if ( file_exists(sorc) )
    CopyFile( sorc, dest, OVERWRITE_EXISTING );
}

/***********************************************************************
*                       RESTORE_SURETRAK_GLOBALS                       *
***********************************************************************/
static void restore_suretrak_globals( TCHAR * computer_name, TCHAR * machine_name )
{
TCHAR dest[MAX_PATH+1];
TCHAR sorc[MAX_PATH+1];
TCHAR * sorc_file_name;

lstrcpy( sorc, machine_directory_name(BackupComputerName, machine_name) );
append_backslash_to_path( sorc );
sorc_file_name = sorc + lstrlen(sorc);

/*
--------------------
Control message file
-------------------- */
lstrcpy( sorc_file_name, CONTROL_MESSAGE_FILE );
lstrcpy( dest, control_message_name(computer_name) );
if ( file_exists(sorc) )
    CopyFile( sorc, dest, FALSE );

/*
--------------------
Control program file
-------------------- */
lstrcpy( sorc_file_name, CONTROL_PROGRAM_FILE );
lstrcpy( dest, control_program_name(computer_name) );
if ( file_exists(sorc) )
    CopyFile( sorc, dest, FALSE );

/*
--------------------------
Global suretrak parameters
-------------------------- */
lstrcpy( sorc_file_name, STPARAM_FILE );
lstrcpy( dest, stparam_filename(computer_name) );
if ( file_exists(sorc) )
    CopyFile( sorc, dest, FALSE );

/*
------------------
Valve test profile
------------------ */
lstrcpy( sorc_file_name, VTMASTER_FILE );
lstrcpy( dest, valve_test_master_filename(computer_name) );
if ( file_exists(sorc) )
    CopyFile( sorc, dest, FALSE );

/*
--------
FasTrak2
-------- */
lstrcpy( sorc_file_name, FTII_EDITOR_SETTINGS_FILE );
lstrcpy( dest, ftii_editor_settings_name(computer_name, machine_name) );
if ( file_exists(sorc) )
    CopyFile( sorc, dest, FALSE );

lstrcpy( sorc_file_name, FTII_GLOBAL_SETTINGS_FILE );
lstrcpy( dest, ftii_global_settings_name(computer_name, machine_name) );
if ( file_exists(sorc) )
    CopyFile( sorc, dest, FALSE );

lstrcpy( sorc_file_name, FTII_CTRL_PROG_STEPS_FILE );
lstrcpy( dest, ftii_ctrl_prog_steps_name(computer_name, machine_name) );
if ( file_exists(sorc) )
    CopyFile( sorc, dest, FALSE );

lstrcpy( sorc_file_name, FTII_MACHINE_SETTINGS_FILE );
lstrcpy( dest, ftii_machine_settings_name(computer_name, machine_name) );
if ( file_exists(sorc) )
    CopyFile( sorc, dest, FALSE );
}

/***********************************************************************
*                           RESTORE_ASENSOR                            *
* The asensor table is copied to the machine directory of the backup   *
***********************************************************************/
static void restore_asensor( TCHAR * dest_computer_name, TCHAR * sorc_machine_name )
{
TCHAR dest[MAX_PATH+1];
TCHAR sorc[MAX_PATH+1];

lstrcpy( sorc, madir_name(BackupComputerName, sorc_machine_name, ASENSOR_DB) );
if ( file_exists(sorc) )
    {
    /*
    ----------------------------------------------------------------------------------------
    I always restore to the local computer so I can use the normal asensor name for the dest
    ---------------------------------------------------------------------------------------- */
    lstrcpy( dest, asensor_dbname() );
    CopyFile( sorc, dest, FALSE );
    }
}

/***********************************************************************
*                           RESTORE_BOARDS                             *
* The boards tables are copied to the machine directory of the backup  *
***********************************************************************/
static void restore_boards( TCHAR * dest_computer_name, TCHAR * sorc_machine_name )
{
TCHAR dest[MAX_PATH+1];
TCHAR sorc[MAX_PATH+1];

lstrcpy( sorc, madir_name(BackupComputerName, sorc_machine_name, BOARDS_DB) );
if ( file_exists(sorc) )
    {
    lstrcpy( dest, boards_dbname(dest_computer_name) );
    CopyFile( sorc, dest, FALSE );
    }

lstrcpy( sorc, madir_name(BackupComputerName, sorc_machine_name, FTII_BOARDS_DB) );
if ( file_exists(sorc) )
    {
    lstrcpy( dest, ftii_boards_dbname(dest_computer_name) );
    CopyFile( sorc, dest, FALSE );
    }

}

/***********************************************************************
*                         GET_FT_TYPE                                  *
* When I restore a machine I can't tell if it is an ft2 machine        *
* unless I have the original board lists. These are in the backup      *
* dir for the machine in question. If the board is not in the ftii     *
* board list then the machine is not ft2.                              *
***********************************************************************/
static void get_ft_type( MACHINE_CLASS & m )
{
STRING_CLASS s;
DB_TABLE     t;

m.is_ftii = FALSE;
s = madir_name( BackupComputerName, m.name, FTII_BOARDS_DB );
if ( file_exists(s.text()) )
    {
    t.open( s.text(), FTII_BOARDS_RECLEN, PFL );
    while ( t.get_next_record(FALSE) )
        {
        if ( m.ft_board_number == t.get_short(FTII_BOARDS_NUMBER_OFFSET) )
            {
            m.is_ftii = TRUE;
            break;
            }
        }
    t.close();
    }
}

/***********************************************************************
*                           IS_WORKSTATION_ZERO                        *
***********************************************************************/
BOOLEAN is_workstation_zero( void )
{
COMPUTER_CLASS    c;
TCHAR * cp;
TCHAR   ws0_name[COMPUTER_NAME_LEN+1];

cp = get_ini_string( VisiTrakIniFile, TEXT("Config"), TEXT("WorkstationZero") );
if ( unknown(cp) )
    lstrcpy( ws0_name, DEFAULT_WS0_NAME );
else
    lstrcpyn( ws0_name, cp, COMPUTER_NAME_LEN+1 );

return strings_are_equal( c.whoami(), ws0_name, COMPUTER_NAME_LEN );
}

/***********************************************************************
*                      GET_BOARD_FOR_NEW_MACHINE                       *
***********************************************************************/
static void get_board_for_new_machine( MACHINE_CLASS & m )
{
STRING_CLASS s;
DB_TABLE t;
short    new_board_number;
short    bn;

new_board_number = NO_BOARD_NUMBER;

if ( m.is_ftii )
    {
    s = ftii_boards_dbname( m.computer );
    if ( file_exists(s.text()) )
        {
        t.open( s.text(), FTII_BOARDS_RECLEN, PFL );
        while ( t.get_next_record(FALSE) )
            {
            bn = t.get_short( FTII_BOARDS_NUMBER_OFFSET );

            if ( new_board_number == NO_BOARD_NUMBER )
                new_board_number = bn;

            if ( bn == m.ft_board_number )
                {
                new_board_number = bn;
                break;
                }
            }
        t.close();
        }
    }
else
    {
    s = boards_dbname( m.computer );
    if ( file_exists(s.text()) )
        {
        t.open( s.text(), BOARDS_RECLEN, PFL );
        while ( t.get_next_record(FALSE) )
            {
            bn = t.get_short( BOARDS_NUMBER_OFFSET );

            if ( new_board_number == NO_BOARD_NUMBER )
                new_board_number = bn;

            if ( bn == m.ft_board_number )
                {
                new_board_number = bn;
                break;
                }
            }
        t.close();
        }
    }

if ( bn != m.ft_board_number )
    {
    m.ft_board_number = bn;

    if ( bn == NO_BOARD_NUMBER )
        m.muxchan = NO_MUX_CHANNEL;
    else
        m.muxchan = MIN_FT_CHANNEL;
    }
}

/***********************************************************************
*                            RESTORE_DATA                              *
***********************************************************************/
void restore_data( void )
{
int     i;
int     choice;
BOOLEAN need_setup;
BOOLEAN have_dest_machine;
BOOLEAN i_own_the_semaphor;

PART_NAME_ENTRY dest;
PART_NAME_ENTRY sorc;

TCHAR   destpath[MAX_PATH+1];

COMPUTER_CLASS    c;
MACHINE_CLASS     m;
MACHINE_CLASS    dm;
DB_TABLE          t;
SAVE_LIST_ENTRY * s;

if ( NtoSave < 1 )
    return;

make_backup_computer();

show_progress( dest );

for ( i=0; i<NtoSave; i++ )
    {
    if ( HaveProgressCancel )
        break;

    s = SaveList+i;

    sorc.set( BackupComputerName, s->name.machine, EmptyString );

    if ( BackupType & ANALOG_SENSOR_DATA  )
        restore_asensor( dest.computer, sorc.machine );

    if ( BackupType & FASTRAK_BOARDS_DATA )
        restore_boards( dest.computer, sorc.machine );

    if ( !m.find(sorc.computer, sorc.machine) )
        {
        resource_message_box( MainInstance, CANT_LOAD_MACHINE_STRING, CANT_DO_THAT_STRING );
        break;
        }

    get_ft_type( m );

    /*
    ---------------------------------------------
    Set the destination computer and machine name
    --------------------------------------------- */
    if ( NtoSave == 1 && !DestMachine.is_empty() )
        {
        lstrcpy( dest.computer, DestMachine.computer );
        lstrcpy( dest.machine,  DestMachine.machine  );
        }
    else
        {
        if ( MachList.find(sorc.machine) )
            dest.set( MachList.computer_name(), sorc.machine, EmptyString );
        else
            dest.set( c.whoami(), sorc.machine, EmptyString );
        }

    show_progress( dest );

    have_dest_machine = dm.find( dest.computer, dest.machine );

    c.find( dest.computer );

    if ( c.is_this_computer() && is_workstation_zero() )
        {
        resource_message_box( MainInstance, NOT_ON_WS0_STRING, DEST_NOT_FOUND_STRING );
        continue;
        }

    if ( !c.is_this_computer() && !is_workstation_zero() )
        {
        resource_message_box( MainInstance, MUST_RESTORE_THERE_STRING, MACHINE_ON_OTHER_STRING );
        continue;
        }

    if ( have_dest_machine )
        {
        if ( s->part.find(dm.current_part) )
            {
            dest.set( dm.current_part );
            show_progress( dest );
            choice = resource_message_box( MainInstance, MONITOR_OFF_STRING, WILL_RESTOR_CURRENT_STRING, MB_YESNO | MB_ICONWARNING | MB_SYSTEMMODAL );
            dest.set( EmptyString );
            if ( choice != IDYES )
                break;
            }
        }

    /*
    ------------------------------------------------
    Make an empty machines table if I can't find one
    ------------------------------------------------ */
    if ( !have_dest_machine )
        {
        lstrcpy( destpath, machset_dbname(dest.computer) );
        if ( !file_exists(destpath) )
            t.create( destpath );
        }

    show_progress( dest );

    lstrcpy( m.computer, dest.computer );
    lstrcpy( m.name,     dest.machine  );

    /*
    -------------------------------------
    Preserve the current monitoring setup
    ------------------------------------- */
    if ( have_dest_machine )
        {
        lstrcpy( m.current_part, dm.current_part );
        lstrcpy( m.opnumber,     dm.opnumber     );
        m.monitor_flags       = dm.monitor_flags;
        m.ft_board_number     = dm.ft_board_number;
        m.muxchan             = dm.muxchan;
        m.is_ftii             = dm.is_ftii;
        m.suretrak_controlled = dm.suretrak_controlled;
        m.profile             = dm.profile;
        m.params              = dm.params;
        }
    else
        {
        m.monitor_flags &= ~MA_MONITORING_ON;
        get_board_for_new_machine( m );
        }
    /*
    ------------------------------------------------------------------
    If this machine already exists then I must own the sempahor before
    making any changes
    ------------------------------------------------------------------ */
    i_own_the_semaphor = FALSE;
    if ( have_dest_machine )
        {
        i_own_the_semaphor = get_shotsave_semaphore( dest.computer, dest.machine );
        if ( !i_own_the_semaphor )
            {
            resource_message_box( MainInstance, CANT_DO_THAT_STRING, SEMAPHOR_FAIL_STRING );
            continue;
            }
        }

    /*
    -------------------------------------------
    Check to see that the current part is valid
    (only if this is a new machine).
    ------------------------------------------- */
    if ( !have_dest_machine && !s->part.find(m.current_part) )
        {
        s->part.rewind();
        if ( s->part.next() )
            lstrcpy( m.current_part, s->part.text() );
        }

    /*
    -----------------------------------------------------------
    Save the machine setup, creating a new machine if necessary
    ----------------------------------------------------------- */
    m.save();

    /*
    -------------------------------------------------------------------
    If I just created a new machine, refill the machine list. I sort by
    computer because that's the way the add_computer loads initially.
    ------------------------------------------------------------------- */
    if ( !have_dest_machine )
        {
        MachList.add( dest.computer, dest.machine, SORT_BY_COMPUTER );
        CreatedNewMachine = TRUE;
        }

    if ( BackupType & DOWNTIME_DATA )
        restore_downtime( dest.computer, dest.machine );

    if ( BackupType & SURETRAK_GLOBAL_DATA )
        restore_suretrak_globals( dest.computer, dest.machine );

    s->part.rewind();
    while ( s->part.next() )
        {
        if ( HaveProgressCancel )
            break;

        dest.set( s->part.text() );
        sorc.set( s->part.text() );
        show_progress( dest );

        need_setup = FALSE;
        if ( BackupType & SETUP_DATA )
            need_setup = TRUE;
        else
            {
            if ( !part_exists(dest.computer, dest.machine, dest.part) )
                need_setup = TRUE;
            }

        if ( need_setup )
            copypart( dest.computer, dest.machine, dest.part, sorc.computer, sorc.machine, sorc.part, COPY_PART_ALL );

        restore_data( dest, sorc );
        }

    if ( i_own_the_semaphor )
        free_shotsave_semaphore();
    }
}

