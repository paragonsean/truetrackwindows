#include <windows.h>
#include <ugctrl.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\events.h"
#include "..\include\fileclas.h"
#include "..\include\iniclass.h"
#include "..\include\listbox.h"
#include "..\include\names.h"
#include "..\include\nameclas.h"
#include "..\include\subs.h"
#include "..\include\timeclas.h"
#include "..\include\v5help.h"
#include "..\include\wclass.h"

#include "resource.h"
#include "password.h"
#include "extern.h"

static TCHAR BackSlashChar = TEXT( '\\' );
static TCHAR Cchar         = TEXT( 'c' );
static TCHAR Dchar         = TEXT( 'd' );
static TCHAR Tchar         = TEXT( 't' );
static TCHAR Nchar         = TEXT( 'n' );
static TCHAR NullChar      = TEXT( '\0' );
static TCHAR PeriodChar    = TEXT( '.' );
static TCHAR SpaceChar     = TEXT( ' ' );
static TCHAR TabChar       = TEXT( '\t' );
static TCHAR EmptyString[] = TEXT( "" );
static TCHAR DefaultPasswordLogFormat[] = TEXT( "c d t n" );
static TCHAR DefaultPasswordName[] = DEFAULT_PASSWORD_NAME;

static TCHAR DefaultFontName[] = TEXT( "MS Sans Serif" );

static STRING_CLASS PasswordLogFormat;

static HWND    EnterWindow = 0;
static HWND    EditWindow  = 0;
static HWND    CoOptionsWindow  = 0;
static BOOLEAN ProgramIsUpdating  = FALSE;

static STRING_CLASS LogFileName;

static PASSWORD_CLASS Password;

class PW_GRID_CLASS : public CUGCtrl
{
private:
int     my_font_index;
void    createfont( void );

public:

PW_GRID_CLASS();
~PW_GRID_CLASS();
void OnCharDown( UINT *vcKey, BOOL processed );
int  OnEditContinue( int oldcol, long oldrow, int * newcol, long * newrow );
int  OnEditFinish( int col, long row, HWND edit, LPCTSTR string, BOOL cancelFlag);
void OnLClicked( int col, long row, int updn, RECT * rect, POINT * point, int processed );
void OnKeyDown( UINT *vcKey, BOOL processed );
void OnSetup();
void delete_row();
void fill_grid();
void insert_row();
void set_row_colors();
void set_row_numbers();
};

PW_GRID_CLASS Grid;
const int  NofCols  = 3;

TCHAR * CoOptionsExeDir = 0;

TCHAR * resource_string( UINT resource_id );

/***********************************************************************
*                             PW_GRID_CLASS                            *
***********************************************************************/
PW_GRID_CLASS::PW_GRID_CLASS()
{
createfont();
}

/***********************************************************************
*                            ~PW_GRID_CLASS                            *
***********************************************************************/
PW_GRID_CLASS::~PW_GRID_CLASS()
{
RemoveFont( my_font_index );
}

/***********************************************************************
*                             PW_GRID_CLASS                            *
*                             OnEditFinish                             *
***********************************************************************/
int PW_GRID_CLASS::OnEditFinish( int col, long row, HWND edit, LPCTSTR string, BOOL cancelFlag)
{
STRING_CLASS s;
int r;
int n;

if ( col == 1 )
    {
    n   = GetNumberRows();
    for ( r=0; r<n; r++ )
        {
        if ( r != row )
            {
            s = QuickGetText( col, r );
            if ( s == string )
                {
                resource_message_box( m_ctrlWnd, MainInstance, PW_DUPLICATE_STRING, INPUT_ERROR_STRING, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL );
                return FALSE;
                }
            }
        }
    }

return TRUE;
}

/***********************************************************************
*                             PW_GRID_CLASS                            *
*                             OnEditContinue                           *
***********************************************************************/
int PW_GRID_CLASS::OnEditContinue( int oldcol, long oldrow, int * newcol, long * newrow )
{
GotoCell( *newcol, *newrow );
return FALSE;
}

/***********************************************************************
*                             PW_GRID_CLASS                            *
*                              OnLClicked                              *
***********************************************************************/
void PW_GRID_CLASS::OnLClicked( int col, long row, int updn, RECT * rect, POINT * point, int processed )
{
if ( updn )
    StartEdit();
}

/***********************************************************************
*                             PW_GRID_CLASS                            *
*                               OnKeyDown                              *
***********************************************************************/
void PW_GRID_CLASS::OnKeyDown( UINT *vcKey, BOOL processed )
{
if ( (*vcKey) == VK_INSERT )
    insert_row();
else if ( (*vcKey) == VK_DELETE )
    delete_row();

}

/***********************************************************************
*                             PW_GRID_CLASS                            *
*                              OnCharDown                              *
***********************************************************************/
void PW_GRID_CLASS::OnCharDown( UINT *vcKey, BOOL processed )
{
if ( (*vcKey) == VK_TAB )
    {
    int n;
    int row;
    int col;

    col = GetCurrentCol();
    n   = GetNumberRows();
    row = GetCurrentRow();

    if ( col < (NofCols-1) )
        {
        col++;
        }
    else if ( row < (n-1) )
        {
        row++;
        col = 0;
        }
    else
        {
        row = 0;
        col = 0;
        }
    GotoCell( col, row );
    }
else if( !processed )
    {
    StartEdit( *vcKey );
    }
}

/***********************************************************************
*                             PW_GRID_CLASS                            *
*                            SET_ROW_COLORS                            *
***********************************************************************/
void PW_GRID_CLASS::set_row_colors()
{
int  col;
long row;
long n;
COLORREF color;

n = GetNumberRows();
for ( row=0; row<n; row++ )
    {
    if ( (row/2)*2 == row )
        color = RGB( 252, 240, 190 );
    else
        color = RGB( 255, 255, 255 );

    for ( col=0; col<NofCols; col++ )
        QuickSetBackColor( col, row, color );
    }
}

/***********************************************************************
*                             PW_GRID_CLASS                            *
*                              CREATEFONT                              *
***********************************************************************/
void PW_GRID_CLASS::createfont( void )
{
LOGFONT    lf;

lf.lfHeight          = 18;
lf.lfWidth           = 0;
lf.lfEscapement      = 0;
lf.lfOrientation     = 0;
lf.lfWeight          = FW_DONTCARE;
lf.lfItalic          = FALSE;
lf.lfUnderline       = FALSE;
lf.lfStrikeOut       = FALSE;
lf.lfCharSet         = ANSI_CHARSET;
lf.lfOutPrecision    = 0;
lf.lfClipPrecision   = 0;
lf.lfQuality         = DEFAULT_QUALITY;
lf.lfPitchAndFamily  = 0;

lstrcpy( lf.lfFaceName, DefaultFontName );

if (PRIMARYLANGID(GetUserDefaultLangID())==LANG_JAPANESE)
    {
    lf.lfCharSet = SHIFTJIS_CHARSET;
    lstrcpy(lf.lfFaceName,TEXT("MS –¾’©"));
    }

ClearAllFonts();

my_font_index = AddFont(
    lf.lfHeight,
    lf.lfWidth,
    lf.lfEscapement,
    lf.lfOrientation,
    lf.lfWeight,
    lf.lfItalic,
    lf.lfUnderline,
    lf.lfStrikeOut,
    lf.lfCharSet,
    lf.lfOutPrecision,
    lf.lfClipPrecision,
    lf.lfQuality,
    lf.lfPitchAndFamily,
    lf.lfFaceName
    );

if ( SetDefFont(my_font_index) != UG_SUCCESS )
    MessageBox( 0, TEXT("SetDefFont"), TEXT("No Font at this index"), MB_OK );
}

