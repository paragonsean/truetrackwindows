#include <windows.h>
#include <ddeml.h>

#include "..\include\visiparm.h"
#include "..\include\bitclass.h"
#include "..\include\dbclass.h"
#include "..\include\events.h"
#include "..\include\fileclas.h"
#include "..\include\iniclass.h"
#include "..\include\names.h"
#include "..\include\nameclas.h"
#include "..\include\structs.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\mrunlist.h"
#include "..\include\part.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\warmupcl.h"
#include "..\include\ftii.h"
#include "..\include\workclas.h"

#include "extern.h"
#include "resource.h"

#define ACQUIRE_ALL                   1
#define ACQUIRE_NONE                  2
#define TIME_SKIP                     3
#define SKIP_COUNT                    4
#define ACQUIRE_COUNT                 5
#define SKIP                          6
#define MAX_SAVED                     7
#define SKIP_LABEL                    8

#define MAX_LABEL_LEN                20
#define IDS_SKIP_COUNT_LABEL          1
#define IDS_TIME_SKIP_LABEL           2
#define NOF_MONITOR_LIST_TABS        11

const int MACHINE_BUF_LEN = MACHINE_NAME_LEN + 1 + PART_NAME_LEN + 2 + 3 + POST_IMPACT_SECONDS_LEN + 1 + POST_IMPACT_SECONDS_LEN + 1 + 40 + 40;
static TCHAR MachineLineBuf[MACHINE_BUF_LEN+1];

static TCHAR TabString[]   = TEXT( "\t" );
static TCHAR ZeroString[]  = TEXT( "0" );
static TCHAR EmptyString[] = TEXT( "" );

static TCHAR * ShotsString = 0;
static TCHAR * SecString   = 0;

const TCHAR NullChar = TEXT( '\0' );
const TCHAR TabChar  = TEXT( '\t' );

static TCHAR     VisiTrakIniFile[]             = TEXT( "visitrak.ini" );
static TCHAR     ConfigSection[]               = TEXT( "Config" );
static TCHAR     DisplaySecondsKey[]           = TEXT( "DisplaySeconds" );
static TCHAR     HaveMultipartRunlistKey[]     = TEXT( "HaveMultipartRunlist" );
static TCHAR     MonitorLogFileKey[]           = TEXT( "MonitorLogFile" );
static TCHAR     MonitorLogFormatKey[]         = TEXT( "MonitorLogFormat" );
static TCHAR     MonallExeFileName[]           = TEXT( "monall.exe" );

BOOLEAN         ProgramIsUpdating   = FALSE;

/***********************************************************************
*                                  CLEANUP                             *
***********************************************************************/
static void cleanup( void )
{
if ( MachineList )
    {
    delete[] MachineList;
    NofMachines = 0;
    MachineList = 0;
    }
}

/***********************************************************************
*                        MONITOR_PROC_CLEANUP                          *
   This is called from the
***********************************************************************/
void monitor_proc_cleanup( void )
{
cleanup();

if ( ShotsString )
    {
    delete [] ShotsString;
    ShotsString = 0;
    }

if ( SecString )
    {
    delete [] SecString;
    SecString = 0;
    }
}

/***********************************************************************
*                          SAVE_COUNTS_TO_LINE                         *
***********************************************************************/
TCHAR * save_counts_to_line( TCHAR * dest, SAVE_COUNTS * sc )
{
TCHAR tMax[MAX_INTEGER_LEN+1];
TCHAR t[MAX_INTEGER_LEN+1];

int32toasc( tMax, sc->max_saved, DECIMAL_RADIX );

if ( sc->flags & ACQUIRE_ALL_BIT )
    {
    dest = copystring(  dest, ZeroString );
    dest = copy_w_char( dest, ShotsString, TabChar );
    dest = copy_w_char( dest, tMax, TabChar );
    dest = copy_w_char( dest, tMax, TabChar );
    }
else if ( sc->flags & ACQUIRE_NONE_BIT )
    {
    dest = copystring(  dest, ZeroString );
    dest = copy_w_char( dest, ShotsString, TabChar );
    dest = copy_w_char( dest, ZeroString,  TabChar );
    dest = copystring(  dest, TabString );
    }
else
    {
    if ( sc->flags & SKIP_TIME_BIT )
        {
        dest = copystring( dest, int32toasc( sc->skip_seconds) );
        dest = copy_w_char( dest, SecString, TabChar );
        }
    else
        {
        dest = copystring( dest, int32toasc( sc->skip_count) );
        dest = copy_w_char( dest, ShotsString, TabChar );
        }

    int32toasc( t, sc->acquire_count, DECIMAL_RADIX );
    dest = copy_w_char( dest, t, TabChar );
    dest = copy_w_char( dest, int32toasc( sc->acquire_count), TabChar );
    dest = copy_w_char( dest, tMax, TabChar );
    }
return dest;
}

