#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\structs.h"
#include "..\include\subs.h"
#include "..\include\names.h"
#include "..\include\computer.h"
#include "..\include\machine.h"
#include "..\include\param.h"
#include "..\include\part.h"
#include "..\include\rectclas.h"
#include "..\include\stringcl.h"
#include "..\include\wclass.h"

#include "..\include\ftii.h"

#include "extern.h"
#include "resource.h"
#include "resrc1.h"

extern HINSTANCE MainInstance;
extern FTII_LIMIT_SWITCH_WIRE_CLASS CurrentLimitSwitchWire;

const int CONTROL_OUTPUT_MODULE   = 1;
const int MONITOR_OUTPUT_MODULE   = 2;
const int CONTROL_WIRE_OFFSET     = 16;    /* Control outputs go from 17 to 32 */
const int SLOW_SHOT_OUTPUT_WIRE   = 17;
const int ACCUMULATOR_OUTPUT_WIRE = 18;
const int VALVE_TEST_IN_OPERATION_WIRE = 21;
const int WARNING_ACTIVE_LOW_WIRE = 28;

static WINDOW_CLASS WireSorc;
static WINDOW_CLASS WireInfoTbox;

static TCHAR ChooseWireClassName[]    = TEXT( "ChooseWireControl" );
static int   ChosenWire               = NO_WIRE;

static int CurrentGoodShotWire    = NO_WIRE;

static int CurrentOutputModule    = 0;
static int CurrentOutputType      = 0;  /* This is either alarm or limit switch so I don't display the current setting on the warnings */
static int CurrentOutputIndex     = 0;  /* This is the current alarm parameter index or limit switch index */
static int NofWires               = 0;
static RECTANGLE_CLASS * LedRect  = 0;
static RECTANGLE_CLASS * WireRect = 0;

HPALETTE AllPurposePalette        = 0;

const TCHAR MinusChar         = TEXT( '-' );
const TCHAR NullChar          = TEXT( '\0' );
const TCHAR PlusChar          = TEXT( '+' );
const TCHAR TabChar           = TEXT( '\t' );
static TCHAR SpaceString[]    = TEXT( " " );
static TCHAR TwoSpaceString[] = TEXT( "  " );

static bool NeedManualDialog = false;
static int  CenterX;   /* Center of dialog */
static int  CenterY;

struct WIRE_MESSAGE_CLASS
{
int  wire_number;
UINT id;
};

static WIRE_MESSAGE_CLASS AlreadyUsedWires[] = {
    { SLOW_SHOT_OUTPUT_WIRE, SLOW_SHOT_OUTPUT_STRING },
    { ACCUMULATOR_OUTPUT_WIRE, ACCUMULATOR_OUTPUT_STRING },
    {  VALVE_TEST_IN_OPERATION_WIRE, VALVE_TEST_IN_OPERATION_STRING, },
    { WARNING_ACTIVE_LOW_WIRE, WARNING_ACTIVE_LOW_STRING }
    };
const int NofAlreadyUsedWires = sizeof(AlreadyUsedWires)/sizeof(WIRE_MESSAGE_CLASS);

/*
----------------------------------------------------------------------------
These are loaded with the led locations. There are 16 for the big opto board
and 8 for the small board.
---------------------------------------------------------------------------- */
const int MAX_CONTROL_WIRE = 16;
static RECTANGLE_CLASS ControlLedRect[MAX_CONTROL_WIRE] =
    {
    { RECTANGLE_CLASS(267, 49, 281, 63) },
    { RECTANGLE_CLASS(267, 18, 281, 32) },
    { RECTANGLE_CLASS(232, 49, 246, 63) },
    { RECTANGLE_CLASS(232, 18, 246, 32) },
    { RECTANGLE_CLASS(194, 49, 208, 63) },
    { RECTANGLE_CLASS(194, 18, 208, 32) },
    { RECTANGLE_CLASS(159, 49, 173, 63) },
    { RECTANGLE_CLASS(159, 18, 173, 32) },
    { RECTANGLE_CLASS(121, 49, 135, 63) },
    { RECTANGLE_CLASS(121, 18, 135, 32) },
    { RECTANGLE_CLASS( 85, 49,  99, 63) },
    { RECTANGLE_CLASS( 85, 18,  99, 32) },
    { RECTANGLE_CLASS( 47, 49,  61, 63) },
    { RECTANGLE_CLASS( 47, 18,  61, 32) },
    { RECTANGLE_CLASS( 12, 49,  26, 63) },
    { RECTANGLE_CLASS( 12, 18,  26, 32) }
    };