/***********************************************************************
*                             PW_GRID_CLASS                            *
*                              DELETE_ROW                              *
***********************************************************************/
void PW_GRID_CLASS::delete_row()
{
if ( GetNumberRows() > 0 )
    DeleteRow( GetCurrentRow() );

if ( GetNumberRows() > 0 )
    {
    set_row_colors();
    set_row_numbers();
    }

RedrawAll();
}

/***********************************************************************
*                             PW_GRID_CLASS                            *
*                              INSERT_ROW                              *
***********************************************************************/
void PW_GRID_CLASS::insert_row()
{
CUGCell cell;
int  col;
long n;
long row;

n = GetNumberRows();

if ( n > 0 )
    row = GetCurrentRow();
else
    row = 0;

InsertRow( row );

GetGridDefault( &cell );
cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN );
cell.SetText( EmptyString );

for ( col=0; col<NofCols; col++ )
    SetCell( col, row, &cell );

set_row_colors();
set_row_numbers();

RedrawAll();

}

/***********************************************************************
*                              PW_GRID_CLASS                           *
*                             SET_ROW_NUMBERS                          *
***********************************************************************/
void PW_GRID_CLASS::set_row_numbers()
{
CUGCell cell;
long    row;
long    n;

GetGridDefault( &cell );
cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN | UG_BDR_LTHIN );

n = GetNumberRows();
for ( row=0; row<n; row++ )
    {
    cell.SetText( int32toasc(row) );
    SetCell( -1, row, &cell );
    }
}

/***********************************************************************
*                              PW_GRID_CLASS                           *
*                               FILL_GRID                              *
***********************************************************************/
void PW_GRID_CLASS::fill_grid()
{
CUGCell cell;
int    row;
int    n;

n = Password.count();

if ( n != GetNumberRows() )
    SetNumberRows( n, FALSE );

GetGridDefault( &cell );

cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN );
for ( row=0; row<n; row++ )
    {
    cell.SetText( int32toasc(Password.pw[row].level) );
    SetCell( 0, row, &cell );

    cell.SetText( Password.pw[row].name );
    SetCell( 1, row, &cell );

    cell.SetText( Password.pw[row].owner );
    SetCell( 2, row, &cell );
    }

set_row_numbers();
set_row_colors();
}

/***********************************************************************
*                              PW_GRID_CLASS                           *
*                                 OnSetup                              *
***********************************************************************/
void PW_GRID_CLASS::OnSetup()
{
CUGCell cell;
HDC     dc;
int     h;
int     wm1;
int     w[NofCols];
RECT    r;

Grid.GetGridDefault( &cell );
cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN );

dc = GetDC( m_ctrlWnd );
if ( dc != NULL )
    {
    h = (int) character_height( dc );
    wm1 = (int) pixel_width( dc, TEXT("000") );
    w[0]= (int) pixel_width( dc, TEXT("Label") );
    w[1]= (int) pixel_width( dc, TEXT("a") );
    w[1] *= PASSWORD_LEN;
    ReleaseDC( m_ctrlWnd, dc );
    }

h += 2;

GetWindowRect( Grid.m_ctrlWnd, &r );
r.right -= GetVS_Width();
r.left  += wm1;
r.left  += w[0];
r.left  += w[1];
w[2] = (r.right - r.left );

SetDefRowHeight( h );
wm1 += 2;
SetSH_Width( wm1 );

SetNumberCols( NofCols );
Set3DHeight( 0 );

cell.SetText( EmptyString );
cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN | UG_BDR_TTHIN | UG_BDR_LTHIN );
SetCell( -1, -1, &cell );

SetColWidth( 0, w[0] );
cell.SetText( resource_string(PW_COL_0_STRING) );
cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN | UG_BDR_TTHIN );
SetCell( 0, -1, &cell );

SetColWidth( 1, w[1] );
cell.SetText( resource_string(PW_COL_1_STRING) );
SetCell( 1, -1, &cell );

SetColWidth( 2, w[2] );
cell.SetText( resource_string(PW_COL_2_STRING) );
SetCell( 2, -1, &cell );

fill_grid();
}

/***********************************************************************
*                               CLEANUP                                *
***********************************************************************/
void PASSWORD_CLASS::cleanup()
{
if ( pw )
    {
    delete[] pw;
    pw = 0;
    n  = 0;
    }
}

/***********************************************************************
*                           ~PASSWORD_CLASS                            *
***********************************************************************/
PASSWORD_CLASS::~PASSWORD_CLASS()
{
cleanup();
}

/**********************************************************************
*                             AM_I_ENCRYPTED                          *
**********************************************************************/
static BOOLEAN am_i_encrypted()
{
NAME_CLASS s;
INI_CLASS  ini;
s.get_local_ini_file_name( DisplayIniFile );
ini.set_file( s.text() );
ini.set_section( ConfigSection );
return ini.get_boolean(PasswordsAreEncriptedKey);
}

#ifdef UNICODE
static int MaxCharValue = 0xFFFF;
static int Adjustor     = 0xFFDF;
#else
static int MaxCharValue = 0xFF;
static int Adjustor     = 0xDF;
#endif

/**********************************************************************
*                            ENCRYPT_PASSWORD                         *
**********************************************************************/
void encrypt_password( TCHAR * sorc )
{
union {
    TCHAR c;
    int   ic;
    };
int  ec;
int  i;

ec = (int) TEXT( '{' );
for ( i=0; i<PASSWORD_LEN; i++ )
    {
    if ( *sorc == SpaceChar )
        continue;
    ic = 0;
    c  = *sorc;
    ic += ec;
    if ( ic > MaxCharValue )
        ic -= Adjustor;
    *sorc = c;
    sorc++;
    ec++;
    }
}

/**********************************************************************
*                            DECRYPT_PASSWORD                         *
**********************************************************************/
static void decrypt_password( TCHAR * sorc )
{
union {
    TCHAR c;
    int   ic;
    };
TCHAR ec;
int   i;

ec = (int) TEXT( '{' );
for ( i=0; i<PASSWORD_LEN; i++ )
    {
    if ( *sorc == SpaceChar )
        continue;
    ic = 0;
    c  = *sorc;
    ic -= ec;
    if ( ic < 0 )
        ic += Adjustor;
    *sorc = c;
    sorc++;
    ec++;
    }
}

/***********************************************************************
*                         PASSWORD_CLASS::LOAD                         *
***********************************************************************/
bool PASSWORD_CLASS::load()
{
DB_TABLE t;
TCHAR  * cp;
TCHAR    p[PASSWORD_LEN+1];
int32    i;
short    reclen;
BOOLEAN  is_encrypted;

cleanup();
is_encrypted = am_i_encrypted();

cp = new_password_dbname();
if ( file_exists(cp) )
    {
    reclen = NEW_PASSWORD_RECLEN;
    }
else
    {
    cp = password_dbname();
    reclen = PASSWORD_RECLEN;
    }

if ( !file_exists(cp) )
    return false;

t.open( cp, reclen, PFL );
if ( setsize(t.nof_recs()) )
    {
    i = 0;
    while ( i < n )
        {
        if ( !t.get_next_record(FALSE) )
            break;
        pw[i].level = t.get_long( PASSWORD_LEVEL_OFFSET );
        t.get_alpha( p,  PASSWORD_OFFSET, PASSWORD_LEN, 0 );
        if ( is_encrypted )
            decrypt_password( p );
        strip( p );
        copystring( pw[i].name, p );
        if ( reclen == NEW_PASSWORD_RECLEN )
            t.get_alpha( pw[i].owner, PASSWORD_OWNER_OFFSET, PASSWORD_OWNER_LEN );
        i++;
        }
    }

t.close();

if ( n > 0 )
    return true;

return false;
}

