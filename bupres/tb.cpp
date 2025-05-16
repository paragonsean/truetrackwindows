#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\machname.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"

#include "resource.h"
#include "extern.h"
#include "fill.h"
#include "save.h"

static const TCHAR AChar         = TEXT( 'A' );
static const TCHAR BackSlashChar = TEXT( '\\' );
static const TCHAR ColonChar     = TEXT( ':' );
static const TCHAR TabChar       = TEXT( '\t' );
static const TCHAR NullChar      = TEXT( '\0' );
static       TCHAR BupresIniFile[] = TEXT( "bupres.ini" );
static       TCHAR ConfigSection[] = TEXT( "Config" );
static       TCHAR LastNKey[]      = TEXT( "LastN" );

/***********************************************************************
*                             BACKUP_BITS                              *
***********************************************************************/
static BITSETYPE backup_bits( HWND w )
{
int i;
int n;
BITSETYPE mask;
static int listbox_id[] = { TB_SETUPS_CHECKBOX, TB_PROFILE_CHECKBOX,
                            TB_PARAM_CHECKBOX, TB_SCRAP_DOWN_CHECKBOX,
                            TB_PURGE_CHECKBOX, TB_LAST_50_CHECKBOX,
                            TB_PURGE_DOWNTIME_CHECKBOX, TB_ALARM_CHECKBOX,
                            TB_PURGE_ALARMS_CHECKBOX };

static BITSETYPE bitmask[] = { SETUP_DATA, PROFILE_DATA,
                               PARAMETER_DATA, DOWNTIME_DATA,
                               WITH_PURGE,     SAVE_LAST_50,
                               PURGE_DOWNTIME, ALARM_DATA,
                               PURGE_ALARM_SUMMARY };

n = sizeof(listbox_id) / sizeof(int);

mask = DO_BACKUP;

for ( i=0; i<n; i++ )
    if ( is_checked(w, listbox_id[i]) )
        mask |= bitmask[i];

return mask;
}

/***********************************************************************
*                            CHECK_ZIP_NAME                            *
***********************************************************************/
static BOOLEAN check_zip_name( HWND w )
{
TCHAR zipname[MAX_PATH+1];
int32 slen;

if ( !get_text(zipname, w, MAX_PATH) )
    {
    resource_message_box( MainInstance, NEED_ZIP_NAME_STRING, CANT_DO_THAT_STRING );
    return FALSE;
    }

/*
-----------------------------------------------------------------------
Don't let them use the temp backup dir as I will then delete their file
----------------------------------------------------------------------- */
slen = lstrlen( BackupDir );
if ( slen > 0 )
    {
    if ( strings_are_equal(BackupDir, zipname, slen) )
        {
        resource_message_box( MainInstance, NO_SAVE_TO_BACKUPDIR_STRING, CANT_DO_THAT_STRING );
        return FALSE;
        }
    }

/*
-------------------------------
Insert a backslash if necessary
------------------------------- */
if ( zipname[0] != BackSlashChar && zipname[1] == ColonChar && zipname[2] != BackSlashChar )
    {
    insert_char( zipname+2, BackSlashChar );
    set_text( w, zipname );
    }

return TRUE;
}

/***********************************************************************
*                         GET_PROFILES_TO_COPY                         *
***********************************************************************/
int32 get_profiles_to_copy( HWND w )
{
int32 i;
BOOL  status;

i = 0;
if ( is_checked(w, TB_RECENT_XBOX) )
    {
    i = (int32) GetDlgItemInt(w, TB_RECENT_COUNT_EBOX, &status, TRUE );
    if ( status )
        {
        if ( i < 1 )
            i = 0;
        }
    else
        {
        i = 0;
        }
    }

return i;
}

