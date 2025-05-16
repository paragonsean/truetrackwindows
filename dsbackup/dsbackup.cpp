#include <windows.h>
#include <htmlhelp.h>
#include <commctrl.h>

#define _MAIN_

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\fileclas.h"
#include "..\include\genlist.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\names.h"
#include "..\include\rectclas.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\wclass.h"

#include "resource.h"

#include "backupcl.h"
#include "extern.h"

const int32 MEG_FOR_FULL_DISK = 50;

/*
-------------------------------------------------------------------
Id offsets from the ACQUIRE_ALL radio button for Params or Profiles
------------------------------------------------------------------- */
const UINT ACQUIRE_ALL_OFFSET      = 0;
const UINT ACQUIRE_NONE_OFFSET     = 1;
const UINT SKIP_TIME_OFFSET        = 2;
const UINT SKIP_COUNT_OFFSET       = 3;
const UINT ACQUIRE_EBOX_OFFSET     = 4;
const UINT SKIP_EBOX_OFFSET        = 5;
const UINT ACQUIRE_LABEL_OFFSET    = 6;
const UINT SKIP_LABEL_OFFSET       = 7;

static HWND SaveOptionsWindow = 0;

HANDLE  MailSlotEvent;
BOOLEAN MailSlotIsRunning;
BOOLEAN NeedToZipAll    = FALSE;
BOOLEAN UpdatingListbox = FALSE;
BOOLEAN UpdatingBoxes   = FALSE;
BOOLEAN ShuttingDown;

TCHAR MyClassName[]   = "DataServer";
TCHAR MyWindowTitle[] = "Data Archiver";

static DS_BACKUP_CLASS SaveConfig;

BOOLEAN       DiskIsFull      = FALSE;
BOOLEAN       BackupAtTime    = FALSE;
static BOOLEAN NeedToReloadSaveConfig = FALSE;
const int32 MAX_BACKUP_TIME_COUNT = 3;
TIME_CLASS BackupTime[MAX_BACKUP_TIME_COUNT];
TCHAR *    BackupTimeIniName[MAX_BACKUP_TIME_COUNT] = {
           { TEXT( "BackupTime1" ) },
           { TEXT( "BackupTime2" ) },
           { TEXT( "BackupTime3" ) }
           };

int32      FreeDiskMegabytes = 0;
int32      NofBackupTimes = 0;

static const DWORD MS_TO_WAIT     = 505;
static const TCHAR MailSlotName[] = "\\\\.\\mailslot\\v5\\eventman";
static const TCHAR BackslashChar  = TEXT( '\\' );
static const TCHAR ColonChar      = TEXT( ':' );
static const TCHAR TabChar        = TEXT( '\t' );
static const TCHAR NullChar       = TEXT( '\0' );

static const TCHAR CrString[] = TEXT( "\r\n" );
static TCHAR YString[]     = TEXT( "Y" );
static TCHAR NString[]     = TEXT( "N" );
static TCHAR EmptyString[] = TEXT( "" );
static TCHAR SpaceString[] = TEXT( " " );

static TCHAR BackupDirKey[]    = TEXT( "BackupDir" );
static TCHAR BackupSection[]   = TEXT( "Backup" );
static TCHAR DsBackupIniFile[] = TEXT( "visitrak.ini" );
static TCHAR DefaultHaveAlarmShot[]   = TEXT( "A" );
static TCHAR DefaultHaveGoodShot[]    = TEXT( "G" );
static TCHAR DefaultHaveWarningShot[] = TEXT( "W" );

TCHAR * resource_string( UINT resource_id );
void resource_message_box( UINT msg_id, UINT title_id );
BOOL CALLBACK global_file_settings_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

static STRING_CLASS Ls;

/***********************************************************************
*                                  log                                 *
*    This writes whatever you put in the global Ls STRING_CLASS.       *
***********************************************************************/
static void log()
{
FILE_CLASS f;
TIME_CLASS   t;
STRING_CLASS s;

if ( !UsingLogFile )
    return;

t.get_local_time();
s = t.text();
s += SpaceString;
s += Ls;

if ( f.open_for_append( LogFilePath.text() ) )
    {
    f.writeline( s.text() );
    f.close();
    }
}

/***********************************************************************
*                                  log                                 *
***********************************************************************/
void log( TCHAR * sorc )
{
FILE_CLASS f;
TIME_CLASS   t;
STRING_CLASS s;

if ( !UsingLogFile )
    return;

t.get_local_time();
s = t.text();
s += SpaceString;
s += sorc;

if ( f.open_for_append( LogFilePath.text() ) )
    {
    f.writeline( s.text() );
    f.close();
    }
}

/***********************************************************************
*                                  log                                 *
***********************************************************************/
void log( STRING_CLASS & sorc )
{
FILE_CLASS f;
TIME_CLASS   t;
STRING_CLASS s;

if ( !UsingLogFile )
    return;

t.get_local_time();
s = t.text();
s += SpaceString;
s += sorc;

if ( f.open_for_append( LogFilePath.text() ) )
    {
    f.writeline( s.text() );
    f.close();
    }
}

