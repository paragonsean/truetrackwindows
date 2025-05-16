#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\machine.h"
#include "..\include\subs.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\computer.h"
#include "..\include\updown.h"
#include "..\include\verrors.h"

#include "resource.h"
#include "category.h"

#define _MAIN_
#include "extern.h"
#include "..\include\events.h"

const int DECIMAL_COLOR_LEN = 3;  /* 0-255 as in R in RGB */
const int LB_LINE_LEN   = 2 *(DOWNCAT_NUMBER_LEN + DOWNCAT_NAME_LEN) + 3;

const int NOF_EDITBOXES = BLUE_EBOX - CAT_INDEX_EDITBOX + 1;

HINSTANCE MainInstance;
HWND      MainWindow;
HWND      DownTimeSetupWindow;
HWND      ColorSwatch = 0;

/*
---------------------------
RGB components (edit boxes)
--------------------------- */
static HWND RedEbox    =  0;
static HWND RedHS      =  0;
static int  RedPos     = -1;

static HWND GreenEbox  =  0;
static HWND GreenHS    =  0;
static int  GreenPos   = -1;

static HWND BlueEbox   =  0;
static HWND BlueHS     =  0;
static int  BluePos    = -1;

static HWND MoveAllHS  =  0;
static int  MoveAllPos = -1;

TCHAR MyClassName[]     = TEXT( "DownTime" );
STRING_CLASS MyWindowTitle;

static TCHAR   NullChar = TEXT('\0');
static TCHAR   TabChar  = TEXT('\t');
static TCHAR   ZeroChar = TEXT('0');
static TCHAR   YChar    = TEXT( 'Y' );
static TCHAR   NStr[]   = TEXT( "N" );
static TCHAR   YStr[]   = TEXT( "Y" );
static TCHAR   EmptyString[] = TEXT( "" );

static BOOLEAN ProgramIsUpdating = FALSE;

static WNDPROC OldEditProc[NOF_EDITBOXES];

static const TCHAR ConfigSection[]   = TEXT( "Config" );
static const TCHAR MonallIniFile[]   = TEXT( "monall.ini" );
static const TCHAR SubtractTimeoutKey[] = TEXT( "SubtractTimeout" );
static const TCHAR SaveDowntimeSetupsToNetworkKey[] = TEXT( "SaveDowntimeSetupsToNetwork" );

class DOWNTIME_CLASS
{
private:

CATEGORY_ENTRY cat;
CATEGORY_ENTRY subcat;
void init( void ){ *cat.number = NullChar; *cat.name = NullChar; *subcat.number=NullChar; *subcat.name=NullChar; color=0; }

public:
COLORREF color;

DOWNTIME_CLASS( void ) { init(); }

TCHAR  * cat_number( void )    { return cat.number; }
TCHAR  * cat_name( void )      { return cat.name; }
TCHAR  * subcat_number( void ) { return subcat.number; }
TCHAR  * subcat_name( void )   { return subcat.name; }

void    inc_subcat_number( void );
void    fill_editboxes( void );
TCHAR * lbline( void);
BOOLEAN read_editboxes( void );
BOOLEAN read( CATEGORY_CLASS & c, DB_TABLE & t );
BOOLEAN read( TCHAR * lb_sorc_line );
BOOLEAN saveall( void );
void    operator=(DOWNTIME_CLASS & sorc );
};

/***********************************************************************
*                             SET_TAB_STOPS                            *
***********************************************************************/
static void set_tab_stops( void )
{
const  int NOF_TABS  = 3;
static int tabs[NOF_TABS];
int        x;

x = LOWORD( GetDialogBaseUnits() );
x /= 2;

tabs[0] = (DOWNCAT_NUMBER_LEN + 4) * x;
tabs[1] = tabs[0] + (DOWNCAT_NAME_LEN + 4) * x;
tabs[2] = tabs[1] + (DOWNCAT_NUMBER_LEN + 4) * x;

SendDlgItemMessage( DownTimeSetupWindow, SUBCAT_LISTBOX, LB_SETTABSTOPS, (WPARAM) NOF_TABS, (LPARAM) tabs );
}