/***********************************************************************
*                         PASSWORD_CLASS::SAVE                         *
***********************************************************************/
bool PASSWORD_CLASS::save()
{
DB_TABLE t;
TCHAR  * cp;
TCHAR    p[PASSWORD_LEN+1];
int32    i;
BOOLEAN  is_encrypted;

if ( !pw )
    return FALSE;

is_encrypted = am_i_encrypted();

cp = new_password_dbname();
if ( !file_exists(cp) )
    t.create( cp );

t.open( cp, NEW_PASSWORD_RECLEN, FL );
t.empty();
for ( i=0; i<n; i++ )
    {
    t.put_long(  PASSWORD_LEVEL_OFFSET,  pw[i].level, PASSWORD_LEVEL_LEN );
    copyfromstr( p, pw[i].name, PASSWORD_LEN );
    p[PASSWORD_LEN] = NullChar;
    if ( is_encrypted )
        encrypt_password( p );
    t.put_alpha( PASSWORD_OFFSET,        p,          PASSWORD_LEN );
    t.put_alpha( PASSWORD_OWNER_OFFSET,  pw[i].owner, PASSWORD_OWNER_LEN );
    t.rec_append();
    }

t.close();

return true;
}

/***********************************************************************
*                         PASSWORD_CLASS::FIND                        *
***********************************************************************/
int32 PASSWORD_CLASS::find( TCHAR * password_to_find )
{
int32 i;

if ( pw )
    {
    for ( i=0; i<n; i++ )
        {
        if ( strings_are_equal(pw[i].name, password_to_find) )
            return i;
        }
    }

return NO_INDEX;
}

/***********************************************************************
*                          PASSWORD_CLASS                              *
*                               []                                     *
***********************************************************************/
PASSWORD_ENTRY & PASSWORD_CLASS::operator[]( int32 i )
{
static PASSWORD_ENTRY EmptyPasswordEntry;

if ( pw )
    {
    if ( i>=0 && i<n )
        return pw[i];
    }

return EmptyPasswordEntry;
}

/***********************************************************************
*                          PASSWORD_CLASS                              *
*                          DEFAULT_LEVEL                               *
***********************************************************************/
int32 PASSWORD_CLASS::default_level()
{
int32 i;
int32 default_level;

default_level = NO_PASSWORD_LEVEL;
i = find( DefaultPasswordName );
if ( i != NO_INDEX )
    default_level = pw[i].level;

return default_level;
}

/***********************************************************************
*                     PASSWORD_CLASS::HIGHEST_LEVEL                    *
***********************************************************************/
int32 PASSWORD_CLASS::highest_level()
{
int32 i;
int32 h;

h = NO_PASSWORD_LEVEL;

if ( pw )
    {
    for ( i=0; i<n; i++ )
        {
        if ( h < pw[i].level )
            h = pw[i].level;
        }
    }

return h;
}

/***********************************************************************
*                            PASSWORD_CLASS                            *
*                               SETSIZE                                *
***********************************************************************/
bool PASSWORD_CLASS::setsize( int32 new_count )
{
if ( n != new_count )
    {
    cleanup();

    pw = new PASSWORD_ENTRY[new_count];
    if ( pw )
        n = new_count;
    }

if ( n > 0 )
    return true;

return false;
}

/***********************************************************************
*                      CHECK_FOR_PASSWORD_LOGGING                      *
***********************************************************************/
bool check_for_password_logging()
{
LogFileName = get_long_ini_string( VisiTrakIniFile, ConfigSection, TEXT("PasswordLogFile") );
if ( !unknown(LogFileName.text()) )
    {
    PasswordLogFormat = get_long_ini_string( VisiTrakIniFile, ConfigSection, TEXT("PasswordLogFormat") );
    if ( unknown(PasswordLogFormat.text()) )
        PasswordLogFormat = DefaultPasswordLogFormat;
    return true;
    }
else
    {
    LogFileName.empty();
    return false;
    }
}

/***********************************************************************
*                             LOG_PASSWORD                             *
***********************************************************************/
static void log_password( TCHAR * owner )
{
TCHAR * dest;
TCHAR * fs;
TCHAR * s;
int32   nof_chars;
TIME_CLASS   t;
FILE_CLASS   f;
TCHAR buf[MAX_PATH];
DWORD buf_size = MAX_PATH;

if ( !check_for_password_logging() )
    return;

if ( PasswordLogFormat.isempty() )
    PasswordLogFormat = DefaultPasswordLogFormat;

t.get_local_time();
GetComputerName( buf, &buf_size );

nof_chars = PasswordLogFormat.len();
nof_chars += ALPHADATE_LEN;
nof_chars += ALPHATIME_LEN;
nof_chars += PASSWORD_OWNER_LEN;
nof_chars += 20;

s = maketext( nof_chars );
if ( s )
    {
    dest = s;
    fs   = PasswordLogFormat.text();

    while ( true )
        {
        if ( *fs == Cchar )
            {
            dest = copystring( dest, buf );
            }
        else if ( *fs == Dchar )
            {
            dest = copystring( dest, t.mmddyyyy() );
            }
        else if ( *fs == Tchar )
            {
            dest = copystring( dest, t.hhmmss() );
            }
        else if ( *fs == Nchar )
            {
            dest = copystring( dest, owner );
            }
        else
            {
            *dest++ = *fs;
            }

        if ( *fs == NullChar )
            break;

        fs++;
        }

    if ( f.open_for_append( LogFileName.text() ) )
        {
        f.writeline( s );
        f.close();
        }

    delete[] s;
    }
}

/***********************************************************************
*                        DEFAULT_PASSWORD_LEVEL                        *
* This returns the current default password level or NO_PASSWORD_LEVEL *
* if none exists.                                                      *
***********************************************************************/
int default_password_level()
{
PASSWORD_CLASS p;

p.load();
return p.default_level();
}

/***********************************************************************
*                      SET_DEFAULT_PASSWORD_LEVEL                      *
***********************************************************************/
void set_default_password_level( bool need_log )
{
DB_TABLE t;
NAME_CLASS s;
TCHAR    name[PASSWORD_LEN+1];
TCHAR    owner[PASSWORD_OWNER_LEN+1];
int32    default_level;
short    reclen;
BOOLEAN  is_encrypted;

is_encrypted = am_i_encrypted();

default_level = NO_PASSWORD_LEVEL;

s = new_password_dbname();
if ( s.file_exists() )
    {
    reclen = NEW_PASSWORD_RECLEN;
    }
else
    {
    s = password_dbname();
    reclen = PASSWORD_RECLEN;
    }

if ( s.file_exists() )
    {
    t.open( s.text(), reclen, PFL );
    while ( t.get_next_record(FALSE) )
        {
        if ( t.get_alpha(name, PASSWORD_OFFSET, PASSWORD_LEN, 0) )
            {
            if ( is_encrypted )
                decrypt_password( name );
            strip( name );
            if ( strings_are_equal(name, DefaultPasswordName) )
                {
                default_level = t.get_long( PASSWORD_LEVEL_OFFSET );
                lstrcpy( owner, UNKNOWN );
                if ( check_for_password_logging() )
                    {
                    if ( need_log && reclen == NEW_PASSWORD_RECLEN )
                        {
                        t.get_alpha( owner, PASSWORD_OWNER_OFFSET, PASSWORD_OWNER_LEN );
                        log_password( owner );
                        }
                    }
                break;
                }
            }
        }

    t.close();
    }

if ( default_level != NO_PASSWORD_LEVEL )
    poke_data( DDE_CONFIG_TOPIC, ItemList[PW_LEVEL_INDEX].name, int32toasc(default_level) );
}

