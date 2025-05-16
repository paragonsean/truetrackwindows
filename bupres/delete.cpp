#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\names.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\verrors.h"

#include "extern.h"
#include "resource.h"
#include "save.h"

static const DWORD MS_TO_WAIT         = 200;
static const int32 WAIT_TIMEOUT_COUNT = 10;

static int32 PARAMETER_X_OFFSET = SHOT_LEN + 1 + ALPHADATE_LEN + 1 + ALPHATIME_LEN + 1;
static int32 PROFILE_X_OFFSET   = PARAMETER_X_OFFSET + 2;

static TCHAR TwoChar       = TEXT( '2' );
static TCHAR XChar         = TEXT( 'X' );
static TCHAR EmptyString[] = TEXT( "" );

extern HWND    ProgressWindow;
extern BOOLEAN HaveProgressCancel;

int32   shot_range( SHOT_RANGE_ENTRY & dest, PART_NAME_ENTRY & sorc );
BOOLEAN set_last_shot_number( TCHAR * computer_name, TCHAR * machine_name, TCHAR * part_name, int32 new_shot_number );

/***********************************************************************
*                            SHOW_PROGRESS                             *
***********************************************************************/
static void show_progress( PART_NAME_ENTRY sorc )
{
SetDlgItemText( ProgressWindow, SAVING_MACHINE_TEXT_BOX, sorc.machine );
SetDlgItemText( ProgressWindow, SAVING_PART_TEXT_BOX, sorc.part );
SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, EmptyString );
}

/***********************************************************************
*                         SET_LAST_SHOT_NUMBER                         *
***********************************************************************/
BOOLEAN set_last_shot_number( PART_NAME_ENTRY & pn )
{
SHOT_RANGE_ENTRY range;
int32            n;

n = shot_range( range, pn );
if ( n > 0 )
    {
    n = range.newest.number;
    }
return set_last_shot_number( pn.computer, pn.machine, pn.part, n );
}

/***********************************************************************
*                            GET_SHOTBOX_LIST                          *
***********************************************************************/
int32 get_shotbox_list( int32 ** shot_number_array, BITSETYPE type )
{

INT   * ip;
INT     i;
int32   n;
int32   count;
int32 * sp;
TCHAR * s;

*shot_number_array = 0;

n = ShotBox.get_select_list( &ip );
if ( n <= 0 )
    return 0;

sp = new int32[n];
if ( !sp )
    return 0;

count = 0;

for ( i=0; i<n; i++ )
    {
    s = ShotBox.item_text( ip[i] );
    if ( s )
        {
        if ( ((type & PARAMETER_DATA) && s[PARAMETER_X_OFFSET] == XChar)
        ||   ((type & PROFILE_DATA)   && s[PROFILE_X_OFFSET]   == XChar) )
            {
            sp[count] = extlong( s, SHOT_LEN );
            count++;
            }
        }
    }

delete[] ip;

if ( !count )
    {
    delete[] sp;
    sp = 0;
    }

*shot_number_array = sp;
return count;
}

/***********************************************************************
*                       DELETE_SHOTBOX_SHOTS                           *
***********************************************************************/
static void delete_shotbox_shots( PART_NAME_ENTRY & pn )
{

DB_TABLE sorc;
DB_TABLE dest;
TCHAR    dest_file_name[MAX_PATH+1];
TCHAR  * cp;
TCHAR  * fn;
TCHAR    sorc_file_name[MAX_PATH+1];

int32    di;
int32    this_shot_number;
BOOL     have_new_graphlst;
BOOL     have_new_shotparm;

int32    n;
int32  * shotnumber;

shotnumber = 0;

show_progress( pn );

if ( BackupType & PROFILE_DATA )
    {
    n = get_shotbox_list( &shotnumber, PROFILE_DATA );
    if ( n )
        {
        have_new_graphlst = FALSE;

        lstrcpy( sorc_file_name, graphlst_dbname( pn.computer, pn.machine, pn.part) );
        lstrcpy( dest_file_name, temp_graphlst_dbname( pn.computer, pn.machine, pn.part) );

        if ( dest.create(dest_file_name) )
            {
            if ( dest.open(dest_file_name, GRAPHLST_RECLEN, FL) )
                {
                if ( sorc.open(sorc_file_name, GRAPHLST_RECLEN, PFL) )
                    {
                    have_new_graphlst = TRUE;
                    di = 0;
                    while( sorc.get_next_record(NO_LOCK) )
                        {
                        this_shot_number = sorc.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
                        if ( di < n && this_shot_number == shotnumber[di] )
                            {
                            fn = profile_name( pn.computer,pn.machine, pn.part, this_shot_number );
                            SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, int32toasc(this_shot_number) );
                            if ( !DeleteFile(fn) )
                                {
                                /*
                                ---------------------------
                                See if this is an FTII file
                                --------------------------- */
                                cp = findstring( TEXT(".pro"), fn );
                                if ( cp )
                                    {
                                    cp+=3;
                                    *cp = TEXT('2');
                                    DeleteFile(fn);
                                    }
                                }
                            di++;
                            }
                        else
                            {
                            dest.copy_rec( sorc );
                            dest.rec_append();
                            }
                        }
                    sorc.close();
                    }

                dest.close();
                }

            SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, EmptyString );
            }

        /*
        -------------------------------------------
        Free the memory created by get_shotbox_list
        ------------------------------------------- */
        delete[] shotnumber;

        /*
        ---------------------------------------------------
        Delete the old graphlst file and rename the new one
        --------------------------------------------------- */
        if ( have_new_graphlst )
            {
            DeleteFile( sorc_file_name );
            MoveFile( dest_file_name, sorc_file_name );
            }
        }
    }