/***********************************************************************
*                           GREEN_COMPONENT                            *
***********************************************************************/
static int green_component( COLORREF c )
{
int green;

green = c >> 8;
green &= 0xFF;
return green;
}

/***********************************************************************
*                            BLUE_COMPONENT                            *
***********************************************************************/
static int blue_component( COLORREF c )
{
int blue;

blue = c >> 16;
blue &= 0xFF;
return blue;
}

/***********************************************************************
*                             RED_COMPONENT                            *
***********************************************************************/
static int red_component( COLORREF c )
{
int red;

red = c & 0xFF;
return red;
}

/***********************************************************************
*                           COLOR_TO_EBOXES                            *
***********************************************************************/
static void color_to_eboxes( COLORREF new_color )
{
int c;
int high;
int low;
HWND focus_window;

low  = 255;
high = 0;

focus_window = GetFocus();

c = red_component( new_color );
if ( c > high )
    high = c;
if ( c < low )
    low = c;
set_text( RedEbox, int32toasc(c) );
if ( focus_window != RedEbox )
    {
    SendMessage( RedHS, SBM_SETPOS, (WPARAM) c, (LPARAM) TRUE );
    RedPos = c;
    }

c = green_component( new_color );
if ( c > high )
    high = c;
if ( c < low )
    low = c;

set_text( GreenEbox, int32toasc(c) );
if ( focus_window != GreenEbox )
    {
    SendMessage( GreenHS, SBM_SETPOS, (WPARAM) c, (LPARAM) TRUE );
    GreenPos = c;
    }

c = blue_component( new_color );
if ( c > high )
    high = c;
if ( c < low )
    low = c;
set_text( BlueEbox, int32toasc(c) );
if ( focus_window != BlueEbox )
    {
    SendMessage( BlueHS, SBM_SETPOS, (WPARAM) c, (LPARAM) TRUE );
    BluePos = c;
    }

c = ( low + high ) / 2;
if ( MoveAllPos != c )
    {
    SendMessage( MoveAllHS, SBM_SETPOS, (WPARAM) c, (LPARAM) TRUE );
    MoveAllPos = c;
    }

InvalidateRect( ColorSwatch, 0, FALSE );
}

/***********************************************************************
*                          COLOR_FROM_EBOXES                           *
***********************************************************************/
static COLORREF color_from_eboxes()
{
STRING_CLASS s;
COLORREF c;

s.get_text( BlueEbox );
c = s.uint_value();
c <<= 8;

s.get_text( GreenEbox );
c |= s.uint_value();
c <<= 8;

s.get_text( RedEbox );
c |= s.uint_value();

return c;
}

/***********************************************************************
*                         DOWNTIME_CLASS::READ                         *
***********************************************************************/
BOOLEAN DOWNTIME_CLASS::read( CATEGORY_CLASS & c, DB_TABLE & t )
{
TCHAR buf[DOWNSCATC_COLOR_LEN+1];

t.get_alpha( cat.number, DOWNSCAT_CAT_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );
lstrcpyn( cat.name, c.name(cat.number), DOWNCAT_NAME_LEN+1 );

t.get_alpha( subcat.number, DOWNSCAT_SUB_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );
t.get_alpha( subcat.name,   DOWNSCAT_SUB_NAME_OFFSET,   DOWNCAT_NAME_LEN   );
if ( t.record_length() == DOWNSCATC_RECLEN )
    {
    t.get_alpha( buf, DOWNSCATC_COLOR_OFFSET, DOWNSCATC_COLOR_LEN   );
    color = (COLORREF) hextoul( buf );
    }

return TRUE;
}

/***********************************************************************
*                         DOWNTIME_CLASS::READ                         *
***********************************************************************/
BOOLEAN DOWNTIME_CLASS::read( TCHAR * lb_sorc_line )
{
TCHAR s[LB_LINE_LEN+1];
TCHAR * cp;

lstrcpyn( s, lb_sorc_line, LB_LINE_LEN+1 );

init();

cp = s;
if ( replace_tab_with_null(cp) )
    {
    lstrcpyn( cat.number, cp, DOWNCAT_NUMBER_LEN+1 );
    cp = nextstring( cp );
    if ( replace_tab_with_null(cp) )
        {
        lstrcpyn( cat.name, cp, DOWNCAT_NAME_LEN+1 );
        cp = nextstring( cp );
        if ( replace_tab_with_null(cp) )
            {
            lstrcpyn( subcat.number, cp, DOWNCAT_NUMBER_LEN+1 );
            cp = nextstring( cp );
            lstrcpyn( subcat.name, cp, DOWNCAT_NAME_LEN+1 );
            return TRUE;
            }
        }
    }

return FALSE;
}