/*
--------------------------------------------------------------------------------------
The left hand set are the opto wires, the right hand set are the connector strip wires
-------------------------------------------------------------------------------------- */
static RECTANGLE_CLASS ControlWireRect[2*MAX_CONTROL_WIRE] =
    {
    { RECTANGLE_CLASS(279, 213, 286, 251) }, { RECTANGLE_CLASS(390,  23, 400,  30) },
    { RECTANGLE_CLASS(279, 163, 286, 251) }, { RECTANGLE_CLASS(357,  23, 400,  30) },
    { RECTANGLE_CLASS(231, 213, 238, 251) }, { RECTANGLE_CLASS(390,  41, 400,  48) },
    { RECTANGLE_CLASS(231, 163, 238, 251) }, { RECTANGLE_CLASS(357,  41, 400,  48) },
    { RECTANGLE_CLASS(205, 213, 212, 251) }, { RECTANGLE_CLASS(390,  59, 400,  66) },
    { RECTANGLE_CLASS(205, 163, 212, 251) }, { RECTANGLE_CLASS(357,  59, 400,  66) },
    { RECTANGLE_CLASS(156, 213, 163, 251) }, { RECTANGLE_CLASS(390,  77, 400,  84) },
    { RECTANGLE_CLASS(156, 163, 163, 251) }, { RECTANGLE_CLASS(357,  77, 400,  84) },
    { RECTANGLE_CLASS(131, 213, 138, 251) }, { RECTANGLE_CLASS(390,  95, 400, 102) },
    { RECTANGLE_CLASS(131, 163, 138, 251) }, { RECTANGLE_CLASS(357,  95, 400, 102) },
    { RECTANGLE_CLASS( 82, 213,  89, 251) }, { RECTANGLE_CLASS(390, 113, 400, 120) },
    { RECTANGLE_CLASS( 82, 163,  89, 251) }, { RECTANGLE_CLASS(357, 113, 400, 120) },
    { RECTANGLE_CLASS( 58, 213,  65, 251) }, { RECTANGLE_CLASS(390, 131, 400, 138) },
    { RECTANGLE_CLASS( 58, 163,  65, 251) }, { RECTANGLE_CLASS(357, 131, 400, 138) },
    { RECTANGLE_CLASS(  9, 213,  16, 251) }, { RECTANGLE_CLASS(390, 149, 400, 156) },
    { RECTANGLE_CLASS(  9, 163,  16, 251) }, { RECTANGLE_CLASS(357, 149, 400, 156) }
    };

const int MAX_MONITOR_WIRE = 8;
static RECTANGLE_CLASS MonitorLedRect[MAX_MONITOR_WIRE] =
    {
    { RECTANGLE_CLASS(123,  92, 192, 114) },
    { RECTANGLE_CLASS(123,  62, 192,  84) },
    { RECTANGLE_CLASS(123,  32, 192,  54) },
    { RECTANGLE_CLASS(123,   3, 192,  25) },
    { RECTANGLE_CLASS( 44,  94, 113, 116) },
    { RECTANGLE_CLASS( 44,  63, 113,  85) },
    { RECTANGLE_CLASS( 44,  33, 113,  55) },
    { RECTANGLE_CLASS( 44,   3, 113,  25) }
    };

static RECTANGLE_CLASS MonitorWireRect[MAX_MONITOR_WIRE] =
    {
    { RECTANGLE_CLASS( 184, 137, 187, 160) },
    { RECTANGLE_CLASS( 168, 137, 171, 160) },
    { RECTANGLE_CLASS( 152, 137, 155, 160) },
    { RECTANGLE_CLASS( 136, 137, 139, 160) },
    { RECTANGLE_CLASS( 120, 137, 123, 160) },
    { RECTANGLE_CLASS( 104, 137, 107, 160) },
    { RECTANGLE_CLASS(  88, 137,  91, 160) },
    { RECTANGLE_CLASS(  72, 137,  75, 160) }
    };

class CHOOSE_WIRE_CLASS : public WINDOW_CLASS
    {
    public:

    CHOOSE_WIRE_CLASS() {}
    ~CHOOSE_WIRE_CLASS() {}
    void operator=( const HWND sorc ) { WINDOW_CLASS::operator=(sorc); }
    int wire_number();
    void set_wire_number( int new_wire_number ) { set_userdata( (LONG) new_wire_number ); }
    };

CHOOSE_WIRE_CLASS ChooseWireCtrl;

/*
------------------------------------------------------------------------
The following are allocated as BYTE arrays and should be deleted as such
------------------------------------------------------------------------ */
static BITMAPFILEHEADER * Bitmap    = 0;
static BITMAPFILEHEADER * LedBitmap = 0;

