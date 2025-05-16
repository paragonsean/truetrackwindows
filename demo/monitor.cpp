#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\dstat.h"
#include "..\include\events.h"
#include "..\include\ftanalog.h"
#include "..\include\machine.h"
#include "..\include\names.h"
#include "..\include\param.h"
#include "..\include\part.h"
#include "..\include\part_analog_sensor.h"
#include "..\include\stddown.h"
#include "..\include\structs.h"
#include "..\include\stringcl.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"
#include "..\include\ftii_curve_types.h"
#include "..\include\ftii_min_max.h"
#include "..\include\ftii_shot_data.h"

#include "resource.h"
#include "extern.h"

static const DWORD MS_TO_WAIT = 1000;
static const short WRITE_PARAM_TIMEOUT = 150;   /* About 1/4 second */

MACHINE_CLASS      SorcMachine;
PART_CLASS         SorcPart;
PARAMETER_CLASS    SorcParam;
PART_ANALOG_SENSOR AnalogSensor;
FTANALOG_CLASS     SorcFtAnalog;
STRING_CLASS       LastSorcDir;

/*
--------------------------------
Indexes for hardware status bits
-------------------------------- */
static const int DATA_COL_MODE_STATE_INDEX = 0;
static const int AUTO_SHOT_STATE_INDEX     = 1;
static const int NEW_DATA_STATE_INDEX      = 2;

bool        HaveCycleStart = false;
bool        Started = false;

long        AutoShotDisplaySeconds = 2;
long        AutoShotDisplayTime    = 0;

SYSTEMTIME  NowSystemTime;
long        Now                    = 0;
int32       ShotNumber             = 0;
long        LastShotTime           = 0;

static STDDOWN_CLASS StdDown;

static PART_NAME_ENTRY DestPart;

static const TCHAR BackSlashChar        = TEXT( '\\' );
static const TCHAR NullChar             = TEXT( '\0' );

static const TCHAR     ProfilePattern[] = TEXT( "*.PR?" );
TCHAR MonallIniFile[]               = TEXT( "monall.ini" );
TCHAR ConfigSection[]               = TEXT( "Config" );

static TEXT_LIST_CLASS FileName;

double * calculate_ftii_parameters( FTII_PROFILE & f, MACHINE_CLASS & machine, PART_CLASS & part, PARAMETER_CLASS & param, PART_ANALOG_SENSOR & asensor, FTANALOG_CLASS & ftanalog );

/***********************************************************************
*                           CURRENT_OPERATOR                           *
***********************************************************************/
TCHAR * current_operator()
{
static TCHAR opnumber[OPERATOR_NUMBER_LEN+1];
DB_TABLE t;

lstrcpy( opnumber, NO_OPERATOR_NUMBER );

if ( t.open(machset_dbname(DestPart.computer), MACHSET_RECLEN, PFL) )
    {
    t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, DestPart.machine, MACHINE_NAME_LEN );
    if ( t.get_next_key_match(1, NO_LOCK) )
        t.get_alpha( opnumber, MACHSET_OPERATOR_NUMBER_OFFSET, OPERATOR_NUMBER_LEN );
    t.close();
    }

return opnumber;
}

/***********************************************************************
*                          SET_DOWNTIME_STATE                          *
***********************************************************************/
void set_downtime_state( TCHAR * newcat, TCHAR * newsubcat, SYSTEMTIME st )

{
TCHAR     * cp;
TCHAR     * worker;
DB_TABLE    t;
DSTAT_CLASS d;
int         old_state;

if ( !Started )
    return;

d.get( DestPart.computer, DestPart.machine );
old_state = d.down_state();

if ( d.same_code(newcat, newsubcat) )
    return;

d.set_cat( newcat );
d.set_subcat( newsubcat );
d.set_time( st );
d.put( DestPart.computer, DestPart.machine );

worker = current_operator();

/*
---------------------------------
Only write if downtime is enabled
--------------------------------- */
if ( DownTimeoutSeconds > 0 )
    {
    if ( ((old_state == HUMAN_DOWN_STATE) && (d.down_state() == MACH_UP_STATE)) ||
         ((old_state == MACH_UP_STATE) && (d.down_state() != MACH_UP_STATE))       )
        {
        if ( t.open(downtime_dbname(DestPart.computer, DestPart.machine), DOWNTIME_RECLEN, PWL) )
            {
            make_downtime_record( t, NowSystemTime, newcat, newsubcat, worker, DestPart.part );
            t.rec_append();
            t.close();
            }
        }
    }

cp = make_downtime_event_string( DestPart.machine, DestPart.part, st, newcat, newsubcat, worker );
if ( cp )
    MainWindow.post( WM_POKEMON, (WPARAM) DOWN_DATA_INDEX, (LPARAM) cp );

CurrentDownState = d.down_state();
}

