#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\names.h"
#include "..\include\subs.h"

#include "resource.h"
#include "fill.h"
#include "extern.h"
#include "save.h"

static HWND DDWindow = 0;

static LISTBOX_CLASS MachBox;
static LISTBOX_CLASS PartBox;
static LISTBOX_CLASS ShotBox;

struct SHOT_LIST_DB
    {
    BOOLEAN     is_finished;
    int32       shot_number;
    SYSTEMTIME  st;
    FIELDOFFSET shot_number_offset;
    FIELDOFFSET date_offset;
    FIELDOFFSET time_offset;
    DB_TABLE     db;
    };

static const int PROFILE_INDEX   = 0;
static const int PARAMETER_INDEX = 1;

static const TCHAR NullChar  = TEXT( '\0' );
static const TCHAR SpaceChar = TEXT( ' ' );
static const TCHAR XChar     = TEXT( 'X' );

/***********************************************************************
*                             BACKUP_BITS                              *
***********************************************************************/
static BITSETYPE backup_bits( void )
{
int       i;
int       n;
BITSETYPE mask;

static int listbox_id[] = { DD_PROFILES_CHECKBOX, DD_PARAMETERS_CHECKBOX,
                            DD_DELETE_RADIOBUTTON,  DD_RENUMBER_RADIOBUTTON };

static BITSETYPE bitmask[] = { PROFILE_DATA, PARAMETER_DATA,
                               DO_DELETE, DO_RENUMBER };

n = sizeof(listbox_id) / sizeof(int);

mask = NO_BACKUP;

for ( i=0; i<n; i++ )
    if ( is_checked(DDWindow, listbox_id[i]) )
        mask |= bitmask[i];

return mask;
}

/***********************************************************************
*                             NEXT_RECORD                              *
***********************************************************************/
void next_record( SHOT_LIST_DB & s )
{

if ( s.is_finished )
    return;

if ( s.db.get_next_record(NO_LOCK) )
    {
    s.shot_number = s.db.get_long( s.shot_number_offset );
    s.db.get_date( s.st, s.date_offset );
    s.db.get_time( s.st, s.time_offset );
    }
else
    {
    s.is_finished = TRUE;
    }
}

/***********************************************************************
*                              SHOW_SHOTS                              *
***********************************************************************/
void show_shots( void )
{
TCHAR        buf[SHOT_LEN+1+ ALPHADATE_LEN + 1 + ALPHATIME_LEN + 5];
TCHAR        computer[COMPUTER_NAME_LEN+1];
TCHAR        machine[MACHINE_NAME_LEN+1];
TCHAR        part[PART_NAME_LEN+1];
TCHAR      * cp;
SHOT_LIST_DB t[2];
int          i;
int          j;
int32        this_shot_number;
HCURSOR      old_cursor;

ShotBox.empty();
if ( is_checked(DDWindow, DD_RENUMBER_RADIOBUTTON) )
    return;

if ( MachBox.select_count() != 1 )
    return;

if ( PartBox.select_count() != 1 )
    return;

lstrcpy( machine, MachBox.selected_text() );
if ( !MachList.find(machine) )
    return;

old_cursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );

lstrcpy( computer, MachList.computer_name() );
lstrcpy( part,     PartBox.selected_text()  );

t[PROFILE_INDEX].is_finished        = TRUE;
t[PROFILE_INDEX].shot_number_offset = GRAPHLST_SHOT_NUMBER_OFFSET;
t[PROFILE_INDEX].date_offset        = GRAPHLST_DATE_OFFSET;
t[PROFILE_INDEX].time_offset        = GRAPHLST_TIME_OFFSET;

if ( is_checked(DDWindow, DD_PROFILES_CHECKBOX) )
    {
    if ( t[PROFILE_INDEX].db.open(graphlst_dbname(computer, machine, part), GRAPHLST_RECLEN, PFL) )
        {
        t[PROFILE_INDEX].is_finished = FALSE;
        next_record( t[PROFILE_INDEX] );
        }
    }

t[PARAMETER_INDEX].is_finished        = TRUE;
t[PARAMETER_INDEX].shot_number_offset = SHOTPARM_SHOT_NUMBER_OFFSET;
t[PARAMETER_INDEX].date_offset        = SHOTPARM_DATE_OFFSET;
t[PARAMETER_INDEX].time_offset        = SHOTPARM_TIME_OFFSET;

if ( is_checked(DDWindow, DD_PARAMETERS_CHECKBOX) )
    {
    if ( t[PARAMETER_INDEX].db.open(shotparm_dbname(computer, machine, part), SHOTPARM_RECLEN, PFL) )
        {
        t[PARAMETER_INDEX].is_finished = FALSE;
        next_record( t[PARAMETER_INDEX] );
        }
    }

