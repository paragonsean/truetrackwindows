#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\listbox.h"
#include "..\include\param.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\alarmsum.h"
#include "..\include\v5help.h"

#include "resource.h"
#include "extern.h"

#ifdef UNICODE
static UINT ClipFormat = CF_UNICODETEXT;
#else
static UINT ClipFormat = CF_TEXT;
#endif

static TCHAR NullChar  = TEXT( '\0' );
static TCHAR TabChar   = TEXT( '\t' );
static TCHAR CrLfStr[] = TEXT( "\r\n" );

void gethelp( UINT helptype, DWORD context );
TCHAR * resource_string( UINT resource_id );

const int32 LB_BUFLEN = ALPHADATE_LEN +1 + SHIFT_LEN + 1 + PARAMETER_NAME_LEN + 7 * (ALARMSUM_COUNTS_LEN + 1) + 2;

/***********************************************************************
*                          COPY_INT32_W_CHAR                           *
***********************************************************************/
TCHAR * copy_int32_w_char( TCHAR * dest, DB_TABLE & t, FIELDOFFSET field_offset )
{
int32 x;

x = t.get_long( field_offset );
return copy_w_char( dest, int32toasc(x), TabChar );
}

/***********************************************************************
*                         FILL_EXPORT_LISTBOX                          *
***********************************************************************/
static void fill_export_listbox( void )
{

DB_TABLE      t;
LISTBOX_CLASS lb;
SYSTEMTIME    shotdate;
int32         pn;
TCHAR       * cp;
TCHAR         buf[LB_BUFLEN+1];
STRING_CLASS  s;

hourglass_cursor();

if ( !ExportAlarmsDialogWindow )
    return;

lb.init( ExportAlarmsDialogWindow, EXPORT_ALARMS_LISTBOX );
lb.redraw_off();
lb.empty();

s = alarmsum_dbname( CurrentPart.computer, CurrentPart.machine, CurrentPart.name );
if ( t.open( s.text(), ALARMSUM_RECLEN, PFL) )
    {
    while ( t.get_next_record(NO_LOCK) )
        {
        t.get_date( shotdate, ALARMSUM_DATE_OFFSET );

        cp = buf;

        cp = copy_w_char( cp, alphadate(shotdate), TabChar );
        cp = copy_int32_w_char( cp, t, ALARMSUM_SHIFT_OFFSET );

        pn = t.get_long( ALARMSUM_PARAMETER_NUMBER_OFFSET );

        if ( pn == TOTALS_PARAMETER_NUMBER )
            {
            cp = copy_w_char( cp, resource_string(TOTALS_STRING), TabChar );
            fillchars( cp, TabChar, 4 );
            cp += 4;
            cp = copy_int32_w_char( cp, t, ALARMSUM_TOTAL_WARNING_COUNT_OFFSET );
            cp = copy_int32_w_char( cp, t, ALARMSUM_TOTAL_ALARM_COUNT_OFFSET );
            cp = copy_int32_w_char( cp, t, ALARMSUM_TOTAL_COUNT_OFFSET );
            }
        else
            {
            cp = copy_w_char( cp, CurrentParam.parameter[pn-1].name, TabChar );
            cp = copy_int32_w_char( cp, t, ALARMSUM_LOW_WARNING_COUNT_OFFSET );
            cp = copy_int32_w_char( cp, t, ALARMSUM_HIGH_WARNING_COUNT_OFFSET );
            cp = copy_int32_w_char( cp, t, ALARMSUM_LOW_ALARM_COUNT_OFFSET );
            cp = copy_int32_w_char( cp, t, ALARMSUM_HIGH_ALARM_COUNT_OFFSET );
            }

        /*
        -----------------------------
        Change the last tab to a null
        ----------------------------- */
        cp--;
        *cp = NullChar;
        lb.add( buf );
        }
    t.close();
    }

lb.redraw_on();

restore_cursor();
}