TCHAR * resource_string( UINT resource_id );

/***********************************************************************
*                         RESOURCE_MESSAGE_BOX                         *
***********************************************************************/
void resource_message_box( UINT msg_id, UINT title_id )
{
resource_message_box( MainInstance, msg_id, title_id );
}

/***********************************************************************
*                           CHOOSE_WIRE_CLASS                          *
*                              wire_number                             *
***********************************************************************/
int CHOOSE_WIRE_CLASS::wire_number()
{
int n;
n = userdata();
if ( n < 1 || n > NofWires )
    n = 0;

return n;
}

/***********************************************************************
*                           CLEANUP_BITMAPS                            *
***********************************************************************/
static void cleanup_bitmaps()
{
BYTE * p;

if ( Bitmap )
    {
    p = (BYTE *) Bitmap;
    Bitmap = 0;
    delete[] p;
    }

if ( LedBitmap )
    {
    p = (BYTE *) LedBitmap;
    LedBitmap = 0;
    delete[] p;
    }
}

/***********************************************************************
*                              DRAW_WIRE                               *
***********************************************************************/
static void draw_wire( HDC dc,  RECTANGLE_CLASS & sorc, double width_multiplier, double height_multiplier )
{
RECTANGLE_CLASS r;

r = sorc;

r.right  *= width_multiplier;
r.left   *= width_multiplier;
r.top    *= height_multiplier;
r.bottom *= height_multiplier;
r.fill( dc, RGB(255, 138, 0) );
}

/***********************************************************************
*                          CHECK_WINDOW_HEIGHT                         *
* The aspect ratio of the two windows is no longer the same. Change    *
* the height of the control based upon the picture.                    *
***********************************************************************/
static void check_window_height()
{
BITMAPINFO * info;
int          dest_height;
int          sorc_height;
int          sorc_width;
RECTANGLE_CLASS r;

if ( Bitmap )
    {
    info = (BITMAPINFO *) (Bitmap + 1) ;
    ChooseWireCtrl.get_move_rect( r );

    sorc_width  = info->bmiHeader.biWidth;
    sorc_height = info->bmiHeader.biHeight;
    if ( sorc_height < 0 )
        sorc_height = -sorc_height;

    dest_height = r.width() * sorc_height;
    if ( sorc_width )
        dest_height /= sorc_width;

    if ( r.height() != dest_height )
        {
        r.bottom = r.top + dest_height;
        ChooseWireCtrl.move( r.left, r.top, r.width(), dest_height, FALSE );
        }
    }
}