/***********************************************************************
*                             NEXTSHOT                                 *
***********************************************************************/
bool nextshot()
{

DB_TABLE   t;
BOOLEAN    have_rec;
BOOLEAN    need_to_insert = FALSE;

/*
--------------------------------------------
If there is no plookup record, start at one.
-------------------------------------------- */
ShotNumber = 0;

t.open( plookup_dbname(DestPart.computer, DestPart.machine), PLOOKUP_RECLEN, PWL );
t.put_alpha(PLOOKUP_PART_NAME_OFFSET, DestPart.part, PART_NAME_LEN);
t.reset_search_mode();
have_rec = t.get_next_key_match(1, TRUE);
if ( have_rec )
    ShotNumber = t.get_long( PLOOKUP_LAST_SHOT_OFFSET );
else
    need_to_insert = t.goto_first_greater_than_record( 1 );

ShotNumber++;
t.put_alpha(PLOOKUP_PART_NAME_OFFSET, DestPart.part, PART_NAME_LEN);
t.put_long( PLOOKUP_LAST_SHOT_OFFSET, ShotNumber, SHOT_LEN );
t.put_date( PLOOKUP_DATE_OFFSET, NowSystemTime );
t.put_time( PLOOKUP_TIME_OFFSET, NowSystemTime );

if ( have_rec )
    {
    t.rec_update();
    t.unlock_record();
    }
else
    {
    if ( need_to_insert )
        t.rec_insert();
    else
        t.rec_append();
    }

t.close();

if ( t.get_global_error() == VS_SUCCESS )
    return true;

return false;
}

/***********************************************************************
*                   SEND_MACHINE_STATE_EVENT_STRING                    *
*                                                                      *
*                         "M01\t00F1\t00FF"                            *
***********************************************************************/
static void send_machine_state_event_string( int index )
{
static const int DDE_LINE_LEN = MACHINE_NAME_LEN + 1 + BOARD_DATA_HEX_LEN + 1 + BOARD_DATA_HEX_LEN + 1;
static const TCHAR * buffers[] = {
    { TEXT( "\t0000\tA000") },
    { TEXT( "\tA000\tA400") },
    { TEXT( "\tA400\t0000") }
    };

TCHAR * buf;
TCHAR * cp;

if ( index < 0 || index > 2 )
    return;

buf = maketext( DDE_LINE_LEN );
if ( !buf )
    return;

cp = buf;
cp = copystring( cp, DestPart.machine );
copystring( cp, buffers[index] );

MainWindow.post( WM_POKEMON, (WPARAM) NEW_MACH_STATE_INDEX, (LPARAM) buf );
}
/***********************************************************************
*                            CHECK_SORC_DIR                            *
***********************************************************************/
static BOOLEAN check_sorc_dir()
{
COMPUTER_CLASS  c;
PART_NAME_ENTRY pn;
TCHAR         * ep;
TCHAR         * s;
TCHAR         * cp;
int             n;
BOOLEAN         status;

status = FALSE;
if ( LastSorcDir != SorcDir )
    {
    LastSorcDir = SorcDir;
    s = maketext( SorcDir.text() );
    if ( s )
        {
        if ( c.find_directory(s) )
            {
            copystring( pn.computer, c.name() );
            n = lstrlen( c.directory() );
            cp = s;
            cp += n;
            ep = findchar( BackSlashChar, cp );
            if ( ep )
                {
                *ep = NullChar;
                copystring( pn.machine, cp );
                cp = ep;
                cp++;
                ep = findchar( BackSlashChar, cp );
                if ( ep )
                    {
                    *ep = NullChar;
                    copystring( pn.part, cp );
                    SorcMachine.find( pn.computer, pn.machine );
                    SorcPart.find( pn.computer, pn.machine, pn.part );
                    SorcParam.find( pn.computer, pn.machine, pn.part );
                    SorcFtAnalog.load( pn.computer, pn.machine, pn.part );
                    status = TRUE;
                    }
                }
            }
        delete[] s;
        }
    }

return status;
}

