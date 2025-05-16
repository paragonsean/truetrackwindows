#include <windows.h>
#include <ugctrl.h>

#include "..\include\visiparm.h"
#include "..\include\genlist.h"
#include "..\include\stringcl.h"
#include "..\include\subs.h"
#include "..\include\wclass.h"

#include "ugtest.h"

HINSTANCE    MainInstance;
WINDOW_CLASS MainWindow;
WINDOW_CLASS GridWindow;

static TCHAR DefaultFontName[] = TEXT( "MS Sans Serif" );

class GRID_CLASS : public CUGCtrl
{
private:
int     my_font_index;
void    createfont( void );
BOOLEAN is_editable( void );

public:

GRID_CLASS();
~GRID_CLASS();
void OnSetup();
void select_row( long row );
void OnCharDown( UINT *vcKey, int processed );
void OnLClicked( int col, long row, int updn, RECT * rect, POINT * point, int processed );
void OnSH_LClicked( int col, long row, int updn, RECT *rect, POINT *point, BOOL processed );
};

GRID_CLASS   Grid;
int32        NofCols = 5;
int32        NofRows = 20;

TCHAR * RowName[] = {
    { "Fill Time" },
    { "Total Shot Time" },
    { "Fast Shot Time" },
    { "Slow Shot Time" },
    { "Cycle Time" },
    { "Avg Fast Shot Pressure" },
    { "Avg Slow Shot Pressure" },
    { "Metal Temperature" },
    { "Fast Shot Rise Time" },
    { "Biscuit Size" },
    { "Calc Start Fast Shot" },
    { "Intens Pressure" },
    { "Intens Rise Time" },
    { "Eff Peak Intens" },
    { "Impact Pressure To Peak" },
    { "Intens Squeeze Distance" },
    { "Sleeve Full Velocity" },
    { "Runner Full Velocity" },
    { "Avg Fill Velocity" },
    { "Fast Shot Distance" }
    };

TCHAR * ColName[] = {
    { "Average" },
    { "Std. Dev." },
    { "UCL" },
    { "LCL" },
    { "Cpk" },
    };

TCHAR MyName[]  = TEXT( "UGTest" );
TCHAR MyTitle[] = TEXT( "Ultimate Grid Test" );

/***********************************************************************
*                               GRID_CLASS                             *
***********************************************************************/
GRID_CLASS::GRID_CLASS()
{
createfont();
}

/***********************************************************************
*                              ~GRID_CLASS                             *
***********************************************************************/
GRID_CLASS::~GRID_CLASS()
{
RemoveFont( my_font_index );
}

/***********************************************************************
*                              GRID_CLASS                              *
*                              CREATEFONT                              *
***********************************************************************/
void GRID_CLASS::createfont( void )
{
LOGFONT    lf;

lf.lfHeight          = 14;
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
    MessageBox( 0, "Hi you guys", "No Font at this index", MB_OK );
}

/***********************************************************************
*                                GRID_CLASS                            *
*                                 OnSetup                              *
***********************************************************************/
void GRID_CLASS::OnSetup()
{
CUGCell cell;
HDC     dc;
static TCHAR mytext[] = TEXT( "0" );

SetNumberRows( 1 );
SetNumberCols( 1 );

GetCell(0,0,&cell);

cell.SetText( mytext );

SetCell(0,0,&cell);

dc = GetDC( m_ctrlWnd );
if ( dc != NULL )
    {
    SetDefRowHeight( (int) character_height(dc) );
    SetRowHeight( 0, (int) character_height(dc) );
    ReleaseDC( m_ctrlWnd, dc );
    }

Set3DHeight( 0 );
SetMultiSelectMode(UG_MULTISELECT_ROW);
}

/***********************************************************************
*                                GRID_CLASS                            *
*                                SELECT_ROW                            *
***********************************************************************/
void GRID_CLASS::select_row( long row )
{
int last_col;

if ( !IsSelected(0, row) )
    {
    last_col = GetNumberCols() - 1;
    SelectRange( 0, row, last_col, row );
    }
}

