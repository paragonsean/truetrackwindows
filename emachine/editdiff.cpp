#include <windows.h>
#include "..\include\ugctrl.h"

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\names.h"
#include "..\include\computer.h"
#include "..\include\listbox.h"
#include "..\include\subs.h"
#include "..\include\v5help.h"

#include "resource.h"

static TCHAR DefaultFontName[] = TEXT( "Arial" );

static const int NOF_COLS = 4;
static const int LAST_COL = 3;

class EDITDIFF_GRID_CLASS : public CUGCtrl
{
private:
static HFONT my_default_font;

public:

EDITDIFF_GRID_CLASS();
~EDITDIFF_GRID_CLASS();

void    createfont( void );
void    OnSetup();
void    delete_highlighted_rows( void );
void    select_row( long row );
void    insert_row( void );
BOOLEAN is_row_selected( int row );
void    refresh_row_labels( void );
void    OnLClicked( int col, long row, int updn, RECT * rect, POINT * point, BOOL processed );
void    OnCharDown( UINT *vcKey, int processed );
void    OnKeyDown( UINT *vcKey, BOOL processed);
int     OnEditFinish( int col, long row, HWND edit, LPCTSTR string, BOOL cancelFlag );
void    OnSH_LClicked( int col, long row, int updn, RECT *rect, POINT *point, BOOL processed );
};

HFONT EDITDIFF_GRID_CLASS::my_default_font = 0;

static EDITDIFF_GRID_CLASS Grid;
static int32 MaxChars;

extern HINSTANCE MainInstance;
extern HWND   MachSetupWindow;
extern HWND   EditEquationsWindow;

TCHAR * resource_string( UINT resource_id );
void gethelp( UINT helptype, DWORD context );

/***********************************************************************
*                          INPUT_ERROR_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK input_error_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        set_text( hWnd, INPUT_ERROR_MAX_CHARS_TBOX, int32toasc(MaxChars) );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case IDOK:
            case IDCANCEL:
                EndDialog( hWnd, 0 );
                return TRUE;
            }
        break;
    }

return FALSE;
}

/***********************************************************************
*                          EDIT0P_GRID_CLASS                           *
***********************************************************************/
void EDITDIFF_GRID_CLASS::createfont( void )
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

if ( PRIMARYLANGID(GetUserDefaultLangID()) == LANG_JAPANESE )
    {
    lf.lfCharSet = SHIFTJIS_CHARSET;
    lstrcpy( lf.lfFaceName, TEXT("MS –¾’©") );
    }

my_default_font = CreateFontIndirect( &lf );
}

/***********************************************************************
*                          EDITDIFF_GRID_CLASS                           *
***********************************************************************/
EDITDIFF_GRID_CLASS::EDITDIFF_GRID_CLASS()
{
if ( my_default_font == 0 )
    createfont();
}

/***********************************************************************
*                         ~EDITDIFF_GRID_CLASS                         *
***********************************************************************/
EDITDIFF_GRID_CLASS::~EDITDIFF_GRID_CLASS()
{

if ( my_default_font != 0 )
    {
    DeleteObject( my_default_font );
    my_default_font = 0;
    }
}

/***********************************************************************
*                           EDITDIFF_GRID_CLASS                        *
*                              SELECT_ROW                              *
***********************************************************************/
void EDITDIFF_GRID_CLASS::select_row( long row )
{

if ( !IsSelected(0, row) || !IsSelected(LAST_COL, row) )
    {
    GotoRow( row );
    SelectRange( 0, row, LAST_COL, row );
    }
}

/***********************************************************************
*                          EDITDIFF_GRID_CLASS                         *
*                          REFRESH_ROW_LABELS                          *
***********************************************************************/
void EDITDIFF_GRID_CLASS::refresh_row_labels( void )
{
CUGCell  cell;
long row;
long nof_rows;

nof_rows = GetNumberRows();

GetCell( -1, 0, &cell );
for ( row=0; row<nof_rows; row++ )
    {
    cell.SetText( int32toasc(row+1) );
    SetCell( -1, row, &cell );
    }

}

/***********************************************************************
*                          EDITDIFF_GRID_CLASS                         *
*                             INSERT_ROW                               *
***********************************************************************/
void EDITDIFF_GRID_CLASS::insert_row( void )
{
CUGCell  cell;
long current_row;
long old_row;
int  col;

current_row = GetCurrentRow();
old_row     = current_row + 1;
Grid.InsertRow( current_row );

/*
---------------------
Duplicate the old row
--------------------- */
for ( col=0; col<NOF_COLS; col++ )
    {
    Grid.GetCell( col, old_row,     &cell );
    Grid.SetCell( col, current_row, &cell );
    }

refresh_row_labels();
SetFocus( Grid.m_ctrlWnd );
}