/***********************************************************************
*                              MAKE_A_SHOT                             *
***********************************************************************/
static void make_a_shot()
{
const BOOL OVERWRITE_EXISTING = FALSE;

HANDLE         fh;
DWORD          access_mode;
DWORD          bytes_read;
DWORD          bytes_written;
DWORD          share_mode;
BOOL           status;
DWORD          seek_status;
PROFILE_HEADER ph;
STRING_CLASS   sorc;
STRING_CLASS   dest;
int            i;
short          nof_parms;
float          parms[MAX_PARMS];
double       * dp;
BITSETYPE      save_flags;
float          value;
DB_TABLE       t;
FIELDOFFSET    fo;
FTII_PROFILE   f;
BOOLEAN        is_ftii;

if ( !FileName.next () )
    {
    if ( IsContinuous )
        {
        FileName.rewind();
        if ( !FileName.next() )
            return;
        }
    else
        {
        return;
        }
    }

sorc = SorcDir;
sorc += FileName.text();

if ( get_shotsave_semaphore(DestPart.computer, DestPart.machine) )
    {
    if ( nextshot() )
        {
        is_ftii = FALSE;
        if ( findstring(PROFILE_II_SUFFIX, FileName.text()) )
            is_ftii = TRUE;

        /*
        ----------------------------------------------------------
        Copy the file to the destination directory with a new name
        ---------------------------------------------------------- */
        if ( is_ftii )
            dest = profile_ii_name( DestPart.computer, DestPart.machine, DestPart.part, ShotNumber );
        else
            dest = profile_name( DestPart.computer, DestPart.machine, DestPart.part, ShotNumber );
        CopyFile( sorc.text(), dest.text(), OVERWRITE_EXISTING );
        status = TRUE;

        if ( is_ftii )
            {
            check_sorc_dir();
            if ( f.get(sorc) )
                {
                dp = calculate_ftii_parameters( f, SorcMachine, SorcPart, SorcParam, AnalogSensor, SorcFtAnalog );
                if ( dp )
                    {
                    for ( i=0; i<MAX_PARMS; i++ )
                        parms[i] = dp[i];
                    delete[] dp;
                    dp = 0;
                    }
                }
            }
        else
            {
            /*
            -----------------------------------------------------------------------
            Put the new machine, part, shot number, and shot time into the new file
            ----------------------------------------------------------------------- */
            access_mode = GENERIC_WRITE | GENERIC_READ;
            share_mode  = FILE_SHARE_READ;
            status      = FALSE;

            fh = CreateFile( dest.text(), access_mode, share_mode, 0, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, 0 );
            if ( fh != INVALID_HANDLE_VALUE )
                {
                status = ReadFile( fh, &ph, sizeof(ph), &bytes_read, 0 );
                if ( status )
                    {
                    status = ReadFile( fh, &nof_parms, sizeof(short), &bytes_read, 0 );

                    if ( status && nof_parms > 0 )
                          status = ReadFile( fh, parms, nof_parms*sizeof(float), &bytes_read, 0 );
                    }
                if ( status )
                    {
                    seek_status = SetFilePointer( fh, 0, 0, FILE_BEGIN );
                    if ( seek_status != 0xFFFFFFFF )
                        {
                        copystring( ph.machine_name, DestPart.machine );
                        copystring( ph.part_name,    DestPart.part    );
                        ph.shot_number = ShotNumber;

                        SystemTimeToFileTime( &NowSystemTime, &ph.time_of_shot );

                        status = WriteFile( fh, &ph, sizeof(PROFILE_HEADER), &bytes_written, 0 );
                        }
                    }

                CloseHandle( fh );
                }
             }

        save_flags = SAVE_PARM_DATA | SAVE_PROFILE_DATA;

        /*
        ------------------------------------
        Append a record to the graphlst file
        ------------------------------------ */
        t.open( graphlst_dbname(DestPart.computer,DestPart.machine, DestPart.part), GRAPHLST_RECLEN, PWL );
        t.put_long(  GRAPHLST_SHOT_NUMBER_OFFSET, ShotNumber, SHOT_LEN );
        t.put_date(  GRAPHLST_DATE_OFFSET, NowSystemTime );
        t.put_time(  GRAPHLST_TIME_OFFSET, NowSystemTime );

        t.put_short( GRAPHLST_SAVE_FLAGS_OFFSET, (short) save_flags, GRAPHLST_SAVE_FLAGS_LEN );
        t.rec_append();
        t.close();

        /*
        ------------------------------------
        Append a record to the shotparm file
        ------------------------------------ */
        if ( status )
            {
            t.open( shotparm_dbname(DestPart.computer,DestPart.machine, DestPart.part), SHOTPARM_RECLEN, WL, WRITE_PARAM_TIMEOUT );
            t.put_long( SHOTPARM_SHOT_NUMBER_OFFSET, ShotNumber, SHOT_LEN );
            t.put_date( SHOTPARM_DATE_OFFSET, NowSystemTime );
            t.put_time( SHOTPARM_TIME_OFFSET, NowSystemTime );
            t.put_short( SHOTPARM_SAVE_FLAGS_OFFSET, (short) save_flags, SHOTPARM_SAVE_FLAGS_LEN );
            fo = SHOTPARM_PARAM_1_OFFSET;
            for ( i=0; i<MAX_PARMS; i++ )
                {
                value = 0.0;
                if ( i < nof_parms )
                    value = parms[i];
                t.put_float( fo, value, SHOTPARM_FLOAT_LEN );
                fo += SHOTPARM_FLOAT_LEN+1;
                }
            t.rec_append();
            t.close();
            }
        }
    free_shotsave_semaphore();
    }

LastShotTime = Now;
MainWindow.post( WM_NEWSHOT, (WPARAM) 0, (LPARAM) ShotNumber );
send_machine_state_event_string( DATA_COL_MODE_STATE_INDEX );
set_downtime_state( DOWNCAT_SYSTEM_CAT, DOWNCAT_UP_SUBCAT, NowSystemTime );
}