/***********************************************************************
*                            READ_EDITBOXES                            *
***********************************************************************/
BOOLEAN DOWNTIME_CLASS::read_editboxes( void )
{
init();

get_text( cat.number,    DownTimeSetupWindow, CAT_INDEX_EDITBOX,    DOWNCAT_NUMBER_LEN );
get_text( cat.name,      DownTimeSetupWindow, CAT_NAME_EDITBOX,     DOWNCAT_NAME_LEN   );
get_text( subcat.number, DownTimeSetupWindow, SUBCAT_INDEX_EDITBOX, DOWNCAT_NUMBER_LEN );
get_text( subcat.name,   DownTimeSetupWindow, SUBCAT_NAME_EDITBOX,  DOWNCAT_NAME_LEN   );
color = color_from_eboxes();

return TRUE;
}

/***********************************************************************
*                            FILL_EDITBOXES                            *
***********************************************************************/
void DOWNTIME_CLASS::fill_editboxes( void )
{
set_text( DownTimeSetupWindow, CAT_INDEX_EDITBOX,    cat.number    );
set_text( DownTimeSetupWindow, CAT_NAME_EDITBOX,     cat.name      );
set_text( DownTimeSetupWindow, SUBCAT_INDEX_EDITBOX, subcat.number );
set_text( DownTimeSetupWindow, SUBCAT_NAME_EDITBOX,  subcat.name   );
color_to_eboxes( color );
}

/***********************************************************************
*                         DOWNTIME_CLASS::LBLINE                       *
***********************************************************************/
TCHAR * DOWNTIME_CLASS::lbline( void )
{
static TCHAR s[LB_LINE_LEN+1];
TCHAR * cp;

cp = s;

cp = copy_w_char( cp, cat.number,    TabChar );
cp = copy_w_char( cp, cat.name,      TabChar );
cp = copy_w_char( cp, subcat.number, TabChar );
copystring(  cp, subcat.name );

return s;
}

/***********************************************************************
*                             OPERATOR =                               *
***********************************************************************/
void DOWNTIME_CLASS::operator=(DOWNTIME_CLASS & sorc )
{
lstrcpy( cat.number, sorc.cat.number );
lstrcpy( cat.name, sorc.cat.name );

lstrcpy( subcat.number, sorc.subcat.number );
lstrcpy( subcat.name, sorc.subcat.name );

color = sorc.color;
}

/***********************************************************************
*                  DOWNTIME_CLASS::INC_SUBCAT_NUMBER                   *
***********************************************************************/
void DOWNTIME_CLASS::inc_subcat_number( void )
{
int32  x;

x = asctoint32( subcat.number );
x++;
insalph( subcat.number, x, DOWNCAT_NUMBER_LEN, ZeroChar, DECIMAL_RADIX );
*(subcat.number+DOWNCAT_NUMBER_LEN) = NullChar;
}

/***********************************************************************
*                         DOWNTIME_CLASS::SAVEALL                      *
***********************************************************************/
BOOLEAN DOWNTIME_CLASS::saveall( void )
{
DB_TABLE       t;
INT            i;
INT            nof_rows;
CATEGORY_CLASS c;
LISTBOX_CLASS  lb;
TCHAR        * s;

lb.init( DownTimeSetupWindow, SUBCAT_LISTBOX );

if ( !c.save(lb) )
    return FALSE;

nof_rows = lb.count();
if ( nof_rows <= 0 )
    return FALSE;

s = downscatc_dbname();
if ( !file_exists(s) )
    t.create( s );

if ( t.open(s, DOWNSCATC_RECLEN, FL) )
    {
    t.empty();
    for ( i=0; i<nof_rows; i++ )
        {
        if ( read(lb.item_text(i)) )
            {
            t.put_alpha( DOWNSCAT_CAT_NUMBER_OFFSET, cat.number,    DOWNCAT_NUMBER_LEN );
            t.put_alpha( DOWNSCAT_SUB_NUMBER_OFFSET, subcat.number, DOWNCAT_NUMBER_LEN );
            t.put_alpha( DOWNSCAT_SUB_NAME_OFFSET,   subcat.name,   DOWNCAT_NAME_LEN );
            color = (COLORREF) lb.get_item_data(i);
            t.put_alpha( DOWNSCATC_COLOR_OFFSET, ultohex((unsigned long) color),   DOWNSCATC_COLOR_LEN   );
            t.rec_append();
            }
        }
    t.close();
    }

return FALSE;
}

