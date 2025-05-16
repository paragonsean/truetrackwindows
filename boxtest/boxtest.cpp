#include <windows.h>
#include <commctrl.h>

#include "..\include\visiparm.h"
#include "..\include\fileclas.h"
#include "..\include\fontclas.h"
#include "..\include\listbox.h"
#include "..\include\rectclas.h"
#include "..\include\stringcl.h"
#include "..\include\wclass.h"
#include "..\include\subs.h"

#include "resource.h"

/*
-------------
Control Types
------------- */
const int CONTROL_COLOR_MENU    = 515;
const int BORDER_RECTANGLE_MENU = 516;
const int FILLED_RECTANGLE_MENU = 517;
const int TEXT_CONTROL_MENU     = 518;
const int EDIT_TEXT_MENU        = 519;
const int CHOOSE_FONT_MENU      = 520;
const int SEND_TO_BACK_MENU     = 521;
const int DUPLICATE_MENU        = 522;
const int CREATE_BUTTON_MENU    = 523;
const int CREATE_CHECKBOX_MENU  = 524;
const int CREATE_RADIO_MENU     = 525;
const int CREATE_LISTBOX_MENU   = 526;
const int CREATE_COMBO_MENU     = 527;

const UINT MIN_CONTROL_ID = 2;
const UINT NO_CONTROL_ID  = 0;

const UINT BORDER_RECTANGLE_TYPE      = 1;
const UINT FILLED_RECTANGLE_TYPE      = 2;
const UINT TEXT_CONTROL_TYPE          = 3;
const UINT MAX_NONWINDOW_CONTROL_TYPE = 3;
const UINT RADIO_CONTROL_TYPE         = 4;
const UINT CHECKBOX_CONTROL_TYPE      = 5;
const UINT LISTBOX_CONTROL_TYPE       = 6;
const UINT COMBO_CONTROL_TYPE         = 7;
const UINT EDIT_CONTROL_TYPE          = 8;
const UINT PUSHBUTTON_CONTROL_TYPE    = 9;
const UINT DUNDAS_GRID_TYPE           = 10;

struct CONTROL_TYPE_ENTRY
    {
    UINT    type;
    TCHAR * name;
    TCHAR * class_name;
    DWORD   window_style;
    };

CONTROL_TYPE_ENTRY ControlTypes[] = {
    { BORDER_RECTANGLE_TYPE,   TEXT( "Border Rectangle" ),   TEXT(""),          0                   },
    { FILLED_RECTANGLE_TYPE,   TEXT( "Filled Rectangle" ),   TEXT(""),          0                   },
    { TEXT_CONTROL_TYPE,       TEXT( "Text Control"),        TEXT(""),          0                   },
    { RADIO_CONTROL_TYPE,      TEXT( "Radio Button"),        TEXT("BUTTON"),    BS_AUTORADIOBUTTON | BS_PUSHLIKE },
    { CHECKBOX_CONTROL_TYPE,   TEXT( "Check Box"),           TEXT("BUTTON"),    BS_AUTOCHECKBOX     },
    { LISTBOX_CONTROL_TYPE,    TEXT( "List Box"),            TEXT("LISTBOX"),   LBS_STANDARD        },
    { COMBO_CONTROL_TYPE,      TEXT( "Combo Box"),           TEXT("COMBOBOX"),  CBS_DROPDOWN        },
    { EDIT_CONTROL_TYPE,       TEXT( "Edit Control"),        TEXT("EDIT"),      ES_LEFT             },
    { PUSHBUTTON_CONTROL_TYPE, TEXT( "Push Button"),         TEXT("BUTTON"),    BS_PUSHBUTTON       },
    { DUNDAS_GRID_TYPE,        TEXT( "Dundas Grid"),         TEXT(""),          0                   }
    };

const int NofControlTypes = sizeof(ControlTypes) / sizeof(CONTROL_TYPE_ENTRY);

const TCHAR CommaChar = TEXT( ',' );

TCHAR HandCursor[] = TEXT( "HAND_CURSOR" );

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;

WNDPROC      WindowsEditProc = 0;

HCURSOR   OldCursor     = 0;
TCHAR   * CursorId      = 0;
int       DragingId     = NO_INDEX;
int       LastDragingId = NO_INDEX;
POINT     DragingOffset = { 0, 0 };
BITSETYPE DragBorders   = 0;
int       PopupIndex    = NO_INDEX;

const COLORREF BorderColor     = RGB(  50,  50, 255 );
const COLORREF RedColor        = RGB( 255,   0,   0 );
const COLORREF GreenColor      = RGB(   0, 255,   0 );
const COLORREF BlackColor      = RGB(   0,   0,   0 );
const COLORREF WhiteColor      = RGB( 255, 255, 255 );
      COLORREF BackGroundColor = RGB( 255, 255, 255 );

/*
-------------------
Control Entry Flags
------------------- */
const BITSETYPE VISIBLE_LINK       = 0x01;
const BITSETYPE GROUP_MEMBER       = 0x02;
const BITSETYPE IS_VISIBLE         = 0x04;
const BITSETYPE HAVE_VISIBLE_LINKS = 0x08;
const BITSETYPE IS_ON              = 0x10;

class CONTROL_ENTRY
    {
    public:
    UINT            id;
    UINT            type;
    BITSETYPE       flags;
    UINT            visible_link_id;          /* I am visible if and only if he is on */
    WINDOW_CLASS    w;
    WNDPROC         proc;
    HFONT           font_handle;
    COLORREF        color;
    FONT_CLASS      lf;
    RECTANGLE_CLASS r;
    STRING_CLASS    s;

    CONTROL_ENTRY();
    ~CONTROL_ENTRY();
    BOOLEAN choose_text_font( void );
    BOOLEAN create_window( void );
    void    destroy_window( void );
    void    draw( COLORREF new_color );
    void    draw( void ) { draw( color ); }
    void    operator=(const CONTROL_ENTRY & sorc );
    BOOLEAN read( FILE_CLASS & f );
    BOOLEAN write( FILE_CLASS & f );
    };