/***********************************************************************
*                        CHECK_ZIP_FILE_ENABLE                         *
***********************************************************************/
static void check_zip_file_enable( HWND w )
{
static int control[] = { TB_RECENT_XBOX, TB_RECENT_COUNT_EBOX, TB_ZIP_NAME_EDITBOX };
const int nof_controls = sizeof(control)/sizeof(int);

static int id[] = { TB_SETUPS_CHECKBOX, TB_SCRAP_DOWN_CHECKBOX, TB_PROFILE_CHECKBOX, TB_PARAM_CHECKBOX, TB_ALARM_CHECKBOX };
const int nof_ids = sizeof(id)/sizeof(int);

int i;
BOOLEAN need_zip_file;
BOOL    is_enabled;

need_zip_file = FALSE;

for ( i=0; i<nof_ids; i++ )
    {
    if ( is_checked(w, id[i]) )
        {
        need_zip_file = TRUE;
        break;
        }
    }

is_enabled = IsWindowEnabled( GetDlgItem(w, TB_ZIP_NAME_EDITBOX) );
if ( need_zip_file && !is_enabled )
    is_enabled = TRUE;
else if ( !need_zip_file && is_enabled )
    is_enabled = FALSE;
else
    return;

for ( i=0; i<nof_controls; i++ )
    EnableWindow( GetDlgItem(w, control[i]), is_enabled );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( HWND w )
{
int32 i;

i = int32_from_ini( BupresIniFile, ConfigSection, LastNKey );
if ( i > 0 )
    {
    set_checkbox( w, TB_RECENT_XBOX, TRUE );
    set_text( w, TB_RECENT_COUNT_EBOX, int32toasc(i) );
    }
}

/***********************************************************************
*                        TOGGLE_ALL_CHECKBOXES                         *
***********************************************************************/
static void toggle_all_checkboxes( HWND parent )
{
static int id[] = {
    TB_SETUPS_CHECKBOX,
    TB_PROFILE_CHECKBOX,
    TB_PARAM_CHECKBOX,
    TB_SCRAP_DOWN_CHECKBOX,
    TB_ALARM_CHECKBOX
    };
const int nof_ids = sizeof(id)/sizeof(int);

int     i;
BOOLEAN need_check;
HWND    w[nof_ids];

need_check = FALSE;

for ( i=0; i<nof_ids; i++ )
    {
    w[i] = GetDlgItem( parent, id[i] );
    if ( !is_checked(w[i]) )
        need_check = TRUE;
    }

for ( i=0; i<nof_ids; i++ )
    {
    set_checkbox( w[i], need_check );
    }
}

/***********************************************************************
*                     ENABLE_NEXT_SHOT_NUMBER_EBOX                     *
***********************************************************************/
static void enable_next_shot_number_ebox( HWND w )
{
BOOL is_enabled;
HWND purge_xbox;
HWND last_50_xbox;
HWND next_shot_number_static;
HWND next_shot_number_ebox;

purge_xbox   = GetDlgItem( w, TB_PURGE_CHECKBOX );
last_50_xbox = GetDlgItem( w, TB_LAST_50_CHECKBOX );
next_shot_number_static = GetDlgItem( w, TB_NEXT_SHOT_NUMBER_STATIC );
next_shot_number_ebox   = GetDlgItem( w, TB_NEXT_SHOT_NUMBER_EBOX   );

is_enabled = FALSE;
if ( is_checked(purge_xbox) && !is_checked(last_50_xbox) )
    is_enabled = TRUE;
EnableWindow( next_shot_number_static, is_enabled );
EnableWindow( next_shot_number_ebox, is_enabled );
}

/***********************************************************************
*                              BACKUP_PROC                             *
***********************************************************************/
BOOL CALLBACK backup_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;
BITSETYPE b;
TCHAR zipname[MAX_PATH+1];

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        startup( w );
        return TRUE;

    case WM_DBINIT:
        fill_startup( w, TB_MACHINE_LISTBOX, TB_PART_LISTBOX );
        fill_machines( MachList );
        fill_parts( MachList, DO_BACKUP );
        check_zip_file_enable( w );
        enable_next_shot_number_ebox( w );
        w = GetDlgItem( w, TB_SETUPS_CHECKBOX );
        SetFocus(w);
        return TRUE;

    case WM_DB_SAVE_DONE:
        return TRUE;

    case WM_DBCLOSE:
        SendDlgItemMessage( w, TB_MACHINE_LISTBOX, LB_RESETCONTENT, TRUE, 0L );
        SendDlgItemMessage( w, TB_PART_LISTBOX,    LB_RESETCONTENT, TRUE, 0L );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case TB_MACHINE_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    fill_parts( MachList, DO_BACKUP );
                return TRUE;

            case TB_PART_LISTBOX:
                return TRUE;

            case TB_LAST_50_CHECKBOX:
            case TB_PURGE_CHECKBOX:
                enable_next_shot_number_ebox( w );
                return TRUE;

            case TB_ALL_MACHINES_BUTTON:
                lb_select_all( w, TB_MACHINE_LISTBOX );

                fill_parts( MachList, DO_BACKUP );
                return TRUE;

            case TB_BACKUP_ALL_BUTTON:
                toggle_all_checkboxes( w );
                check_zip_file_enable( w );
                return TRUE;

            case TB_ALL_TEXT_FILES_PB:
                if ( check_zip_name(GetDlgItem(w, TB_ZIP_NAME_EDITBOX)) )
                    {
                    if ( get_text(zipname, w, TB_ZIP_NAME_EDITBOX, MAX_PATH+1) )
                        save_all_text_files( zipname  );
                    }
                return TRUE;

            case TB_SETUPS_CHECKBOX:
            case TB_SCRAP_DOWN_CHECKBOX:
            case TB_PROFILE_CHECKBOX:
            case TB_PARAM_CHECKBOX:
            case TB_ALARM_CHECKBOX:
                if ( cmd == BN_CLICKED )
                    {
                    check_zip_file_enable( w );
                    return TRUE;
                    }
                break;

            case IDCANCEL:
                SendMessage( MainWindow, WM_CLOSE, 0, 0L );
                return TRUE;

            case IDOK:
                if ( check_zip_name(GetDlgItem(w, TB_ZIP_NAME_EDITBOX)) )
                    {
                    ProfilesToCopy = get_profiles_to_copy( w );
                    int32_to_ini( BupresIniFile, ConfigSection, LastNKey, ProfilesToCopy );

                    NextShotNumber = 0;

                    if ( get_text(zipname, w, TB_ZIP_NAME_EDITBOX, MAX_PATH+1) )
                        {
                        b = backup_bits(w);
                        if ( b == DO_BACKUP )
                            {
                            resource_message_box( MainInstance, NOTHING_TO_DO_STRING, NOTE_STRING );
                            SetFocus( GetDlgItem(w, TB_SETUPS_CHECKBOX) );
                            }
                        else
                            {
                            if ( IsWindowEnabled(GetDlgItem(w, TB_NEXT_SHOT_NUMBER_EBOX)) )
                                NextShotNumber = GetDlgItemInt( w, TB_NEXT_SHOT_NUMBER_EBOX, 0, FALSE );
                            save( zipname, b, w, TB_MACHINE_LISTBOX, TB_PART_LISTBOX );
                            NextShotNumber = 0;
                            }
                        }
                    else
                        return FALSE;
                    }
                else
                    {
                    SetFocus( GetDlgItem(w, TB_ZIP_NAME_EDITBOX) );
                    }
                fill_parts( MachList, DO_BACKUP );
                return TRUE;

            }
        break;
    }