while ( !(t[PROFILE_INDEX].is_finished && t[PARAMETER_INDEX].is_finished) )
    {
    if ( t[PROFILE_INDEX].is_finished )
        {
        i = PARAMETER_INDEX;
        }
    else if ( t[PARAMETER_INDEX].is_finished )
        {
        i = PROFILE_INDEX;
        }
    else if ( t[PROFILE_INDEX].shot_number <= t[PARAMETER_INDEX].shot_number )
        {
        i = PROFILE_INDEX;
        }
    else
        {
        i = PARAMETER_INDEX;
        }

    /*
    ------------------------------------------
    I is the primary index, set j to the other
    ------------------------------------------ */
    j = i ^ 1;

    this_shot_number = t[i].shot_number;
    cp = buf;

    /*
    ----------------------------------------------
    Put shot number and date from sorc into string
    ---------------------------------------------- */
    copyfromstr( cp, ascii_shot_number(this_shot_number), SHOT_LEN );
    cp += SHOT_LEN;
    *cp++ = SpaceChar;

    copyfromstr( cp, alphadate(t[i].st), ALPHADATE_LEN );
    cp += ALPHADATE_LEN;
    *cp++ = SpaceChar;

    copyfromstr( cp, alphatime(t[i].st), ALPHATIME_LEN );
    cp += ALPHATIME_LEN;
    *cp++ = SpaceChar;

    /*
    -----------------------------------------
    Note: get_shotbox_list uses these offsets
    ----------------------------------------- */
    if ( !t[PARAMETER_INDEX].is_finished && (t[PARAMETER_INDEX].shot_number == this_shot_number) )
        *cp++ = XChar;
    else
        *cp++ = SpaceChar;

    *cp++ = SpaceChar;
    if ( !t[PROFILE_INDEX].is_finished && (t[PROFILE_INDEX].shot_number == this_shot_number) )
        *cp++ = XChar;
    else
        *cp++ = SpaceChar;

    *cp = NullChar;

    ShotBox.add( buf );

    next_record( t[i] );
    if ( !t[j].is_finished && t[j].shot_number == this_shot_number )
        next_record( t[j] );
    }

t[PROFILE_INDEX].db.close();
t[PARAMETER_INDEX].db.close();

SetCursor( old_cursor );

}

/***********************************************************************
*                       ENABLE_RENUMBER_CONTROLS                       *
***********************************************************************/
void enable_renumber_controls( void )
{
BOOL enable_state;

if ( is_checked(DDWindow, DD_RENUMBER_RADIOBUTTON) )
    enable_state = TRUE;
else
    enable_state = FALSE;

EnableWindow( GetDlgItem(DDWindow, DD_RENUMBER_EDITBOX ), enable_state );
EnableWindow( GetDlgItem(DDWindow, DD_RENUMBER_LABEL),    enable_state );
}

/***********************************************************************
*                            RENUMBER_START                            *
***********************************************************************/
int32 renumber_start( void )
{
return (int32) GetDlgItemInt( DDWindow, DD_RENUMBER_EDITBOX, 0, FALSE );
}

/***********************************************************************
*                           DELETE_DATA_PROC                           *
***********************************************************************/
BOOL CALLBACK delete_data_proc( HWND w, UINT msg, WPARAM wparam, LPARAM lparam )
{
int id;
int cmd;

id  = LOWORD( wparam );
cmd = HIWORD( wparam );

switch (msg)
    {
    case WM_INITDIALOG:
        DDWindow = w;
        MachBox.init( w, DD_MACHINE_LISTBOX );
        PartBox.init( w, DD_PART_LISTBOX );
        ShotBox.init( w, DD_SHOT_LISTBOX );
        enable_renumber_controls();
        return TRUE;

    case WM_DBINIT:
        fill_startup( w, DD_MACHINE_LISTBOX, DD_PART_LISTBOX );
        fill_machines( MachList );
        return TRUE;

    case WM_DB_SAVE_DONE:
        show_shots();
        return TRUE;

    case WM_DBCLOSE:
        MachBox.empty();
        PartBox.empty();
        ShotBox.empty();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                save( backup_bits(), w, DD_MACHINE_LISTBOX, DD_PART_LISTBOX, DD_SHOT_LISTBOX, renumber_start() );
                return TRUE;

            case DD_ALL_SHOTS_BUTTON:
                ShotBox.select_all();
                return TRUE;

            case DD_PARAMETERS_CHECKBOX:
            case DD_PROFILES_CHECKBOX:
                show_shots();
                return TRUE;

            case DD_RENUMBER_RADIOBUTTON:
            case DD_DELETE_RADIOBUTTON:
                enable_renumber_controls();
                show_shots();
                return TRUE;

            case DD_MACHINE_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    fill_parts( MachList, DO_DELETE );
                    show_shots();
                    }
                return TRUE;

            case DD_PART_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    show_shots();
                    }
                return TRUE;

            }
        break;
    }


return FALSE;
}