/***********************************************************************
*                               LB_LINE                                *
*                                                                      *
*                        Parameters       Profiles                     *
*                Machine Skip      Acquire     Skip       Acquire      *
*                M01       10 Sec.    1         10 Shots     1         *
*                                                                      *
***********************************************************************/
static TCHAR * lb_line( DS_BACKUP_ENTRY * sorc )
{
const int32 MAX_LABEL_LEN = 20;
const int32 SLEN = MACHINE_NAME_LEN + (4*MAX_INTEGER_LEN) + (2 * MAX_LABEL_LEN) + 4;
static TCHAR buf[SLEN+1];
TCHAR * cp;

cp = copy_w_char( buf,  sorc->name.machine, TabChar );

cp = copy_w_char( cp, sorc->params.text(), TabChar );
copystring( cp, sorc->profile.text() );

return buf;
}

/***********************************************************************
*                          SAVE_SAVE_OPTIONS                           *
***********************************************************************/
static void save_save_options( void )
{
SaveConfig.rewind();

while ( SaveConfig.next() )
    SaveConfig.current_entry->write();

NeedToReloadSaveConfig = TRUE;
}

/***********************************************************************
*                       SET_ACQUIRE_PUSHBUTTONS                        *
***********************************************************************/
static void set_acquire_pushbuttons( UINT first_id, BITSETYPE b )
{
UINT x;

x = first_id;

if ( b & ACQUIRE_NONE_BIT )
    x += ACQUIRE_NONE_OFFSET;
else if ( b & SKIP_TIME_BIT )
    x += SKIP_TIME_OFFSET;
else if ( b & SKIP_COUNT_BIT )
    x += SKIP_COUNT_OFFSET;

CheckRadioButton( SaveOptionsWindow, first_id, first_id+3, x );
}

/***********************************************************************
*                             SET_INT_EBOX                             *
***********************************************************************/
void set_int_ebox( UINT id, int32 value )
{
set_text( SaveOptionsWindow, id, int32toasc(value) );
}

/***********************************************************************
*                           UPDATE_COUNT_BOXES                         *
*           base is the id of the ACQUIRE_ALL radio button             *
***********************************************************************/
static void update_count_boxes( DS_SAVE_COUNTS & sc, UINT base )
{
const BITSETYPE ALL_OR_NONE_MASK   = ACQUIRE_ALL_BIT | ACQUIRE_NONE_BIT;

UINT  id;
int32 n;
BOOL  isenabled;

set_int_ebox( base+ACQUIRE_EBOX_OFFSET, sc.acquire_count );

if ( sc.flags & SKIP_TIME_BIT )
    {
    n = sc.skip_seconds;
    id = SKIP_TIME_STRING;
    }
else
    {
    n   = sc.skip_count;
    id = SKIP_COUNT_STRING;
    }

set_int_ebox( base+SKIP_EBOX_OFFSET, n );

set_text( SaveOptionsWindow, base+SKIP_LABEL_OFFSET, resource_string(id) );

/*
-------------------------------------
Gray the boxes if this is all or none
------------------------------------- */
if ( sc.flags & ALL_OR_NONE_MASK )
    isenabled = FALSE;
else
    isenabled = TRUE;

for ( id= base+ACQUIRE_EBOX_OFFSET; id<base+SKIP_LABEL_OFFSET; id++ )
    EnableWindow( GetDlgItem(SaveOptionsWindow, id), isenabled );
}

/***********************************************************************
*                             UPDATE_BOXES                             *
***********************************************************************/
static void update_boxes(  DS_BACKUP_ENTRY * b )
{
UpdatingBoxes = TRUE;

update_count_boxes( b->params,  PARM_ACQUIRE_ALL );
update_count_boxes( b->profile, PROF_ACQUIRE_ALL );

set_acquire_pushbuttons( PARM_ACQUIRE_ALL, b->params.flags );
set_acquire_pushbuttons( PROF_ACQUIRE_ALL, b->profile.flags );

UpdatingBoxes = FALSE;
}

/***********************************************************************
*                          LOAD_SAVE_OPTIONS                           *
*                                                                      *
*                        Parameters       Profiles                     *
*                Machine Skip      Acquire     Skip       Acquire      *
*                M01       10 Sec.    1         10 Shots     1         *
*                                                                      *
***********************************************************************/
static void load_save_options( void )
{
LISTBOX_CLASS lb;

lb.init( SaveOptionsWindow, MACHINE_LISTBOX );

SaveConfig.load();
SaveConfig.rewind();

UpdatingListbox = TRUE;

while ( SaveConfig.next() )
    lb.add( lb_line(SaveConfig.current_entry) );

SaveConfig.rewind();
SaveConfig.next();

if ( lb.count() > 0 )
    lb.setcursel( 0 );

UpdatingListbox = FALSE;

if ( SaveConfig.current_entry )
    update_boxes( SaveConfig.current_entry );
}