/***********************************************************************
*                         MACHINE_LISTBOX_STRING                       *
***********************************************************************/
static TCHAR * machine_listbox_string( int i )
{
TCHAR      * cp;
TCHAR      * yes_or_no;

static TCHAR yes[] = TEXT("Y");
static TCHAR no[]  = TEXT("N");

MACHINE_CLASS * m;
PART_CLASS    * p;

m = &MachineList[i].machine;
p = &MachineList[i].part;

cp = MachineLineBuf;

cp = copy_w_char( cp, m->name, TabChar );
cp = copy_w_char( cp, m->current_part, TabChar );

if ( m->monitor_flags & MA_ALARM_MONITORING_ON )
    yes_or_no = yes;
else
    yes_or_no = no;
cp = copy_w_char( cp, yes_or_no, TabChar );

if ( m->monitor_flags & MA_MONITORING_ON )
    yes_or_no = yes;
else
    yes_or_no = no;
cp = copy_w_char( cp, yes_or_no, TabChar );

post_impact_seconds( cp, p->nof_time_samples, p->ms_per_time_sample );
cp += POST_IMPACT_SECONDS_LEN;
*cp = TabChar;
cp++;
if ( MachineList[i].display_seconds.isempty() )
    *cp++ = TabChar;
else
    cp = copy_w_char( cp, MachineList[i].display_seconds.text(), TabChar );

cp = save_counts_to_line( cp, &m->params );
cp = save_counts_to_line( cp, &m->profile );
cp--;
*cp = NullChar;

return MachineLineBuf;
}

/***********************************************************************
*                         SAVE_SAMPLE_SECONDS                          *
*               Pass the machine index [0,NofMachines).                *
***********************************************************************/
static void save_sample_seconds( short i )
{

INI_CLASS         ini;
NAME_CLASS        s;
FTII_VAR_ARRAY va;
DB_TABLE t;
TCHAR    partname[PART_NAME_LEN+1];
MACHINE_CLASS * m;
PART_CLASS * p;

m = &MachineList[i].machine;
p = &MachineList[i].part;

if ( i >= 0 )
    {
    t.open( parts_dbname(ComputerName, MachineList[i].machine.name), PARTS_RECLEN, PWL );
    while ( t.get_next_record(NO_LOCK) )
        {
        t.get_alpha( partname, PARTS_PART_NAME_OFFSET, PART_NAME_LEN );
        if ( compare(partname, m->current_part, PART_NAME_LEN) == 0 )
            {
            if ( t.get_current_record(WITH_LOCK) )
                {
                t.put_short( PARTS_MS_PER_TIME_SAMPLE_OFFSET, p->ms_per_time_sample, PARTS_DB_SHORT_LEN );
                t.put_short( PARTS_N_OF_TIME_SAMPLES_OFFSET,  p->nof_time_samples,   PARTS_DB_SHORT_LEN );
                t.rec_update();
                t.unlock_record();
                }
            break;
            }
        }
    t.close();
    }

s.get_part_display_ini_name( p->computer, p->machine, p->name );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
s = MachineList[i].display_seconds;
if ( s.isempty() )
    ini.remove_key( DisplaySecondsKey );
else
    ini.put_string( DisplaySecondsKey, s.text() );

if ( m->is_ftii )
    {
    va.upsize(2);
    va[0].set( TIME_INTERVAL_MS_VN, (unsigned) p->ms_per_time_sample );
    va[1].set( NOF_TIME_SAMPLES_VN, (unsigned) p->nof_time_samples   );
    va.put_into( ftii_part_settings_name(p->computer, p->machine, p->name) );
    }
}

/***********************************************************************
*                           GET_SAVE_COUNTS                            *
***********************************************************************/
void get_save_counts( int base, SAVE_COUNTS * sc )
{
int32 mycount;

sc->flags = 0;

/*
------------
Monitor Bits
------------ */
if ( is_checked(MonitorWindow, base+ACQUIRE_ALL) )
    sc->flags |= ACQUIRE_ALL_BIT;

if ( is_checked(MonitorWindow, base+ACQUIRE_NONE) )
    sc->flags |= ACQUIRE_NONE_BIT;

if ( is_checked(MonitorWindow, base+TIME_SKIP) )
    sc->flags |= SKIP_TIME_BIT;

if ( is_checked(MonitorWindow, base+SKIP_COUNT) )
    sc->flags |= SKIP_COUNT_BIT;

/*
------------------
Skip Time or Count
------------------ */
mycount = GetDlgItemInt( MonitorWindow, base+SKIP, 0, FALSE );

if ( sc->flags & SKIP_TIME_BIT )
    sc->skip_seconds = (short) mycount;
else
    sc->skip_count = mycount;

/*
-------------
Acquire Count
------------- */
sc->acquire_count = GetDlgItemInt( MonitorWindow, base+ACQUIRE_COUNT, 0, FALSE );

/*
---------
Max Saved
--------- */
sc->max_saved = GetDlgItemInt( MonitorWindow, base+MAX_SAVED, 0, FALSE );
}

