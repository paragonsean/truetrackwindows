#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\names.h"
#include "..\include\param.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\v5help.h"

#include "resource.h"
#include "extern.h"

static TCHAR Delimiter = TEXT( '\t' );
static TCHAR CommaChar = TEXT( ',' );
static TCHAR NChar     = TEXT( 'N' );
static TCHAR NullChar  = TEXT( '\0' );
static TCHAR TabChar   = TEXT( '\t' );
static TCHAR YChar     = TEXT( 'Y' );

static TCHAR CrLfStr[] = TEXT( "\r\n" );
static TCHAR ExportingToCsvKey[] = TEXT( "ExportingToCsv" );
static TCHAR CsvFileNameKey[]    = TEXT( "ExportCsvFile" );
static TCHAR SelectParamsKey[]   = TEXT( "SelectedExportParams" );

static int TITLE_LEN   = MACHINE_NAME_LEN + PART_NAME_LEN + 3;
static int START_OF_LINE_LEN = SHOT_LEN+1+ALPHADATE_LEN+1+ALPHATIME_LEN+1;
static int END_OF_LINE_LEN   = 3;

#ifdef UNICODE
static UINT ClipFormat = CF_UNICODETEXT;
#else
static UINT ClipFormat = CF_TEXT;
#endif

static HWND MyWindow = 0;
static BOOLEAN OriginalExportToCsv = FALSE;
static BOOLEAN ExportingToCsv      = FALSE;

TCHAR * resource_string( UINT resource_id );

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
static void gethelp( UINT helptype, DWORD context )
{
TCHAR fname[MAX_PATH+1];

if ( get_exe_directory(fname) )
    {
    lstrcat( fname, TEXT("export.hlp") );
    WinHelp( MyWindow, fname, helptype, context );
    }

}

/***********************************************************************
*                             ADD_LIMITS                               *
***********************************************************************/
static TCHAR * add_limits( TCHAR * dest, int nof_params, INT * pindex )
{
int        i;
int        j;
int        k;
int        last_param;
float      x;
PARAMETER_CLASS p;

static TCHAR * alarm_type[] = { TEXT("Alarm Low"), TEXT("Alarm High"), TEXT("Warn Low"), TEXT("Warn High") };
static TCHAR * limit_type[] = { TEXT("Min"), TEXT("Max"), TEXT("Min"), TEXT("Max") };

last_param = nof_params - 1;

p.find( ComputerName, MachineName, PartName );

for ( k=0; k<NOF_ALARM_LIMIT_TYPES; k++ )
    {
    dest = copy_w_char( dest, alarm_type[k], Delimiter );
    dest = copy_w_char( dest, limit_type[k], Delimiter );
    *dest = Delimiter;
    dest++;

    for ( i=0; i<nof_params; i++ )
        {
        j = pindex[i];
        x = p.parameter[j].limits[k].value;
        dest = copystring( dest, ascii_float(x) );

        if ( i < last_param )
            {
            *dest = Delimiter;
            dest++;
            }
        else
            {
            dest = copystring( dest, CrLfStr );
            }
        }
    }

/*
---------
Add units
--------- */
dest = copy_w_char( dest, resource_string(UNITS_TITLE_STRING), Delimiter );
*dest = Delimiter;
dest++;
*dest = Delimiter;
dest++;

for ( i=0; i<nof_params; i++ )
    {
    j = pindex[i];
    dest = copystring( dest, units_name(p.parameter[j].units) );
    if ( i < last_param )
        {
        *dest = Delimiter;
        dest++;
        }
    else
        {
        dest = copystring( dest, CrLfStr );
        }

    }
return dest;
}