if ( BackupType & PARAMETER_DATA )
    {
    n = get_shotbox_list( &shotnumber, PARAMETER_DATA );
    if ( n )
        {
        have_new_shotparm = FALSE;

        lstrcpy( sorc_file_name, shotparm_dbname( pn.computer, pn.machine, pn.part) );
        lstrcpy( dest_file_name, temp_shotparm_dbname( pn.computer, pn.machine, pn.part) );

        if ( dest.create(dest_file_name) )
            {
            if ( dest.open(dest_file_name, SHOTPARM_RECLEN, FL) )
                {
                if ( sorc.open(sorc_file_name, SHOTPARM_RECLEN, PFL) )
                    {
                    have_new_shotparm = TRUE;
                    di = 0;
                    while( sorc.get_next_record(NO_LOCK) )
                        {
                        this_shot_number = sorc.get_long( SHOTPARM_SHOT_NUMBER_OFFSET );
                        if ( di < n && this_shot_number == shotnumber[di] )
                            {
                            di++;
                            }
                        else
                            {
                            dest.copy_rec( sorc );
                            dest.rec_append();
                            }
                        }
                    sorc.close();
                    }

                dest.close();
                }
            }

        /*
        -------------------------------------------
        Free the memory created by get_shotbox_list
        ------------------------------------------- */
        delete[] shotnumber;

        /*
        ---------------------------------------------------
        Delete the old shotparm file and rename the new one
        --------------------------------------------------- */
        if ( have_new_shotparm )
            {
            DeleteFile( sorc_file_name );
            MoveFile( dest_file_name, sorc_file_name );
            }
        }
    }

set_last_shot_number( pn );
}

/***********************************************************************
*                           DELETE_ALL_SHOTS                           *
***********************************************************************/
static void delete_all_shots( PART_NAME_ENTRY & pn )
{
DB_TABLE t;
TCHAR  * fn;
TCHAR  * cp;
int32    this_shot_number;

if ( BackupType & PROFILE_DATA )
    {
    fn = graphlst_dbname( pn.computer, pn.machine, pn.part );
    t.open(fn, GRAPHLST_RECLEN, FL );
    while( t.get_next_record(NO_LOCK) )
        {
        this_shot_number = t.get_long( GRAPHLST_SHOT_NUMBER_OFFSET );
        SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, int32toasc(this_shot_number) );
        fn = profile_name( pn.computer,pn.machine, pn.part, this_shot_number );
        if ( !DeleteFile(fn) )
            {
            cp = fn + lstrlen(fn);
            cp--;
            *cp = TwoChar;
            DeleteFile( fn );
            }
        }
    t.empty();
    t.close();
    SetDlgItemText( ProgressWindow, SAVING_FILE_TEXT_BOX, EmptyString );
    }

if ( BackupType & PARAMETER_DATA )
    {
    fn = shotparm_dbname( pn.computer, pn.machine, pn.part );
    t.open( fn, SHOTPARM_RECLEN, FL );
    t.empty();
    t.close();
    }

set_last_shot_number( pn );
}

/***********************************************************************
*                             DELETE_DATA                              *
***********************************************************************/
void delete_data( void )
{

int               i;
SAVE_LIST_ENTRY * s;
PART_NAME_ENTRY   pn;

for ( i=0; i<NtoSave; i++ )
    {
    if ( HaveProgressCancel )
        break;

    s = SaveList+i;
    s->part.rewind();

    lstrcpy( pn.computer, s->name.computer );
    lstrcpy( pn.machine,  s->name.machine  );

    if ( !waitfor_shotsave_semaphor( pn.computer, pn.machine) )
        {
        resource_message_box( MainInstance, CANT_DO_THAT_STRING, SEMAPHOR_FAIL_STRING );
        continue;
        }

    if ( NtoSave == 1 && s->part.count() == 1 )
        {
        if ( s->part.next() )
            {
            lstrcpy( pn.part, s->part.text() );
            delete_shotbox_shots( pn );
            }
        free_shotsave_semaphore();
        return;
        }

    while ( s->part.next() )
        {
        lstrcpy( pn.part, s->part.text() );
        delete_all_shots( pn );
        }

    free_shotsave_semaphore();
    }

}