class CONTROL_ARRAY_CLASS
    {
    public:
    int max_controls;        /* Current array size */
    int n;                   /* Current number of controls */
    CONTROL_ENTRY * p;

    CONTROL_ARRAY_CLASS();
    ~CONTROL_ARRAY_CLASS();
    void cleanup( void );
    BOOLEAN checksize( int index_wanted );
    CONTROL_ENTRY & operator[]( int i ) { return p[i]; }
    };

CONTROL_ARRAY_CLASS Control;

HWND CaptureHandle  = 0;

static TCHAR MyClassName[] = "BoxTest";
static HDC   MyDc          = 0;
static HFONT MyFont        = 0;

static BOOLEAN IHaveFocus = FALSE;

FONT_LIST_CLASS FontList;

/***********************************************************************
*                             TO_POINT                                 *
***********************************************************************/
void to_point( POINT & p, LPARAM lParam )
{
short i;

i = LOWORD( lParam );
p.x = (long) i;

i = HIWORD( lParam );
p.y = (long) i;
}

/***********************************************************************
*                        CONTROL_TYPE_INDEX                            *
***********************************************************************/
int control_type_index( UINT type_to_find )
{
int i;

for ( i=0; i<NofControlTypes; i++ )
    {
    if ( type_to_find == ControlTypes[i].type )
        return i;
    }

return 0;
}

/***********************************************************************
*                        CONTROL_TYPE_INDEX                            *
***********************************************************************/
int control_type_index( TCHAR * name_to_find )
{
int i;

for ( i=0; i<NofControlTypes; i++ )
    {
    if ( strings_are_equal(ControlTypes[i].name, name_to_find) )
        return i;
    }

return 0;
}

/***********************************************************************
*                          SET_VISIBLE_LINKS                           *
***********************************************************************/
void set_visible_links( int control_index )
{
int     i;
UINT    control_id;
BOOLEAN control_is_on;
BOOLEAN have_change;


have_change = FALSE;
control_id  = Control[control_index].id;

if ( Control[control_index].flags & IS_ON )
    control_is_on = TRUE;
else
    control_is_on = FALSE;

for ( i=0; i<Control.n; i++ )
    {
    if ( Control[i].visible_link_id == control_id )
        {
        if ( control_is_on )
            Control[i].flags |= IS_VISIBLE;
        else
            Control[i].flags &= ~IS_VISIBLE;
        have_change = TRUE;
        }
    }

if ( have_change )
    MainWindow.refresh();
}

/***********************************************************************
*                           TURN_ON_CONTROL                            *
***********************************************************************/
static void turn_on_control( int i, BITSETYPE b )
{

if ( b != (Control[i].flags & IS_ON) )
    {
    if ( b )
        Control[i].flags |= IS_ON;
    else
        Control[i].flags &= ~IS_ON;

    if ( Control[i].flags & HAVE_VISIBLE_LINKS )
        set_visible_links( i );
    }
}

/***********************************************************************
*                        BUTTON_CONTROL_PROC                           *
***********************************************************************/
LRESULT CALLBACK button_control_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int i;
long x;
long y;
BITSETYPE b;

i = 0;
while ( i < Control.n )
    {
    if ( Control[i].w == hWnd )
        break;
    i++;
    }

if ( msg == WM_KEYDOWN && wParam == VK_TAB )
    MessageBox( hWnd, "Hi you guys", "Tab", MB_OK );