/***********************************************************************
*                          DO_NEW_LISTBOX_SELECT                       *
***********************************************************************/
static void do_new_listbox_select( void )
{
LISTBOX_CLASS lb;
TCHAR * s;

/*
--------------------------------------------------------
Do nothing if the listbox is being updated by user input
-------------------------------------------------------- */
if ( UpdatingListbox )
    return;

lb.init( SaveOptionsWindow, MACHINE_LISTBOX );
s = maketext( lb.selected_text() );
if ( s )
    {
    if ( replace_tab_with_null(s) )
        {
        if ( SaveConfig.find(s) )
            update_boxes( SaveConfig.current_entry );
        }
    delete[] s;
    }
}

/***********************************************************************
*                                                                      *
*                          DO_RADIO_BUTTON_PRESS                       *
*                                                                      *
***********************************************************************/
static void do_radio_button_press( DS_SAVE_COUNTS & sc, UINT base )
{
const BITSETYPE CLEAR_MY_BITS_MASK = ~(SKIP_COUNT_BIT | SKIP_TIME_BIT | ACQUIRE_ALL_BIT | ACQUIRE_NONE_BIT);

/*
---------------------
Clear all of the bits
--------------------- */
sc.flags &= CLEAR_MY_BITS_MASK;

/*
--------------------------------------
Set the bit based on the radio buttons
-------------------------------------- */
if ( is_checked(SaveOptionsWindow, base+ACQUIRE_ALL_OFFSET ) )
    sc.flags |= ACQUIRE_ALL_BIT;
else if ( is_checked(SaveOptionsWindow, base+ACQUIRE_NONE_OFFSET ) )
    sc.flags |= ACQUIRE_NONE_BIT;
else if ( is_checked(SaveOptionsWindow, base+SKIP_COUNT_OFFSET ) )
    sc.flags |= SKIP_COUNT_BIT;
else if ( is_checked(SaveOptionsWindow, base+SKIP_TIME_OFFSET ) )
    sc.flags |= SKIP_TIME_BIT;

UpdatingBoxes = TRUE;
update_count_boxes( sc, base );
UpdatingBoxes = FALSE;
}

/***********************************************************************
*                                                                      *
*                          COPY_COUNT_CHANGES                          *
*                                                                      *
* Copy the changes made on the screen to the DS_SAVE_COUNTS struct     *
*                                                                      *
***********************************************************************/
static void copy_count_changes( DS_SAVE_COUNTS & sc, UINT base )
{
const BITSETYPE SKIP_MASK          = SKIP_COUNT_BIT | SKIP_TIME_BIT;
int32 n;

/*
----------------------------------------------------------
If this is a skip config, save the acquire and skip counts
---------------------------------------------------------- */
if ( sc.flags & SKIP_MASK )
    {
    sc.acquire_count = GetDlgItemInt( SaveOptionsWindow, base+ACQUIRE_EBOX_OFFSET, 0, FALSE );
    n                = GetDlgItemInt( SaveOptionsWindow, base+SKIP_EBOX_OFFSET,    0, FALSE );
    if ( sc.flags & SKIP_COUNT_BIT )
        sc.skip_count   = n;
    else
        sc.skip_seconds = n;
    }
}

/***********************************************************************
*                            SET_TAB_STOPS                             *
***********************************************************************/
static void set_tab_stops( void )
{
const WPARAM NOF_TABS = 4;
int tabs[NOF_TABS];
int x;

x = LOWORD( GetDialogBaseUnits() );
x /= 2;

tabs[0] = (MACHINE_NAME_LEN + 9) * x;
tabs[1] = tabs[0] + ( 10 * x);
tabs[2] = tabs[1] + ( 18 * x);
tabs[3] = tabs[2] + ( 10 * x);

SendDlgItemMessage( SaveOptionsWindow, MACHINE_LISTBOX, LB_SETTABSTOPS, NOF_TABS, (LPARAM) tabs );
}

/***********************************************************************
*                             UPDATE_LISTBOX                           *
***********************************************************************/
void update_listbox( void )
{
LISTBOX_CLASS lb;

if ( !SaveConfig.current_entry )
    return;

lb.init( SaveOptionsWindow, MACHINE_LISTBOX );
UpdatingListbox = TRUE;
lb.replace( lb_line(SaveConfig.current_entry) );
UpdatingListbox = FALSE;
}

/***********************************************************************
*                             DO_PARM_EBOXES                           *
***********************************************************************/
void do_parm_eboxes( void )
{
if ( UpdatingBoxes )
    return;

if ( !SaveConfig.current_entry )
    return;

copy_count_changes( SaveConfig.current_entry->params, PARM_ACQUIRE_ALL );
update_listbox();
}

/***********************************************************************
*                             DO_PROF_EBOXES                           *
***********************************************************************/
void do_prof_eboxes( void )
{
if ( UpdatingBoxes )
    return;

if ( !SaveConfig.current_entry )
    return;

copy_count_changes( SaveConfig.current_entry->profile, PROF_ACQUIRE_ALL );
update_listbox();
}

/***********************************************************************
*                          DO_PARM_RADIO_BUTTONS                       *
***********************************************************************/
void do_parm_radio_buttons( void )
{
if ( UpdatingBoxes )
    return;

if ( !SaveConfig.current_entry )
    return;

do_radio_button_press( SaveConfig.current_entry->params, PARM_ACQUIRE_ALL );
update_listbox();
}