/***********************************************************************
*                              DO_SELECT                               *
***********************************************************************/
static void do_select()
{
LISTBOX_CLASS lb;
DOWNTIME_CLASS d;

lb.init( DownTimeSetupWindow, SUBCAT_LISTBOX );

d.read( lb.selected_text() );
d.color = (COLORREF) lb.get_item_data();
ProgramIsUpdating = TRUE;

d.fill_editboxes();

ProgramIsUpdating = FALSE;
}

/***********************************************************************
*                             FILL_LISTBOX                             *
***********************************************************************/
static void  fill_listbox( void )
{
CATEGORY_CLASS c;
DOWNTIME_CLASS d;
LISTBOX_CLASS  lb;
DB_TABLE       t;
short          rlen;
int            x;
TCHAR        * s;

c.read();
lb.init( DownTimeSetupWindow, SUBCAT_LISTBOX );
lb.empty();

s = downscatc_dbname();
if ( file_exists(s) )
    {
    rlen = DOWNSCATC_RECLEN;
    }
else
    {
    rlen = DOWNSCAT_RECLEN;
    s = downscat_dbname();
    }


if ( t.open(s, rlen, PFL) )
    {
    while ( t.get_next_record(NO_LOCK) )
        {
        d.read( c, t );
        x = lb.add( d.lbline() );
        lb.set_item_data( x, (int) d.color );
        }
    t.close();
    }
lb.setcursel( 0 );
do_select();
}

/***********************************************************************
*                           INIT_CHECKBOXES                            *
***********************************************************************/
static void init_checkboxes()
{
BOOLEAN need_check;

need_check = FALSE;

if ( *get_ini_string(MonallIniFile, ConfigSection, SubtractTimeoutKey) == YChar )
    need_check = TRUE;

set_checkbox( DownTimeSetupWindow, SUBTRACT_TIMEOUT_XBOX, need_check );

if ( *get_ini_string(MonallIniFile, ConfigSection, SaveDowntimeSetupsToNetworkKey) == YChar )
    set_checkbox( DownTimeSetupWindow, COPY_TO_NETWORK_XBOX, TRUE );
}

/***********************************************************************
*                        REFRESH_LISTBOX_ENTRY                         *
***********************************************************************/
static void refresh_listbox_entry()
{
DOWNTIME_CLASS d;
LISTBOX_CLASS  lb;

if ( ProgramIsUpdating )
    return;

lb.init( DownTimeSetupWindow, SUBCAT_LISTBOX );
d.read_editboxes();
lb.replace( d.lbline() );
lb.set_item_data( (int) d.color );
}

/***********************************************************************
*                              DO_INSERT                               *
***********************************************************************/
static void do_insert()
{
DOWNTIME_CLASS d1;
DOWNTIME_CLASS d2;
LISTBOX_CLASS  lb;
INT            i;
INT            last_index;

lb.init( DownTimeSetupWindow, SUBCAT_LISTBOX );

last_index = lb.count() - 1;
i          = lb.current_index();
d2.read( lb.selected_text() );
if ( i < last_index )
    {
    if ( i > 0 )
        {
        d1.read( lb.item_text(i-1) );
        if ( lstrcmp( d1.cat_number(), d2.cat_number() ) != 0 )
            {
            d1.inc_subcat_number();
            d2 = d1;
            }
        else if ( i < last_index )
            {
            d1 = d2;
            i++;
            d2.read( lb.item_text(i) );
            if ( lstrcmp( d1.cat_number(), d2.cat_number() ) != 0 )
                {
                d1.inc_subcat_number();
                d2 = d1;
                }
            }

        }
    i = lb.insert( i, d2.lbline() );
    }
else
    {
    d2.inc_subcat_number();
    i = lb.add( d2.lbline() );
    }

lb.set_item_data( i, d2.color );
lb.setcursel( i );
do_select();
/* SetFocus( GetDlgItem(DownTimeSetupWindow, CAT_INDEX_EDITBOX) ); */
}