/***********************************************************************
*                      COPY_PASSWORDS_TO_NETWORK                       *
***********************************************************************/
static void copy_passwords_to_network()
{
COMPUTER_CLASS c;
INI_CLASS      ini;
NAME_CLASS     s;
STRING_CLASS dest_file;
STRING_CLASS sorc_file;
TCHAR * cp;
int     count;
int     n;
BOOLEAN is_encrypted;
BOOLEAN ok_to_copy;

is_encrypted = is_checked( EditWindow, ENCRYPT_PASSWORDS_XBOX );
sorc_file = new_password_dbname();

c.rewind();
while ( c.next() )
    {
    cp = c.directory();
    if ( *cp == BackSlashChar && *(cp+1) == BackSlashChar )
        {
        if ( c.is_present() )
            {
            dest_file = c.directory();
            cp = dest_file.text();
            n = dest_file.len();
            cp += n;
            count = 0; // count of backslashes.
            while ( n )
                {
                cp--;
                if ( *cp == BackSlashChar )
                    {
                    count++;
                    if ( count == 2 )
                        {
                        cp++;
                        *cp = NullChar;
                        break;
                        }
                    }
                n--;
                }

            if ( count == 2 )
                {
                ok_to_copy = TRUE;
                if ( is_encrypted )
                    {
                    s.get_exe_dir_file_name( c.name(), VisiTrakIniFile );
                    ini.set_file( s.text() );
                    ini.set_section( ConfigSection );
                    s = ini.get_string( VersionKey );
                    cp = s.text();
                    if ( asctoint32(cp) < 6 )
                        {
                        ok_to_copy = FALSE;
                        }
                    else
                        {
                        cp = s.find( PeriodChar );
                        if ( !cp )
                            {
                            ok_to_copy = FALSE;
                            }
                        else
                            {
                            cp++;
                            if ( asctoint32(cp) < 56 )
                                ok_to_copy = FALSE;
                            }    
                        }
                    }
                if ( ok_to_copy )
                    {
                    /*
                    -------------------------------------------
                    Update the ini file on the network computer
                    ------------------------------------------- */
                    s.get_exe_dir_file_name( c.name(), DisplayIniFile );
                    ini.set_file( s.text() );
                    ini.set_section( ConfigSection );
                    ini.put_boolean( PasswordsAreEncriptedKey, is_encrypted );
    
                    /*
                    ----------------------
                    Copy the password file
                    ---------------------- */
                    dest_file += NEW_PASSWORD_DB;
                    CopyFile( sorc_file.text(), dest_file.text(), FALSE );
                    }
                else
                    {
                    resource_message_box( MainInstance, SOME_VERSIONS_TOO_OLD_STRING, CANNOT_COMPLY_STRING );
                    }
                }
            }
        }
    }
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static bool save_changes()
{
INI_CLASS  ini;
NAME_CLASS s;
int row;
int col;
int n;
bool ok_to_save;

ok_to_save = true;

n = Grid.GetNumberRows();
if ( n > 0 )
    {
    for ( row=0; row<n; row++ )
        {
        for ( col=0; col<2; col++ )
            {
            s = Grid.QuickGetText( col, row );
            if ( s.isempty() )
                {
                ok_to_save = false;
                goto finished;
                }
            }
        }

    finished:
    if ( !ok_to_save )
        {
        resource_message_box( EditWindow, MainInstance, PW_BLANK_STRING, INPUT_ERROR_STRING, MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL );
        Grid.GotoCell( col, row );
        SetFocus( Grid.m_ctrlWnd );
        return false;
        }

    s.get_local_ini_file_name( DisplayIniFile );
    ini.set_file( s.text() );
    ini.set_section( ConfigSection );
    ini.put_boolean(PasswordsAreEncriptedKey, is_checked(EditWindow, ENCRYPT_PASSWORDS_XBOX) );

    if ( Password.setsize(n) )
        {
        for ( row=0; row<n; row++ )
            {
            s = Grid.QuickGetText( 0, row );
            Password.pw[row].level = s.int_value();
            s = Grid.QuickGetText( 1, row );
            lstrcpyn( Password.pw[row].name,  s.text(), PASSWORD_LEN+1 );
            s = Grid.QuickGetText( 2, row );
            lstrcpyn( Password.pw[row].owner, s.text(), PASSWORD_OWNER_LEN+1 );
            }
        Password.save();

        if ( is_checked(EditWindow, COPY_TO_NETWORK_XBOX) )
            {
            copy_passwords_to_network();
            }
        }
    }

return true;
}
/***********************************************************************
*                            COOPTIONSHELP                             *
***********************************************************************/
static void cooptionshelp( UINT helptype, DWORD context )
{
TCHAR fname[MAX_PATH+1];

if ( get_exe_directory(fname) )
    {
    lstrcat( fname, TEXT("v5help.hlp") );
    WinHelp( EnterWindow, fname, helptype, context );
    }

}

/***********************************************************************
*                               GETHELP                                *
***********************************************************************/
static void gethelp( UINT helptype, DWORD context )
{
TCHAR fname[MAX_PATH+1];

if ( get_exe_directory(fname) )
    {
    lstrcat( fname, TEXT("password.hlp") );
    WinHelp( EnterWindow, fname, helptype, context );
    }

}

/***********************************************************************
*                         EXIT_EDIT_PASSWORDS                          *
***********************************************************************/
static void exit_edit_passwords()
{

if ( EnterWindow )
    SetFocus( GetDlgItem(EnterWindow, ENTER_PASSWORD_EBOX) );

EndDialog( EditWindow, 0 );
EditWindow = 0;
}

/***********************************************************************
*                          EDIT_PASSWORDS_PROC                         *
***********************************************************************/
BOOL CALLBACK edit_passwords_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        EditWindow = hWnd;
        RECT r;
        RECT sr;
        GetClientRect(hWnd,&r);
        GetWindowRect( GetDlgItem(hWnd, IDOK), &sr );
        r.bottom += sr.top - sr.bottom;
        Grid.CreateGrid( WS_CHILD|WS_VISIBLE, r, hWnd, PW_GRID_ID );
        set_checkbox( hWnd, ENCRYPT_PASSWORDS_XBOX, am_i_encrypted() );
        break;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, PASSWORD_EDIT_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                if ( !save_changes() )
                    return TRUE;

            case IDCANCEL:
                exit_edit_passwords();
                return TRUE;

            }
        break;

    case WM_CLOSE:
        exit_edit_passwords();
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         EXIT_CO_OPTIONS                              *
***********************************************************************/
static void exit_co_options()
{
HWND w;
w = CoOptionsWindow;
if ( w )
    {
    CoOptionsWindow = 0;
    if ( EnterWindow )
        SetFocus( GetDlgItem(EnterWindow, ENTER_PASSWORD_EBOX) );

    EndDialog( w, 0 );
    }
}