/***********************************************************************
*                          DO_PROF_RADIO_BUTTONS                       *
***********************************************************************/
void do_prof_radio_buttons( void )
{
if ( UpdatingBoxes )
    return;

if ( !SaveConfig.current_entry )
    return;

do_radio_button_press( SaveConfig.current_entry->profile, PROF_ACQUIRE_ALL );
update_listbox();
}

/***********************************************************************
*                       SAVE_OPTIONS_DIALOG_PROC                       *
***********************************************************************/
BOOL CALLBACK save_options_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
LISTBOX_CLASS lb;
int           id;
int           cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        SaveOptionsWindow = hWnd;
        set_tab_stops();
        load_save_options();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case PROF_ACQUIRE_ALL:
            case PROF_ACQUIRE_NONE:
            case PROF_TIME_SKIP:
            case PROF_SKIP_COUNT:
                do_prof_radio_buttons();
                return TRUE;

            case PROF_ACQUIRE_EBOX:
            case PROF_SKIP_EBOX:
                if ( cmd == EN_CHANGE )
                    do_prof_eboxes();
                return TRUE;


            case PARM_ACQUIRE_ALL:
            case PARM_ACQUIRE_NONE:
            case PARM_TIME_SKIP:
            case PARM_SKIP_COUNT:
                do_parm_radio_buttons();
                return TRUE;

            case PARM_ACQUIRE_EBOX:
            case PARM_SKIP_EBOX:
                if ( cmd == EN_CHANGE )
                    do_parm_eboxes();
                return TRUE;

            case MACHINE_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    do_new_listbox_select();
                return TRUE;

            case SAVE_CHANGES_BUTTON:
                save_save_options();
                SaveOptionsWindow = 0;
                EndDialog( hWnd, 0 );
                return TRUE;

            case IDCANCEL:
                SaveOptionsWindow = 0;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        if ( SaveOptionsWindow )
            {
            SaveOptionsWindow = 0;
            EndDialog( hWnd, 0 );
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                          READ_NEXT_MESSAGE                           *
***********************************************************************/
static BOOLEAN read_next_message( HANDLE mailslot_handle, DWORD slen )
{
TCHAR * s;
DWORD   bytes_read;

if ( slen > 0 )
    {
    s = maketext( slen );
    if ( s )
        {
        if( ReadFile(mailslot_handle, s, slen, &bytes_read, NULL) )
            {
            *(s+bytes_read) = NullChar;
            if ( !DiskIsFull )
                BList.process_mailslot_message( s );
            }
        delete[] s;
        }
    }

return TRUE;
}

/***********************************************************************
*                           MAILSLOT_THREAD                            *
***********************************************************************/
DWORD mailslot_thread( int * notused )
{
BOOL   status;
int32  i;
HANDLE mailslot_handle;
DWORD  next_msg_size;
DWORD  nof_msg;
unsigned long error_number;
TIME_CLASS tc;

mailslot_handle = CreateMailslot(
        MailSlotName,
        0,                             // no maximum message size
        MAILSLOT_WAIT_FOREVER,         // no time-out for operations
        (LPSECURITY_ATTRIBUTES) NULL); // no security attributes

if ( mailslot_handle == INVALID_HANDLE_VALUE)
    {
    resource_message_box( MAILSLOT_STRING, CREATE_FAILED_STRING );
    return 0;
    }

MailSlotIsRunning = TRUE;

while ( TRUE )
    {
    if ( ShuttingDown )
        {
        Ls = TEXT( "Mailslot Thread: Shutdown is TRUE" );
        log();
        break;
        }

    Ls = EmptyString;

    if ( NeedToReloadSaveConfig )
        {
        Ls += "BList.reload_save_configs() ";
        NeedToReloadSaveConfig = FALSE;
        BList.reload_save_configs();
        }

    if ( NeedToReloadZipPaths )
        {
        Ls += "BList.reload_zip_paths() ";
        NeedToReloadZipPaths = FALSE;
        BList.reload_zip_paths();
        }

    if ( BackupAtTime )
        {
        tc.get_local_time();
        for ( i=0; i<NofBackupTimes; i++ )
            {
            if ( tc > BackupTime[i] )
                {
                Ls += "BackupAtTime: NeedToZipAll = TRUE ";
                NeedToZipAll = TRUE;
                break;
                }
            }

        for ( i=0; i<NofBackupTimes; i++ )
            {
            while ( tc > BackupTime[i] )
                {
                BackupTime[i] += SEC_PER_DAY;
                }
            }
        }

    if ( BackupDirectory )
        {
        i = free_kilobytes( BackupDirectory );
        i /= 1000;
        if ( FreeDiskMegabytes != i )
            {
            FreeDiskMegabytes = i;
            set_text( MainDialogWindow, K_FREE_TBOX, int32toasc(i) );
            if ( i <= MEG_FOR_FULL_DISK && !DiskIsFull )
                {
                DiskIsFull = TRUE;
                ShowWindow( GetDlgItem(MainDialogWindow, DISK_FULL_TBOX), SW_SHOW );
                }

            if ( i > MEG_FOR_FULL_DISK && DiskIsFull )
                {
                DiskIsFull = FALSE;
                ShowWindow( GetDlgItem(MainDialogWindow, DISK_FULL_TBOX), SW_HIDE );
                }
            }
        }

    if ( !Ls.isempty() )
        {
        log();
        Ls = EmptyString;
        }

    if ( NeedToZipAll )
        {
        NeedToZipAll = FALSE;
        BList.zip_all_parts();
        }

    status = GetMailslotInfo(
        mailslot_handle,
        (LPDWORD) NULL,               // no maximum message size
        &next_msg_size,               // size of next message
        &nof_msg,                     // number of messages
        (LPDWORD) NULL);              // no read time-out

    if ( status )
        {
        if ( next_msg_size != MAILSLOT_NO_MESSAGE )
            read_next_message( mailslot_handle, next_msg_size );
        }
    else
        {
        Ls = "Error from GetMailslotInfo = $";
        error_number = (unsigned long) GetLastError();
        Ls += ultohex( error_number );
        log();
        }

    if ( WaitForSingleObject(MailSlotEvent, MS_TO_WAIT) == WAIT_FAILED )
        resource_message_box( UNABLE_TO_CONTINUE_STRING, MAILSLOT_WAIT_FAIL_STRING );
    }

CloseHandle( mailslot_handle );

MailSlotIsRunning = FALSE;
        Ls = TEXT( "mailslot_thread: sending WM_CLOSE to MainWindow" );
        log();

PostMessage( MainWindow, WM_CLOSE, 0, 0L );
return 0;
}

/***********************************************************************
*                         UPDATE_ZIP_TIME                              *
***********************************************************************/
static void update_zip_time( void )
{
TCHAR      buf[ALPHATIME_LEN+1];
TIME_CLASS t;
UINT       id;
int32      i;

NofBackupTimes = 0;
Ls = "Update_zip_time ";

for ( id = TIME_1_EBOX; id <= TIME_3_EBOX; id++ )
    {
    if ( get_text(buf, MainDialogWindow, id, ALPHATIME_LEN) )
        {
        Ls += "buf=[";
        Ls += buf;
        Ls += "] ";
        log();
        if ( is_time(buf) )
            {
            t.get_local_time();
            BackupTime[NofBackupTimes] = t;
            BackupTime[NofBackupTimes].set_time( buf );
            if ( t > BackupTime[NofBackupTimes] )
                BackupTime[NofBackupTimes] += SEC_PER_DAY;
            put_ini_string( DsBackupIniFile, ConfigSection, BackupTimeIniName[NofBackupTimes], buf );
            NofBackupTimes++;
            }
        }
    }

for ( i=NofBackupTimes; i<MAX_BACKUP_TIME_COUNT; i++ )
    put_ini_string( DsBackupIniFile, ConfigSection, BackupTimeIniName[NofBackupTimes], UNKNOWN );
}

/***********************************************************************
*                         UPDATE_MAX_ZIP_COUNT                         *
***********************************************************************/
static void update_max_zip_count( void )
{
TCHAR buf[MAX_INTEGER_LEN+1];

if ( get_text(buf, MainDialogWindow, ZIP_COUNT_EBOX, MAX_INTEGER_LEN) )
    {
    if ( is_checked( MainDialogWindow, ZIP_COUNT_XBOX) )
        BList.set_max_zip_count( asctoint32(buf) );
    else
        BList.clear_max_zip_count();

    put_ini_string( DsBackupIniFile, ConfigSection, "MaxZipCount", buf );
    }
}

/***********************************************************************
*                           INIT_DIALOG_CONTROLS                       *
***********************************************************************/
static void init_dialog_controls( HWND w )
{
static TCHAR zero[] = TEXT( "0" );

int32   i;
TCHAR * cp;
BOOLEAN need_check;
TIME_CLASS t;

Using_Machine_Based_Zip_Paths = boolean_from_ini( IniFileName, DefaultSection, MachineBasedZipPathsKey );

HaveMirror = boolean_from_ini( IniFileName, ConfigSection, HaveMirrorKey );
if ( HaveMirror )
    {
    MirrorRoot = get_ini_string( IniFileName, ConfigSection, MirrorRootKey );
    if ( MirrorRoot.len() < 2 )
        {
        HaveMirror = FALSE;
        }
    else
        {
        cp = MirrorRoot.text();
        cp++;
        if ( *cp != ColonChar && *cp != BackslashChar )
            {
            HaveMirror = FALSE;
            }
        }

    if ( !HaveMirror )
        MessageBox( w, "Not a Root Directory", "Error in Mirror Drive", MB_OK );
    }

DefaultMachineZipPath = get_ini_string( IniFileName, DefaultSection, BackupMachinePathString );
DefaultPartZipPath    = get_ini_string( IniFileName, DefaultSection, BackupPartPathString    );

UsingLogFile = boolean_from_ini( DsBackupIniFile, DsBackupSection, UseLogFileKey );
set_checkbox( w, USE_LOG_FILE_XBOX, UsingLogFile );

if ( UsingLogFile )
    log( "******************* Startup **********************" );


/*
----------------------
Max zip count controls
---------------------- */
need_check = boolean_from_ini( DsBackupIniFile, ConfigSection, TEXT("UseMaxZipCount") );
set_checkbox( w, ZIP_COUNT_XBOX, need_check );

SkipDowntimeZip = boolean_from_ini( DsBackupIniFile, ConfigSection, TEXT("SkipDowntimeZip") );
set_checkbox( w, IGNORE_DOWNTIME_XBOX, SkipDowntimeZip );

cp = get_ini_string( DsBackupIniFile, ConfigSection, "MaxZipCount" );
if ( unknown(cp) )
    cp = zero;

set_text( w, ZIP_COUNT_EBOX, cp );

if ( need_check )
    BList.set_max_zip_count( asctoint32(cp) );

/*
-------------------------
Backup at a time controls
------------------------- */
BackupAtTime = boolean_from_ini( DsBackupIniFile, ConfigSection, TEXT("UseBackupTime") );

NofBackupTimes = 0;
for ( i=0; i<MAX_BACKUP_TIME_COUNT; i++ )
    {
    cp = get_ini_string( DsBackupIniFile, ConfigSection, BackupTimeIniName[i] );
    if ( unknown(cp) )
        break;

    t.get_local_time();
    BackupTime[NofBackupTimes] = t;
    BackupTime[NofBackupTimes].set_time( cp );
    if ( t > BackupTime[NofBackupTimes] )
        BackupTime[NofBackupTimes] += SEC_PER_DAY;
    set_text( w, TIME_1_EBOX+NofBackupTimes, cp );
    NofBackupTimes++;
    }

/*
----------------------------------
Blank out the boxes I am not using
---------------------------------- */
i = NofBackupTimes;
while ( i < MAX_BACKUP_TIME_COUNT )
    {
    set_text( w, TIME_1_EBOX+i, EmptyString );
    i++;
    }

set_checkbox( w, ZIP_TIME_XBOX, BackupAtTime );
}

/***********************************************************************
*                            SET_TAB_STOPS                             *
***********************************************************************/
static void set_tab_stops( HWND w )
{
int tabs[3];
int x;
x = LOWORD( GetDialogBaseUnits() );
x /= 2;

tabs[0] = (MACHINE_NAME_LEN + 4) * x;            /* Part name       */
tabs[1] = tabs[0] + ((PART_NAME_LEN + 4) * x);   /* Count           */
tabs[2] = tabs[1] + ( 6 * x);                    /* Last shot date  */

SendDlgItemMessage( w, ZIP_COUNT_LISTBOX, LB_SETTABSTOPS, (WPARAM) 3, (LPARAM) tabs );
}

/***********************************************************************
*                            MAIN_DIALOG_PROC                          *
***********************************************************************/
BOOL CALLBACK main_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static BOOLEAN HaveNewZipCount = FALSE;
static BOOLEAN HaveNewZipTime  = FALSE;

TCHAR * cp;
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        init_dialog_controls( hWnd );
        set_tab_stops( hWnd );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                if ( HaveNewZipCount )
                    {
                    update_max_zip_count();
                    HaveNewZipCount = FALSE;
                    }

                if ( HaveNewZipTime )
                    {
                    update_zip_time();
                    HaveNewZipTime = FALSE;
                    }
                return TRUE;

            case USE_LOG_FILE_XBOX:
                if ( is_checked( hWnd, USE_LOG_FILE_XBOX) )
                    UsingLogFile = TRUE;
                else
                    UsingLogFile = FALSE;
                boolean_to_ini(DsBackupIniFile, DsBackupSection, UseLogFileKey, UsingLogFile );
                return TRUE;

            case ZIP_TIME_XBOX:
                if ( is_checked( MainDialogWindow, ZIP_TIME_XBOX) )
                    {
                    update_zip_time();
                    BackupAtTime = TRUE;
                    cp = YString;
                    }
                else
                    {
                    BackupAtTime  = FALSE;
                    cp = NString;
                    }

                put_ini_string( DsBackupIniFile, ConfigSection, "UseBackupTime", cp );
                return TRUE;


            case TIME_1_EBOX:
            case TIME_2_EBOX:
            case TIME_3_EBOX:
                if ( cmd == EN_CHANGE )
                    {
                    HaveNewZipTime = TRUE;
                    return TRUE;
                    }

                if ( cmd == EN_KILLFOCUS )
                    {
                    if ( HaveNewZipTime )
                        {
                        update_zip_time();
                        HaveNewZipTime = FALSE;
                        }
                    }

                return TRUE;

            case ZIP_COUNT_XBOX:
                update_max_zip_count();
                HaveNewZipCount = FALSE;
                if ( is_checked( MainDialogWindow, ZIP_COUNT_XBOX) )
                    cp = YString;
                else
                    cp = NString;
                put_ini_string( DsBackupIniFile, ConfigSection, "UseMaxZipCount", cp );
                return TRUE;

            case IGNORE_DOWNTIME_XBOX:
                if ( is_checked( MainDialogWindow, IGNORE_DOWNTIME_XBOX) )
                    {
                    SkipDowntimeZip = TRUE;
                    cp = YString;
                    }
                else
                    {
                    SkipDowntimeZip = FALSE;
                    cp = NString;
                    }
                put_ini_string( DsBackupIniFile, ConfigSection, "SkipDowntimeZip", cp );
                return TRUE;

            case ZIP_COUNT_EBOX:
                if ( cmd == EN_CHANGE )
                    {
                    HaveNewZipCount = TRUE;
                    return TRUE;
                    }

                if ( cmd == EN_KILLFOCUS )
                    {
                    if ( HaveNewZipCount )
                        {
                        update_max_zip_count();
                        HaveNewZipCount = FALSE;
                        }
                    return TRUE;
                    }
                break;

            case ZIP_NOW_BUTTON:
                if ( MailSlotEvent && !NeedToZipAll )
                    {
                    NeedToZipAll = TRUE;
                    SetEvent( MailSlotEvent );
                    }
                return TRUE;

            default:
                break;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                        START_MAILSLOT_THREAD                         *
***********************************************************************/
static void start_mailslot_thread( void )
{
DWORD  id;
HANDLE h;

h = CreateThread(
    NULL,                                       /* no security attributes        */
    0,                                          /* use default stack size        */
    (LPTHREAD_START_ROUTINE) mailslot_thread,   /* thread function       */
    0,                                          /* argument to thread function   */
    0,                                          /* use default creation flags    */
    &id );                                      /* returns the thread identifier */

if ( !h )
    resource_message_box( CREATE_FAILED_STRING, MAILSLOT_THREAD_STRING );
}

/***********************************************************************
*                           ADD_BACKUP_COMPUTER                        *
***********************************************************************/
static void add_backup_computer( void )
{
COMPUTER_CLASS c;
TCHAR        * cp;
int32          slen;
STRING_CLASS   s;

cp = get_ini_string( DsBackupIniFile, BackupSection, BackupDirKey );

if ( unknown(cp) )
    {
    resource_message_box( UNABLE_TO_CONTINUE_STRING, BACKUP_NOT_FOUND_STRING );
    }
else
    {
    slen = lstrlen(cp);
    if ( slen > 2 )
        {
        slen += lstrlen( BackupComputerName ) + 1;
        BackupDirectory = maketext( slen );
        if ( BackupDirectory )
            {
            lstrcpy( BackupDirectory, cp );
            append_directory_to_path( BackupDirectory, BackupComputerName );
            if ( c.add(BackupComputerName, BackupDirectory) )
                return;
            }
        }
    }

resource_message_box( UNABLE_TO_CONTINUE_STRING, ADD_BUP_FAILED_STRING );
s = TEXT( "add_backup_computer: sending WM_CLOSE TO MainWindow" );
log( s );
PostMessage( MainWindow, WM_CLOSE, 0, 0L );
}

/***********************************************************************
*                              ABOUT_DIALOG_PROC                       *
***********************************************************************/
BOOL CALLBACK about_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int           id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        break;
    }