/***********************************************************************
*                     REFRESH_MACHINE_LISTBOX                          *
***********************************************************************/
static void refresh_machine_listbox( void )
{
TCHAR           s[MAX_FLOAT_LEN+1];
MACHINE_CLASS * m;
PART_CLASS    * p;
LISTBOX_CLASS   lb;

/*
-------------------------
React to human input only
------------------------- */
if ( ProgramIsUpdating )
    return;

if ( CurrentMachineIndex == NO_INDEX )
    return;

m = &MachineList[CurrentMachineIndex].machine;
p = &MachineList[CurrentMachineIndex].part;

MachineList[CurrentMachineIndex].has_changed = TRUE;

lb.init( MonitorWindow, MONITOR_LISTBOX );

/*
-------------------
Get machine monitor
------------------- */
m->monitor_flags &= ~(MA_MONITORING_ON | MA_ALARM_MONITORING_ON);

if ( is_checked(MonitorWindow, MONITOR_MACHINE_CHECKBOX) )
    m->monitor_flags |= MA_MONITORING_ON;

/*
-----------------
Get alarm monitor
----------------- */
if ( is_checked(MonitorWindow, MONITOR_ALARMS_CHECKBOX) )
    m->monitor_flags |= MA_ALARM_MONITORING_ON;

/*
---------------
Get sample time
--------------- */
get_text( s, MonitorWindow, SAMPLE_TIME_EDIT, MAX_FLOAT_LEN );
post_impact_n_ms( &p->nof_time_samples, &p->ms_per_time_sample, s );
/*
--------------------
Get the display time
-------------------- */
MachineList[CurrentMachineIndex].display_seconds.get_text( MonitorWindow, DISPLAY_TIME_EBOX );

/*
-------------------------
Get save count selections
------------------------- */
get_save_counts( PARM_GROUPBOX, &m->params );
get_save_counts( PROF_GROUPBOX, &m->profile );
lb.replace( machine_listbox_string(CurrentMachineIndex) );
}

/***********************************************************************
*                        UPDATE_MACHINE_LISTBOX                        *
***********************************************************************/
void update_machine_listbox( int i )
{
LISTBOX_CLASS   lb;
INT             x;

ProgramIsUpdating = TRUE;

lb.init( MonitorWindow, MONITOR_LISTBOX );
lb.redraw_off();

x = lb.current_index();

if ( lb.remove(i) )
    lb.insert( i, machine_listbox_string(i) );

lb.setcursel( x );
lb.redraw_on();

ProgramIsUpdating = FALSE;
}