/***********************************************************************
*                           FILL_CO_OPTIONS                            *
***********************************************************************/
static void fill_co_options()
{
int          i;
int          id;
HWND         w;
BOOLEAN      b;
INI_CLASS    ini;
STRING_CLASS s;

w = CoOptionsWindow;

/*
-----------
Display.ini
----------- */
ini.set_file( ini_file_name(DisplayIniFile) );
ini.set_section( ConfigSection );

/*
------------------
Snapshot is active
------------------ */
b = TRUE;
if ( ini.exists() )
    {
    if ( ini.find(SnapshotKey) )
        b = ini.get_boolean();
    }
set_checkbox( w, ENABLE_SNAPSHOT_XBOX, b );

/*
---------------------------
Exit All Visi-Trak Programs
--------------------------- */
b = TRUE;
if ( ini.exists() )
    {
    if ( ini.find(ExitAllDefaultKey) )
        b = ini.get_boolean();
    }

set_checkbox( w, EXIT_ALL_DEFAULT_XBOX, b );

if ( ini.find(LowLimitTypeKey) )
    {
    id = LOW_LIMIT_LOW_ALARM_RADIO;
    i  = ini.get_int();
    switch( i )
        {
        case ALARM_MIN:
            id = LOW_LIMIT_LOW_ALARM_RADIO;
            break;

        case ALARM_MAX:
            id = LOW_LIMIT_HIGH_ALARM_RADIO;
            break;

        case WARNING_MIN:
            id = LOW_LIMIT_LOW_WARN_RADIO;
            break;

        case WARNING_MAX:
            id = LOW_LIMIT_HIGH_WARN_RADIO;
            break;
        }
    }
CheckRadioButton( w, LOW_LIMIT_LOW_ALARM_RADIO, LOW_LIMIT_HIGH_ALARM_RADIO, id );

if ( ini.find(HighLimitTypeKey) )
    {
    id = LOW_LIMIT_LOW_ALARM_RADIO;
    i  = ini.get_int();
    switch( i )
        {
        case ALARM_MIN:
            id = HIGH_LIMIT_LOW_ALARM_RADIO;
            break;

        case ALARM_MAX:
            id = HIGH_LIMIT_HIGH_ALARM_RADIO;
            break;

        case WARNING_MIN:
            id = HIGH_LIMIT_LOW_WARN_RADIO;
            break;

        case WARNING_MAX:
            id = HIGH_LIMIT_HIGH_WARN_RADIO;
            break;
        }
    }
CheckRadioButton( w, HIGH_LIMIT_LOW_ALARM_RADIO, HIGH_LIMIT_HIGH_ALARM_RADIO, id );

/*
------------
Visitrak.ini
------------ */
ini.set_file( ini_file_name(VisiTrakIniFile) );

set_checkbox( w, PART_BASED_DISPLAY_XBOX, ini.get_boolean(ConfigSection, PartBasedDisplayKey) );
set_checkbox( w, POPUP_DOWNTIME_XBOX,     ini.get_boolean(ConfigSection, UseDowntimeEntryKey) );
set_checkbox( w, HAVE_SURETRAK_XBOX,      ini.get_boolean(ConfigSection, HaveSureTrakKey)     );
set_checkbox( w, MULTIPART_RUNTIME_XBOX,  ini.get_boolean(ConfigSection, HaveMultipartRunlistKey) );
set_checkbox( w, NO_NETWORK_XBOX,         ini.get_boolean(ConfigSection, NoNetworkKey) );

if ( ini.find(ButtonLevelsSection, EditControlButtonKey) )
    set_text( w, EDIT_CONTROL_PROGRAM_EBOX, ini.get_string() );

if ( ini.find(ButtonLevelsSection, DowntimePurgeButtonKey) )
    set_text( w, PURGE_DOWNTIMES_EBOX, ini.get_string() );

if ( ini.find(ButtonLevelsSection, EditOperatorsButtonKey) )
    set_text( w, EDIT_OPERATORS_EBOX, ini.get_string() );

if ( ini.find( ConfigSection, PasswordLevelTimeoutKey) )
    set_text( w, PASSWORD_LEVEL_TIMEOUT_EBOX, ini.get_string() );

/*
----------
Monall.ini
---------- */
ini.set_file( ini_file_name(MonallIniFile) );
ini.set_section( ConfigSection );
set_checkbox( w, SUB_TIMEOUT_XBOX,    ini.get_boolean(SubtractTimeoutKey)   );
set_checkbox( w, SAVE_ALL_ALARM_XBOX, ini.get_boolean(SaveAllAlarmShotsKey) );
set_checkbox( w, CLEAR_MONITOR_WIRE_ON_DOWN_XBOX, ini.get_boolean(ClearMonitorWireOnDownKey) );
set_checkbox( w, CLEAR_FROM_POPUP_XBOX, ini.get_boolean(ClearDownWireFromPopupKey) );

if ( ini.find(CurrentFtValuesGrayLevelKey) )
    set_text( w, READ_CURRENT_ANALOG_EBOX, ini.get_string() );

if ( ini.find(Ioex4GrayLevelKey) )
    set_text( w, IOEX4_GRAY_LEVEL_EBOX, ini.get_string() );

if ( ini.find(DownTimeAlarmWireKey) )
    set_text( w, ALARM_ON_DOWN_WIRE_EBOX, ini.get_string() );

if ( ini.find(MonitorWireKey) )
    set_text( w, MONITOR_WIRE_EBOX, ini.get_string() );

if ( ini.find(Ioex4MenuGrayLevelKey) )
    set_text( w, SHOW_BOARD_MONITOR_LEVEL_EBOX, ini.get_string() );

/*
------------
Suretrak.ini
------------ */
ini.set_file( ini_file_name(SureTrakIniFile) );
ini.set_section( ConfigSection );

if ( ini.find(ServiceToolsGrayLevelKey) )
    set_text( w, SERVICE_TOOLS_EBOX, ini.get_string() );

if ( ini.find(StExitLevelKey) )
    set_text( w, ST_EXIT_LEVEL_EBOX, ini.get_string() );

set_checkbox( w, USE_VALVE_TEST_OUTPUT_XBOX, ini.get_boolean(UseValveTestOutputKey) );
set_checkbox( w, USE_NULL_VALVE_OUTPUT_XBOX, ini.get_boolean(UseNullValveOutputKey) );

id = WIRE_1_MULTIPART_RUNTIME_EBOX;
ini.set_section( MultipartSetupSection );
for ( i=0; i<MAX_MULTIPART_RUNLIST_WIRES; i++ )
    {
    s = MultipartBaseWireKey;
    s += int32toasc( i );

    if ( ini.find(s.text()) )
        s = ini.get_string();
    else
        s = EmptyString;
    s.set_text( w, id );
    id++;
    }

/*
------------
Editpart.ini
------------ */
ini.set_file( ini_file_name(EditPartIniFile) );
ini.set_section( ConfigSection );

set_checkbox( w, SHOW_SETUP_SHEET_XBOX, ini.get_boolean(ShowSetupSheetKey)  );
set_checkbox( w, PRINT_PARAMETERS_XBOX, ini.get_boolean(PrintParametersKey) );
set_checkbox( w, HIDE_SURETRAK_LS_WIRES_XBOX, ini.get_boolean(HideSureTrakLimitSwitchWiresKey) );

if ( ini.find(ParameterTabEditLevelKey) )
    set_text( w, CHANGE_PARAM_LEVEL_EBOX, ini.get_string() );

if ( ini.find(SetupSheetLevelKey) )
    set_text( w,  EDIT_SETUP_SHEET_EBOX, ini.get_string() );

if ( ini.find(EditOtherComputerKey) )
    set_text( w,  OTHER_COMPUTER_EBOX, ini.get_string() );

if ( ini.find(EditOtherCurrentPartKey) )
    set_text( w, OTHER_CURRENT_PART_EBOX, ini.get_string() );

if ( ini.find(EditOtherSuretrakKey) )
    set_text( w, OTHER_SURETRAK_EBOX, ini.get_string() );

if ( ini.find(EditPressureSetupLevelKey) )
    set_text( w, PC_SETUP_LEVEL_EBOX, ini.get_string() );

if ( ini.find(EditPressureProfileLevelKey) )
    set_text( w, PC_PROFILE_LEVEL_EBOX, ini.get_string() );

if ( ini.find(SaveAllPartsLevelKey) )
    set_text( w, SAVE_ALL_PARTS_LEVEL_EBOX, ini.get_string() );

if ( ini.find(SaveChangesLevelKey) )
    set_text( w, SAVE_CHANGES_LEVEL_EBOX, ini.get_string() );

if ( ini.find(VelocityChangesLevelKey) )
    set_text( w, VELOCITY_CHANGES_EBOX, ini.get_string() );

}