return FALSE;
}

/***********************************************************************
*                    POSITION_MACHINE_LIST_LISTBOX                     *
***********************************************************************/
static void position_machine_list_listbox( HWND hWnd )
{
RECTANGLE_CLASS r;
RECTANGLE_CLASS rc;
WINDOW_CLASS w;

w = hWnd;
w.get_client_rect( r );

w = w.control( MACHINE_LIST_LB );
w.get_move_rect( rc );
rc.left = r.left + 4;
rc.right = r.right - 4;
rc.bottom = r.bottom - 4;
w.move(rc);
}

/***********************************************************************
*                       MAKE_A_LIST_OF_MACHINES                        *
***********************************************************************/
static void make_a_list_of_machines( HWND hWnd )
{
COMPUTER_CLASS c;
LISTBOX_CLASS  lb;
MACHINE_NAME_LIST_CLASS m;
STRING_CLASS   s;

c.rewind();
while ( c.next() )
    {
    if ( c.is_present() && !c.is_this_computer() )
        m.add_computer( c.name() );
    }

lb.init( hWnd, MACHINE_LIST_LB );
lb.set_tabs( COMPUTER_NAME_STATIC, COMPUTER_DIR_STATIC );

if ( m.count() < 1 )
    {
    lb.add( resource_string(NO_MACHINES_FOUND_STRING) );
    return;
    }

s.upsize( MAX_PATH );
m.rewind();
while ( m.next() )
    {
    c.find( m.computer_name() );
    s = m.name();
    s += TabChar;
    s += c.name();
    s += TabChar;
    s += c.directory();
    lb.add( s.text() );
    }
}