/***********************************************************************
*                 SAVE_DOWNTIME_SETUP_FILES_TO_NETWORK                 *
***********************************************************************/
static void save_downtime_setup_files_to_network()
{
static TCHAR DATA_STRING[] = TEXT( "data\\" );
COMPUTER_CLASS c;
STRING_CLASS dest;
STRING_CLASS catsorc;
STRING_CLASS scatsorc;

catsorc  = downcat_dbname();
scatsorc = downscatc_dbname();

c.rewind();
while ( c.next() )
    {
    if ( !c.is_this_computer() )
        {
        dest = c.directory();
        dest.cat_path( DATA_STRING );
        dest.cat_path( DOWNCAT_DB );
        CopyFile( catsorc.text(), dest.text(), FALSE );

        dest = c.directory();
        dest.cat_path( DATA_STRING );
        dest.cat_path( DOWNSCATC_DB );
        CopyFile( scatsorc.text(), dest.text(), FALSE );
        }
    }
}

/***********************************************************************
*                                DO_SAVE                               *
***********************************************************************/
static void do_save( void )
{
DOWNTIME_CLASS d;
TCHAR * cp;

d.saveall();

if ( is_checked(DownTimeSetupWindow, SUBTRACT_TIMEOUT_XBOX) )
    cp = YStr;
else
    cp = NStr;

put_ini_string( MonallIniFile, ConfigSection, SubtractTimeoutKey, cp );

if ( is_checked(DownTimeSetupWindow, COPY_TO_NETWORK_XBOX) )
    {
    cp = YStr;
    save_downtime_setup_files_to_network();
    }
else
    {
    cp = NStr;
    }

put_ini_string( MonallIniFile, ConfigSection, SaveDowntimeSetupsToNetworkKey, cp );
}

/***********************************************************************
*                              DO_REMOVE                               *
***********************************************************************/
static void do_remove()
{
LISTBOX_CLASS  lb;
INT            i;
INT            last_index;

lb.init( DownTimeSetupWindow, SUBCAT_LISTBOX );

last_index = lb.count() - 1;
if ( last_index > 0 )
    {
    i = lb.current_index();
    lb.remove( i );
    last_index--;
    if ( i > last_index )
        i = last_index;
    lb.setcursel( i );
    do_select();
    }
}

/***********************************************************************
*                              SUB_PROC                                *
* Subclass the four edit controls so if the operator presses an up     *
* or down arrow, the current selection in the listbox changes.         *
***********************************************************************/
LRESULT CALLBACK sub_proc( HWND w, UINT msg, WPARAM wParam, LPARAM lParam )
{
int  i;
LISTBOX_CLASS lb;

if ( msg == WM_KEYDOWN )
    {
    if ( wParam == VK_UP || wParam == VK_DOWN )
        {
        lb.init( DownTimeSetupWindow, SUBCAT_LISTBOX );
        i = lb.current_index();
        if ( wParam == VK_UP && i > 0 )
            i--;
        else if ( wParam == VK_DOWN && i < (lb.count() - 1) )
            i++;
        else
            return 0;

        lb.setcursel( i );
        do_select();
        SetFocus( w );
        eb_select_all( w );
        return 0;
        }

    }

i = GetWindowLong( w, GWL_ID ) - CAT_INDEX_EDITBOX;
return CallWindowProc( OldEditProc[i], w, msg, wParam, lParam );
}

/***********************************************************************
*                          SUB_CLASS_CONTROLS                          *
***********************************************************************/
static void sub_class_controls( HWND parent )
{
int i;
HWND w;

for ( i=0; i<NOF_EDITBOXES; i++ )
    {
    w = GetDlgItem( parent, CAT_INDEX_EDITBOX+i );
    OldEditProc[i] = (WNDPROC) SetWindowLong( w, GWL_WNDPROC, (LONG) sub_proc );
    }
}

