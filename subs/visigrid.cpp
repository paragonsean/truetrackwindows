#include <windows.h>
#include "ugctrl.h"

#include "..\include\visiparm.h"
#include "..\include\visigrid.h"
#include "..\include\subs.h"

static BOOLEAN FirstTime = TRUE;
static TCHAR DefaultFontName[] = TEXT( "MS Sans Serif" );
static TCHAR EmptyString[]     = TEXT( "" );

/***********************************************************************
*                           VISI_GRID_CLASS                            *
***********************************************************************/
VISI_GRID_CLASS::VISI_GRID_CLASS()
{
if ( FirstTime )
    {
    FirstTime = FALSE;
    createfont();
    }

prev_control = 0;
next_control = 0;
char_height  = 1;
char_width   = 1;
use_row_numbers = TRUE;
}

/***********************************************************************
*                          ~VISI_GRID_CLASS                            *
***********************************************************************/
VISI_GRID_CLASS::~VISI_GRID_CLASS()
{
}

/***********************************************************************
*                              CREATEFONT                              *
***********************************************************************/
void VISI_GRID_CLASS::createfont( void )
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
    MessageBox( 0, TEXT("Dundas SetDefFont"), TEXT("No Font at this index"), MB_OK );
}

/***********************************************************************
*                                 OnSetup                              *
***********************************************************************/
void VISI_GRID_CLASS::OnSetup()
{
CUGCell cell;
HDC     dc;

dc = GetDC( m_ctrlWnd );
if ( dc != NULL )
    {
    char_height = (int) character_height( dc );
    char_width  = (int) average_character_width( dc );
    ReleaseDC( m_ctrlWnd, dc );
    }

SetDefRowHeight( char_height + 2 );
Set3DHeight( 0 );

GetGridDefault( &cell );
cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN );
cell.SetText( EmptyString );
SetGridDefault( &cell );
}

/***********************************************************************
*                               set_neighbors                          *
***********************************************************************/
void VISI_GRID_CLASS::set_neighbors( HWND prev_control_handle, HWND next_control_handle )
{
prev_control = prev_control_handle;
next_control = next_control_handle;
}

/***********************************************************************
*                              OnCharDown                              *
***********************************************************************/
void VISI_GRID_CLASS::OnCharDown( UINT *vcKey, BOOL processed )
{
static HWND w;
static CUGCell cell;
static int     col;
static long    row;

if ( (*vcKey) == VK_TAB )
    {
    if ( GetKeyState(VK_SHIFT) < 0 )
        w = prev_control;
    else
        w = next_control;
    if ( w )
        SetFocus( w );
    return;
    }

if( processed )
    return;

col = GetCurrentCol();
row = GetCurrentRow();

GetCell( col, row, &cell );

if ( cell.GetCellType() == UGCT_DROPLIST )
    return;

if ( col >= 0 && row >= 0 )
    StartEdit( *vcKey );
}

/***********************************************************************
*                              OnKeyDown                               *
***********************************************************************/
void VISI_GRID_CLASS::OnKeyDown( UINT *vcKey, BOOL processed )
{
if ( (*vcKey) == VK_INSERT   )
    {
    insert_row();
    SetFocus( m_ctrlWnd );
    }

if ( (*vcKey) == VK_DELETE   )
    {
    delete_row();
    SetFocus( m_ctrlWnd );
    }
}

/***********************************************************************
*                              OnLClicked                              *
***********************************************************************/
void VISI_GRID_CLASS::OnLClicked( int col, long row, int updn, RECT * rect, POINT * point, int processed )
{
CUGCell cell;

GetCell( col, row, &cell );
if ( cell.GetCellType() == UGCT_DROPLIST )
    return;

if( updn && (row >= 0) && (col >= 0) )
    StartEdit();
}

/***********************************************************************
*                             OnEditContinue                           *
***********************************************************************/
int VISI_GRID_CLASS::OnEditContinue( int oldcol, long oldrow, int * newcol, long * newrow )
{

if ( GetKeyState(VK_RETURN) < 0 )
    {
    if ( oldcol == (GetNumberCols()-1) )
        {
        *newcol = 0;
        }
    else
        {
        *newcol = oldcol+1;
        *newrow = oldrow;
        }
    }

return TRUE;
}

/***********************************************************************
*                           SET_ROW_NUMBERS                            *
***********************************************************************/
void VISI_GRID_CLASS::set_row_numbers()
{
long row;
long n;
TCHAR s[MAX_INTEGER_LEN+1];

if ( use_row_numbers )
    {
    n = GetNumberRows();
    for ( row=0; row<n; row++ )
        {
        int32toasc( s, (int32) row+1, DECIMAL_RADIX );

        QuickSetText( -1, row, s );
        }
    }
}

/***********************************************************************
*                              DELETE_ROW                              *
***********************************************************************/
void VISI_GRID_CLASS::delete_row()
{
if ( GetNumberRows() > 0 )
    DeleteRow( GetCurrentRow() );

if ( GetNumberRows() > 0 )
    {
    set_row_numbers();
    }

RedrawAll();
}

/***********************************************************************
*                              INSERT_ROW                              *
***********************************************************************/
void VISI_GRID_CLASS::insert_row( BOOLEAN append )
{
CUGCell cell;
int  col;
long n;
long row;

n   = GetNumberRows();
row = 0;

if ( n > 0 )
    row = GetCurrentRow();

if ( append )
    AppendRow();
else
    InsertRow( row );

GetGridDefault( &cell );
cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN );
cell.SetText( TEXT("") );

n = GetNumberCols();
for ( col=0; col<n; col++ )
    SetCell( col, row, &cell );

/*
---------------------
Left border cells |_|
--------------------- */
cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN | UG_BDR_LTHIN );
SetCell( -1, row, &cell );

set_row_numbers();

RedrawAll();
}

/***********************************************************************
*                                init                                  *
***********************************************************************/
void VISI_GRID_CLASS::init( int nof_cols, long nof_rows )
{
CUGCell cell;
int  col;
long row;
TCHAR s[MAX_INTEGER_LEN+1];

GetGridDefault( &cell );

SetNumberCols( nof_cols );
SetNumberRows( nof_rows, FALSE );

/*
------------------------
                      _
Top left corner cell |_|
------------------------ */
cell.SetText( EmptyString );
cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN | UG_BDR_TTHIN | UG_BDR_LTHIN );
SetCell( -1, -1, &cell );

/*
---------------------
Left border cells |_|
--------------------- */
cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN | UG_BDR_LTHIN );
for ( row=0; row<nof_rows; row++ )
    {
    if ( use_row_numbers )
        {
        int32toasc( s, (int32) row+1, DECIMAL_RADIX );
        cell.SetText( s );
        }
    SetCell( -1, row, &cell );
    }

/*
-------------------
                 _
Top border cells _|
------------------- */
cell.SetBorder( UG_BDR_RTHIN | UG_BDR_BTHIN | UG_BDR_TTHIN );
cell.SetCellTypeEx( UGCT_NORMALMULTILINE );
cell.SetText( EmptyString );
for ( col=0; col<nof_cols; col++ )
    {
    cell.SetAlignment( UG_ALIGNCENTER );
    SetCell( col, -1, &cell );
    }

AdjustComponentSizes();
}

/***********************************************************************
*                            SET_COL_WIDTH                             *
***********************************************************************/
void VISI_GRID_CLASS::set_col_width(int col, int width_in_chars )
{
int width;
width = width_in_chars * char_width;
if ( col < 0 )
    SetSH_Width( width );
else
    SetColWidth( col, width );
}
