#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\names.h"
#include "..\include\computer.h"
#include "..\include\dbclass.h"
#include "..\include\machine.h"
#include "..\include\param.h"
#include "..\include\subs.h"

#include "resource.h"
#include "extern.h"

HWND MyWin;

/***********************************************************************
*                           REDISPLAY_WINDOW                           *
***********************************************************************/
static void redisplay_window( HWND w )
{
if ( !w )
    return;

if ( IsWindowVisible(w) )
    InvalidateRect( w, 0, TRUE );
}

/***********************************************************************
*                           REDISPLAY_COLORS                           *
***********************************************************************/
static void redisplay_colors( UINT id )
{

switch ( id )
    {
    case NO_ALARM_COLOR_SQUARE:
    case ALARM_COLOR_SQUARE:
    case WARNING_COLOR_SQUARE:
    case DIALOG_TEXT_COLOR_SQUARE:
        redisplay_window( ParameterWindow );
        //redisplay_window( TwoParamBoxWindow );
        redisplay_window( CornerParamDialogWindow );
        redisplay_window( ParamHistoryWindow );
        redisplay_window( ViewParametersDialogWindow );
        break;

    default:
        redisplay_window( MainPlotWindow );
        break;
    }

}

/***********************************************************************
*                              GET_COLOR                               *
***********************************************************************/
COLORREF get_color( UINT id )
{
int i;

for ( i=0; i<NOF_COLORS; i++ )
    {
    if ( id == UINT(SCREEN_NORMAL_SQUARE_ONE+i) )
        return NormalColor[i];

    if ( id == UINT(PRINT_NORMAL_SQUARE_ONE+i) )
        return PrNormalColor[i];
    }

for ( i=0; i<NOF_AXES; i++ )
    {
    if ( id == UINT(SCREEN_REF_SQUARE_ONE+i) )
        return ReferenceColor[i];

    if ( id == UINT(PRINT_REF_SQUARE_ONE+i) )
        return PrReferenceColor[i];
    }

switch ( id )
    {
    case NO_ALARM_COLOR_SQUARE:
        return NoAlarmColor;

    case ALARM_COLOR_SQUARE:
        return AlarmColor;

    case WARNING_COLOR_SQUARE:
        return WarningColor;

    case DIALOG_TEXT_COLOR_SQUARE:
        return DialogTextColor;
    }


return 0;
}

COLORREF SavedNormalColor[NOF_COLORS];
COLORREF SavedPrNormalColor[NOF_COLORS];
COLORREF SavedReferenceColor[NOF_COLORS];
COLORREF SavedPrReferenceColor[NOF_COLORS];
COLORREF SavedAlarmColor;
COLORREF SavedNoAlarmColor;
COLORREF SavedWarningColor;
COLORREF SavedDialogTextColor;

/***********************************************************************
*                         SAVE_ORIGINAL_COLORS                         *
***********************************************************************/
void save_original_colors( void )
{
int i;

for ( i=0; i<NOF_COLORS; i++ )
    {
    SavedNormalColor[i]      = NormalColor[i];
    SavedPrNormalColor[i]    = PrNormalColor[i];
    SavedReferenceColor[i]   = ReferenceColor[i];
    SavedPrReferenceColor[i] = PrReferenceColor[i];
    }

SavedAlarmColor      = AlarmColor;
SavedNoAlarmColor    = NoAlarmColor;
SavedWarningColor    = WarningColor;
SavedDialogTextColor = DialogTextColor;
}

/***********************************************************************
*                       RESTORE_ORIGINAL_COLORS                        *
***********************************************************************/
void restore_original_colors( void )
{
int i;

for ( i=0; i<NOF_COLORS; i++ )
    {
    NormalColor[i]      = SavedNormalColor[i];
    PrNormalColor[i]    = SavedPrNormalColor[i];
    ReferenceColor[i]   = SavedReferenceColor[i];
    PrReferenceColor[i] = SavedPrReferenceColor[i];
    }

AlarmColor      = SavedAlarmColor;
NoAlarmColor    = SavedNoAlarmColor;
WarningColor    = SavedWarningColor;
DialogTextColor = SavedDialogTextColor;
}