/***********************************************************************
*                             ADD_SHOT_DATA                            *
***********************************************************************/
static TCHAR * add_shot_data( TCHAR * dest, int nof_shots, INT * sindex, int nof_params, INT * pindex )
{
long       shot_number;
int        i;
int        j;
int        last_param;
float      x;
DB_TABLE   t;
SYSTEMTIME st;

last_param = nof_params - 1;

t.open( shotparm_dbname(ComputerName, MachineName, PartName), SHOTPARM_RECLEN, PFL );
i = 0;
while ( t.get_next_record(NO_LOCK) )
    {
    if ( t.current_record_number() == sindex[i] )
        {
        shot_number = t.get_long( SHOTPARM_SHOT_NUMBER_OFFSET );
        int32toasc( dest, shot_number, DECIMAL_RADIX );
        dest += lstrlen( dest );
        *dest = Delimiter;
        dest++;

        t.get_date( st, SHOTPARM_DATE_OFFSET );
        copychars( dest, alphadate(st), ALPHADATE_LEN );
        dest += ALPHADATE_LEN;
        *dest = Delimiter;
        dest++;

        t.get_time( st, SHOTPARM_TIME_OFFSET );
        copychars( dest, alphatime(st), ALPHATIME_LEN );
        dest += ALPHATIME_LEN;
        *dest = Delimiter;
        dest++;

        for ( j=0; j<nof_params; j++ )
            {
            x    = t.get_float( SHOTPARM_PARAM_1_OFFSET + pindex[j]*(SHOTPARM_FLOAT_LEN+1) );
            dest = copystring( dest, ascii_float(x) );
            if ( j < last_param )
                {
                *dest = Delimiter;
                dest++;
                }
            else
                {
                dest = copystring( dest, CrLfStr );
                }
            }

        i++;
        if ( i >= nof_shots )
            break;
        }
    }
t.close();

return dest;
}

/***********************************************************************
*                     SAVE_SELECTED_PARAMETER_LIST                     *
***********************************************************************/
static void save_selected_parameter_list( INT * p, int n )
{
int     i;
TCHAR * buf;
TCHAR * cp;

if ( n <= 0 || !p )
    return;

buf = maketext( n*(PARAMETER_NUMBER_LEN+1) );
if ( buf )
    {
    cp = buf;
    for ( i=0; i<n; i++ )
        cp = copy_w_char( cp, int32toasc((int32)p[i]), CommaChar );
    cp--;
    *cp = NullChar;
    put_ini_string( DisplayIniFile, ConfigSection, SelectParamsKey, buf );
    delete[] buf;
    }

}