/***********************************************************************
*                             START_A_SHOT                             *
***********************************************************************/
static void start_a_shot()
{
if ( !Started )
    return;

send_machine_state_event_string( AUTO_SHOT_STATE_INDEX );

AutoShotDisplayWindow.show();
AutoShotDisplayTime = Now;
HaveCycleStart      = true;
}

/***********************************************************************
*                           START_MONITORING                           *
***********************************************************************/
static void start_monitoring()
{

hourglass_cursor();

set_text( AutoShotDisplayWindow.control(AUTO_SHOT_MACH_NAME_TBOX),  DestPart.machine  );

FileName.empty();
FileName.get_file_list( SorcDir.text(), ProfilePattern );
if ( FileName.count() < 1 )
    {
    Started = false;
    BrouseWindow.post( WM_NO_DATA );
    return;
    }

FileName.sort();
FileName.rewind();

restore_cursor();

HaveNewSorc = FALSE;        // Tell the brouse dialog I am using the current sorc dir
if ( SorcDirEbox )
    InvalidateRect( SorcDirEbox, 0, TRUE );

Started = true;
if ( !ManualMode )
    start_a_shot();
}

/***********************************************************************
*                       GET_DOWN_TIMEOUT_SECONDS                       *
* This is called just before copying the CurrentPart to the DestPart   *
* so I want to use the CurrentPart values.                             *
***********************************************************************/
void get_down_timeout_seconds()
{
DB_TABLE t;

DownTimeoutSeconds = 0;

t.open( machset_dbname(CurrentPart.computer), MACHSET_RECLEN, PFL );
t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, CurrentPart.machine, MACHINE_NAME_LEN );
if ( t.get_next_key_match(1, NO_LOCK) )
DownTimeoutSeconds = (long) t.get_short( MACHSET_CYCLE_TIMEOUT_OFFSET );
t.close();
}

