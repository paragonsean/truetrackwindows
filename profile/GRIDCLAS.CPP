#include <windows.h>
#include "gridclas.h"

typedef long int int32;

#define WM_NEW_TC_CONFIG  WM_USER+0x116
#define WM_NEW_CL_CONFIG  WM_USER+0x119

static TCHAR DefaultFontName[] = TEXT( "Arial" );

HFONT GRID_CLASS::my_default_font = 0;

void show_one_parameter( int32 parameter_index );
extern HWND MainWindow;

/***********************************************************************
*                               GRID_CLASS                             *
***********************************************************************/
void GRID_CLASS::createfont( void )
{
LOGFONT    lf;

lf.lfHeight          = 13;
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

my_default_font = CreateFontIndirect( &lf );
}

/***********************************************************************
*                               GRID_CLASS                             *
***********************************************************************/
GRID_CLASS::GRID_CLASS()
{

if ( my_default_font == 0 )
    createfont();

}

/***********************************************************************
*                              ~GRID_CLASS                             *
***********************************************************************/
GRID_CLASS::~GRID_CLASS()
{

if ( my_default_font != 0 )
    {
    DeleteObject( my_default_font );
    my_default_font = 0;
    }

}

/***********************************************************************
*                                OnSetup                               *
*            This function is called just after the grid window        *
*            is created or attached to a dialog item.                  *
*            It can be used to initially setup the grid                *
***********************************************************************/
void GRID_CLASS::OnSetup()
{
CUGCell cell;
static TCHAR mytext[] = TEXT( "0" );

SetNumberRows( 1 );
SetNumberCols( 1 );

GetCell(0,0,&cell);

if ( my_default_font )
    cell.SetFont( my_default_font );

cell.SetText( mytext );
SetCell(0,0,&cell);
}

/***********************************************************************
*                                GRID_CLASS                            *
*                                OnLClicked                            *
*   Sent whenever the user clicks the left mouse                       *
*   button within the grid                                             *
*   this message is sent when the button goes down                     *
*   then again when the button goes up                                 *
*                                                                      *
*   'col' and 'row' are negative if the area clicked                   *
*   in is not a valid cell                                             *
*   'rect' the rectangle of the cell that was clicked in               *
*   'point' the point where the mouse was clicked                      *
*   'updn'  TRUE if the button is down FALSE if the                    *
*           button just when up                                        *
***********************************************************************/
void GRID_CLASS::OnLClicked( int col, long row, int updn, RECT * rect, POINT * point, int processed )
{

if( updn )
    StartEdit();

}

/***********************************************************************
*                                GRID_CLASS                            *
*                               OnSH_LClicked                          *
***********************************************************************/
void GRID_CLASS::OnSH_LClicked( int col, long row, int updn, RECT * rect, POINT * point, int processed )
{
show_one_parameter( row );
PostMessage( MainWindow, WM_NEW_TC_CONFIG, 0, 0L );
}


/***********************************************************************
*                                GRID_CLASS                            *
*                                OnEditStart                           *
*       This message is sent whenever the grid is ready to start       *
*       editing a cell A return of TRUE allows the editing a return    *
*       of FALSE stops editing Plus the properties of the CEdit        *
*       class can be modified                                          *
***********************************************************************/
int GRID_CLASS::OnEditStart( int col, long row, HWND *edit )
{
if ( col == LCL_COLUMN_NUMBER || col == UCL_COLUMN_NUMBER )
    {
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                                GRID_CLASS                            *
*                               OnEditFinish                           *
***********************************************************************/
int GRID_CLASS::OnEditFinish( int col, long row, HWND edit, LPCTSTR string, BOOL cancelFlag )
{
if ( SendMessage(edit, EM_GETMODIFY, 0, 0L) )
    {
    if ( MainWindow )
        PostMessage( MainWindow, WM_NEW_CL_CONFIG, 0, 0L );
    }

return TRUE;
}

/***********************************************************************
*                                GRID_CLASS                            *
*                                OnCharDown                            *
***********************************************************************/
void GRID_CLASS::OnCharDown( UINT *vcKey, int processed )
{
if( !processed )
    StartEdit( *vcKey );
}