/***********************************************************************
*                         CHECK_MONITOR_STATUS                         *
* See if monitoring is being turned on to replace another machine      *
* This needs to be done BEFORE saving any changes to the db so that    *
* all machines can be updated before the monitor status events are     *
* issued.                                                              *
***********************************************************************/
static void check_monitor_status( void )
{
MACHINE_CLASS * m;
short my_board_number;
short my_muxchan;
int   i;

if ( CurrentMachineIndex == NO_INDEX )
    return;

/*
-------------------------------------------------------
I only have to worry if this machine is to be monitored
------------------------------------------------------- */
i = CurrentMachineIndex;

if ( !(MachineList[i].machine.monitor_flags & MA_MONITORING_ON) )
    return;

my_board_number = MachineList[i].machine.ft_board_number;
my_muxchan      = MachineList[i].machine.muxchan;

for ( i=0; i<NofMachines; i++ )
    {
    if ( i == CurrentMachineIndex )
        continue;

    m = &MachineList[i].machine;
    if ( m->monitor_flags & MA_MONITORING_ON )
        {
        if ( m->ft_board_number == my_board_number && m->muxchan == my_muxchan )
            {
            /*
            -------------------
            Turn off monitoring
            ------------------- */
            m->monitor_flags &= ~MA_MONITORING_ON;
            MachineList[i].has_changed = TRUE;
            update_machine_listbox( i );
            }
        }
    }
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
void save_changes()
{
int             i;
DB_TABLE        t;
MACHINE_CLASS * m;
TCHAR           old_part_name[PART_NAME_LEN+1];
BIT_CLASS       current_part_has_changed( NofMachines );
WARMUP_CLASS    warmup;
MULTIPART_RUNLIST_CLASS mrl;

WORKER_LIST_CLASS worker;
STRING_CLASS file;
STRING_CLASS format;
STRING_CLASS s;
FILE_CLASS   f;
BOOLEAN have_monitor_log = FALSE;

file = get_ini_string( VisiTrakIniFile, ConfigSection, MonitorLogFileKey );
if ( file != unknown_string() )
    {
    format = get_ini_string( VisiTrakIniFile, ConfigSection, MonitorLogFormatKey );
    if ( format != unknown_string() )
        {
        worker.load();
        have_monitor_log = TRUE;
        }
    }

t.open( machset_dbname(ComputerName), MACHSET_RECLEN, PWL );
for ( i=0; i<NofMachines; i++ )
    {
    if ( MachineList[i].has_changed )
        {
        m = &MachineList[i].machine;
        t.reset_search_mode();
        t.put_alpha( MACHSET_MACHINE_NAME_OFFSET, m->name, MACHINE_NAME_LEN );
        if ( t.get_next_key_match(1, WITH_LOCK) )
            {
            t.get_alpha( old_part_name, MACHSET_CURRENT_PART_OFFSET,  PART_NAME_LEN );
            if ( !strings_are_equal(m->current_part, old_part_name) )
                {
                current_part_has_changed.set(i);
                t.put_alpha( MACHSET_CURRENT_PART_OFFSET,     m->current_part,          PART_NAME_LEN );
                }

            t.put_long( MACHSET_PROFILE_SKIP_COUNT_OFFSET,    m->profile.skip_count,    SKIP_COUNT_LEN );
            t.put_long( MACHSET_PROFILE_ACQUIRE_COUNT_OFFSET, m->profile.acquire_count, ACQUIRE_COUNT_LEN );
            t.put_long( MACHSET_PROFILE_MAX_SAVED_OFFSET,     m->profile.max_saved,     MAX_SAVED_LEN );
            t.put_long( MACHSET_PROFILE_TIME_SKIP_OFFSET,     m->profile.skip_seconds,  SKIP_TIME_LEN );
            t.put_long( MACHSET_PROFILE_FLAGS_OFFSET,         m->profile.flags,         BITSETYPE_LEN );

            t.put_long( MACHSET_PARAM_SKIP_COUNT_OFFSET,    m->params.skip_count,    SKIP_COUNT_LEN );
            t.put_long( MACHSET_PARAM_ACQUIRE_COUNT_OFFSET, m->params.acquire_count, ACQUIRE_COUNT_LEN );
            t.put_long( MACHSET_PARAM_MAX_SAVED_OFFSET,     m->params.max_saved,     MAX_SAVED_LEN );
            t.put_long( MACHSET_PARAM_TIME_SKIP_OFFSET,     m->params.skip_seconds,  SKIP_TIME_LEN );
            t.put_long( MACHSET_PARAM_FLAGS_OFFSET,         m->params.flags,         BITSETYPE_LEN );

            t.put_long( MACHSET_MONITOR_FLAGS_OFFSET, m->monitor_flags, BITSETYPE_LEN );

            t.rec_update();
            t.unlock_record();
            if ( have_monitor_log )
                {
                if ( log_file_string(s, format.text(), worker.full_name(m->opnumber), m->name, old_part_name, m->current_part) )
                    {
                    if ( f.open_for_append(file.text()) )
                        {
                        f.writeline( s.text() );
                        f.close();
                        }
                    }
                }
            }
        }
    }
t.close();

for ( i=0; i<NofMachines; i++ )
    {
    if ( MachineList[i].has_changed )
        {
        save_sample_seconds( i );
        if ( current_part_has_changed[i] )
            {
            m = &MachineList[i].machine;

            if ( boolean_from_ini(VisiTrakIniFile, ConfigSection, HaveMultipartRunlistKey) )
                {
                mrl.get( ComputerName, m->name, m->current_part );
                mrl.create_runlist( ComputerName,  m->name, m->current_part );
                }
            else
                {
                warmup.get( ComputerName, m->name, m->current_part );
                warmup.create_runlist( ComputerName,  m->name, m->current_part );
                }
            }
        }
    }

for ( i=0; i<NofMachines; i++ )
    {
    if ( MachineList[i].has_changed )
        {
        poke_data( DDE_MONITOR_TOPIC, ItemList[MONITOR_SETUP_INDEX].name, MachineList[i].machine.name );
        MachineList[i].has_changed = FALSE;
        }
    }
}

/***********************************************************************
*                            SET_TAB_STOPS                             *
***********************************************************************/
static void set_tab_stops( void )
{
int id[NOF_MONITOR_LIST_TABS] = {
    PART_STATIC, ALARM_STATIC, MONITOR_STATIC, POST_SECONDS_STATIC, DISPLAY_SECONDS_STATIC,
    PARAM_SKIP_STATIC, PARAM_ACQUIRE_STATIC, PARAM_MAX_STATIC, SHOT_SKIP_STATIC,
    SHOT_ACQUIRE_STATIC, SHOT_MAX_STATIC
    };

set_listbox_tabs_from_title_positions( MonitorWindow, MONITOR_LISTBOX, id, NOF_MONITOR_LIST_TABS );

//int tabs[NOF_MONITOR_LIST_TABS];
//int x;

//x = LOWORD( GetDialogBaseUnits() );
//x /= 2;

//tabs[0] = (MACHINE_NAME_LEN + 2) * x;            /* Part name       */
//tabs[1] = tabs[0] + ((PART_NAME_LEN + 3) * x);   /* Monitor alarm   */
//tabs[2] = tabs[1] + ( 2 * x);                    /* Monitor machine */
//tabs[3] = tabs[2] + ( 2 * x);                    /* Post impact sec */
//tabs[4] = tabs[3] + ( 8 * x);                    /* Skip            */
//tabs[5] = tabs[4] + (12 * x);                    /* Acquire         */
//tabs[6] = tabs[5] + ( 8 * x);                    /* Max             */
//tabs[7] = tabs[6] + ( 8 * x);                    /* Skip            */
//tabs[8] = tabs[7] + (12 * x);                    /* Acquire         */
//tabs[9] = tabs[8] + ( 8 * x);                    /* Max             */

//SendDlgItemMessage( MonitorWindow, MONITOR_LISTBOX, LB_SETTABSTOPS, (WPARAM) NOF_MONITOR_LIST_TABS, (LPARAM) tabs );
}

/***********************************************************************
*                            FILL_MACHINES                             *
***********************************************************************/
void fill_machines()
{
short           i;
DB_TABLE        t;
MACHINE_CLASS * m;
PART_CLASS    * p;
TCHAR           mn[MACHINE_NAME_LEN+1];
LISTBOX_CLASS   lb;
NAME_CLASS      s;
INI_CLASS       ini;

cleanup();
t.open( machset_dbname(ComputerName), MACHSET_RECLEN, PFL );
NofMachines = t.nof_recs();
if ( NofMachines > 0 )
    {
    MachineList = new MONITOR_ENTRY[NofMachines];
    if ( !MachineList )
        NofMachines = 0;
    }

i=0;
while ( t.get_next_record(NO_LOCK) )
    {
    if ( i >= NofMachines )
        break;

    m = &MachineList[i].machine;
    p = &MachineList[i].part;

    t.get_alpha( mn, MACHSET_MACHINE_NAME_OFFSET, MACHINE_NAME_LEN );

    if ( m->find(ComputerName, mn) )
        p->find( ComputerName, mn, m->current_part );
    s.get_part_display_ini_name( ComputerName, m->name, m->current_part );
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );

    if ( ini.find(DisplaySecondsKey) )
        MachineList[i].display_seconds = ini.get_string();
    else
        MachineList[i].display_seconds.null();

    MachineList[i].has_changed = FALSE;
    i++;
    }

t.close();

ProgramIsUpdating = TRUE;

NofMachines = i;
lb.init( MonitorWindow, MONITOR_LISTBOX );
lb.redraw_off();

for ( i=0; i<NofMachines; i++ )
    lb.add( machine_listbox_string(i) );

ProgramIsUpdating   = FALSE;

if ( NofMachines )
    {
    lb.setcursel( 0 );
    CurrentMachineIndex = 0;
    }

lb.redraw_on();
}