/***********************************************************************
*                         BOARD_MONITOR_THREAD                         *
***********************************************************************/
DWORD board_monitor_thread( int * notused )
{
DWORD       status;
SYSTEMTIME  st;
TIME_CLASS  tc;
DSTAT_CLASS d;

BoardMonitorIsRunning = TRUE;

StdDown.startup();
AnalogSensor.get();

while ( TRUE )
    {
    GetLocalTime(&NowSystemTime);
    Now = sec_since1990( NowSystemTime );

    if ( ShuttingDown )
        {
        BoardMonitorIsRunning = FALSE;
        if ( CurrentDownState == PROG_UP_STATE || CurrentDownState == MACH_UP_STATE || CurrentDownState == NO_DOWN_STATE )
            set_downtime_state( DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_EXIT_SUBCAT, NowSystemTime );

        MainWindow.close();
        break;
        }

    if ( HaveNewPart )
        {
        /*
        -----------------------------
        Down this machine if it is up
        ----------------------------- */
        if ( CurrentDownState == PROG_UP_STATE || CurrentDownState == MACH_UP_STATE || CurrentDownState == NO_DOWN_STATE )
            set_downtime_state( DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_EXIT_SUBCAT, NowSystemTime );

        /*
        ----------------------------------------
        Get the timeout seconds for the new part
        ---------------------------------------- */
        get_down_timeout_seconds();

        DestPart = CurrentPart;

        d.get( DestPart.computer, DestPart.machine );
        CurrentDownState = d.down_state();

        if ( CurrentDownState == PROG_DOWN_STATE )
            set_downtime_state( DOWNCAT_SYSTEM_CAT, DOWNCAT_PROG_START_SUBCAT, NowSystemTime );

        HaveNewPart = FALSE;
        LastShotTime = Now;
        }

    if ( BoardMonitorCommand )
        {
        switch ( BoardMonitorCommand )
            {
            case START_MONITORING:
                start_monitoring();
                break;

            case STOP_MONITORING:
                break;

            case MANUAL_SHOT_REQ:
                start_a_shot();
                break;
            }
        BoardMonitorCommand = 0;
        }

   if ( HaveCycleStart )
        {
        if ( (Now - AutoShotDisplayTime) > AutoShotDisplaySeconds )
            {
            send_machine_state_event_string( NEW_DATA_STATE_INDEX );
            HaveCycleStart = false;
            AutoShotDisplayWindow.hide();
            make_a_shot();
            }
        }
    else if ( Started && !ManualMode )
        {
        if ( (Now - LastShotTime) > SecondsBetweenShots )
            start_a_shot();
        }

    else if ( Started && DownTimeoutSeconds > 0 )
        {
        /*
        ------------------------------------------------------------
        If there was no status change for this board, check for down
        ------------------------------------------------------------ */
        if ( CurrentDownState == PROG_UP_STATE || CurrentDownState == MACH_UP_STATE || CurrentDownState == NO_DOWN_STATE )
            {
            if ( (Now - LastShotTime) > DownTimeoutSeconds )
                {
                if ( SubtractTimeoutFromAutoDown )
                    {
                    tc.set( (unsigned long) Now );
                    tc -= (unsigned long) DownTimeoutSeconds;
                    st = tc.system_time();
                    }
                else
                    {
                    st = NowSystemTime;
                    }

                if ( StdDown.find( Now ) )
                    set_downtime_state( StdDown.cat(), StdDown.subcat(), st );
                else
                    set_downtime_state( DOWNCAT_SYSTEM_CAT, DOWNCAT_UNSP_DOWN_SUBCAT, st );
                }
            }
        }
    status = WaitForSingleObject( BoardMonitorEvent, MS_TO_WAIT );
    if ( status == WAIT_FAILED )
        resource_message_box( MainInstance,  UNABLE_TO_CONTINUE_STRING, THREAD_WAIT_FAIL_STRING,  MB_OK | MB_SYSTEMMODAL );
    }

return 0;
}

/***********************************************************************
*                      START_AQUIRING_DATA_THREAD                      *
***********************************************************************/
void start_aquiring_data_thread( void )
{
DWORD  id;
HANDLE h;

h = CreateThread(
    NULL,                                          /* no security attributes        */
    0,                                             /* use default stack size        */
    (LPTHREAD_START_ROUTINE) board_monitor_thread, /* thread function       */
    0,                                             /* argument to thread function   */
    0,                                             /* use default creation flags    */
    &id );                                         /* returns the thread identifier */

if ( !h )
    resource_message_box( MainInstance, CREATE_FAILED_STRING, MONITOR_THREAD_STRING, MB_OK | MB_SYSTEMMODAL );

}