/***********************************************************************
*                          COPY_TO_CLIPBOARD                           *
***********************************************************************/
static void copy_to_clipboard( void )
{
LISTBOX_CLASS sb;
LISTBOX_CLASS pb;
int nof_shots;
int nof_params;
int max_line_len;
int       i;
INT     * sindex;
INT     * pindex;
DWORD     slen;
DWORD     nof_bytes;
HGLOBAL   mymem;
HGLOBAL   newmem;
TCHAR  * buf;
TCHAR  * cp;
TCHAR    shot_number_header[PARAMETER_NAME_LEN+1];
TCHAR    date_name_header[PARAMETER_NAME_LEN+1];
TCHAR    time_name_header[PARAMETER_NAME_LEN+1];
STRING_CLASS s1;
STRING_CLASS s2;

if ( is_checked(MyWindow, EXPORT_CSV_FILE_RADIO) )
    {
    ExportingToCsv = true;
    Delimiter = CommaChar;
    }
else
    {
    ExportingToCsv = false;
    Delimiter = TabChar;
    }

lstrcpy( shot_number_header, resource_string(SHOT_NUMBER_STRING) );
lstrcpy( date_name_header, resource_string(DATE_NAME_STRING) );
lstrcpy( time_name_header, resource_string(TIME_NAME_STRING) );

sb.init( MyWindow, SHOT_LISTBOX );
pb.init( MyWindow, PARAM_NAME_LISTBOX );

nof_shots  = sb.count();
nof_params = pb.count();

slen =  TITLE_LEN;
slen += 3*(PARAMETER_NAME_LEN + 1);                  /* Shot No., Date, Time */
slen += nof_params * (PARAMETER_NAME_LEN + 1) + 1;

max_line_len = START_OF_LINE_LEN + END_OF_LINE_LEN;
max_line_len += nof_params * 12;

slen += max_line_len * ( nof_shots + NOF_ALARM_LIMIT_TYPES + 1 ); /* 1 is for units */
slen ++;

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
cp = copy_w_char( cp, MachineName, Delimiter );
cp = copystring( cp, PartName );
cp = copystring( cp, CrLfStr );

cp = copy_w_char( cp, shot_number_header, Delimiter );
cp = copy_w_char( cp, date_name_header,   Delimiter );
cp = copy_w_char( cp, time_name_header,   Delimiter );

nof_params = pb.get_select_list( &pindex );
if ( nof_params )
    {
    for ( i=0; i<nof_params; i++ )
        cp = copy_w_char( cp, pb.item_text(pindex[i]), Delimiter );
    cp = copystring( cp, CrLfStr );
    nof_shots = sb.get_select_list( &sindex );
    if ( nof_shots )
        {
        cp = add_shot_data( cp, nof_shots, sindex, nof_params, pindex );
        delete[] sindex;
        }

    cp = add_limits( cp, nof_params, pindex );

    save_selected_parameter_list( pindex, nof_params );
    delete[] pindex;
    }

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

    if ( ExportingToCsv )
        {
        HANDLE fh;
        BOOL status;
        DWORD bytes_written;
        STRING_CLASS s;
        s.get_text( MyWindow, CSV_FILE_NAME_EBOX );

        if ( s.isempty() )
            {
            resource_message_box( MainInstance, EXPORT_NO_CSV_STRING, CANNOT_COMPLY_STRING );
            }
        else
            {
            buf = (TCHAR *) GlobalLock(mymem);
            fh = CreateFile( s.text(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
            if ( fh != INVALID_HANDLE_VALUE )
                {
                status = WriteFile( fh, buf, nof_bytes, &bytes_written, 0 );
                CloseHandle( fh );
                }
            GlobalUnlock(mymem);
            if ( bytes_written != nof_bytes )
                {
                s2 = resource_string( EXPORT_TOO_SMALL_STRING );
                s1 = ultoascii( (unsigned long) (bytes_written/sizeof(TCHAR)) );
                s1 += resource_string( EXPORT_OF_STRING );
                s1 += ultoascii( (unsigned long) (nof_bytes/sizeof(TCHAR)) );
                s1 += resource_string( EXPORT_WRITTEN_STRING );
                MessageBox( MyWindow, s1.text(), s2.text(), MB_OK | MB_SYSTEMMODAL );
                }
            }
        }
    else
        {
        if ( OpenClipboard(MyWindow) )
            {
            EmptyClipboard();
            if ( SetClipboardData(ClipFormat, mymem) )
                mymem = 0;
            CloseClipboard();
            }
        }
    }

if ( mymem )
    GlobalFree( mymem );
}

/***********************************************************************
*                         FILL_PARAMETER_NAMES                         *
***********************************************************************/
static void fill_parameter_names( void )
{
int           i;
int           n;
LISTBOX_CLASS lb;
TCHAR       * s;
TCHAR       * cp;
BOOLEAN       havemore;

lb.init( MyWindow, PARAM_NAME_LISTBOX );

n = (int) CurrentParam.count();
if ( n <= 0 )
    return;

for ( i=0; i<n; i++ )
    lb.add( CurrentParam.parameter[i].name );

s = maketext( get_long_ini_string( DisplayIniFile, ConfigSection, SelectParamsKey) );
if ( !unknown(s) )
    {
    cp = s;
    while ( true )
        {
        havemore = replace_char_with_null( cp, CommaChar );
        if ( !is_numeric(*cp) )
            break;
        i = (int) asctoint32( cp );
        if ( i < n )
            lb.set_select( i, TRUE );
        if ( !havemore )
            break;
        cp = nextstring( cp );
        }
    }

if ( s )
    delete[] s;
}

/***********************************************************************
*                              FILL_SHOTS                              *
***********************************************************************/
static void fill_shots( void )
{

SYSTEMTIME    st;
DB_TABLE      t;
TCHAR       * cp;
TCHAR         s[SHOT_LEN+1+ALPHADATE_LEN+1+ALPHATIME_LEN+1];
HCURSOR       old_cursor;
long          shot_number;
LISTBOX_CLASS lb;


old_cursor = SetCursor( LoadCursor(NULL, IDC_WAIT) );

lb.init( MyWindow, SHOT_LISTBOX );
lb.empty();
lb.redraw_off();

t.open( shotparm_dbname(ComputerName, MachineName, PartName), SHOTPARM_RECLEN, PFL );
while ( t.get_next_record(NO_LOCK) )
    {
    cp = s;

    shot_number = t.get_long( SHOTPARM_SHOT_NUMBER_OFFSET );
    insalph( cp, shot_number, SHOT_LEN, TEXT('0'), DECIMAL_RADIX );
    cp += SHOT_LEN;
    *cp = TEXT( ' ' );
    cp++;

    t.get_date( st, SHOTPARM_DATE_OFFSET );
    copychars( cp, alphadate(st), ALPHADATE_LEN );
    cp += ALPHADATE_LEN;
    *cp = TEXT( ' ' );
    cp++;

    t.get_time( st, SHOTPARM_TIME_OFFSET );
    copychars( cp, alphatime(st), ALPHATIME_LEN );
    cp += ALPHATIME_LEN;
    *cp = TEXT( '\0');
    lb.add( s );
    }
t.close();

lb.redraw_on();
lb.select_all();
SetCursor( old_cursor );
}

/***********************************************************************
*                              SELECT_ALL                              *
***********************************************************************/
static void select_all( int id )
{
LISTBOX_CLASS lb;
INT listbox_id;

if ( id == ALL_SHOTS_BUTTON )
    listbox_id = SHOT_LISTBOX;
else if ( id == ALL_PARAMETERS_BUTTON )
    listbox_id = PARAM_NAME_LISTBOX;
else
    return;

lb.init( MyWindow, listbox_id );
lb.select_all();
}

/***********************************************************************
*                          EXPORT_SHOTS_PROC                           *
***********************************************************************/
BOOL CALLBACK export_shots_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static STRING_CLASS s;
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        MyWindow = hWnd;
        OriginalExportToCsv = boolean_from_ini( DisplayIniFile, ConfigSection, ExportingToCsvKey );
        ExportingToCsv = OriginalExportToCsv;
        if ( ExportingToCsv )
            id = EXPORT_CSV_FILE_RADIO;
        else
            id = EXPORT_TO_CLIPBOARD_RADIO;
        CheckRadioButton( MyWindow, EXPORT_TO_CLIPBOARD_RADIO, EXPORT_CSV_FILE_RADIO, id );
        s = get_ini_string( DisplayIniFile, ConfigSection, CsvFileNameKey );
        if ( unknown(s.text()) )
            s.empty();
        s.set_text( MyWindow, CSV_FILE_NAME_EBOX );
        s.empty();
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        fill_shots();
        fill_parameter_names();
        SetFocus( GetDlgItem(hWnd,SHOT_LISTBOX) );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, EXPORT_MENU_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case ALL_SHOTS_BUTTON:
            case ALL_PARAMETERS_BUTTON:
                select_all( id );
                return TRUE;

            case IDOK:
                copy_to_clipboard();
                if ( ExportingToCsv != OriginalExportToCsv )
                    boolean_to_ini( DisplayIniFile, ConfigSection, ExportingToCsvKey, ExportingToCsv );
                if ( SendDlgItemMessage(MyWindow, CSV_FILE_NAME_EBOX, EM_GETMODIFY, 0, 0L) == TRUE )
                    {
                    s.get_text( MyWindow, CSV_FILE_NAME_EBOX );
                    put_ini_string( DisplayIniFile, ConfigSection, CsvFileNameKey, s.text() );
                    s.empty();
                    }

            case IDCANCEL:
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        DialogIsActive = FALSE;
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