/***********************************************************************
*                           ADD_ONE_MACHINE                            *
***********************************************************************/
void add_one_machine( TCHAR * new_machine_name )
{
short           i;
short           n;
MONITOR_ENTRY   e;
MONITOR_ENTRY * ep;
MONITOR_ENTRY * ecopy;
LISTBOX_CLASS   lb;

lb.init( MonitorWindow, MONITOR_LISTBOX );

if ( e.machine.find(ComputerName, new_machine_name) )
    {
    if ( e.part.find(ComputerName, new_machine_name, e.machine.current_part) )
        {
        e.has_changed = FALSE;

        n = NofMachines + 1;

        ep = new MONITOR_ENTRY[n];
        if ( !ep )
            return;

        /*
        -------------------------
        Copy the existing entries
        ------------------------- */
        for ( i=0; i<NofMachines; i++ )
            ep[i] = MachineList[i];

        /*
        ---------------
        Add the new one
        --------------- */
        ep[NofMachines] = e;

        /*
        ---------------------------------------------------
        Copy the pointer to the old list so I can delete it
        --------------------------------------------------- */
        ecopy = MachineList;

        /*
        -------------------------------------
        Replace the old list with the new one
        ------------------------------------- */
        MachineList = ep;
        NofMachines = n;
        lb.add( machine_listbox_string(n-1) );

        /*
        ------------------
        Delete the old one
        ------------------ */
        delete[] ecopy;
        }
    }
}