/***********************************************************************
*                              PAINT_ME                                *
***********************************************************************/
static void paint_me( HWND w )
{
PAINTSTRUCT  ps;
HDC          dc;
BITMAPINFO * info;
BYTE       * bits;
int          dest_height;
int          dest_width;
int          sorc_height;
int          sorc_width;
int          i;
int          x;
int          y;
double       width_multiplier;
double       height_multiplier;
RECTANGLE_CLASS r;

BeginPaint( w, &ps );
dc = ps.hdc;
GetClientRect( w, &r );
if ( Bitmap )
    {

    if ( AllPurposePalette )
        {
        SelectPalette( dc, AllPurposePalette, FALSE) ;
        RealizePalette( dc ) ;
        }

    info = (BITMAPINFO *) (Bitmap + 1) ;
    bits = (BYTE *) Bitmap + Bitmap->bfOffBits ;

    sorc_width  = info->bmiHeader.biWidth;
    sorc_height = info->bmiHeader.biHeight;
    if ( sorc_height < 0 )
        sorc_height = -sorc_height;

    dest_width  = (int) r.width();
    dest_height = (int) r.height();
    StretchDIBits (dc, 0, 0, dest_width, dest_height, 0, 0, sorc_width, sorc_height, bits, info, DIB_RGB_COLORS, SRCCOPY );
    }

/*
-------------------------------------------------------------------------------------------------------------------------
The wire number is stored in the userdata for the window. Wires for this control are 1 thru 16. 0 means no wire selected.
------------------------------------------------------------------------------------------------------------------------- */
i = GetWindowLong( w, GWL_USERDATA );
i--;

if ( i >= 0 && i < NofWires && LedBitmap != 0 )
    {
    width_multiplier = (double) dest_width;
    if ( sorc_width > 0 )
        width_multiplier /= (double) sorc_width;

    height_multiplier = (double) dest_height;
    if ( sorc_height > 0 )
        height_multiplier /= (double) sorc_height;

    info = (BITMAPINFO *) (LedBitmap + 1) ;
    bits = (BYTE *) LedBitmap + LedBitmap->bfOffBits ;

    x = (int) ( double(LedRect[i].left) * width_multiplier );
    y = (int) ( double(LedRect[i].top ) * height_multiplier );
    sorc_width  = info->bmiHeader.biWidth;
    sorc_height = info->bmiHeader.biHeight;
    if ( sorc_height < 0 )
        sorc_height = -sorc_height;
    dest_width  = (int) ( double(sorc_width)  * width_multiplier );
    dest_height = (int) ( double(sorc_height) * height_multiplier );

    StretchDIBits (dc, x, y, dest_width, dest_height, 0, 0, sorc_width, sorc_height, bits, info, DIB_RGB_COLORS, SRCCOPY );

    /*
    -------------
    Draw the wire
    ------------- */
    if ( CurrentOutputModule == CONTROL_OUTPUT_MODULE )
        i *= 2;
    r = WireRect[i];
    draw_wire( dc,  r, width_multiplier, height_multiplier );

    if ( CurrentOutputModule == CONTROL_OUTPUT_MODULE )
        {
        i++;
        r = WireRect[i];
        draw_wire( dc,  r, width_multiplier, height_multiplier );
        }
    else if ( CurrentOutputModule == MONITOR_OUTPUT_MODULE )
        {
        r.top+=2;
        r.bottom = r.top + 1;
        r.left--;
        r.right++;

        draw_wire( dc,  r, width_multiplier, height_multiplier );

        r.top++;
        r.bottom++;
        draw_wire( dc,  r, width_multiplier, height_multiplier );

        r.top++;
        r.bottom++;
        r.left--;
        r.right++;
        draw_wire( dc,  r, width_multiplier, height_multiplier );

        r.top++;
        r.bottom++;
        r.left--;
        r.right++;
        draw_wire( dc,  r, width_multiplier, height_multiplier );


        r = WireRect[i];
        r.left  -= 8;
        r.right -= 8;
        draw_wire( dc,  r, width_multiplier, height_multiplier );

        r.bottom -= 2;
        r.top = r.bottom - 1;
        r.left--;
        r.right++;

        draw_wire( dc,  r, width_multiplier, height_multiplier );

        r.top--;
        r.bottom--;
        draw_wire( dc,  r, width_multiplier, height_multiplier );

        r.top--;
        r.bottom--;
        r.left--;
        r.right++;
        draw_wire( dc,  r, width_multiplier, height_multiplier );

        r.top--;
        r.bottom--;
        r.left--;
        r.right++;
        draw_wire( dc,  r, width_multiplier, height_multiplier );
        }


    }

    /*
    SetDIBitsToDevice(
        dc,     // handle of device context
        0,      // x-coordinate of upper-left corner of dest. rect.
        0,      // y-coordinate of upper-left corner of dest. rect.
        width,  // source rectangle width
        height, // source rectangle height
        0,      // x-coordinate of lower-left corner of source rect.
        0,      // y-coordinate of lower-left corner of source rect.
        0,      // first scan line in array
        height, // number of scan lines
        bits,   // address of array with DIB bits
        info,   // address of structure with bitmap info.
        0       // RGB or palette indices
        );
    */

EndPaint( w, &ps );
}

/***********************************************************************
*                              read_bitmap                             *
*                                                                      *
* This function returns a pointer to a BITMAPFILEHEADER but it is      *
* really a BYTE array containing the whole file. When you delete       *
* it you should cast to a BYTE array first.                            *
*                                                                      *
***********************************************************************/
static BITMAPFILEHEADER * read_bitmap( TCHAR * filename )
{
BOOL   read_status;
DWORD  bytes_needed;
DWORD  high_bytes;
DWORD  bytes_read;
HANDLE fh;
BYTE * p;
BITMAPFILEHEADER * bitmap;

bitmap = 0;

fh = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL ) ;
if ( fh == INVALID_HANDLE_VALUE )
    return FALSE;

bytes_needed = GetFileSize( fh, &high_bytes );
p = new BYTE[bytes_needed];
if ( p )
    {
    read_status = ReadFile ( fh, p, bytes_needed, &bytes_read, 0 );
    CloseHandle( fh ) ;

    bitmap = (BITMAPFILEHEADER *) p;
    if (!read_status || (bytes_read != bytes_needed ) || (bitmap->bfType != * (WORD *) "BM") || (bitmap->bfSize != bytes_needed) )
        {
        bitmap = 0;
        delete[] p;
        }
    }

return bitmap;
}