/***********************************************************************
*                         EDITDIFF_GRID_CLASS                          *
*                          IS_ROW_SELECTED                             *
***********************************************************************/
BOOLEAN EDITDIFF_GRID_CLASS::is_row_selected( int row )
{
if ( IsSelected(0, row) && IsSelected(LAST_COL, row) )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                       DELETE_HIGHLIGHTED_ROWS                        *
***********************************************************************/
void EDITDIFF_GRID_CLASS::delete_highlighted_rows( void )
{
long nof_rows;
long row;
BOOLEAN have_select;
BOOLEAN one_row_remains;

have_select     = FALSE;
one_row_remains = FALSE;
nof_rows = GetNumberRows();

for ( row = 0; row<nof_rows; row++ )
    {
    if ( is_row_selected(row) )
        have_select = TRUE;
    else
        one_row_remains = TRUE;
    }

if ( !have_select )
    {
    resource_message_box( MainInstance, SELECT_ROW_STRING, NOTHING_SELECTED_STRING );
    return;
    }

if ( !one_row_remains )
    {
    resource_message_box( MainInstance, NO_DEL_ALL_ROWS_STRING, ATTEMPT_DEL_ALL_ROWS_STRING );
    return;
    }

row = nof_rows-1;
while ( row >= 0 )
    {
    if ( is_row_selected(row) )
        DeleteRow( row );
    row--;
    }

ClearSelections();
refresh_row_labels();
RedrawAll();
SetFocus( Grid.m_ctrlWnd );
}


/***********************************************************************
*                          EDITDIFF_GRID_CLASS                         *
*                             OnEditFinish                             *
***********************************************************************/
int EDITDIFF_GRID_CLASS::OnEditFinish( int col, long row, HWND edit, LPCTSTR string, BOOL cancelFlag )
{
int32 n;

if ( !EditEquationsWindow )
    return TRUE;

if ( col != 1 )
    return TRUE;

n = lstrlen( string );

if ( n > DCURVE_DESC_LEN )
    {
    MaxChars = DCURVE_DESC_LEN;
    DialogBox( MainInstance, TEXT("INPUT_ERROR_DIALOG"), EditEquationsWindow, (DLGPROC) input_error_dialog_proc );
    Grid.GotoCell( col, row );
    return FALSE;
    }

return TRUE;
}

/***********************************************************************
*                          EDITDIFF_GRID_CLASS                         *
*                                OnSetup                               *
***********************************************************************/
void EDITDIFF_GRID_CLASS::OnSetup()
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
SetMultiSelectMode(UG_MULTISELECT_ROW);
}

/***********************************************************************
*                          EDITDIFF_GRID_CLASS                         *
*                             OnLClicked                               *
***********************************************************************/
void EDITDIFF_GRID_CLASS::OnLClicked( int col, long row, int updn, RECT * rect, POINT * point, int processed )
{

if( updn )
    {
    GotoCell( col, row );
    StartEdit();
    }
}

/***********************************************************************
*                           EDITDIFF_GRID_CLASS                        *
*                              OnSH_LClicked                           *
***********************************************************************/
void EDITDIFF_GRID_CLASS::OnSH_LClicked( int col, long row, int updn, RECT *rect, POINT *point, BOOL processed )
{
BOOLEAN shift_key_down   = FALSE;
BOOLEAN control_key_down = FALSE;

if ( updn )
    {
    if ( !is_row_selected(row) )
        {
        if ( GetKeyState(VK_CONTROL) < 0 )
            control_key_down = TRUE;

        if ( GetKeyState(VK_SHIFT) < 0 )
            shift_key_down = TRUE;

        if ( !shift_key_down && !control_key_down )
            ClearSelections();

        if ( shift_key_down )
            m_GI->m_multiSelect->EndBlock( LAST_COL, row );
        else
            select_row( row );

        Update();
        }
    }
}

/***********************************************************************
*                         EDITDIFF_GRID_CLASS                          *
*                              OnCharDown                              *
***********************************************************************/
void EDITDIFF_GRID_CLASS::OnCharDown( UINT *vcKey, int processed )
{
if( !processed )
    StartEdit( *vcKey );
}

/***********************************************************************
*                         EDITDIFF_GRID_CLASS                          *
*                              OnKeyDown                               *
***********************************************************************/
void EDITDIFF_GRID_CLASS::OnKeyDown( UINT *vcKey, BOOL processed )
{

if ( *vcKey == VK_INSERT )
    {
    insert_row();
    RedrawAll();
    }

}