/***********************************************************************
*                            ENABLE_ENTRIES                            *
***********************************************************************/
void enable_entries( int base, BOOLEAN enable )
 {
EnableWindow( GetDlgItem(MonitorWindow, base+ACQUIRE_COUNT), enable );
EnableWindow( GetDlgItem(MonitorWindow, base+SKIP),          enable );
}

/***********************************************************************
*                         SET_SKIP_COUNT_LABEL                         *
***********************************************************************/
void set_skip_count_label( int base )
{
UINT id;

if ( is_checked(MonitorWindow, base+TIME_SKIP) )
    id = SKIP_SECONDS_STRING;
else
    id = SKIP_COUNT_STRING;

set_text( MonitorWindow, base+SKIP_LABEL, resource_string(MainInstance, id) );
}

/***********************************************************************
*                          UPDATE_SKIP_VALUE                           *
***********************************************************************/
void update_skip_value( int base )
{
SAVE_COUNTS * sc;
UINT          count;

if ( CurrentMachineIndex == NO_INDEX )
    return;

if ( base == PARM_GROUPBOX )
    sc = &MachineList[CurrentMachineIndex].machine.params;
else
    sc = &MachineList[CurrentMachineIndex].machine.profile;

if ( is_checked(MonitorWindow, base+SKIP_COUNT) )
    count = sc->skip_count;
else
    count = sc->skip_seconds;

SetDlgItemInt( MonitorWindow, base+SKIP, count, FALSE );
}

/***********************************************************************
*                           PUT_VARS_IN_DLG                            *
***********************************************************************/
void put_vars_in_dlg( int base, SAVE_COUNTS * sc )
{
WPARAM  all_state;
WPARAM  none_state;
WPARAM  skip_state;
WPARAM  time_state;
BOOLEAN enable_state;
UINT    count;

all_state    = FALSE;
none_state   = FALSE;
skip_state   = FALSE;
time_state   = FALSE;
enable_state = FALSE;

if ( sc->flags & ACQUIRE_ALL_BIT )
    all_state = TRUE;

else if ( sc->flags & ACQUIRE_NONE_BIT )
    none_state = TRUE;

else
    {
    enable_state = TRUE;

    if ( sc->flags & SKIP_TIME_BIT )
        time_state = TRUE;

    if ( sc->flags & SKIP_COUNT_BIT )
        skip_state = TRUE;
    }

SendDlgItemMessage( MonitorWindow, base+SKIP_COUNT,   BM_SETCHECK, skip_state, 0L );
SendDlgItemMessage( MonitorWindow, base+ACQUIRE_ALL,  BM_SETCHECK, all_state,  0L );
SendDlgItemMessage( MonitorWindow, base+ACQUIRE_NONE, BM_SETCHECK, none_state, 0L );
SendDlgItemMessage( MonitorWindow, base+TIME_SKIP,    BM_SETCHECK, time_state, 0L );

enable_entries( base, enable_state );
set_skip_count_label( base );

if ( time_state )
    count = sc->skip_seconds;
else
    count = sc->skip_count;

SetDlgItemInt( MonitorWindow, base+SKIP, count, FALSE );

count = sc->acquire_count;
SetDlgItemInt( MonitorWindow, base+ACQUIRE_COUNT, count, FALSE );

count = sc->max_saved;
SetDlgItemInt( MonitorWindow, base+MAX_SAVED, count, FALSE );
}

/***********************************************************************
*                            LOAD_PARTLIST                             *
*          Load the part list combo box in the edit window.            *
***********************************************************************/
static void load_partlist()
{
DB_TABLE t;
TCHAR    partname[PART_NAME_LEN+1];
LISTBOX_CLASS lb;

if ( CurrentMachineIndex == NO_INDEX )
    return;

lb.init( MonitorWindow, PART_NAME_COMBOBOX );
lb.empty();

t.open( parts_dbname(ComputerName, MachineList[CurrentMachineIndex].machine.name), PARTS_RECLEN, PFL );
while ( t.get_next_record(NO_LOCK) )
    {
    t.get_alpha( partname, PARTS_PART_NAME_OFFSET, PART_NAME_LEN );
    lb.add( partname );
    }
t.close();
}

/***********************************************************************
*                     DISPLAY_POST_IMPACT_SECONDS                      *
***********************************************************************/
static void display_post_impact_seconds( PART_CLASS * p )
{
TCHAR s[POST_IMPACT_SECONDS_LEN+1];

post_impact_seconds( s, p->nof_time_samples, p->ms_per_time_sample );
*(s+POST_IMPACT_SECONDS_LEN) = NullChar;

set_text( MonitorWindow, SAMPLE_TIME_EDIT, s );
}