/***********************************************************************
*                      SET_EXPORT_ALARMS_TAB_STOPS                     *
***********************************************************************/
static void set_export_alarms_tab_stops( void )
{
static int id[] = {
 ALARMSUM_SHIFT_STATIC,
 ALARMSUM_PARAMETER_STATIC,
 LOW_WARNING_COUNT_STATIC,
 HIGH_WARNING_COUNT_STATIC,
 LOW_ALARM_COUNT_STATIC,
 HIGH_ALARM_COUNT_STATIC,
 TOTAL_WARNINGS_STATIC,
 TOTAL_ALARMS_STATIC,
 TOTAL_ALARM_COUNT_STATIC
 };

const int n = sizeof(id)/sizeof(int);

static int tabs[n];

int        i;
int        pixels_per_dialog_unit;
int        left;
HWND       w;
RECT       r;

if ( !ExportAlarmsDialogWindow )
    return;

r.bottom = 1;
r.left   = 1;
r.right  = 4;
r.top    = 1;
MapDialogRect( ExportAlarmsDialogWindow, &r );
pixels_per_dialog_unit = r.right;

w = GetDlgItem(ExportAlarmsDialogWindow, EXPORT_ALARMS_LISTBOX );
GetWindowRect( w, &r );
left = r.left;

for ( i=0; i<n; i++ )
    {
    w = GetDlgItem(ExportAlarmsDialogWindow, id[i] );
    GetWindowRect( w, &r );
    tabs[i] = r.left - left;
    tabs[i] *= 4;
    tabs[i] /= pixels_per_dialog_unit;
    }

SendDlgItemMessage( ExportAlarmsDialogWindow, EXPORT_ALARMS_LISTBOX, LB_SETTABSTOPS, (WPARAM) n, (LPARAM) tabs );
}

/***********************************************************************
*                          COPY_TO_CLIPBOARD                           *
***********************************************************************/
static void copy_to_clipboard( void )
{
LISTBOX_CLASS lb;
int          i;
int          n;
INT        * sindex;
DWORD        slen;
DWORD        nof_bytes;
HGLOBAL      mymem;
HGLOBAL      newmem;
TCHAR      * buf;
TCHAR      * cp;
STRING_CLASS s;

if ( !ExportAlarmsDialogWindow )
    return;

lb.init( ExportAlarmsDialogWindow, EXPORT_ALARMS_LISTBOX );

n = lb.get_select_list( &sindex );
if ( n > 0 )
    {
    s = resource_string( ALARMSUM_EXPORT_HEADER_STRING );
    slen = LB_BUFLEN + 2;
    slen *= n;
    slen += MACHINE_NAME_LEN + 1 + PART_NAME_LEN + 2;
    slen += s.len() + 2;
    slen++;
    slen *= sizeof( TCHAR );

    mymem = GlobalAlloc( GMEM_MOVEABLE | GMEM_DDESHARE, slen );
    if ( !mymem )
        {
        resource_message_box( MainInstance, NO_MEMORY_STRING, CANNOT_COMPLY_STRING );
        return;
        }

    buf = (TCHAR *) GlobalLock(mymem);
    if ( !buf )
        {
        GlobalFree( mymem );
        return;
        }

    cp = buf;

    cp = copy_w_char( cp, CurrentPart.machine, TabChar );
    cp = copystring( cp, CurrentPart.name );
    cp = copystring( cp, CrLfStr );

    cp = copystring( cp, s.text() );
    cp = copystring( cp, CrLfStr );

    for ( i=0; i<n; i++ )
        {
        cp = copystring( cp, lb.item_text(sindex[i]) );
        cp = copystring( cp, CrLfStr );
        }

    delete[] sindex;
    sindex = 0;

    *cp = NullChar;
    cp++;

    GlobalUnlock(mymem);

    if ( DWORD(cp) > DWORD(buf) )
        {
        nof_bytes = DWORD(cp) - DWORD(buf);
        if ( nof_bytes < slen )
            {
            newmem = GlobalReAlloc( mymem, nof_bytes, 0 );
            if ( !newmem )
                {
                GlobalFree( mymem );
                return;
                }
            else
                mymem = newmem;
            }

        if ( OpenClipboard(ExportAlarmsDialogWindow) )
            {
            EmptyClipboard();
            if ( SetClipboardData(ClipFormat, mymem) )
                mymem = 0;
            CloseClipboard();
            }
        }

    if ( mymem )
        GlobalFree( mymem );
    }
}