/***********************************************************************
*                        MACHINE_LIST_DIALOG_PROC                      *
* A modal dialog that displays a list of the machines I know about     *
* and the computers and directories associated with them.              *
***********************************************************************/
BOOL CALLBACK machine_list_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int     id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        make_a_list_of_machines( hWnd );
        return TRUE;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;

    case WM_SIZE:
        position_machine_list_listbox( hWnd );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
            case IDOK:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                                GETHELP                               *
***********************************************************************/
void gethelp( HWND w )
{
TCHAR s[MAX_PATH+1];

get_exe_directory( s );
append_filename_to_path(  s, TEXT("DsBackup.chm") );

HtmlHelp( w, s, HH_DISPLAY_TOPIC, NULL );
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
BOOL CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static STRING_CLASS s;
static TIME_CLASS   t;

int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_CREATE:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return 0;

    case WM_DBINIT :
        shrinkwrap( MainWindow, MainDialogWindow );
        shifts_startup();
        BList.load();
        /*
        --------------------------------------------------------------
        Once I've loaded the backup list I can add the backup computer
        -------------------------------------------------------------- */
        add_backup_computer();
        start_mailslot_thread();
        return 0;

    case WM_HELP:
        gethelp( hWnd );
        return 0;

    case WM_COMMAND:

        switch ( id )
            {
            case HELP_MENU:
                gethelp( hWnd );
                return 0;

            case ABOUT_MENU:
                DialogBox(
                    MainInstance,
                    "ABOUT_DIALOG",
                    hWnd,
                    (DLGPROC) about_dialog_proc );
                return 0;

            case SAVE_CONFIG_MENU:
                DialogBox(
                    MainInstance,
                    "SAVE_OPTIONS",
                    hWnd,
                    (DLGPROC) save_options_dialog_proc );
                return 0;

            case GLOBAL_FILE_SETTINGS_MENU:
                DialogBox(
                    MainInstance,
                    "GLOBAL_FILE_SETTINGS_DIALOG",
                    hWnd,
                    (DLGPROC) global_file_settings_dialog_proc );
                return 0;

            case MACHINE_LIST_MENU:
                DialogBox(
                    MainInstance,
                    "MACHINE_LIST_DIALOG",
                    hWnd,
                    (DLGPROC) machine_list_dialog_proc );
                return 0;

            case MINIMIZE_MENU:
                CloseWindow( MainWindow );
                return 0;

            case EXIT_MENU:
                s = TEXT( "EXIT_MENU received at " );
                t.get_local_time();
                s += t.text();
                log( s );
                PostMessage( hWnd, WM_CLOSE, 0, 0L );
                return 0;
            }
        break;

    case WM_CLOSE:
        if ( MailSlotIsRunning )
            {
            s = TEXT( "main_proc: WM_CLOSE received at " );
            t.get_local_time();
            s += t.text();
            log( s );
            if ( !ShuttingDown )
                {
                s = TEXT( "main_proc: Setting ShuttingDown to TRUE" );
                log( s );
                ShuttingDown = TRUE;
                BList.shutdown();

                if ( MailSlotEvent )
                    {
                    SetEvent( MailSlotEvent );
                    return 0;
                    }
                }
            }
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
BOOL init( HINSTANCE this_instance )
{
WNDCLASS wc;
COMPUTER_CLASS c;
INI_CLASS      ini;

MainInstance = this_instance;

ShuttingDown = FALSE;
names_startup();
c.startup();

/*
-----------------------------------------------
Make a path for a log file in the exe directory
----------------------------------------------- */
LogFilePath = exe_directory();
LogFilePath.cat_path( DsBackupLogFile );

ini.set_file( ini_file_name(IniFileName) );
ini.set_section( ShotparmSection );

if ( ini.find(HaveAlarmShotKey) )
    HaveAlarmShot = maketext( ini.get_string() );
else
    HaveAlarmShot = maketext( DefaultHaveAlarmShot );

if ( ini.find(HaveGoodShotKey) )
    HaveGoodShot = maketext( ini.get_string() );
else
    HaveGoodShot = maketext( DefaultHaveGoodShot );

if ( ini.find(HaveWarningShotKey) )
    HaveWarningShot = maketext( ini.get_string() );
else
    HaveWarningShot = maketext( DefaultHaveWarningShot );

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon(this_instance, TEXT("DATA_SERVER_ICON") );
wc.lpszMenuName  = (LPSTR) "MainMenu";
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyWindowTitle,
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    279, 244,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

if ( !MainWindow )
    return FALSE;

/*
---------------------------------------
Create an event for the mailslot thread
--------------------------------------- */
MailSlotEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
if ( !MailSlotEvent )
    resource_message_box( UNABLE_TO_CONTINUE_STRING, MAILSLOT_EVENT_FAIL_STRING );

MainDialogWindow  = CreateDialog(
    MainInstance,
    TEXT("MainDialog"),
    MainWindow,
    (DLGPROC) main_dialog_proc );

ShowWindow( MainWindow, SW_SHOW );
UpdateWindow( MainWindow );

return TRUE;
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{
shifts_shutdown();
killtext( HaveAlarmShot );
killtext( HaveGoodShot  );
killtext( HaveWarningShot );
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG  msg;
BOOL status;

if ( is_previous_instance(MyClassName, MyWindowTitle) )
    return 0;

if ( !init(this_instance) )
    return FALSE;

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( MainDialogWindow )
        status = IsDialogMessage( MainDialogWindow, &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
        }
    }

shutdown();
return msg.wParam;
}