/***********************************************************************
*                             DISPLAY_CONFIG                           *
***********************************************************************/
static void display_config()
{
WPARAM  set;
MACHINE_CLASS * m;
PART_CLASS    * p;
LISTBOX_CLASS   lb;

if ( CurrentMachineIndex == NO_INDEX )
    return;

m = &MachineList[CurrentMachineIndex].machine;
p = &MachineList[CurrentMachineIndex].part;

ProgramIsUpdating = TRUE;

lb.init( MonitorWindow, PART_NAME_COMBOBOX );

lb.setcursel( m->current_part );

if ( m->monitor_flags & MA_ALARM_MONITORING_ON )
    set = TRUE;
else
    set = FALSE;
SendDlgItemMessage( MonitorWindow, MONITOR_ALARMS_CHECKBOX, BM_SETCHECK, set, 0L );

if ( m->monitor_flags & MA_MONITORING_ON )
    set = TRUE;
else
    set = FALSE;
SendDlgItemMessage( MonitorWindow, MONITOR_MACHINE_CHECKBOX, BM_SETCHECK, set, 0L );

display_post_impact_seconds( p );
MachineList[CurrentMachineIndex].display_seconds.set_text( MonitorWindow, DISPLAY_TIME_EBOX );

put_vars_in_dlg( PARM_GROUPBOX, &m->params  );
put_vars_in_dlg( PROF_GROUPBOX, &m->profile );

ProgramIsUpdating = FALSE;
}

/***********************************************************************
*                             GET_NEW_PART                             *
***********************************************************************/
void get_new_part()
{
INI_CLASS       ini;
LISTBOX_CLASS   lb;
MACHINE_CLASS * m;
PART_CLASS    * p;
NAME_CLASS      s;

if ( CurrentMachineIndex == NO_INDEX )
    return;

m = &MachineList[CurrentMachineIndex].machine;
p = &MachineList[CurrentMachineIndex].part;

/*
------------------------------------------------
Load the new part name into the global variables
------------------------------------------------ */
lb.init( MonitorWindow, PART_NAME_COMBOBOX );
s = lb.selected_text();
if ( !s.isempty() )
    {
    p->find( ComputerName, m->name, s.text() );
    copystring( m->current_part, s.text() );
    }

/*
--------------------------------------------------------
See if there is a value for the post impact display time
-------------------------------------------------------- */
s.get_part_display_ini_name( p->computer, p->machine, p->name );
ini.set_file( s.text() );
ini.set_section( ConfigSection );

if ( ini.find(DisplaySecondsKey) )
    MachineList[CurrentMachineIndex].display_seconds = ini.get_string();
else
    MachineList[CurrentMachineIndex].display_seconds.null();

ProgramIsUpdating = TRUE;
display_post_impact_seconds( p );
MachineList[CurrentMachineIndex].display_seconds.set_text( MonitorWindow, DISPLAY_TIME_EBOX );
ProgramIsUpdating = FALSE;
}

/***********************************************************************
*                             DO_NEWSETUP                              *
***********************************************************************/
static void do_newsetup()
{
int i;

for ( i=0; i<NofMachines; i++ )
    {
    if ( compare(MachineList[i].machine.name, NewSetupMachine, MACHINE_NAME_LEN) == 0 )
        return;
    }

add_one_machine( NewSetupMachine );
}

/***********************************************************************
*                          DO_MACHINE_SELECT                           *
***********************************************************************/
static BOOLEAN do_machine_select()
{
LISTBOX_CLASS lb;
int           i;
int32         slen;
TCHAR       * cp;

lb.init( MonitorWindow, MONITOR_LISTBOX );

lstrcpy( MachineLineBuf, lb.selected_text() );

for ( i=0; i<NofMachines; i++ )
    {
    cp = findchar( TabChar, MachineLineBuf );
    if ( cp )
        {
        slen = cp - MachineLineBuf;
        if ( slen == lstrlen(MachineList[i].machine.name) )
            {
            if ( compare(MachineList[i].machine.name, MachineLineBuf, slen) == 0 )
                {
                CurrentMachineIndex = i;
                load_partlist();
                display_config();
                return TRUE;
                }
            }
        }
    }

return FALSE;
}

/***********************************************************************
*                           POSITION_MYSELF                            *
***********************************************************************/
static void position_myself( HWND w )
{
RECT mr;
RECT dr;
long delta;

HWND desktop_window;

shrinkwrap( MainWindow, w );

GetWindowRect( MainWindow, &mr );
dr = mr;

/*
------------------------------------
Get the bounds of the desktop window
------------------------------------ */
desktop_window = GetDesktopWindow();
if ( desktop_window )
    {
    GetClientRect( desktop_window, &dr );
    }

delta = (dr.right - dr.left) - (mr.right - mr.left);
delta /= 2;

mr.right += delta;
mr.left  += delta;

delta = (dr.bottom - dr.top) - (mr.bottom - mr.top);
delta /= 2;

mr.bottom += delta;
mr.top   += delta;

MoveWindow( MainWindow, mr.left, mr.top, mr.right-mr.left, mr.bottom-mr.top, TRUE );

if ( monitor_is_running() )
    EnableWindow( GetDlgItem(w,BEGIN_MONITORING_BUTTON), FALSE );
else
    EnableWindow( GetDlgItem(w,BEGIN_MONITORING_BUTTON), TRUE );
}