/***********************************************************************
*                                GRID_CLASS                            *
*                               OnSH_LClicked                          *
***********************************************************************/
void GRID_CLASS::OnSH_LClicked( int col, long row, int updn, RECT *rect, POINT *point, BOOL processed )
{
BOOLEAN shift_key_down   = FALSE;
BOOLEAN control_key_down = FALSE;

if ( updn )
    {
    if ( !IsSelected(col, row) )
        {
        if ( GetKeyState(VK_CONTROL) < 0 )
            control_key_down = TRUE;

        if ( GetKeyState(VK_SHIFT) < 0 )
            shift_key_down = TRUE;

        if ( !shift_key_down && !control_key_down )
            ClearSelections();

        if ( shift_key_down )
            m_GI->m_multiSelect->EndBlock( GetNumberCols()-1, row );
        else
            select_row( row );

        Update();
        }
    }
}

/***********************************************************************
*                             IS_EDITABLE                              *
***********************************************************************/
BOOLEAN GRID_CLASS::is_editable( void )
{
int  col;
long row;

col = GetCurrentCol();
row = GetCurrentRow();

if ( col==0 && (row==0 || row==1) )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                              GRID_CLASS                              *
*                              OnCharDown                              *
***********************************************************************/
void GRID_CLASS::OnCharDown( UINT *vcKey, int processed )
{

if( !processed )
    if ( is_editable() )
        StartEdit( *vcKey );
}

/***********************************************************************
*                              GRID_CLASS                              *
*                              OnLClicked                              *
***********************************************************************/
void GRID_CLASS::OnLClicked( int col, long row, int updn, RECT * rect, POINT * point, int processed )
{
if( updn && is_editable() )
    StartEdit();
}

/***********************************************************************
*                               FILL_GRID                              *
***********************************************************************/
static void fill_grid( void )
{
static BOOLEAN firstime = TRUE;

HDC     dc;
int32   col;
int32   row;
int32   maxwide;
int32   i;
int32   w;
RECT    r;
CUGCell cell;

if ( firstime )
    firstime = FALSE;

Grid.GetCell( 0, 0, &cell );

Grid.SetNumberRows( NofRows, FALSE );
Grid.SetNumberCols( NofCols, FALSE );

i = Grid.GetRowHeight(0) + 1;
i *= 2;
Grid.SetTH_NumberRows( 2 );
Grid.SetTH_Height( i );

cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN | UG_BDR_TTHIN );
cell.SetText( "Hi you guys" );
Grid.SetCell( 0, -2, &cell );

cell.SetText( "" );
for ( col=1; col<NofCols; col++ )
    {
    cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN | UG_BDR_TTHIN );
    Grid.SetCell( col, -2, &cell );
    }

cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN | UG_BDR_TTHIN | UG_BDR_LTHIN );
cell.SetText( "What's Up" );
Grid.SetCell( -1, -1, &cell );

Grid.JoinCells( 0, -2, 2, -2 );

cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN | UG_BDR_TTHIN );
for ( col=0; col<NofCols; col++ )
    {
    cell.SetText( ColName[col] );
    Grid.SetCell( col, -1, &cell );
    }

cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN | UG_BDR_LTHIN );
for ( row=0; row<NofRows; row++ )
    {
    cell.SetText( RowName[row]  );
    Grid.SetCell( -1, row, &cell );
    }

cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN );
cell.SetText( "0.0" );
for ( row=0; row<NofRows; row++ )
    for ( col=0; col<NofCols; col++ )
        Grid.SetCell( col, row, &cell );

cell.SetText( "Don't Touch" );
Grid.SetCell( 0, 0, &cell );

cell.SetCellType( UGCT_DROPLIST );
cell.SetText( "Computer" );
cell.SetLabelText( "Computer\nZippy\n0.0" );
Grid.SetCell( 0, 1, &cell );