/***********************************************************************
*                      UPDATE_TRACKBAR_FROM_EBOX                       *
***********************************************************************/
static void update_trackbar_from_ebox( int id )
{
HWND e;
HWND t;
STRING_CLASS s;
unsigned pos;
LISTBOX_CLASS lb;

e = 0;
switch( id )
    {
    case RED_EBOX:
        e = RedEbox;
        t = RedHS;
        break;

    case GREEN_EBOX:
        e = GreenEbox;
        t = GreenHS;
        break;

    case BLUE_EBOX:
        e = BlueEbox;
        t = BlueHS;
        break;
    }

if ( e )
    {
    if ( e == GetFocus() )
        {
        s.get_text( e );
        pos = s.uint_value();
        if ( pos > 255 )
            {
            s = TEXT("255");
            s.set_text( e );
            }
        else
            {
            SendMessage(t, SBM_SETPOS, (WPARAM) pos, (LPARAM) TRUE );
            InvalidateRect( ColorSwatch, 0, FALSE );
            }
        }

    lb.init( DownTimeSetupWindow, SUBCAT_LISTBOX );
    lb.set_item_data( (int) color_from_eboxes() );
    }
}


/***********************************************************************
*                      DRAW_SUBCAT_COLOR                               *
***********************************************************************/
static void draw_subcat_color( DRAWITEMSTRUCT * dp )
{
HBRUSH   oldbrush;
HBRUSH   mybrush;
RECT     r;
HDC      dc;
COLORREF c;

c = color_from_eboxes();

dc = dp->hDC;
r  = dp->rcItem;

mybrush  = (HBRUSH) CreateSolidBrush( c );
oldbrush = (HBRUSH) SelectObject( dc, mybrush );

FillRect( dc, &r, mybrush );

SelectObject( dc, oldbrush );
DeleteObject( mybrush );
}

/***********************************************************************
*                              DO_HSCROLL                              *
*                      This is only for the sliders                    *
***********************************************************************/
static void do_hscroll( HWND w, int pos )
{
HWND   dest_ebox;
int   * p;

if ( w == RedHS )
    {
    dest_ebox = RedEbox;
    p = &RedPos;
    }
else if ( w == GreenHS )
    {
    dest_ebox = GreenEbox;
    p = &GreenPos;
    }
else if ( w == BlueHS )
    {
    dest_ebox = BlueEbox;
    p = &BluePos;
    }

if ( *p != pos )
    {
    *p = pos;
    set_text( dest_ebox, int32toasc((int32) pos) );
    SendMessage( w, SBM_SETPOS, (WPARAM) pos, (LPARAM) TRUE );
    MoveAllPos = ( RedPos + GreenPos + BluePos )/3;
    SendMessage( MoveAllHS, SBM_SETPOS, (WPARAM) MoveAllPos, (LPARAM) TRUE );
    InvalidateRect( ColorSwatch, 0, FALSE );
    }
}

/***********************************************************************
*                             DO_INC_DEC                               *
*                This handles the horizontal scroll bar                *
***********************************************************************/
static void do_inc_dec( int new_position )
{
int big;
int delta;
int i;
int x;

if ( new_position == MoveAllPos )
    return;

delta = new_position - MoveAllPos;
MoveAllPos = new_position;

if ( ProgramIsUpdating )
    return;

big   = 0;
if ( delta < 0 )
    big = 255;

x = 0;
for ( i=0; i<3; i++ )
    {
    switch ( i )
        {
        case 0:
            x = RedPos;
            break;
        case 1:
            x = GreenPos;
            break;
        case 2:
            x = BluePos;
            break;
        }

    if ( delta > 0 )
        {
        if ( x > big )
            big = x;
        }
    else
        {
        if ( x < big )
            big = x;
        }
    }

if ( big < 1 || big > 254 )
    return;

if ( delta < 0 )
    {
    big = -big;
    if ( big > delta )
       delta = big;
    }
else if ( big > (255- delta) )
    {
    delta = 255-big;
    }

RedPos   += delta;
GreenPos += delta;
BluePos  += delta;

set_text( RedEbox, int32toasc((int32) RedPos) );
SendMessage( RedHS, SBM_SETPOS, (WPARAM) RedPos, (LPARAM) TRUE );

set_text( GreenEbox, int32toasc((int32) GreenPos) );
SendMessage( GreenHS, SBM_SETPOS, (WPARAM) GreenPos, (LPARAM) TRUE );

set_text( BlueEbox, int32toasc((int32) BluePos) );
SendMessage( BlueHS, SBM_SETPOS, (WPARAM) BluePos, (LPARAM) TRUE );

InvalidateRect( ColorSwatch, 0, FALSE );
}