/***********************************************************************
*                         START_BOARD_MONITOR                          *
***********************************************************************/
static void start_board_monitor()
{
static STARTUPINFO info = {
    sizeof(STARTUPINFO),
    0, 0, 0,            // lpReserved, lpDesktop, lpTitle
    0,                  // dwX
    0,                  // dwY
    0, 0, 0, 0, 0,      // dwXSize, dwYSize, dwXCountChars, dwYCountChars, dwFillAttribute
    0,                  // dwFlags
    0, 0, 0, 0, 0, 0    // wShowWindow, cbReserved2, lpReserved2, hStdInput, hStdOutput, hStdError
    };

STRING_CLASS path;
PROCESS_INFORMATION pi;

path = exe_directory();
path.cat_path( MonallExeFileName );

CreateProcess( 0,          // lpApplicationName
    path.text(),           // lpCommandLine
    0, 0, 0,               // lpProcessAttributes, lpThreadAttributes, bInheritHandles,
    HIGH_PRIORITY_CLASS,   // dwCreationFlags
    0, 0,                  // lpEnvironment, lpCurrentDirectory,
    &info,                 // lpStartupInfo = input
    &pi                    // lpProcessInformation = output
    );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup()
{
if ( !ShotsString )
    ShotsString = maketext( resource_string( MainInstance, SHOTS_STRING) );

if ( !SecString )
    SecString = maketext( resource_string( MainInstance, SEC_STRING) );
}

/***********************************************************************
*                             MonitorProc                              *
***********************************************************************/
BOOL CALLBACK MonitorProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int     cmd;
int     id;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        startup();
        position_myself( hWnd );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_DBINIT:
        set_tab_stops();
        fill_machines();
        load_partlist();
        display_config();
        return TRUE;

    case WM_NEWSETUP:
        do_newsetup();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case MONITOR_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    if ( !do_machine_select() )
                        resource_message_box( MainInstance, CANNOT_FIND_MACHINE_STRING, DONOT_CONTINUE_STRING );
                    return TRUE;
                    }
                break;

            case IDCANCEL:
                SendMessage( MainWindow, WM_CLOSE, 0, 0L );
                break;

            case PART_NAME_COMBOBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    get_new_part();
                    refresh_machine_listbox();
                    return TRUE;
                    }
                break;

            case PARM_ACQUIRE_ALL:
            case PARM_ACQUIRE_NONE:
                enable_entries (PARM_GROUPBOX, FALSE);
                refresh_machine_listbox();
                return TRUE;

            case PARM_TIME_SKIP:
            case PARM_SKIP_COUNT:
                enable_entries (PARM_GROUPBOX, TRUE);
                set_skip_count_label( PARM_GROUPBOX );
                update_skip_value( PARM_GROUPBOX );
                refresh_machine_listbox();
                return TRUE;

            case PROF_ACQUIRE_ALL:
            case PROF_ACQUIRE_NONE:
                enable_entries (PROF_GROUPBOX, FALSE );
                refresh_machine_listbox();
                return TRUE;

            case PROF_TIME_SKIP:
            case PROF_SKIP_COUNT:
                enable_entries (PROF_GROUPBOX, TRUE);
                set_skip_count_label( PROF_GROUPBOX );
                update_skip_value( PROF_GROUPBOX );
                refresh_machine_listbox();
                return TRUE;

            case MONITOR_MACHINE_CHECKBOX:
                refresh_machine_listbox();
                check_monitor_status();
                return TRUE;

            case MONITOR_ALARMS_CHECKBOX:
            case SAMPLE_TIME_EDIT:
            case DISPLAY_TIME_EBOX:
            case PARM_ACQUIRE_COUNT:
            case PARM_SKIP:
            case PARM_MAX_SAVED:
            case PROF_ACQUIRE_COUNT:
            case PROF_SKIP:
            case PROF_MAX_SAVED:
                refresh_machine_listbox();
                return TRUE;

            case SAVE_CHANGES_BUTTON:
                save_changes();
                if ( !IsWindowEnabled( GetDlgItem(hWnd,BEGIN_MONITORING_BUTTON) ) )
                    SendMessage( MainWindow, WM_CLOSE, 0, 0L );
                return TRUE;

            case BEGIN_MONITORING_BUTTON:
                start_board_monitor();
                SendMessage( MainWindow, WM_CLOSE, 0, 0L );
                return TRUE;
            }
        break;
    }

return FALSE;
}