dc = GetDC( Grid.m_ctrlWnd );
if ( dc != NULL )
    {
    maxwide = 0;
    for ( row=0; row<NofRows; row++ )
        {
        i = pixel_width( dc, RowName[row] );
        if ( i > maxwide )
            maxwide = i;
        }

    ReleaseDC( Grid.m_ctrlWnd, dc );

    if ( maxwide > 0 )
        Grid.SetSH_Width( maxwide );

    GetWindowRect( Grid.m_ctrlWnd, &r );
    r.right -= Grid.GetVS_Width();
    r.left  += maxwide;
    w = r.right - r.left;
    if ( w > 0 )
        {
        w /= NofCols;
        for ( col=0; col<NofCols; col++ )
            Grid.SetColWidth( col, w );
        }

    Grid.AdjustComponentSizes();
    }

}

/***********************************************************************
*                       DELETE_HIGHLIGHTED_ROWS                        *
***********************************************************************/
void delete_highlighted_rows( void )
{
long row;
BOOLEAN one_row_remains;

one_row_remains = FALSE;

for ( row = 0; row<NofRows; row++ )
    {
    if ( !Grid.IsSelected(0, row) )
        {
        one_row_remains = TRUE;
        break;
        }
    }

if ( !one_row_remains )
    {
    MessageBox( NULL, "You may not delete all the rows", "Attempt to Delete All Rows", MB_OK );
    return;
    }

row = NofRows-1;
while ( row >= 0 )
    {
    if ( Grid.IsSelected(0, row) && Grid.IsSelected(1,row) )
        {
        Grid.DeleteRow( row );
        NofRows--;
        }
    row--;
    }

Grid.ClearSelections();
Grid.RedrawAll();
SetFocus( Grid.m_ctrlWnd );
}

/***********************************************************************
*                         EDIT_DOWN_DIALOG_PROC                        *
***********************************************************************/
BOOL CALLBACK edit_down_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;
id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        GridWindow = hWnd;
        Grid.AttachGrid( hWnd, GRID_ID );
        PostMessage( hWnd, WM_DBINIT, 0, 0L );
        break;

    case WM_DBINIT:
        MainWindow.shrinkwrap( hWnd );
        fill_grid();
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
            case IDCANCEL:
                MainWindow.close();
                return TRUE;

            case INSERT_BUTTON:
                return TRUE;

            case DELETE_BUTTON:
                delete_highlighted_rows();
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
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_COMMAND:
        switch (id)
            {
            case 108:
                MainWindow.close();
                return 0;
            }
        break;

    case WM_DBINIT:
        return 0;

    case WM_CREATE:
        MainWindow = hWnd;
        MainWindow.post( WM_DBINIT );
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
static void startup( void )
{

WNDCLASS wc;

wc.lpszClassName = MyName;
wc.hInstance     = MainInstance;
wc.lpfnWndProc   = (WNDPROC) main_proc;
wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
wc.hIcon         = LoadIcon( MainInstance, MyName );
wc.lpszMenuName  = (LPSTR) "MainMenu";
wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
wc.style         = CS_HREDRAW | CS_VREDRAW;
wc.cbClsExtra    = 0;
wc.cbWndExtra    = 0;

RegisterClass( &wc );

CreateWindowEx(
    WS_EX_APPWINDOW,
    MyName,
    MyTitle,
    WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
    CW_USEDEFAULT, CW_USEDEFAULT,     // X,y
    405, 100,                         // W,h
    NULL,
    NULL,
    MainInstance,
    NULL
    );

CreateDialog(
    MainInstance,
    TEXT("EDIT_DOWN_DIALOG"),
    MainWindow.handle(),
    (DLGPROC) edit_down_dialog_proc
    );

MainWindow.show( SW_SHOW );
GridWindow.show( SW_SHOW );

UpdateWindow( MainWindow.handle() );
}

/***********************************************************************
*                               SHUTDOWN                               *
***********************************************************************/
static void shutdown( void )
{
}

/***********************************************************************
*                               WINMAIN                                *
***********************************************************************/
APIENTRY WinMain( HINSTANCE this_instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
MSG   msg;
BOOL status;

MainInstance = this_instance;

startup();

while ( GetMessage(&msg, NULL, 0, 0) )
    {
    status = FALSE;

    if ( !status && GridWindow.handle() )
        status = IsDialogMessage( GridWindow.handle(), &msg );

    if ( !status )
        {
        TranslateMessage( &msg );
        DispatchMessage(  &msg );
        }
    }

shutdown();
return(msg.wParam);
}