/***********************************************************************
*                              LOAD_GRID                               *
***********************************************************************/
static void load_grid( void )
{
CUGCell  cell;
DB_TABLE t;
int32    equation_number;
int32    n;
int32    remaining_width;
int32    width;
double   x;
TCHAR    s[DCURVE_DESC_LEN+1];
RECT     r;

t.open( diffcurve_dbname(), DCURVE_RECLEN, PFL );
n = t.nof_recs();
if ( n < 1 )
    n = 1;

Grid.GetCell( 0, 0, &cell );

Grid.SetNumberCols( NOF_COLS, FALSE );

cell.SetText( resource_string(GRID_NUMBER_STRING) );
Grid.SetCell( 0,-1, &cell );

cell.SetText( resource_string(GRID_NAME_STRING) );
Grid.SetCell( 1,-1, &cell );

cell.SetText( resource_string(GRID_HEAD_COEF_STRING) );
Grid.SetCell( 2,-1, &cell );

cell.SetText( resource_string(GRID_ROD_COEF_STRING) );
Grid.SetCell( 3,-1, &cell );

Grid.SetNumberRows( n, FALSE );

GetClientRect( Grid.m_CUGGrid->m_gridWnd, &r );
remaining_width = r.right - GetSystemMetrics(SM_CXVSCROLL) - 1;
width           = remaining_width / NOF_COLS;

for ( n=0; n<LAST_COL; n++ )
    {
    Grid.SetColWidth( n, width);
    remaining_width -= width;
    }

Grid.SetColWidth( LAST_COL, remaining_width);

n = 0;
while ( t.get_next_record(NO_LOCK) )
    {
    cell.SetText( int32toasc(n+1) );
    Grid.SetCell( -1, n, &cell );

    equation_number = t.get_short( DCURVE_CURVE_NUMBER_OFFSET );
    cell.SetText( int32toasc(equation_number) );
    Grid.SetCell( 0, n, &cell );

    t.get_alpha( s, DCURVE_DESC_OFFSET, DCURVE_DESC_LEN );
    cell.SetText( s );
    Grid.SetCell( 1, n, &cell );

    x = t.get_double( DCURVE_HEAD_PRES_OFFSET );
    cell.SetText( ascii_double(x) );
    Grid.SetCell( 2, n, &cell );

    x  = t.get_double( DCURVE_ROD_PRES_OFFSET );
    cell.SetText( ascii_double(x) );
    Grid.SetCell( 3, n, &cell );

    n++;
    }
t.close();
}

/***********************************************************************
*                             SAVE_CHANGES                             *
***********************************************************************/
static BOOLEAN save_changes( void )
{
CUGCell  cell;
DB_TABLE t;
long     n;
long     row;

n = Grid.GetNumberRows();
if ( n <= 0 )
    return FALSE;

if ( t.open(diffcurve_dbname(), DCURVE_RECLEN, FL) )
    {
    t.empty();
    for ( row=0; row<n; row++ )
        {
        if ( Grid.GetCell(0, row, &cell) == UG_SUCCESS )
            t.put_long( DCURVE_CURVE_NUMBER_OFFSET, asctoint32(cell.GetText()), DCURVE_NUMBER_LEN );

        if ( Grid.GetCell(1, row, &cell) == UG_SUCCESS )
            t.put_alpha( DCURVE_DESC_OFFSET, cell.GetText(), DCURVE_DESC_LEN );

        if ( Grid.GetCell(2, row, &cell) == UG_SUCCESS )
            t.put_double( DCURVE_HEAD_PRES_OFFSET, extdouble(cell.GetText()), DCURVE_COEF_LEN );

        if ( Grid.GetCell(3, row, &cell) == UG_SUCCESS )
            t.put_double( DCURVE_ROD_PRES_OFFSET, extdouble(cell.GetText()), DCURVE_COEF_LEN );

        t.rec_append();
        }

    t.close();
    resource_message_box( MainInstance, RESTART_STRING, CHANGES_SAVED_STRING );
    }

return TRUE;
}

/***********************************************************************
*                          EDIT_DCURVE_DIALOG_PROC                     *
***********************************************************************/
BOOL CALLBACK edit_dcurve_dialog_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
int id;

id  = LOWORD( wParam );

switch (msg)
    {
    case WM_INITDIALOG:
        EditEquationsWindow = hWnd;
        Grid.AttachGrid( hWnd, GRID_POSITION_BORDER );
        load_grid();
        break;

    case WM_HELP:
    case WM_CONTEXTMENU:
        gethelp( HELP_CONTEXT, EDIT_DIFF_EQUATIONS_HELP );
        break;

    case WM_COMMAND:
        switch ( id )
            {
            case EDITDIFF_CANCEL_BUTTON:
            case IDCANCEL:
                EditEquationsWindow = 0;
                EndDialog( hWnd, 0 );
                return TRUE;

            case INSERT_EQUATION_BUTTON:
                Grid.insert_row();
                Grid.RedrawAll();
                return TRUE;

            case DELETE_EQUATION_BUTTON:
                Grid.delete_highlighted_rows();
                Grid.RedrawAll();
                return TRUE;

            case EDITDIFF_SAVE_CHANGES_BUTTON:
                if ( save_changes() )
                    {
                    EditEquationsWindow = 0;
                    EndDialog( hWnd, 0 );

                    /*
                    ----------------------------------------
                    Tell the advanced setup window to reload
                    ---------------------------------------- */
                    PostMessage( MachSetupWindow, WM_NEWSETUP, 0, 0L );
                    }
                return TRUE;

            }
        break;

    case WM_CLOSE:
        if ( EditEquationsWindow )
            {
            EditEquationsWindow = 0;
            EndDialog( hWnd, 0 );
            }
        return TRUE;

    }

return FALSE;
}