/***********************************************************************
*                                INI_NAME                              *
* Local version of ini_file_name() that uses the CoOptionsExeDir       *
* so I can save to all the network computers.                          *
***********************************************************************/
TCHAR * ini_name( const TCHAR * file_name )
{
static TCHAR buf[MAX_PATH+1];
TCHAR * cp;

cp = copystring( buf, CoOptionsExeDir );
copystring( cp, file_name );

return buf;
}

/***********************************************************************
*                     SAVE_CO_OPTIONS_TO_COMPUTER                      *
*                                                                      *
*        Saves the options to the ini files in CoOptionsExeDir         *
*                                                                      *
***********************************************************************/
static void save_co_options_to_computer()
{
HWND         w;
INI_CLASS    ini;
STRING_CLASS s;
STRING_CLASS keyname;
int          i;
int          id;
double       x;

w = CoOptionsWindow;

/*
-----------
Display.ini
----------- */
ini.set_file( ini_name(DisplayIniFile) );
ini.set_section( ConfigSection );

SnapshotIsActive = is_checked( w, ENABLE_SNAPSHOT_XBOX );
ini.put_boolean( SnapshotKey,       SnapshotIsActive );
ini.put_boolean( ExitAllDefaultKey, is_checked( w, EXIT_ALL_DEFAULT_XBOX) );

i  = ALARM_MIN;
if ( is_checked(w, LOW_LIMIT_HIGH_ALARM_RADIO) )
    i = ALARM_MAX;
else if ( is_checked(w, LOW_LIMIT_LOW_WARN_RADIO) )
    i = WARNING_MIN;
else if ( is_checked(w, LOW_LIMIT_HIGH_WARN_RADIO) )
    i = WARNING_MAX;
LowLimitType = i;
ini.put_int( LowLimitTypeKey, i );

i = ALARM_MAX;
if ( is_checked(w, HIGH_LIMIT_LOW_ALARM_RADIO) )
    i  = ALARM_MIN;
else if ( is_checked(w, HIGH_LIMIT_LOW_WARN_RADIO) )
    i = WARNING_MIN;
else if ( is_checked(w, HIGH_LIMIT_HIGH_WARN_RADIO) )
    i = WARNING_MAX;
HighLimitType = i;
ini.put_int( HighLimitTypeKey, i );

/*
------------
Visitrak.ini
------------ */
ini.set_file( ini_name(VisiTrakIniFile) );

PartBasedDisplay = is_checked( w, PART_BASED_DISPLAY_XBOX );
ini.put_boolean( ConfigSection, PartBasedDisplayKey,     PartBasedDisplay );
ini.put_boolean( ConfigSection, UseDowntimeEntryKey,     is_checked(w, POPUP_DOWNTIME_XBOX) );
ini.put_boolean( ConfigSection, HaveSureTrakKey,         is_checked(w, HAVE_SURETRAK_XBOX)  );
ini.put_boolean( ConfigSection, HaveMultipartRunlistKey, is_checked( w, MULTIPART_RUNTIME_XBOX) );
ini.put_boolean( ConfigSection, NoNetworkKey,            is_checked( w, NO_NETWORK_XBOX) );
s.get_text( w, PASSWORD_LEVEL_TIMEOUT_EBOX );
if ( s.isempty() )
    {
    ini.remove_key( PasswordLevelTimeoutKey );
    /*
    -------------------------------------------------------------
    I don't kill the timer here. I let the timer routine do that.
    ------------------------------------------------------------- */
    PasswordLevelTimeoutMs = 0;
    }
else
    {
    ini.put_string( PasswordLevelTimeoutKey, s.text() );
    x = s.real_value();
    x *= 60000.0;
    PasswordLevelTimeoutMs = (UINT) x;
    if ( PasswordLevelTimeoutMs > 0 )
        {
        if ( PasswordLevelTimeoutId == 0 )
            {
            PasswordLevelTimeoutId = SetTimer( MainWindow, 0, PasswordLevelTimeoutMs, 0 );
            }
        }
    }

s.get_text( w, EDIT_CONTROL_PROGRAM_EBOX );
ini.put_string( ButtonLevelsSection, EditControlButtonKey, s.text() );

s.get_text( w, PURGE_DOWNTIMES_EBOX );
ini.put_string( ButtonLevelsSection, DowntimePurgeButtonKey, s.text() );

s.get_text( w, EDIT_OPERATORS_EBOX );
ini.put_string( ButtonLevelsSection, EditOperatorsButtonKey, s.text() );

/*
----------
Monall.ini
---------- */
ini.set_file( ini_name(MonallIniFile) );

ini.put_boolean( ConfigSection, SubtractTimeoutKey,        is_checked(w, SUB_TIMEOUT_XBOX)                );
ini.put_boolean( ConfigSection, SaveAllAlarmShotsKey,      is_checked(w, SAVE_ALL_ALARM_XBOX)             );
ini.put_boolean( ConfigSection, ClearMonitorWireOnDownKey, is_checked(w, CLEAR_MONITOR_WIRE_ON_DOWN_XBOX) );
ini.put_boolean( ConfigSection, ClearDownWireFromPopupKey, is_checked(w, CLEAR_FROM_POPUP_XBOX)           );

s.get_text( w, IOEX4_GRAY_LEVEL_EBOX );
ini.put_string( ConfigSection, Ioex4GrayLevelKey, s.text() );

s.get_text( w, READ_CURRENT_ANALOG_EBOX );
ini.put_string( ConfigSection, CurrentFtValuesGrayLevelKey, s.text() );

s.get_text( w, ALARM_ON_DOWN_WIRE_EBOX );
ini.put_string( ConfigSection, DownTimeAlarmWireKey, s.text() );

s.get_text( w, MONITOR_WIRE_EBOX );
ini.put_string( ConfigSection,  MonitorWireKey, s.text() );

s.get_text( w, READ_CURRENT_ANALOG_EBOX );
ini.put_string( ConfigSection, CurrentFtValuesGrayLevelKey, s.text() );

s.get_text( w, SHOW_BOARD_MONITOR_LEVEL_EBOX );
ini.put_string( ConfigSection, Ioex4MenuGrayLevelKey, s.text() );

/*
------------
Suretrak.ini
------------ */
ini.set_file( ini_name(SureTrakIniFile) );

s.get_text( w, SERVICE_TOOLS_EBOX );
ini.put_string( ConfigSection, ServiceToolsGrayLevelKey, s.text() );

s.get_text( w, ST_EXIT_LEVEL_EBOX );
ini.put_string( ConfigSection, StExitLevelKey, s.text() );

ini.put_boolean( UseValveTestOutputKey, is_checked(w, USE_VALVE_TEST_OUTPUT_XBOX) );
ini.put_boolean( UseNullValveOutputKey, is_checked(w, USE_NULL_VALVE_OUTPUT_XBOX) );

id = WIRE_1_MULTIPART_RUNTIME_EBOX;
for ( i=0; i<MAX_MULTIPART_RUNLIST_WIRES; i++ )
    {
    keyname = MultipartBaseWireKey;
    keyname += int32toasc( i );

    s.get_text( w, id );
    ini.put_string( MultipartSetupSection, keyname.text(), s.text() );
    id++;
    }

/*
------------
Editpart.ini
------------ */
ini.set_file( ini_name(EditPartIniFile) );
ini.set_section( ConfigSection );

ini.put_boolean( ShowSetupSheetKey,               is_checked(w, SHOW_SETUP_SHEET_XBOX) );
ini.put_boolean( PrintParametersKey,              is_checked(w, PRINT_PARAMETERS_XBOX) );
ini.put_boolean( HideSureTrakLimitSwitchWiresKey, is_checked(w, HIDE_SURETRAK_LS_WIRES_XBOX) );

s.get_text( w, CHANGE_PARAM_LEVEL_EBOX );
ini.put_string( ConfigSection, ParameterTabEditLevelKey, s.text() );

s.get_text( w, EDIT_SETUP_SHEET_EBOX );
ini.put_string( ConfigSection, SetupSheetLevelKey, s.text() );

s.get_text( w, OTHER_COMPUTER_EBOX );
ini.put_string( ConfigSection, EditOtherComputerKey, s.text() );

s.get_text( w, OTHER_CURRENT_PART_EBOX );
ini.put_string( ConfigSection, EditOtherCurrentPartKey, s.text() );

s.get_text( w, OTHER_SURETRAK_EBOX );
ini.put_string( ConfigSection, EditOtherSuretrakKey, s.text() );

s.get_text( w, PC_SETUP_LEVEL_EBOX );
ini.put_string( ConfigSection, EditPressureSetupLevelKey, s.text() );

s.get_text( w, PC_PROFILE_LEVEL_EBOX );
ini.put_string( ConfigSection, EditPressureProfileLevelKey, s.text() );

s.get_text( w, SAVE_ALL_PARTS_LEVEL_EBOX );
ini.put_string( ConfigSection, SaveAllPartsLevelKey, s.text() );

s.get_text( w, SAVE_CHANGES_LEVEL_EBOX );
ini.put_string( ConfigSection, SaveChangesLevelKey, s.text() );

s.get_text( w, VELOCITY_CHANGES_EBOX );
ini.put_string( ConfigSection, VelocityChangesLevelKey, s.text() );

}