/***********************************************************************
*                          KILL_ALARMSUM_DATA                          *
***********************************************************************/
static void kill_alarmsum_data( void )
{
STRING_CLASS s;
DB_TABLE t;

LISTBOX_CLASS lb;
INT     * sindex;
int       i;
int       n;
int       ntokill;
int       first_rec;
BOOLEAN   need_to_kill;

if ( !ExportAlarmsDialogWindow )
    return;

lb.init( ExportAlarmsDialogWindow, EXPORT_ALARMS_LISTBOX );

s = alarmsum_dbname( ComputerName, MachineName, PartName );
if ( !file_exists(s.text()) )
    return;

if ( t.open(s.text(), ALARMSUM_RECLEN, FL) )
    {
    ntokill   = 0;
    first_rec = 0;
    n = lb.get_select_list( &sindex );
    if ( n )
        {
        i = n-1;
        while ( TRUE )
            {
            if ( ntokill )
                {
                need_to_kill = TRUE;
                if ( i >= 0 )
                    {
                    if ( sindex[i] == (first_rec-1) )
                        need_to_kill = FALSE;
                    }

                if ( need_to_kill )
                    {
                    if ( t.goto_record_number(first_rec) )
                        t.rec_delete( ntokill );
                    ntokill = 0;
                    }
                }

            if ( i < 0 )
                break;

            ntokill++;
            first_rec = sindex[i];
            i--;
            }

        delete[] sindex;
        }

    t.close();
    }

}

/***********************************************************************
*                             GRAY_BUTTONS                             *
***********************************************************************/
static void gray_buttons()
{
TCHAR * cp;
short   level_needed;

cp = get_ini_string( VisiTrakIniFile, ButtonLevelsSection, DowntimePurgeButtonKey );
if ( cp )
    {
    if ( !unknown(cp) )
        {
        level_needed = (short) asctoint32( cp );
        if ( level_needed > CurrentPasswordLevel )
            EnableWindow( GetDlgItem(ExportAlarmsDialogWindow,DEL_ALARM_RANGE_BUTTON), FALSE );
        }
    }
}

/***********************************************************************
*                      CLOSE_EXPORT_ALARMS_DIALOG                      *
***********************************************************************/
static void close_export_alarms_dialog( void )
{
HWND w;

if ( ExportAlarmsDialogWindow )
    {
    DialogIsActive = FALSE;
    w = ExportAlarmsDialogWindow;
    ExportAlarmsDialogWindow = 0;
    EndDialog( w, 0 );
    }

}

/***********************************************************************
*                       EXPORT_ALARMS_DIALOG_PROC                      *
*                             Modal Dialog                             *
***********************************************************************/
BOOL CALLBACK export_alarms_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  id;
id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        ExportAlarmsDialogWindow = hWnd;
        gray_buttons();
        set_export_alarms_tab_stops();
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_DBINIT:
        fill_export_listbox();
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, EXPORT_ALARMSUM_DATA_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case DEL_ALARM_RANGE_BUTTON:
                kill_alarmsum_data();
                fill_export_listbox();
                return TRUE;

            case EXPORT_ALARM_RANGE_BUTTON:
                copy_to_clipboard();
                return TRUE;

            case IDCANCEL:
                close_export_alarms_dialog();
                return TRUE;
            }
        break;
    }

return FALSE;
}