/***********************************************************************
*                        DOWNTIME_SETUP_PROC                           *
***********************************************************************/
BOOL CALLBACK downtime_setup_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

int   id;
int   cmd;
int   pos;
int * p;
HWND  w;

id  = LOWORD( wParam );
cmd = HIWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        DownTimeSetupWindow = hWnd;
        sub_class_controls( hWnd );
        ColorSwatch = GetDlgItem( hWnd, COLOR_SWATCH );
        RedEbox   = GetDlgItem( hWnd, RED_EBOX );
        GreenEbox = GetDlgItem( hWnd, GREEN_EBOX );
        BlueEbox  = GetDlgItem( hWnd, BLUE_EBOX );
        set_text( RedEbox, 0 );
        SendMessage( RedEbox, EM_LIMITTEXT, 3, 0 );
        set_text( GreenEbox, 0 );
        SendMessage( GreenEbox, EM_LIMITTEXT, 3, 0 );
        set_text( BlueEbox, 0 );
        SendMessage( BlueEbox, EM_LIMITTEXT, 3, 0 );

        RedHS   = GetDlgItem( hWnd, RED_HS );
        SendMessage( RedHS, SBM_SETRANGE, (WPARAM) 0, (LPARAM) 255 );

        GreenHS = GetDlgItem( hWnd, GREEN_HS );
        SendMessage( GreenHS, SBM_SETRANGE, (WPARAM) 0, (LPARAM) 255 );

        BlueHS  = GetDlgItem( hWnd, BLUE_HS );
        SendMessage( BlueHS, SBM_SETRANGE, (WPARAM) 0, (LPARAM) 255 );

        MoveAllHS = GetDlgItem( hWnd, MOVEALL_HS );
        SendMessage( MoveAllHS, SBM_SETRANGE, (WPARAM) 0, (LPARAM) 255 );

        shrinkwrap( MainWindow, hWnd );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        return TRUE;

    case WM_DBINIT:
        limit_text( DownTimeSetupWindow, CAT_INDEX_EDITBOX,    DOWNCAT_NUMBER_LEN );
        limit_text( DownTimeSetupWindow, CAT_NAME_EDITBOX,     DOWNCAT_NAME_LEN   );
        limit_text( DownTimeSetupWindow, SUBCAT_INDEX_EDITBOX, DOWNCAT_NUMBER_LEN );
        limit_text( DownTimeSetupWindow, SUBCAT_NAME_EDITBOX,  DOWNCAT_NAME_LEN   );
        set_tab_stops();
        fill_listbox();
        init_checkboxes();
        w = GetDlgItem( hWnd, CAT_INDEX_EDITBOX );
        SetFocus( w );
        eb_select_all( w );
        return TRUE;

    case WM_DRAWITEM:
        draw_subcat_color( (DRAWITEMSTRUCT *) lParam );
        return TRUE;

    case WM_HSCROLL:
        w   = (HWND) lParam;
        if ( w == MoveAllHS )
            p = &MoveAllPos;
        else if ( w == RedHS )
            p = &RedPos;
        else if ( w == GreenHS )
            p = &GreenPos;
        else if ( w == BlueHS )
            p = &BluePos;
        else
            break;

        cmd = (int) LOWORD( wParam );
        if ( cmd == SB_LINELEFT || cmd == SB_LINERIGHT || cmd == SB_THUMBTRACK || cmd == SB_THUMBPOSITION || cmd == SB_PAGELEFT || cmd == SB_PAGERIGHT )
            {
            if ( cmd == SB_THUMBTRACK || cmd == SB_THUMBPOSITION )
                {
                pos = (int) HIWORD( wParam );
                if ( cmd == SB_THUMBPOSITION )
                    {
                    *p = pos;
                    SendMessage( w, SBM_SETPOS, (WPARAM) pos, (LPARAM) TRUE );
                    return TRUE;
                    }
                }
            else
                {
                pos = *p;
                switch ( cmd )
                    {
                    case SB_LINELEFT:
                        pos--;
                        break;

                    case SB_LINERIGHT:
                        pos++;
                        break;

                    case SB_PAGELEFT:
                        pos -= 10;
                        break;

                    case SB_PAGERIGHT:
                        pos += 10;
                        break;
                    }
                if ( pos < 0 )
                    pos = 0;

                if ( pos > 255 )
                    pos = 255;

                SendMessage( w, SBM_SETPOS, (WPARAM) pos, (LPARAM) TRUE );
                }

            if ( w == MoveAllHS )
                do_inc_dec( pos );
            else
                do_hscroll( w, pos );
            return TRUE;
            }
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case CAT_INDEX_EDITBOX:
            case CAT_NAME_EDITBOX:
            case SUBCAT_INDEX_EDITBOX:
            case SUBCAT_NAME_EDITBOX:
                if ( cmd == EN_CHANGE )
                    refresh_listbox_entry();
                return TRUE;

            case RED_EBOX:
            case GREEN_EBOX:
            case BLUE_EBOX:
                if ( cmd == EN_CHANGE )
                    {
                    update_trackbar_from_ebox( id );
                    return TRUE;
                    }
                break;

            case SUBCAT_LISTBOX:
                if ( cmd == LBN_SELCHANGE )
                    {
                    do_select();
                    return TRUE;
                    }
                break;

            case INSERT_SUBCAT_BUTTON:
                do_insert();
                return TRUE;

            case REMOVE_SUBCAT_BUTTON:
                do_remove();
                return TRUE;

            case SAVE_BUTTON:
                do_save();

            case IDCANCEL:
                PostMessage( MainWindow, WM_CLOSE, 0, 0L );
                return TRUE;
            }
        break;

    }