return FALSE;
}

/***********************************************************************
*                         FILL_BACKUP_LISTBOX                          *
***********************************************************************/
static void fill_backup_listbox( HWND w )
{

const BUFLEN = MACHINE_NAME_LEN + PART_NAME_LEN + SHOT_LEN + 2;
TCHAR         buf[BUFLEN+1];
TCHAR       * cp;
LISTBOX_CLASS lb;
DB_TABLE      t;
STRING_CLASS  s;

lb.init( w, BACKUP_LIST_LISTBOX );
lb.empty();

s = backup_list_dbname();
if ( !file_exists(s.text()) )
    return;

t.open( s.text(), BACKUP_LIST_RECLEN, PFL );
while ( t.get_next_record(NO_LOCK) )
    {
    cp = buf;
    t.get_alpha( cp, BACKUP_LIST_MN_OFFSET, MACHINE_NAME_LEN );
    cp += charlen( cp, MACHINE_NAME_LEN );
    *cp = TabChar;
    cp++;
    t.get_alpha( cp, BACKUP_LIST_PN_OFFSET, PART_NAME_LEN );
    cp += charlen( cp, PART_NAME_LEN );
    *cp = TabChar;
    cp++;
    t.get_alpha( cp, BACKUP_LIST_SN_OFFSET, SHOT_LEN );
    cp += charlen( cp, SHOT_LEN );
    *cp = NullChar;

    lb.add( buf );
    }

t.close();
}

/***********************************************************************
*                            BACKUP_LIST_PROC                          *
***********************************************************************/
BOOL CALLBACK backup_list_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
static int title_ids[] = {
    BACKUP_LIST_PART_TBOX,
    BACKUP_LIST_SHOT_TBOX
    };
const int nof_title_ids = sizeof(title_ids)/sizeof(int);

static int id;
static TCHAR zipname[MAX_PATH+1];
static BITSETYPE b = DO_BACKUP | SETUP_DATA | PROFILE_DATA;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        set_listbox_tabs_from_title_positions( w, BACKUP_LIST_LISTBOX, title_ids, nof_title_ids );
        return TRUE;

    case WM_DBINIT:
        fill_backup_listbox(w);
        SetFocus( GetDlgItem(w, TB_ZIP_NAME_EDITBOX) );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDCANCEL:
                return TRUE;

            case IDOK:
                if ( check_zip_name(GetDlgItem(w, TB_ZIP_NAME_EDITBOX)) )
                    {
                    if ( get_text(zipname, w, TB_ZIP_NAME_EDITBOX, MAX_PATH+1) )
                        {
                        save_backup_list( zipname, b );
                        }
                    }
                return TRUE;
            }
        break;
    }

return FALSE;
}