/***********************************************************************
*                              read_bitmaps                            *
***********************************************************************/
static BOOLEAN read_bitmaps()
{
static TCHAR control_opto_pic[] = TEXT( "Choose-Wires.bmp" );
static TCHAR control_led_pic[]  = TEXT( "led.bmp" );

static TCHAR monitor_opto_pic[] = TEXT( "Choose-Monitor-Wire.bmp" );
static TCHAR monitor_led_pic[]  = TEXT( "monitor-led.bmp" );

STRING_CLASS s;

/*
------------------------------------
I only need to load the bitmaps once
------------------------------------ */
if ( Bitmap )
    return TRUE;

if ( LedBitmap )
    cleanup_bitmaps();

s = exe_directory();
if ( CurrentOutputModule == CONTROL_OUTPUT_MODULE )
    s.cat_path( control_opto_pic );
else
    s.cat_path( monitor_opto_pic );
Bitmap = read_bitmap( s.text() );

s = exe_directory();
if ( CurrentOutputModule == CONTROL_OUTPUT_MODULE )
    s.cat_path( control_led_pic );
else
    s.cat_path( monitor_led_pic );
if ( Bitmap )
    LedBitmap = read_bitmap( s.text() );

if ( LedBitmap == 0 )
    return FALSE;
else
    return TRUE;
}

/***********************************************************************
*                            SHOW_WIRE_INFO                            *
***********************************************************************/
static void show_wire_info()
{
int selected_wire_number;
int a;
int i;
int n;
STRING_CLASS s;

selected_wire_number = ChooseWireCtrl.wire_number();

if ( selected_wire_number == NO_WIRE )
    {
    WireInfoTbox.set_title( resource_string(NO_OUTPUT_STRING) );
    }
else
    {
    s = resource_string( WIRE_NUMBER_STRING );
    s += selected_wire_number;

    if ( CurrentOutputModule == CONTROL_OUTPUT_MODULE )
        {
        selected_wire_number += CONTROL_WIRE_OFFSET;
        s += TwoSpaceString;
        s += resource_string( OUTPUT_NUMBER_STRING );
        s += selected_wire_number;
        s += SpaceString;
        }

    if ( CurrentMachine.suretrak_controlled )
        {
        for ( i=0; i<NofAlreadyUsedWires; i++ )
            {
            if ( selected_wire_number == AlreadyUsedWires[i].wire_number )
                {
                s += SpaceString;
                s += resource_string( ALREADY_IN_USE_STRING );
                s += resource_string( AlreadyUsedWires[i].id );
                break;
                }
            }
        }

    /*
    ------------------------------------------------------------------------------
    The following two sections are the only areas where I use the common part data
    ------------------------------------------------------------------------------ */
    n = CurrentParam.count();
    for ( i=0; i<n; i++ )
        {
        if ( CurrentOutputType == ALARM_OUTPUT_TYPE && CurrentOutputIndex == i )
            continue;

        for ( a=0; a<NOF_ALARM_LIMIT_TYPES; a++ )
            {
            if ( CurrentParam.parameter[i].limits[a].wire_number == selected_wire_number )
                {
                s += SpaceString;
                s += CurrentParam.parameter[i].name;
                s += SpaceString;
                s += resource_string( ADVANCED_GRID_1_STRING );
                break;
                }
            }
        }

    for ( i=0; i<MAX_FTII_LIMIT_SWITCHES; i++ )
        {
        if ( CurrentOutputType == LIMIT_SWITCH_OUTPUT_TYPE && CurrentOutputIndex == i )
            continue;

        if ( CurrentLimitSwitchWire[i] == (unsigned) selected_wire_number )
            {
            s += SpaceString;
            s += resource_string( LIMIT_SWITCH_WIRE_STRING );
            s += i + 1;
            }
        }

    if ( CurrentOutputType != GOOD_SHOT_OUTPUT_TYPE )
        {
        if ( CurrentGoodShotWire == selected_wire_number )
            {
            s += SpaceString;
            s += resource_string( GOOD_SHOT_WIRE_STRING );
            }
        }

    WireInfoTbox.set_title( s.text() );
    }
}

/***********************************************************************
*                              DO_LED_TAB                              *
***********************************************************************/
static void do_led_tab( TCHAR key )
{
int current_wire_number;

current_wire_number = ChooseWireCtrl.wire_number();

if ( key == MinusChar )
    current_wire_number--;
else
    current_wire_number++;

if ( current_wire_number < NO_WIRE )
    current_wire_number = NofWires;

if ( current_wire_number > NofWires )
    current_wire_number = NO_WIRE;

ChooseWireCtrl.set_wire_number( current_wire_number );
show_wire_info();
}