/***********************************************************************
*                           SAVE_CO_OPTIONS                            *
***********************************************************************/
static BOOLEAN save_co_options()
{
static TCHAR data_string[] = TEXT( "data" );
static TCHAR exe_dir_path_end[] = TEXT( "exes\\" );
COMPUTER_CLASS c;
TCHAR        * cp;

if ( !CoOptionsExeDir )
    CoOptionsExeDir = maketext( MAX_PATH );

if ( !CoOptionsExeDir )
    return FALSE;

get_exe_directory( CoOptionsExeDir );
save_co_options_to_computer();

if ( !is_checked( CoOptionsWindow, COPY_TO_NETWORK_XBOX) )
    return TRUE;

if ( is_checked( CoOptionsWindow, NO_NETWORK_XBOX) )
    return TRUE;

hourglass_cursor();

c.rewind();
while ( c.next() )
    {
    if ( !c.is_this_computer() )
        {
        copystring( CoOptionsExeDir, c.directory() );
        cp = findstring( data_string, CoOptionsExeDir );
        if ( cp )
            {
            copystring( cp, exe_dir_path_end );
            save_co_options_to_computer();
            }
        }
    }

delete[] CoOptionsExeDir;
CoOptionsExeDir = 0;

restore_cursor();

return TRUE;
}

/***********************************************************************
*                         CO_OPTIONS_HELP                              *
***********************************************************************/
static void co_options_help( UINT helptype, DWORD context )
{
TCHAR fname[MAX_PATH+1];

if ( get_exe_directory(fname) )
    {
    lstrcat( fname, TEXT("v5help.hlp") );
    WinHelp( EnterWindow, fname, helptype, context );
    }
}