return FALSE;
}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

switch ( msg )
    {
    case WM_CREATE:
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        client_dde_startup( hWnd );
        return 0;

    case WM_EV_SHUTDOWN:
        SendMessage( hWnd, WM_CLOSE, 0, 0L );
        return 0;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                                 INIT                                 *
***********************************************************************/
void init( int cmd_show )
{

WNDCLASS wc;
COMPUTER_CLASS c;

DownTimeSetupWindow    = 0;

names_startup();
c.startup();

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, MAKEINTRESOURCE(DOWNTIME_ICON) );
wc.lpszMenuName  = 0;
wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

MainWindow = CreateWindow(
    MyClassName,
    MyWindowTitle.text(),
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
    0, 0,                             // X,y
    225, 225,                         // W,H
    NULL,
    NULL,
    MainInstance,
    NULL
    );

CreateDialog(
    MainInstance,
    TEXT("DOWNTIME_CATEGORY_DIALOG"),
    MainWindow,
    (DLGPROC) downtime_setup_proc );

ShowWindow( MainWindow, cmd_show );
UpdateWindow( MainWindow );
}

/***********************************************************************
*                              SHUTDOWN                                *
***********************************************************************/
static void shutdown( void )
{
COMPUTER_CLASS c;

client_dde_shutdown();
c.shutdown();
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
BOOL status;
MSG   msg;

InitCommonControls();

MainInstance = this_instance;

MyWindowTitle = resource_string( MainInstance, MAIN_WINDOW_TITLE_STRING );
if ( is_previous_instance(MyClassName, MyWindowTitle.text()) )
    return 0;

init( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
   {
   status = FALSE;

   if (DownTimeSetupWindow )
      status = IsDialogMessage( DownTimeSetupWindow, &msg );

   if ( !status )
      {
      TranslateMessage( &msg );
      DispatchMessage(  &msg );
      }
   }

shutdown();
return msg.wParam;
}