/***********************************************************************
*                           CHECK_LED_CLICK                            *
***********************************************************************/
static BOOLEAN check_led_click( LPARAM lParam )
{
POINT p;
int   i;
int          dest_height;
int          dest_width;
int          sorc_height;
int          sorc_width;
RECTANGLE_CLASS r;
BITMAPINFO    * info;
double          height_multiplier;
double          width_multiplier;
int             current_index;

if ( !Bitmap )
    return FALSE;

ChooseWireCtrl.get_client_rect( r );
info = (BITMAPINFO *) (Bitmap + 1) ;

sorc_width  = info->bmiHeader.biWidth;
sorc_height = info->bmiHeader.biHeight;
if ( sorc_height < 0 )
    sorc_height = -sorc_height;

dest_width  = (int) r.width();
dest_height = (int) r.height();

width_multiplier = (double) dest_width;
if ( sorc_width > 0 )
    width_multiplier /= (double) sorc_width;

height_multiplier = (double) dest_height;
if ( sorc_height > 0 )
    height_multiplier /= (double) sorc_height;

p.x = LOWORD(lParam);  // horizontal position of cursor
p.y = HIWORD(lParam);  // vertical position of cursor

current_index = ChooseWireCtrl.wire_number() - 1;
for ( i=0; i<NofWires; i++ )
    {
    r = LedRect[i];
    r.right  *= width_multiplier;
    r.left   *= width_multiplier;
    r.top    *= height_multiplier;
    r.bottom *= height_multiplier;

    if ( r.contains(p) )
        {
        if ( current_index != i )
            {
            ChooseWireCtrl.set_wire_number(i+1);
            show_wire_info();
            return TRUE;
            }
        else
            {
            return FALSE;
            }
        }
    }

if ( current_index != NO_INDEX )
    {
    ChooseWireCtrl.set_wire_number( NO_WIRE );
    show_wire_info();
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                              REDISPLAY                               *
***********************************************************************/
static void redisplay( HWND parent, int new_output_module )
{

if ( CurrentOutputModule == new_output_module )
    return;

CurrentOutputModule = new_output_module;

if ( CurrentOutputModule == CONTROL_OUTPUT_MODULE )
    {
    cleanup_bitmaps();
    read_bitmaps();
    check_window_height();
    NofWires = MAX_CONTROL_WIRE;
    LedRect  = ControlLedRect;
    WireRect = ControlWireRect;
    show_wire_info();
    }
else
    {
    CurrentOutputModule = MONITOR_OUTPUT_MODULE;
    cleanup_bitmaps();
    read_bitmaps();
    check_window_height();
    NofWires = MAX_MONITOR_WIRE;
    LedRect  = MonitorLedRect;
    WireRect = MonitorWireRect;
    show_wire_info();
    }

InvalidateRect( parent, 0, TRUE );
}

/***********************************************************************
*                      CREATE_ALL_PURPOSE_PALETTE                      *
*           Now this just makes a copy of the system palette           *
***********************************************************************/
void create_all_purpose_palette( HWND w )
{
int    i;
BYTE * p;
HDC    dc;
DWORD  bytes_needed;
LOGPALETTE   * plp;

/*
---------------------------------------------
Do nothing if this is not a 256 color display
--------------------------------------------- */
p  = 0;
dc = GetDC( w );
i = GetDeviceCaps( dc, BITSPIXEL );
if ( i == 8 )
    {
    bytes_needed = sizeof(LOGPALETTE) + 255 * sizeof(PALETTEENTRY);
    p = new BYTE[bytes_needed];
    if ( p )
        {
        plp = (LOGPALETTE *) p;
        plp->palVersion    = 0x0300 ;
        plp->palNumEntries = 256 ;
        GetSystemPaletteEntries( dc, 0, 256, plp->palPalEntry );
        }
    }

ReleaseDC( w, dc );
if ( !p )
    return;

AllPurposePalette = CreatePalette( plp );
delete[] p;
}

/***********************************************************************
*                           CHOOSE_WIRE_PROC                           *
***********************************************************************/
long CALLBACK choose_wire_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
TCHAR c;
int   id;
HDC   dc;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_CREATE:
        SetWindowLong( hWnd, GWL_USERDATA, (LONG) NO_WIRE );
        create_all_purpose_palette( hWnd );
        return 0;

    case WM_GETDLGCODE:
        return DLGC_WANTCHARS | DLGC_WANTTAB;

    case WM_CHAR:
        c = (TCHAR) wParam;
        if ( c == TabChar )
            {
            if ( GetKeyState(VK_SHIFT) < 0 )
                c = MinusChar;
            else
                c = PlusChar;
            }

        if ( c == PlusChar || c == MinusChar )
            {
            do_led_tab( c );
            InvalidateRect( hWnd, 0, TRUE );
            return 0;
            }
        break;

    case WM_QUERYNEWPALETTE:
        if ( AllPurposePalette )
            {
            dc = GetDC( hWnd );
            SelectPalette( dc, AllPurposePalette, FALSE) ;
            RealizePalette( dc) ;
            InvalidateRect( hWnd, NULL, TRUE );
            ReleaseDC( hWnd, dc );
            }
        return TRUE ;

    case WM_PALETTECHANGED:
        if ( (HWND) wParam != hWnd && AllPurposePalette )
            {
            dc = GetDC( hWnd );
            SelectPalette( dc, AllPurposePalette, FALSE );
            RealizePalette( dc );
            UpdateColors( dc );
            ReleaseDC( hWnd, dc );
            }
        break ;


    case WM_DESTROY:
        if ( AllPurposePalette )
            {
            DeleteObject( AllPurposePalette );
            AllPurposePalette = 0;
            }
        break;

    case WM_PAINT:
        paint_me( hWnd );
        return 0;

    case WM_LBUTTONDOWN:
        if ( check_led_click(lParam) )
            InvalidateRect( hWnd, 0, TRUE );
        return 0;
    }

return  DefWindowProc( hWnd, msg, wParam, lParam );
}