/***********************************************************************
*                        RESTORE_DEFAULT_COLORS                        *
***********************************************************************/
static void restore_default_colors()
{
static COLORREF DefAlarmColor      = RGB(255,   0,   0 );
static COLORREF DefNoAlarmColor    = RGB(  0, 255,   0 );
static COLORREF DefWarningColor    = RGB(255, 255,   0 );
static COLORREF DefDialogTextColor = RGB(  0,   0,   0 );

static COLORREF DefNormalColor[NOF_COLORS] =
    {
    RGB( 192, 192, 192 ),     /* Time       */
    RGB( 255, 255,   0 ),     /* Y Position */
    RGB( 192, 192, 192 ),     /* X Position */
    RGB(   0, 255,   0 ),     /* Velocity   */
    RGB( 255, 255, 255 ),     /* Analog 1   */
    RGB(   0, 255, 255 ),     /* Analog 2   */
    RGB( 128, 128,   0 ),     /* Analog 3   */
    RGB(   0, 255,   0 ),     /* Analog 4   */
    RGB(   0,   0,   0 ),     /* Background */
    RGB(   0,   0, 255 ),     /* Border     */
    RGB( 128, 128, 128 ),     /* Grid       */
    RGB( 255, 255, 255 ),     /* Text       */
    RGB(  41,  46,  52 )      /* Trend Background */
    };

static COLORREF DefReferenceColor[NOF_COLORS] =
    {
    RGB( 198,   0,   0 ),     /* Time       */
    RGB( 198,   0,   0 ),     /* Y Position */
    RGB( 198,   0,   0 ),     /* X Position */
    RGB( 255,   0,   0 ),     /* Velocity   */
    RGB( 198,  50,   0 ),     /* Analog 1   */
    RGB( 198,   0,   0 ),     /* Analog 2   */
    RGB( 198,   0,   0 ),     /* Analog 3   */
    RGB(   0,   0,   0 ),     /* Analog 4   */
    RGB(   0,   0,   0 ),     /* Background */
    RGB(   0,   0, 255 ),     /* Border     */
    RGB( 150, 150, 150 ),     /* Grid       */
    RGB( 255, 255, 255 ),     /* Text       */
    RGB(  41,  46,  52 )      /* Trend Background */
    };

static COLORREF DefPrNormalColor[NOF_COLORS] =
    {
    RGB(   0,   0,   0 ),     /* Time       */
    RGB( 255, 255,   0 ),     /* Y Position */
    RGB(   0,   0, 255 ),     /* X Position */
    RGB(   0, 255,   0 ),     /* Velocity   */
    RGB(   0,   0,   0 ),     /* Analog 1   */
    RGB(   0, 255, 150 ),     /* Analog 2   */
    RGB(   0,   0, 255 ),     /* Analog 3   */
    RGB(   0, 255,   0 ),     /* Analog 4   */
    RGB( 255, 255, 255 ),     /* Background */
    RGB(  50,  50, 255 ),     /* Border     */
    RGB( 150, 150, 150 ),     /* Grid       */
    RGB(  64,   0,  64 ),     /* Text       */
    RGB( 244, 244, 244 )      /* Trend Background */
    };

static COLORREF DefPrReferenceColor[NOF_COLORS] =
    {
    RGB(  64,  64,  64 ),     /* Time       */
    RGB( 198,   0,   0 ),     /* Y Position */
    RGB( 198,   0,   0 ),     /* X Position */
    RGB( 198,   0,   0 ),     /* Velocity   */
    RGB( 198,  50,   0 ),     /* Analog 1   */
    RGB( 198,   0,   0 ),     /* Analog 2   */
    RGB( 198,   0,   0 ),     /* Analog 3   */
    RGB( 198,   0,   0 ),     /* Analog 4   */
    RGB( 255, 255, 255 ),     /* Background */
    RGB(  50,  50, 255 ),     /* Border     */
    RGB( 150, 150, 150 ),     /* Grid       */
    RGB(  64,   0,  64 ),     /* Text       */
    RGB( 244, 244, 244 )      /* Trend Background */
    };
int i;

for ( i=0; i<NOF_COLORS; i++ )
    {
    NormalColor[i]      = DefNormalColor[i];
    PrNormalColor[i]    = DefPrNormalColor[i];
    ReferenceColor[i]   = DefReferenceColor[i];
    PrReferenceColor[i] = DefPrReferenceColor[i];
    }

AlarmColor      = DefAlarmColor;
NoAlarmColor    = DefNoAlarmColor;
WarningColor    = DefWarningColor;
DialogTextColor = DefDialogTextColor;
}