/***********************************************************************
*                          SHOW_CONTEXT_HELP                           *
***********************************************************************/
static BOOL show_context_help( HWND w, LPARAM lParam )
{
struct ID_CONTEXT
    {
    UINT id;
    DWORD context;
    };

static ID_CONTEXT control[] = {
    { PART_BASED_DISPLAY_XBOX,         OPTIONS_USE_PART_BASED_HELP },
    { POPUP_DOWNTIME_XBOX,             OPTIONS_DOWNTIME_ENTRY_HELP },
    { ENABLE_SNAPSHOT_XBOX,            OPTIONS_SNAPSHOT_HELP       },
    { EXIT_ALL_DEFAULT_XBOX,           OPTIONS_EXIT_ALL_HELP       },
    { HAVE_SURETRAK_XBOX,              OPTIONS_HAVE_SURETRAK_HELP  },
    { USE_VALVE_TEST_OUTPUT_XBOX,      OPTIONS_VALVE_TEST_OUTPUT_HELP },
    { USE_NULL_VALVE_OUTPUT_XBOX,      OPTIONS_NULL_VALVE_OUTPUT_HELP },
    { CLEAR_MONITOR_WIRE_ON_DOWN_XBOX, OPTIONS_CLEAR_MONITOR_WIRE_ON_DOWN_HELP },
    { HIDE_SURETRAK_LS_WIRES_XBOX,     OPTIONS_HIDE_SURETRAK_LS_WIRES_HELP },
    { SAVE_ALL_ALARM_XBOX,             OPTIONS_SAVE_ALL_ALARM_HELP    },
    { SHOW_SETUP_SHEET_XBOX,           OPTIONS_SHOW_SETUP_SHEET_HELP  },
    { SUB_TIMEOUT_XBOX,                OPTIONS_SUB_TIMEOUT_HELP       },
    { MULTIPART_RUNTIME_XBOX,          OPTIONS_MULTIPART_RUNLIST_HELP },
    { PRINT_PARAMETERS_XBOX,           OPTIONS_PRINT_PARAMETERS_HELP  },
    { CLEAR_FROM_POPUP_XBOX,           OPTIONS_CLEAR_FROM_POPUP_HELP  },
    { COPY_TO_NETWORK_XBOX,            OPTIONS_COPY_TO_NETWORK_HELP   },
    { NO_NETWORK_XBOX,                 OPTIONS_NO_NETWORK_HELP        }
    };

const int NOF_CONTROLS = sizeof(control)/sizeof(ID_CONTEXT);

static ID_CONTEXT static_control[] = {
    { VELOCITY_CHANGES_TBOX,          OPTIONS_VELOCITY_CHANGES_HELP          },
    { MONITOR_WIRE_TBOX,              OPTIONS_MONITOR_WIRE_HELP              },
    { PURGE_DOWNTIMES_TBOX,           OPTIONS_PURGE_DOWNTIMES_HELP           },
    { EDIT_OPERATORS_TBOX,            OPTIONS_EDIT_OPERATORS_HELP            },
    { READ_CURRENT_ANALOG_TBOX,       OPTIONS_READ_CURRENT_ANALOG_HELP       },
    { SHOW_BOARD_MONITOR_LEVEL_TBOX,  OPTIONS_SHOW_BOARD_MONITOR_LEVEL_HELP  },
    { IOEX4_GRAY_LEVEL_TBOX,          OPTIONS_IOEX4_GRAY_LEVEL_HELP          },
    { EDIT_CONTROL_PROGRAM_TBOX,      OPTIONS_EDIT_CONTROL_PROGRAM_HELP      },
    { SERVICE_TOOLS_TBOX,             OPTIONS_SERVICE_TOOLS_HELP             },
    { CHANGE_PARAM_LEVEL_TBOX,        OPTIONS_CHANGE_PARAM_LEVEL_HELP        },
    { OTHER_COMPUTER_TBOX,            OPTIONS_OTHER_COMPUTER_HELP            },
    { OTHER_CURRENT_PART_TBOX,        OPTIONS_OTHER_CURRENT_PART_HELP        },
    { OTHER_SURETRAK_TBOX,            OPTIONS_OTHER_SURETRAK_HELP            },
    { EDIT_SETUP_SHEET_TBOX,          OPTIONS_EDIT_SETUP_SHEET_HELP          },
    { ALARM_ON_DOWN_WIRE_TBOX,        OPTIONS_ALARM_ON_DOWN_HELP             },
    { WIRE_1_TBOX,                    OPTIONS_MULTIPART_WIRE_HELP            },
    { WIRE_2_TBOX,                    OPTIONS_MULTIPART_WIRE_HELP            },
    { WIRE_3_TBOX,                    OPTIONS_MULTIPART_WIRE_HELP            },
    { PASSWORD_LEVEL_TIMEOUT_TBOX,    OPTIONS_PASSWORD_LEVEL_TIMEOUT_HELP    },
    { SAVE_CHANGES_LEVEL_TBOX,        OPTIONS_SAVE_CHANGES_HELP              },
    { SAVE_ALL_PARTS_LEVEL_TBOX,      OPTIONS_SAVE_ALL_PARTS_HELP            }
    };

const int NOF_STATIC_CONTROLS = sizeof(static_control)/sizeof(ID_CONTEXT);

int32 i;
POINT p;
HWND  sw;
RECT  r;

if ( !CoOptionsWindow )
    return FALSE;

p.x = LOWORD(lParam);
p.y = HIWORD(lParam);

for ( i=0; i<NOF_STATIC_CONTROLS; i++ )
    {
    sw = GetDlgItem( CoOptionsWindow, static_control[i].id );
    if ( GetWindowRect(sw, &r) )
        {
        if ( PtInRect(&r, p) )
            {
            co_options_help( HELP_CONTEXTPOPUP, static_control[i].context );
            return TRUE;
            }
        }
    }

for ( i=0; i<NOF_CONTROLS; i++ )
    {
    if ( GetDlgItem(CoOptionsWindow, control[i].id) == w )
        {
        co_options_help( HELP_CONTEXTPOPUP, control[i].context );
        return TRUE;
        }
    }

return TRUE;
}

/***********************************************************************
*                        CO_OPTIONS_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK co_options_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
int cmd;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        CoOptionsWindow = hWnd;
        fill_co_options();
        break;

    case WM_CONTEXTMENU:
        return show_context_help( (HWND) wParam, lParam );

    case WM_HELP:
        gethelp( HELP_CONTEXT, OPTIONS_EDIT_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                save_co_options();

            case IDCANCEL:
                exit_co_options();
                return TRUE;

            }
        break;

    case WM_CLOSE:
        exit_co_options();
        return TRUE;
    }

return FALSE;
}



/***********************************************************************
*                        ENABLE_EDIT_PASSWORDS                         *
***********************************************************************/
static void enable_edit_passwords( int32 password_level )
{
WINDOW_CLASS w;
BOOLEAN is_enabled;
int32   required_level;

required_level = Password.highest_level();

if ( password_level >= required_level )
    is_enabled = TRUE;
else
    is_enabled = FALSE;

w = GetDlgItem( EnterWindow, EDIT_PASSWORDS_BUTTON );
w.enable( is_enabled );

w = GetDlgItem( EnterWindow, CO_OPTIONS_BUTTON );
if ( is_enabled )
    w.show();
else
    w.hide();
}

/***********************************************************************
*                          SET_PASSWORD_LEVEL                          *
* Return TRUE if the level has changed from less than the highest to   *
* the highest level.                                                   *
***********************************************************************/
static BOOLEAN set_password_level( void )
{
BOOLEAN b;
TCHAR s[PASSWORD_LEN+1];
int32 i;
int32 new_password_level;
int32 highest_password_level;

b = FALSE;
highest_password_level = Password.highest_level();

if ( get_text( s, EnterWindow, ENTER_PASSWORD_EBOX, PASSWORD_LEN)  )
    {
    i = Password.find(s);
    if ( i == NO_INDEX )
        {
        resource_message_box( MainInstance, PASSWORD_NOT_FOUND_STRING, CANNOT_COMPLY_STRING );
        }
    else
        {
        new_password_level = Password[i].level;
        if ( CurrentPasswordLevel < highest_password_level && new_password_level >= highest_password_level )
            {
            b = TRUE;
            /*
            -----------------------------------------------------------------
            Update the current level box as I am not going to kill myself now
            ----------------------------------------------------------------- */
            set_text( EnterWindow, CURRENT_PASSWORD_LEVEL_TBOX, int32toasc(new_password_level) );
            set_text( EnterWindow, ENTER_PASSWORD_EBOX, EmptyString );
            }
        poke_data( DDE_CONFIG_TOPIC, ItemList[PW_LEVEL_INDEX].name, int32toasc(Password[i].level) );
        enable_edit_passwords( Password[i].level );
        log_password( Password[i].owner );
        }
    }

return b;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
/***********************************************************************
*                          ENTER_PASSWORD_PROC                         *
***********************************************************************/
BOOL CALLBACK enter_password_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
HWND w;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        EnterWindow = hWnd;
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        Password.load();
        set_text( hWnd, CURRENT_PASSWORD_LEVEL_TBOX, int32toasc(CurrentPasswordLevel) );
        w = GetDlgItem( EnterWindow, ENTER_PASSWORD_EBOX );
        set_text( w, EmptyString );
        SetFocus( w );
        enable_edit_passwords( CurrentPasswordLevel );
        return TRUE;

    case WM_CONTEXTMENU:
    case WM_HELP:
        gethelp( HELP_CONTEXT, PASSWORD_ENTRY_HELP );
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case EDIT_PASSWORDS_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("EDIT_PASSWORDS_DIALOG"),
                    MainPlotWindow,
                    (DLGPROC) edit_passwords_proc );
                return TRUE;

            case CO_OPTIONS_BUTTON:
                DialogBox(
                    MainInstance,
                    TEXT("CO_OPTIONS_DIALOG"),
                    MainPlotWindow,
                    (DLGPROC) co_options_dialog_proc );
                return TRUE;

            case IDOK:
                if ( set_password_level() )
                    return TRUE;

            case IDCANCEL:
                EnterWindow = 0;
                DialogIsActive = FALSE;
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        EnterWindow = 0;
        DialogIsActive = FALSE;
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}