/***********************************************************************
*                     REGISTER_CHOOSE_WIRE_CONTROL                     *
***********************************************************************/
void register_choose_wire_control()
{
WNDCLASS wc;

/*
------------------------------------
Register the ChooseWire window class
------------------------------------ */
wc.lpszClassName = ChooseWireClassName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) choose_wire_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = 0;
wc.lpszMenuName  = 0;
wc.hbrBackground = 0; /* No background */
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );
}

/***********************************************************************
*                              CHECK_WIRE                              *
***********************************************************************/
BOOLEAN check_wire( HWND w )
{
int i;

if ( CurrentMachine.suretrak_controlled )
    {
    for ( i=0; i<NofAlreadyUsedWires; i++ )
        {
        if ( ChosenWire == AlreadyUsedWires[i].wire_number )
            {
            resource_message_box( w, MainInstance, DONT_USE_WIRE_STRING, AlreadyUsedWires[i].id, MB_OK | MB_SYSTEMMODAL );
            return FALSE;
            }
        }
    }
return TRUE;
}

/***********************************************************************
*                    POSITION_FT_OUTPUT_WIRE_DIALOG                    *
***********************************************************************/
static void position_ft_output_wire_dialog( HWND w )
{
RECTANGLE_CLASS r;

GetWindowRect(w, &r );
CenterY -= r.height();

MoveWindow( w, CenterX, CenterY, r.width(), r.height(), TRUE );
}

/***********************************************************************
*                         SAVE_FT_OUTPUT_WIRE                          *
***********************************************************************/
static void save_ft_output_wire( HWND w )
{
STRING_CLASS s;
s.get_text( w, FT_OUTPUT_WIRE_EBOX );
WireSorc.set_title( s.text() );
}

/***********************************************************************
*                      FT_OUTPUT_WIRE_DIALOG_PROC                      *
***********************************************************************/
BOOL CALLBACK ft_output_wire_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;