/***********************************************************************
*                            SET_ONE_COLOR                             *
***********************************************************************/
void set_one_color( UINT id, COLORREF mycolor )
{
int i;

for ( i=0; i<NOF_AXES; i++ )
    {
    if ( id == UINT(SCREEN_NORMAL_SQUARE_ONE+i) )
        {
        NormalColor[i] = mycolor;
        return;
        }

    if ( id == UINT(PRINT_NORMAL_SQUARE_ONE+i) )
        {
        PrNormalColor[i] = mycolor;
        return;
        }

    if ( id == UINT(SCREEN_REF_SQUARE_ONE+i) )
        {
        ReferenceColor[i] = mycolor;
        return;
        }

    if ( id == UINT(PRINT_REF_SQUARE_ONE+i) )
        {
        PrReferenceColor[i] = mycolor;
        return;
        }
    }

for ( i=NOF_AXES; i<NOF_COLORS; i++ )
    {
    if ( id == UINT(SCREEN_NORMAL_SQUARE_ONE+i) )
        {
        NormalColor[i]    = mycolor;
        ReferenceColor[i] = mycolor;
        return;
        }

    if ( id == UINT(PRINT_NORMAL_SQUARE_ONE+i) )
        {
        PrNormalColor[i]    = mycolor;
        PrReferenceColor[i] = mycolor;
        return;
        }
    }

switch ( id )
    {
    case NO_ALARM_COLOR_SQUARE:
        NoAlarmColor    = mycolor;
        return;

    case ALARM_COLOR_SQUARE:
        AlarmColor      = mycolor;
        return;

    case WARNING_COLOR_SQUARE:
        WarningColor    = mycolor;
        return;

    case DIALOG_TEXT_COLOR_SQUARE:
        DialogTextColor = mycolor;
        return;
    }

return;
}

/***********************************************************************
*                            DRAW_ONE_ITEM                             *
***********************************************************************/
static void draw_one_item( UINT item_id, DRAWITEMSTRUCT * dp )
{
HBRUSH   oldbrush;
HBRUSH   mybrush;
RECT     r;
HDC      dc;

dc = dp->hDC;
r  = dp->rcItem;

mybrush  = (HBRUSH) CreateSolidBrush( get_color(item_id) );
oldbrush = (HBRUSH) SelectObject( dc, mybrush );

FillRect( dc, &r, mybrush );

SelectObject( dc, oldbrush );
DeleteObject( mybrush );
}

/***********************************************************************
*                           CHOOSE_ONE_COLOR                           *
***********************************************************************/
COLORREF choose_one_color( UINT id )
{

CHOOSECOLOR cc;                 // common dialog box structure
static COLORREF acrCustClr[16]; // array of custom colors
COLORREF mycolor;

mycolor = get_color( id );

ZeroMemory( &cc, sizeof(CHOOSECOLOR) );

cc.lStructSize    = sizeof(CHOOSECOLOR);
cc.hwndOwner      = MyWin;
cc.hInstance      = (HWND) MainInstance;
cc.lpCustColors   = (LPDWORD) acrCustClr;
cc.rgbResult      = mycolor;
/*cc.Flags          = CC_ENABLETEMPLATE | CC_RGBINIT | CC_PREVENTFULLOPEN; */
cc.Flags          =   CC_RGBINIT | CC_FULLOPEN;
cc.lpTemplateName = TEXT( "ChooseColor" );

if (ChooseColor(&cc)==TRUE)
    {
    mycolor = cc.rgbResult;
    set_one_color( id, mycolor );
    InvalidateRect( GetDlgItem(MyWin, id), 0, TRUE );
    InvalidateRect( MainPlotWindow, 0, TRUE );
    }

return mycolor;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
/***********************************************************************
*                     COLOR_DEFINITION_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK color_definition_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
static BOOLEAN need_to_restore = TRUE;
int id;

id  = LOWORD( wParam );

switch ( msg )
    {
    case WM_INITDIALOG:
        MyWin = hWnd;
        need_to_restore = TRUE;
        save_original_colors();
        return FALSE;

    case WM_DRAWITEM:
        draw_one_item( (UINT) wParam, (DRAWITEMSTRUCT *) lParam );
        return TRUE;

    case WM_COMMAND:
        if ( id >= SCREEN_NORMAL_SQUARE_ONE && id <= LAST_COLOR_SQUARE )
            {
            choose_one_color( id );
            redisplay_colors( id );
            return TRUE;
            }

        switch ( id )
            {
            case DEFAULT_COLORS_PB:
                restore_default_colors();
                InvalidateRect( MainPlotWindow, 0, TRUE );
                InvalidateRect( ParamHistoryWindow, 0, TRUE );
                InvalidateRect( hWnd, 0, TRUE );
                return TRUE;

            case IDCANCEL:
                restore_original_colors();
                InvalidateRect( MainPlotWindow, 0, TRUE );
                InvalidateRect( ParamHistoryWindow, 0, TRUE );
                InvalidateRect( hWnd, 0, TRUE );
                return TRUE;

            case IDOK:
                need_to_restore = FALSE;
                save_original_colors();
                SendMessage( hWnd, WM_CLOSE, 0, 0L );
                return TRUE;
            }
        break;

    case WM_CLOSE:
        if ( need_to_restore )
            {
            restore_original_colors();
            InvalidateRect( MainPlotWindow, 0, TRUE );
            InvalidateRect( ParamHistoryWindow, 0, TRUE );
            }
        ShowWindow( hWnd, SW_HIDE );
        return TRUE;

    }

return FALSE;
}