if ( i < Control.n && msg != WM_CONTEXTMENU )
    {
    if ( msg == BM_SETCHECK )
        {
        if ( wParam == BST_CHECKED )
            b = IS_ON;
        else
            b = 0;

        turn_on_control( i, b );
        }

    if ( msg == WM_MOVE )
        {
        x = (long) LOWORD(lParam);
        y = (long) HIWORD(lParam);
        Control[i].r.moveto( x, y );
        }
    else if ( msg == WM_SIZE )
        {
        x = (long) LOWORD(lParam);
        y = (long) HIWORD(lParam);
        Control[i].r.setsize( x, y );
        }

    if ( Control[i].proc )
        return CallWindowProc( Control[i].proc, hWnd, msg, wParam, lParam );
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                             READ_RECTANGLE                           *
***********************************************************************/
BOOLEAN read_rectangle( RECTANGLE_CLASS & r, FILE_CLASS & f )
{
const int BUFLEN = 50;
TCHAR buf[BUFLEN];
TCHAR * cp;

lstrcpy( buf, f.readline() );
cp = buf;

replace_char_with_null( cp, CommaChar );
r.left = (LONG) asctoint32( cp );

cp = nextstring( cp );
replace_char_with_null( cp, CommaChar );
r.top = (LONG) asctoint32( cp );

cp = nextstring( cp );
replace_char_with_null( cp, CommaChar );
r.right = (LONG) asctoint32( cp );

cp = nextstring( cp );
r.bottom = (LONG) asctoint32( cp );

return TRUE;
}

/***********************************************************************
*                             CONTROL_ENTRY                            *
*                             CONTROL_ENTRY                            *
***********************************************************************/
CONTROL_ENTRY::CONTROL_ENTRY()
{
id              = NO_CONTROL_ID;
proc            = 0;
font_handle     = INVALID_FONT_HANDLE;
flags           = 0;
visible_link_id = NO_CONTROL_ID;
}

/***********************************************************************
*                             CONTROL_ENTRY                            *
*                            ~CONTROL_ENTRY                            *
***********************************************************************/
CONTROL_ENTRY::~CONTROL_ENTRY()
{
destroy_window();
}

/***********************************************************************
*                            CONTROL_ENTRY                             *
*                            DESTROY_WINDOW                            *
***********************************************************************/
void CONTROL_ENTRY::destroy_window( void )
{
if ( !w.handle() )
    return;

w.destroy();
w    = 0;
proc = 0;

if ( font_handle != INVALID_FONT_HANDLE )
    {
    FontList.free_handle( font_handle );
    font_handle = INVALID_FONT_HANDLE;
    }
}

/***********************************************************************
*                             CONTROL_ENTRY                            *
*                             CREATE_WINDOW                            *
***********************************************************************/
BOOLEAN CONTROL_ENTRY::create_window( void )
{
int   i;
DWORD mystyle;

if ( (type <= MAX_NONWINDOW_CONTROL_TYPE) || (type == DUNDAS_GRID_TYPE) )
    return FALSE;

destroy_window();

i = control_type_index( type );

mystyle =  WS_CHILD | WS_VISIBLE | WS_THICKFRAME | ControlTypes[i].window_style;
//  mystyle = (WS_CHILD | WS_VISIBLE | ControlTypes[i].window_style) & ~WS_THICKFRAME;

w = CreateWindow(
    ControlTypes[i].class_name,
    s.text(),
    mystyle,
    r.left,
    r.top,
    r.width(),
    r.height(),
    MainWindow.handle(),
    (HMENU) id,
    MainInstance,
    NULL
    );

proc = (WNDPROC) SetWindowLong( w.handle(), GWL_WNDPROC, (LONG) button_control_proc );

font_handle = FontList.get_handle( lf );
if ( font_handle != INVALID_FONT_HANDLE )
    w.post( WM_SETFONT, (WPARAM) font_handle, TRUE );

return TRUE;
}

/***********************************************************************
*                             CONTROL_ENTRY                            *
*                                  READ                                *
***********************************************************************/
BOOLEAN CONTROL_ENTRY::read( FILE_CLASS & f )
{
TCHAR * cp;
int     i;

i     = control_type_index( f.readline() );
type  = ControlTypes[i].type;
color = (COLORREF)  hextoul( f.readline() );

cp              = f.readline();
id              = (UINT) asctoul( cp );
flags           = 0;
visible_link_id = 0;
if ( replace_char_with_null(cp, CommaChar) )
    {
    cp = nextstring( cp );
    flags = (BITSETYPE) hextoul( cp );
    if ( replace_char_with_null(cp, CommaChar) )
        {
        cp = nextstring( cp );
        visible_link_id = (UINT) asctoul( cp );
        }
    }

s = f.readline();
lf.get( s.text() );
read_rectangle( r, f );

s = f.readline();

create_window();

return TRUE;
}

/***********************************************************************
*                          CONTROL_ARRAY_CLASS                         *
***********************************************************************/
void CONTROL_ARRAY_CLASS::cleanup( void )
{
if ( p )
    {
    delete[] p;
    p = 0;
    }

n = 0;
max_controls = 0;
}

/***********************************************************************
*                          CONTROL_ARRAY_CLASS                         *
***********************************************************************/
CONTROL_ARRAY_CLASS::CONTROL_ARRAY_CLASS()
{
p = 0;
n = 0;
max_controls = 0;
}

/***********************************************************************
*                          CONTROL_ARRAY_CLASS                         *
***********************************************************************/
CONTROL_ARRAY_CLASS::~CONTROL_ARRAY_CLASS()
{
cleanup();
}

/***********************************************************************
*                          CONTROL_ARRAY_CLASS                         *
*                              CHECKSIZE                               *
***********************************************************************/
BOOLEAN CONTROL_ARRAY_CLASS::checksize( int index_wanted )
{

if ( index_wanted >= n )
    {
    if ( index_wanted >= max_controls )
        {
        int j;
        int new_max;
        CONTROL_ENTRY * pc;

        /*
        ----------------------------
        Double the size of the array
        ---------------------------- */
        if ( max_controls > 0 )
            new_max = max_controls * 2;
        else
            new_max = 50;

        if ( new_max <= index_wanted )
            new_max = index_wanted+1;

        pc = new CONTROL_ENTRY[new_max];
        if ( !pc )
            return FALSE;

        /*
        -----------------------------
        Copy and delete the old array
        ----------------------------- */
        if ( p )
            {
            for ( j=0; j<n; j++ )
                pc[j] = p[j];

            delete[] p;
            }

        p = pc;
        max_controls = new_max;
        }

    n = index_wanted+1;
    }

return TRUE;
}

/***********************************************************************
*                           WRITE_RECTANGLE                            *
***********************************************************************/
BOOLEAN write_rectangle( FILE_CLASS & f, RECTANGLE_CLASS & r )
{
const int BUFLEN = 50;
TCHAR buf[BUFLEN];
TCHAR * cp;

cp = copy_w_char( buf, int32toasc((int32) r.left),   CommaChar );
cp = copy_w_char( cp,  int32toasc((int32) r.top),    CommaChar );
cp = copy_w_char( cp,  int32toasc((int32) r.right),  CommaChar );
copystring( cp, int32toasc((int32) r.bottom) );

f.writeline( buf );

return TRUE;
}

/***********************************************************************
*                             CONTROL_ENTRY                            *
*                                 WRITE                                *
***********************************************************************/
BOOLEAN CONTROL_ENTRY::write( FILE_CLASS & f )
{
const int BUFLEN = (MAX_INTEGER_LEN * 3) + 4;

TCHAR   buf[BUFLEN];
TCHAR * cp;
int     i;

i = control_type_index( type );

f.writeline( ControlTypes[i].name );
f.writeline( ultohex((unsigned long) color) );

/*
----------------------------------------------------
Write the id, flags, and visible link id on one line
---------------------------------------------------- */
cp = copy_w_char( buf, ultoascii((unsigned long) id),  CommaChar );
cp = copy_w_char( cp,  ultohex((unsigned long) flags), CommaChar );
copystring(cp,  ultoascii((unsigned long) visible_link_id) );

f.writeline( buf );

f.writeline( lf.put() );

if ( type > MAX_NONWINDOW_CONTROL_TYPE )
    {
    POINT p;
    GetWindowRect( w.handle(), &r );
    p.x = r.left;
    p.y = r.top;
    ScreenToClient( MainWindow.handle(), &p );
    r.moveto( p );
    }

write_rectangle( f, r );

f.writeline( s.text() );

return TRUE;
}

/***********************************************************************
*                             CONTROL_ENTRY                            *
*                                   =                                  *
***********************************************************************/
void CONTROL_ENTRY::operator=(const CONTROL_ENTRY & sorc )
{
id              = sorc.id;
type            = sorc.type;
flags           = sorc.flags;
visible_link_id = sorc.visible_link_id;
color           = sorc.color;
lf              = sorc.lf;
r               = sorc.r;
s               = sorc.s;
w               = sorc.w;
proc            = sorc.proc;
}

/***********************************************************************
*                             CONTROL_ENTRY                            *
*                                  DRAW                                *
***********************************************************************/
void CONTROL_ENTRY::draw( COLORREF new_color )
{

if ( !(flags & IS_VISIBLE) )
    return;

if ( type == BORDER_RECTANGLE_TYPE )
    r.draw( MyDc, new_color );

if ( type == FILLED_RECTANGLE_TYPE )
    r.fill( MyDc, new_color );

if ( type == TEXT_CONTROL_TYPE )
    {
    HFONT oldfont;
    SetBkMode( MyDc, TRANSPARENT );
    SetTextColor( MyDc, new_color );
    oldfont = (HFONT) SelectObject( MyDc, CreateFontIndirect(&lf) );
    if ( r.is_empty() )
        {
        int  nof_bytes;
        SIZE sz;
        nof_bytes = s.len() * sizeof(TCHAR);
        GetTextExtentPoint32( MyDc, s.text(), nof_bytes, &sz );
        r.setsize( sz );
        }
    TextOut( MyDc, r.left, r.top, s.text(), s.len() );
    DeleteObject( SelectObject(MyDc, oldfont) );
    }

}

/***********************************************************************
*                           CHOOSE_TEXT_FONT                           *
***********************************************************************/
BOOLEAN CONTROL_ENTRY::choose_text_font( void )
{
if ( lf.choose(MainWindow.handle(), color) )
    return TRUE;
return FALSE;
}

/***********************************************************************
*                          OPEN_CONTROL_LIST                           *
***********************************************************************/
static void open_control_list()
{
int i;
int n;
FILE_CLASS f;

f.open_for_read( TEXT("BOZO.LST") );

i = Control.n;
n = i + (int) asctoint32( f.readline() );
while ( i < n )
    {
    if ( !Control.checksize(i) )
        break;
    Control[i].read( f );
    i++;
    }

f.close();
MainWindow.refresh();
}

/***********************************************************************
*                          SAVE_CONTROL_LIST                           *
***********************************************************************/
static void save_control_list()
{
int i;
FILE_CLASS f;

f.open_for_write( TEXT("BOZO.LST") );

f.writeline( int32toasc(Control.n) );

for ( i=0; i<Control.n; i++ )
    Control[i].write( f );

f.close();
}

/***********************************************************************
*                         GET_NEXT_CONTROL_ID                          *
***********************************************************************/
static UINT get_next_control_id( void )
{
UINT my_id;
int  i;

my_id = MIN_CONTROL_ID;
for ( i=0; i<Control.n; i++ )
    {
    if ( Control[i].id >= my_id )
        my_id = Control[i].id + 1;
    }

return my_id;
}

/***********************************************************************
*                             RESOURCE_STRING                          *
***********************************************************************/
TCHAR * resource_string( UINT resource_id )
{
return resource_string( MainInstance, resource_id );
}

/***********************************************************************
*                             CONTROL_INDEX                            *
***********************************************************************/
static int control_index( POINT & p )
{
int i;

i = Control.n - 1;
while ( i >= 0 )
    {
    if ( (Control[i].flags & IS_VISIBLE) && Control[i].r.contains(p) )
        return i;
    i--;
    }

return NO_INDEX;
}

/***********************************************************************
*                            CONTROL_INDEX                             *
***********************************************************************/
static int control_index( LPARAM lParam )
{
POINT p;

p.x = LOWORD( lParam );
p.y = HIWORD( lParam );

return control_index( p );
}

/***********************************************************************
*                               PAINT_ME                               *
***********************************************************************/
static void paint_me( void )
{
PAINTSTRUCT ps;
int         i;

BeginPaint( MainWindow.handle(), &ps );

MyDc = ps.hdc;

for ( i=0; i<Control.n; i++ )
    {
    if ( Control[i].w.handle() == 0 )
        Control[i].draw();
    }

MyDc = 0;

EndPaint( MainWindow.handle(), &ps );
}

/***********************************************************************
*                              CREATE_BOX                              *
***********************************************************************/
static void create_box( void )
{
RECTANGLE_CLASS * sorc;
RECTANGLE_CLASS * dest;
RECTANGLE_CLASS parent;
long x;
int  n;

n = Control.n;
if ( !Control.checksize(n) )
    return;

GetClientRect( MainWindow.handle(), &parent );

dest = &Control[n].r;

Control[n].id    = get_next_control_id();
Control[n].flags = IS_VISIBLE;
Control[n].color = WhiteColor;
Control[n].type  = BORDER_RECTANGLE_TYPE;
Control[n].s     = TEXT( "Hi you guys" );

GetObject( GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &Control[n].lf );

if ( n == 0 )
    {
    dest->set( 10, 10, 310, 60 );
    }
else
    {
    sorc = &Control[n-1].r;
    *dest = *sorc;
    if ( (sorc->right + sorc->width()) < parent.right )
        {
        dest->offset( sorc->width(), 0 );
        }
    else
        {
        x = 10;
        if ( sorc->left < x )
            x = sorc->left;
        dest->moveto( x, sorc->bottom + 1 );
        }
    }

MyDc = GetDC( MainWindow.handle() );
Control[n].draw();
ReleaseDC( MainWindow.handle(), MyDc );
MyDc = 0;
}

/***********************************************************************
*                           ADJUST_RECTANGLE                           *
***********************************************************************/
static void adjust_rectangle( POINT & p )
{
long x;
long y;

x = p.x;
y = p.y;

if ( (DragBorders & W_BORDER) && (x >= Control[DragingId].r.right) )
    {
    DragBorders &= ~W_BORDER;
    DragBorders |= E_BORDER;
    if ( x == Control[DragingId].r.right )
        x = Control[DragingId].r.right + 1;
    }

if ( (DragBorders & E_BORDER) && (x <= Control[DragingId].r.left) )
    {
    DragBorders &= ~E_BORDER;
    DragBorders |= W_BORDER;
    if ( x == Control[DragingId].r.left )
        x = Control[DragingId].r.left - 1;
    }

if ( (DragBorders & N_BORDER) && (y >= Control[DragingId].r.bottom) )
    {
    DragBorders &= ~N_BORDER;
    DragBorders |= S_BORDER;
    if ( y == Control[DragingId].r.bottom )
        y = Control[DragingId].r.bottom + 1;
    }

if ( (DragBorders & S_BORDER) && (y <= Control[DragingId].r.top) )
    {
    DragBorders &= ~S_BORDER;
    DragBorders |= N_BORDER;
    if ( y == Control[DragingId].r.top )
        y = Control[DragingId].r.top - 1;
    }

if ( DragingId != NO_INDEX )
    {
    if ( DragBorders & W_BORDER )
        Control[DragingId].r.left = x;

    if ( DragBorders & E_BORDER )
        Control[DragingId].r.right = x;

    if ( DragBorders & N_BORDER )
        Control[DragingId].r.top = y;

    if ( DragBorders & S_BORDER )
        Control[DragingId].r.bottom = y;
    }

}

/***********************************************************************
*                             CHOOSE_COLOR                             *
***********************************************************************/
static BOOLEAN choose_color( HWND w, COLORREF & color )
{

CHOOSECOLOR cc;
static COLORREF acrCustClr[16];   // array of custom colors

ZeroMemory( &cc, sizeof(CHOOSECOLOR) );

cc.lStructSize    = sizeof(CHOOSECOLOR);
cc.hwndOwner      = w;
cc.hInstance      = (HWND) MainInstance;
cc.lpCustColors   = (LPDWORD) acrCustClr;
cc.rgbResult      = color;
cc.Flags          = CC_RGBINIT | CC_FULLOPEN;
cc.lpTemplateName = TEXT( "ChooseColor" );

if ( ChooseColor(&cc) )
    {
    color = cc.rgbResult;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                            DO_POPUP_MENU                             *
***********************************************************************/
static void do_popup_menu( LPARAM lParam )
{
HMENU m;
POINT p;
short i;

if ( PopupIndex == NO_INDEX )
    return;

m = CreatePopupMenu();
if ( !m )
    return;

i = LOWORD( lParam );
p.x = (long) i;

i = HIWORD( lParam );
p.y = (long) i;
//ClientToScreen( MainWindow.handle(), (LPPOINT) &p );

if ( Control[PopupIndex].type == TEXT_CONTROL_TYPE )
    {
    AppendMenu( m, MF_STRING, EDIT_TEXT_MENU,   TEXT( "Edit Text") );
    AppendMenu( m, MF_STRING, CHOOSE_FONT_MENU, TEXT( "Choose Font") );
    }
else
    {
    AppendMenu( m, MF_STRING, CONTROL_COLOR_MENU, TEXT( "Color") );
    AppendMenu( m, MF_STRING, TEXT_CONTROL_MENU,  TEXT( "Text Control") );
    }

if ( (Control[PopupIndex].type != BORDER_RECTANGLE_TYPE) )
    AppendMenu( m, MF_STRING, BORDER_RECTANGLE_MENU,  TEXT( "Border Rectangle") );

if ( (Control[PopupIndex].type != FILLED_RECTANGLE_TYPE) )
    AppendMenu( m, MF_STRING, FILLED_RECTANGLE_MENU,  TEXT( "Filled Rectangle") );

if ( PopupIndex > 0 )
    AppendMenu( m, MF_STRING, SEND_TO_BACK_MENU, TEXT( "Send to Back") );

AppendMenu( m, MF_STRING, DUPLICATE_MENU, TEXT( "Duplicate") );
AppendMenu( m, MF_STRING, CREATE_BUTTON_MENU,   TEXT("Make into Button") );
AppendMenu( m, MF_STRING, CREATE_CHECKBOX_MENU, TEXT("Make into Check Box") );
AppendMenu( m, MF_STRING, CREATE_RADIO_MENU,    TEXT("Make into Radio Button") );
AppendMenu( m, MF_STRING, CREATE_LISTBOX_MENU,  TEXT("Make into List Box") );
AppendMenu( m, MF_STRING, CREATE_COMBO_MENU,    TEXT("Make into Combo Box") );

TrackPopupMenu( m, TPM_LEFTALIGN | TPM_LEFTBUTTON, p.x, p.y, 0, MainWindow.handle(), 0 );

DestroyMenu( m );
}

/***********************************************************************
*                             SEND_TO_BACK                             *
***********************************************************************/
static void send_to_back( void )
{
int i;
CONTROL_ENTRY c;

if ( PopupIndex == NO_INDEX )
    return;

c = Control[PopupIndex];

for ( i=PopupIndex; i>0; i-- )
    Control[i]   = Control[i-1];

Control[0] = c;
PopupIndex = 0;

MainWindow.refresh();
}

/***********************************************************************
                           CHOOSE_CONTROL_COLOR                        *
***********************************************************************/
static void choose_control_color( void )
{

if ( PopupIndex == NO_INDEX )
    return;

if ( choose_color(MainWindow.handle(), Control[PopupIndex].color) )
    MainWindow.refresh();

}

/***********************************************************************
                            CHOOSE_CONTROL_FONT                        *
***********************************************************************/
static void choose_control_font( void )
{
if ( PopupIndex == NO_INDEX )
    return;

if ( Control[PopupIndex].choose_text_font() )
    {
    Control[PopupIndex].r.empty();
    MainWindow.refresh();
    }
}

/***********************************************************************
*                          EDIT_CONTROL_PROC                           *
***********************************************************************/
LRESULT CALLBACK edit_control_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

if ( msg == WM_KEYDOWN && wParam == VK_RETURN )
    {
    if ( PopupIndex != NO_INDEX )
        {
        Control[PopupIndex].type = TEXT_CONTROL_TYPE;
        Control[PopupIndex].s.get_text( Control[PopupIndex].w.handle() );
        Control[PopupIndex].r.empty();
        }
    Control[PopupIndex].w.destroy();
    WindowsEditProc = 0;
    MainWindow.refresh();
    return 0;
    }

if ( WindowsEditProc )
    return CallWindowProc( WindowsEditProc, hWnd, msg, wParam, lParam );

return 0;
}

/***********************************************************************
*                          EDIT_CONTROL_TEXT                           *
***********************************************************************/
static void edit_control_text( void )
{
if ( PopupIndex == NO_INDEX )
    return;

if ( Control[PopupIndex].id == NO_CONTROL_ID )
    Control[PopupIndex].id = get_next_control_id();

Control[PopupIndex].type = EDIT_CONTROL_TYPE;
Control[PopupIndex].s.text(),
Control[PopupIndex].w = CreateWindow(
    TEXT( "EDIT" ),
    Control[PopupIndex].s.text(),
    WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
    Control[PopupIndex].r.left,
    Control[PopupIndex].r.top,
    Control[PopupIndex].r.width(),
    Control[PopupIndex].r.height(),
    MainWindow.handle(),
    (HMENU) Control[PopupIndex].id,
    MainInstance,
    NULL
    );

WindowsEditProc = (WNDPROC) SetWindowLong( Control[PopupIndex].w.handle(), GWL_WNDPROC, (LONG) edit_control_proc );

Control[PopupIndex].w.set_focus();
}

/***********************************************************************
*                            CREATE_BUTTON                             *
***********************************************************************/
static void create_button( int menu_chosen )
{
struct WINDOW_ENTRY
    {
    int     menu_id;
    UINT    type;
    };

const WINDOW_ENTRY Buttons[] = {
    { CREATE_BUTTON_MENU,   PUSHBUTTON_CONTROL_TYPE },
    { CREATE_CHECKBOX_MENU, CHECKBOX_CONTROL_TYPE   },
    { CREATE_RADIO_MENU,    RADIO_CONTROL_TYPE      },
    { CREATE_LISTBOX_MENU,  LISTBOX_CONTROL_TYPE    },
    { CREATE_COMBO_MENU,    COMBO_CONTROL_TYPE      }
    };

const NofButtonTypes = sizeof(Buttons)/sizeof(WINDOW_ENTRY);

int i;

if ( PopupIndex == NO_INDEX )
    return;

i = 0;
while ( TRUE  )
    {
    if ( i >= NofButtonTypes )
        return;

    if ( Buttons[i].menu_id == menu_chosen )
        {
        i = control_type_index( Buttons[i].type );
        break;
        }
    i++;
    }

Control[PopupIndex].destroy_window();

Control[PopupIndex].type = ControlTypes[i].type;

if ( Control[PopupIndex].id == NO_CONTROL_ID )
    Control[PopupIndex].id = get_next_control_id();

Control[PopupIndex].create_window();
 
if ( Control[PopupIndex].type == LISTBOX_CONTROL_TYPE )
    {
    LISTBOX_CLASS lb;
    lb.init( Control[PopupIndex].w.handle() );
    lb.add( "Hi" );
    lb.add( "you" );
    lb.add( "guys" );
    }
}

/***********************************************************************
*                              DUPLICATE                               *
***********************************************************************/
static void duplicate( void )
{
int n;

if ( PopupIndex == NO_INDEX )
    return;

n = Control.n;
if ( !Control.checksize(n) )
    return;

Control[n] = Control[PopupIndex];
Control[n].r.offset( 0, 10 );
Control[n].id = get_next_control_id();
PopupIndex = n;
MainWindow.refresh();
}

/***********************************************************************
                             SET_CONTROL_TYPE                          *
***********************************************************************/
static void set_control_type( int menu_id )
{
if ( PopupIndex == NO_INDEX )
    return;

Control[PopupIndex].destroy_window();

switch ( menu_id )
    {
    case BORDER_RECTANGLE_MENU:
        Control[PopupIndex].type = BORDER_RECTANGLE_TYPE;
        break;

    case FILLED_RECTANGLE_MENU:
        Control[PopupIndex].type = FILLED_RECTANGLE_TYPE;
        break;

    case TEXT_CONTROL_MENU:
        Control[PopupIndex].type = TEXT_CONTROL_TYPE;
        Control[PopupIndex].r.empty();
        break;
    }

MainWindow.refresh();
}

/***********************************************************************
*                        DO_RIGHT_BUTTON_DOWN                          *
***********************************************************************/
static void do_right_button_down( LPARAM lParam )
{
POINT p;

to_point( p, lParam );

ScreenToClient( MainWindow.handle(), &p );

PopupIndex = control_index( p );

if ( PopupIndex == NO_INDEX )
    return;

do_popup_menu( lParam );
}

/***********************************************************************
*                          DO_LEFT_BUTTON_UP                           *
***********************************************************************/
static void do_left_button_up( LPARAM lParam )
{
POINT p;

if ( CaptureHandle )
    {
    ReleaseCapture();
    CaptureHandle = 0;
    }

to_point( p, lParam );

adjust_rectangle( p );

LastDragingId = DragingId;
DragingId = NO_INDEX;

MainWindow.refresh();
}

/***********************************************************************
*                         DO_LEFT_BUTTON_DOWN                          *
***********************************************************************/
static void do_left_button_down( WPARAM wParam, LPARAM lParam )
{
int   i;
POINT p;

to_point( p, lParam );

i = control_index( p );
if ( i == NO_INDEX )
    return;

DragingId   = i;
DragBorders = 0;

if ( wParam & MK_SHIFT )
    {
    DragingOffset.x = p.x - Control[i].r.left;
    DragingOffset.y = p.y - Control[i].r.top;
    }
else
    {
    DragBorders = Control[i].r.near_borders( p );
    if ( !DragBorders )
        DragingId = NO_INDEX;
    }

if ( DragingId != NO_INDEX )
    {
    int  orimode;

    MyDc = GetDC( MainWindow.handle() );
    if ( Control[i].type == BORDER_RECTANGLE_TYPE )
        Control[i].draw( BackGroundColor );
    if ( DragBorders )
        adjust_rectangle( p );
    orimode = SetROP2( MyDc, R2_NOT );
    Control[i].r.draw( MyDc );
    SetROP2( MyDc, orimode );
    ReleaseDC( MainWindow.handle(), MyDc );
    MyDc = 0;

    if ( !CaptureHandle )
        {
        CaptureHandle = MainWindow.handle();
        SetCapture( CaptureHandle );
        }
    }

}


/***********************************************************************
*                              SET_CURSOR_TYPE                         *
***********************************************************************/
static void set_cursor_type( TCHAR * new_cursor )
{
HCURSOR cursor;

cursor = 0;

if ( CursorId != new_cursor )
    {
    CursorId = new_cursor;
    if ( CursorId == HandCursor )
        cursor = SetCursor( LoadCursor(MainInstance, CursorId) );
    else
        cursor = SetCursor( LoadCursor(NULL, CursorId) );
    }
/*
----------------------
Save the system cursor
---------------------- */
if ( !OldCursor )
    {
    if ( cursor )
        OldCursor = cursor;
    else
        OldCursor = LoadCursor( NULL, IDC_ARROW );
    }
}


/***********************************************************************
*                           SET_CURSOR_TYPE                            *
***********************************************************************/
static void set_cursor_type( void )
{
CursorId = 0;

//if ( !OldCursor )
//  OldCursor = LoadCursor( NULL, IDC_ARROW );

if ( OldCursor )
    {
    SetCursor( OldCursor );
    OldCursor = 0;
    }

}

/***********************************************************************
*                             DO_MOUSEMOVE                             *
***********************************************************************/
static void do_mousemove( WPARAM wParam, LPARAM lParam )
{
int     i;
TCHAR * c;
POINT   p;

to_point( p, lParam );

if ( DragingId != NO_INDEX )
    {
    int  orimode;

    MyDc = GetDC( MainWindow.handle() );
    orimode = SetROP2( MyDc, R2_NOT );
    Control[DragingId].r.draw( MyDc );

    if ( DragBorders )
        {
        adjust_rectangle( p );
        }
    else
        {
        p.x -= DragingOffset.x;
        p.y -= DragingOffset.y;
        Control[DragingId].r.moveto( p );
        }

    Control[DragingId].r.draw( MyDc );
    SetROP2( MyDc, orimode );

    ReleaseDC( MainWindow.handle(), MyDc );
    MyDc = 0;
    return;
    }

i = control_index( p );
if ( i != NO_INDEX && (WindowsEditProc == 0) )
    {
    if ( Control[i].w.handle() == 0 )
        {
        if ( !IHaveFocus )
            MainWindow.set_focus();

        if ( wParam & MK_SHIFT )
            {
            set_cursor_type( HandCursor );
            return;
            }
        else
            {
            c = (TCHAR *) Control[i].r.getcursor( p );
            if ( c )
                {
                set_cursor_type( c );
                return;
                }
            }
        }
    }

set_cursor_type();
}

/***********************************************************************
*                      GET_OFFSET_FROM_ARROW_KEY                       *
***********************************************************************/
static BOOLEAN get_offset_from_arrow_key( POINT & dest, int key )
{
dest.x = 0;
dest.y = 0;

switch( key )
    {
    case VK_DOWN:
        dest.y++;
        break;

    case VK_UP:
        dest.y--;
        break;

    case VK_RIGHT:
        dest.x++;
        break;

    case VK_LEFT:
        dest.x--;
        break;
    }

if ( dest.x || dest.y )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                             DO_KEYPRESS                              *
***********************************************************************/
static void do_keypress( UINT msg, WPARAM wParam )
{
int   key;
POINT p;

key = (int) wParam;

GetCursorPos( &p );
ScreenToClient( MainWindow.handle(), &p );

/*
-------------------------------------------------------------------------
If I'm not dragging anything but am over a control then change the cursor
------------------------------------------------------------------------- */
if ( key == VK_SHIFT && DragingId == NO_INDEX )
    {
    if ( control_index(p) != NO_INDEX )
        {
        if ( msg == WM_KEYDOWN )
            set_cursor_type( HandCursor );
        }
    return;
    }

if ( LastDragingId == NO_INDEX )
    return;

if ( msg == WM_KEYDOWN && DragingId == NO_INDEX && LastDragingId != NO_INDEX )
    {
    if ( get_offset_from_arrow_key(p, key) )
        {
        if ( (Control[LastDragingId].r.left + p.x ) >= 0 && (Control[LastDragingId].r.top + p.y) >= 0 )
            {
            MyDc = GetDC( MainWindow.handle() );
            Control[LastDragingId].draw( BackGroundColor );
            Control[LastDragingId].r.offset( p );
            Control[LastDragingId].draw();
            ReleaseDC( MainWindow.handle(), MyDc );
            MyDc = 0;
            }
        }
    }

}

/***********************************************************************
*                              MAIN_PROC                               *
***********************************************************************/
long CALLBACK main_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        MainWindow = hWnd;
        MainWindow.post( WM_DBINIT );
        break;

    case WM_PAINT:
        paint_me();
        return 0;

    case WM_KEYDOWN:
    case WM_KEYUP:
        do_keypress( msg, wParam );
        break;

    case WM_DBINIT:
        return 0;

    case WM_KILLFOCUS:
        IHaveFocus = FALSE;
        return 0;

    case WM_SETFOCUS:
        IHaveFocus = TRUE;
        return 0;

    case WM_SETCURSOR:
        if ( OldCursor )
            return TRUE;
        break;

    case WM_CONTEXTMENU:
        do_right_button_down( lParam );
        return 0;

    case WM_LBUTTONUP:
        do_left_button_up( lParam );
        return 0;

    case WM_LBUTTONDOWN:
        do_left_button_down( wParam, lParam );
        return 0;

    case WM_MOUSEMOVE:
        do_mousemove( wParam, lParam );
        return 0;

    case WM_COMMAND:
        switch (id)
            {
            case CONTROL_COLOR_MENU:
                choose_control_color();
                return 0;

            case BORDER_RECTANGLE_MENU:
                set_control_type( id );
                return 0;

            case FILLED_RECTANGLE_MENU:
                set_control_type( id );
                return 0;

            case TEXT_CONTROL_MENU:
                set_control_type( id );
                return 0;

            case EDIT_TEXT_MENU:
                edit_control_text();
                return 0;

            case CHOOSE_FONT_MENU:
                choose_control_font();
                return 0;

            case SEND_TO_BACK_MENU:
                send_to_back();
                return 0;

            case DUPLICATE_MENU:
                duplicate();
                return 0;

            case CREATE_BUTTON_MENU:
            case CREATE_CHECKBOX_MENU:
            case CREATE_RADIO_MENU:
            case CREATE_LISTBOX_MENU:
            case CREATE_COMBO_MENU:
                create_button( id );
                return 0;

            case EXIT_CHOICE:
                MainWindow.close();
                return 0;

            case OPEN_CHOICE:
                open_control_list();
                return 0;

            case SAVE_CHOICE:
                save_control_list();
                return 0;

            case CREATE_BOX_CHOICE:
                create_box();
                return 0;

            case MAXIMIZE_CHOICE:
                MainWindow.show( SW_MAXIMIZE );
                return 0;

            case RESTORE_CHOICE:
                MainWindow.show( SW_RESTORE );
                return 0;
            }
        break;

    case WM_DESTROY:
        PostQuitMessage( 0 );
        return 0;

    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                               STARTUP                                *
***********************************************************************/
static void startup( int cmd_show )
{
STRING_CLASS title;
WNDCLASS wc;

BackGroundColor = (COLORREF) GetSysColor( COLOR_WINDOW );

wc.lpszClassName = MyClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, TEXT("MY_ICON") );
wc.lpszMenuName  = TEXT( "MAINMENU" );
//wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);  /* Use standard background */
wc.style         = 0;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

title = resource_string( MAIN_WINDOW_TITLE_STRING );

CreateWindow(
    MyClassName,
    title.text(),
    WS_POPUP | WS_VISIBLE | WS_THICKFRAME,  //WS_OVERLAPPEDWINDOW,
    0, 0,                             // X,y
    600, 300,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

MainWindow.show( cmd_show );
UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{
if ( MyFont != 0 )
    {
    DeleteObject( MyFont );
    MyFont = 0;
    }
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show )
{
MSG   msg;

InitCommonControls();

MainInstance = this_instance;

startup( cmd_show );

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    TranslateMessage( &msg );
    DispatchMessage(  &msg );
    }

shutdown();
return( msg.wParam );
}