switch ( msg )
    {
    case WM_INITDIALOG:
        position_ft_output_wire_dialog( hWnd );
        id = ChosenWire;
        if ( CurrentOutputModule == CONTROL_OUTPUT_MODULE )
            id += CONTROL_WIRE_OFFSET;
        set_text( hWnd, FT_OUTPUT_WIRE_EBOX, int32toasc((int32) id) );
        return TRUE;

    case WM_COMMAND:
        id  = LOWORD( wParam );
        switch ( id )
            {
            case IDOK:
                save_ft_output_wire( hWnd );
                EndDialog( hWnd, 1 );
                return TRUE;

            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                            SAVE_POSITION                             *
*  Save the center of the dialog so I can use this to position the     *
*  manual wire entry dialog.                                           *
***********************************************************************/
static void save_position( HWND parent )
{
WINDOW_CLASS w;
RECTANGLE_CLASS r;

w = GetDlgItem( parent, MANUAL_CHOICE_PB );

w.getrect( r );
CenterX = r.left;
CenterY = r.bottom;
}

/***********************************************************************
*                      CHOOSE_WIRE_DIALOG_PROC                         *
***********************************************************************/
BOOL CALLBACK choose_wire_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        WireInfoTbox = GetDlgItem( hWnd, CHOOSE_WIRE_INFO_TBOX );
        ChooseWireCtrl = GetDlgItem( hWnd, CHOOSE_WIRE_CONTROL );
        ChooseWireCtrl.set_wire_number( ChosenWire );
        read_bitmaps();
        check_window_height();
        show_wire_info();
        return TRUE;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
                ChosenWire = ChooseWireCtrl.wire_number();
                if ( CurrentOutputModule==CONTROL_OUTPUT_MODULE && ChosenWire>0 )
                    ChosenWire += CONTROL_WIRE_OFFSET;
                if ( check_wire(hWnd) )
                    {
                    WireSorc.set_title( int32toasc((int32)ChosenWire) );
                    ChooseWireCtrl = 0;
                    EndDialog( hWnd, 1 );
                    }
                return TRUE;

            case IDCANCEL:
                ChooseWireCtrl = 0;
                EndDialog( hWnd, 0 );
                return TRUE;

            case CONTROL_OUTPUT_PB:
                if ( CurrentOutputModule != CONTROL_OUTPUT_MODULE )
                    redisplay( hWnd, CONTROL_OUTPUT_MODULE );
                return TRUE;

            case MONITOR_OUTPUT_PB:
                if ( CurrentOutputModule != MONITOR_OUTPUT_MODULE )
                    redisplay( hWnd, MONITOR_OUTPUT_MODULE );
                return TRUE;

            case MANUAL_CHOICE_PB:
                NeedManualDialog = true;
                save_position( hWnd );
                ChooseWireCtrl = 0;
                EndDialog( hWnd, 1 );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        ChooseWireCtrl = 0;
        EndDialog( hWnd, 0 );
        return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                             CHOOSE_WIRE                              *
* The sorc window is assumed to have the text of the current output    *
* number [1-32].  1-16 are monitor outputs. 17-32 are control outputs. *
* I return TRUE if the user presses OK else FALSE.                     *
*                                                                      *
* I use the CurrentMachine to determine if this is a st2               *
***********************************************************************/
BOOLEAN choose_wire( HWND sorc, HWND parent, int output_type, int output_index )
{
STRING_CLASS s;
BOOL    status;
RECTANGLE_CLASS r;
HWND desktop_window;

NeedManualDialog = false;
status           = FALSE;

/*
------------------------------------------------------------------------------
Old systems with 640x480 screens the graphic is too large. On these I just pop
up the manual entry screen.
------------------------------------------------------------------------------*/
desktop_window = GetDesktopWindow();
if ( desktop_window )
    GetClientRect( desktop_window, &r );

if ( r.width() < 800 )
    {
    status = TRUE;
    NeedManualDialog = true;
    CenterX = r.left + r.width()/2;
    CenterY = r.bottom - r.height()/2;
    }

WireSorc = sorc;
s.get_text( sorc );
ChosenWire = s.int_value();

CurrentOutputType   = output_type;
CurrentOutputIndex  = output_index;
CurrentGoodShotWire = CurrentPart.good_shot_wire;

if ( (CurrentMachine.suretrak_controlled && (ChosenWire == 0 || ChosenWire > CONTROL_WIRE_OFFSET)) || ChosenWire > CONTROL_WIRE_OFFSET )
    {
    if ( ChosenWire > CONTROL_WIRE_OFFSET )
        ChosenWire -= CONTROL_WIRE_OFFSET;

    if ( CurrentOutputModule != CONTROL_OUTPUT_MODULE )
        cleanup_bitmaps();

    CurrentOutputModule = CONTROL_OUTPUT_MODULE;

    NofWires = MAX_CONTROL_WIRE;
    LedRect  = ControlLedRect;
    WireRect = ControlWireRect;
    }
else
    {
    if ( CurrentOutputModule != MONITOR_OUTPUT_MODULE )
        cleanup_bitmaps();

    CurrentOutputModule = MONITOR_OUTPUT_MODULE;

    NofWires = MAX_MONITOR_WIRE;
    LedRect  = MonitorLedRect;
    WireRect = MonitorWireRect;
    }

if ( !NeedManualDialog )
    status = DialogBox( MainInstance, TEXT("CHOOSE_WIRE_DIALOG"), parent, choose_wire_dialog_proc );

if ( status && NeedManualDialog )
    status = DialogBox( MainInstance, TEXT("FT_OUTPUT_WIRE_DIALOG"), parent, ft_output_wire_dialog_proc );

if ( status )
    return TRUE;

return FALSE;
}