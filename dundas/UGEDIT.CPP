#include <windows.h>

/***********************************************************************
*                                                                      *
*    CUGEdit                                                           *
*                                                                      *
*    This is the standard edit control for the grid. The grid          *
*    can use any control derived from CWnd as a edit control.          *
*                                                                      *
*    This control automatically expands to the right as text is        *
*    entered, once the control reaches the right side of the           *
*    grid then it expands downward until it reaches the bottom.        *
*    Once it reaches the bottom then it will start scrolling           *
*    text as it is entered.                                            *
*                                                                      *
*    When editing first starts the control also automatically          *
*    expands to fit the inital text.                                   *
*                                                                      *
*    When editing finishes it is the responsability of the edit        *
*    control to call the grid's CUGCtrl::EditCtrlFinished method       *
*    to notify the grid that editing has finished.                     *
*                                                                      *
*    The edit control can also call the grid's OnEditVerify            *
*    during editing as well to give the programmer greater             *
*    control over what is being entered.                               *
*                                                                      *
*    New edit controls can be used by the grid by calling              *
*    CUGCtrl::SetNewEditClass. This function can be called just        *
*    before editing starts, thus allowing different cells to use       *
*    different edit controls.                                          *
*                                                                      *
***********************************************************************/
#include <ctype.h>
#include "UGCtrl.h"

/***********************************************************************
*                               CUGEdit                                *
***********************************************************************/
CUGEdit::CUGEdit()
{
m_autoSize = TRUE;
}

/***********************************************************************
*                              ~CUGEdit                                *
***********************************************************************/
CUGEdit::~CUGEdit()
{
}

/***********************************************************************
*                             OnKillFocus                              *
***********************************************************************/
void CUGEdit::OnKillFocus( HWND hNewWnd )
{
int     slen;
TCHAR * s;

if ( hNewWnd )
    {
    if ( hNewWnd != m_ctrl->m_ctrlWnd && GetParent(hNewWnd) != m_ctrl->m_ctrlWnd )
        m_cancel = TRUE;
    }
else
    {
    m_cancel = TRUE;
    }

slen = GetWindowTextLength( m_editWnd );
slen++;
s = new TCHAR[slen];

GetWindowText( m_editWnd, s, slen );
if ( m_ctrl->EditCtrlFinished(s, m_cancel, m_continueFlag, m_continueCol, m_continueRow) == FALSE )
    {
    m_ctrl->GotoCell( m_ctrl->m_editCol, m_ctrl->m_editRow );
    SetCapture( m_editWnd );
    ReleaseCapture();
    SetFocus( m_editWnd );
    }

if ( s )
    delete [] s;
}

/***********************************************************************
*                             OnSetFocus                               *
***********************************************************************/
void CUGEdit::OnSetFocus( HWND hOldWnd )
{
RECT rect;
/*
-----------------------
Get the original height
----------------------- */
GetClientRect( m_ctrl->m_ctrlWnd, &rect );
m_origHeight = rect.bottom +4;
m_currentHeight = m_origHeight;

CalcEditRect();

m_cancel = FALSE;
m_continueFlag = FALSE;

SendMessage( m_editWnd, EM_SETSEL, 0, -1 );
 }

/***********************************************************************
*                             OnKeyDown                                *
***********************************************************************/
void CUGEdit::OnKeyDown( UINT nChar, UINT nFlags )
{
int     col;
int     col2;
long    row;
long    row2;
CUGCell cell;

col = col2 = m_ctrl->m_editCol;
row = row2 = m_ctrl->m_editRow;
m_ctrl->GetCellIndirect(col, row, &cell);

/*
----------------------------------
Check to see if the cell is joined
---------------------------------- */
if ( cell.IsPropertySet(UGCELL_JOIN_SET) )
    {
    m_ctrl->GetJoinStartCell(&col, &row, &cell);
    m_ctrl->GetJoinRange(&col, &row, &col2, &row2);
    }

switch ( nChar )
    {
    case VK_TAB:
        if ( GetKeyState(VK_SHIFT) < 0 )
            {
            m_continueCol = col - 1;
            m_continueRow = row;
            }
        else
            {
            m_continueCol = col2 + 1;
            m_continueRow = row;
            }

        m_continueFlag = TRUE;
        SetFocus(m_ctrl->m_ctrlWnd);
        return;

    case VK_RETURN:
        m_continueCol = col;
        m_continueRow = row2 + 1;
        m_continueFlag = TRUE;
        SetFocus(m_ctrl->m_ctrlWnd);
        return;

    case VK_ESCAPE:
        m_cancel = TRUE;
        m_continueFlag = FALSE;
        SetFocus(m_ctrl->m_ctrlWnd);
        return;

    case VK_UP:
        if ( m_currentHeight != m_origHeight )
            break;

        row--;
        m_ctrl->GetJoinStartCell(&col, &row);

        m_continueCol = col;
        m_continueRow = row;
        m_continueFlag = TRUE;
        SetFocus(m_ctrl->m_ctrlWnd);
        return;

    case VK_DOWN:
        if(m_currentHeight != m_origHeight)
            break;

        m_continueCol = col;
        m_continueRow = row2 + 1;
        m_continueFlag = TRUE;
        SetFocus(m_ctrl->m_ctrlWnd);
        return;
    }
}

/***********************************************************************
*                              OnChar                                  *
***********************************************************************/
BOOL CUGEdit::OnChar( UINT nChar, UINT nFlags )
{
int  col;
long row;

/*
----------------------------------------
Allow OnEditVerify a look at the char...
---------------------------------------- */
col = m_ctrl->GetCurrentCol();
row = m_ctrl->GetCurrentRow();

if ( m_ctrl->OnEditVerify(col, row, m_editWnd, &nChar)==FALSE )
    return FALSE;

if ( nChar == VK_TAB || nChar == VK_RETURN || nChar == VK_ESCAPE )
    return FALSE;

return TRUE;
}


/***********************************************************************
*                           OnMouseActivate                            *
***********************************************************************/
int CUGEdit::OnMouseActivate(HWND hDesktopWnd, UINT nHitTest, UINT message)
{
return MA_ACTIVATE;
}

/***********************************************************************
*                            OnSetCursor                               *
***********************************************************************/
BOOL CUGEdit::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
{
SetCursor(LoadCursor(NULL,IDC_IBEAM));
return 1;
}


/***********************************************************************
*                            OnUpdate                                  *
***********************************************************************/
void CUGEdit::OnUpdate()
{
CalcEditRect();
}

/***********************************************************************
*                            CalcEditRect                              *
***********************************************************************/
void CUGEdit::CalcEditRect( void )
{
RECT   editRect;
RECT   parentRect;
RECT   fmtRect;
HDC    dc;
HFONT  oldFont;
int    len;
LPTSTR string;
POINT  point;
HFONT  font;
int    col;
long   row;
RECT   rect;

if ( !m_autoSize )
    return;

string = 0;
len    = GetWindowTextLength( m_editWnd );
if ( len > 0 )
    string = new TCHAR[len + 1];

if ( len > 0 )
    GetWindowText( m_editWnd, string, len+1 );

AddTwoString( string, TEXT(" X X") );

GetWindowRect( m_editWnd, &editRect );
point.x = editRect.left;
point.y = editRect.top;

ScreenToClient( m_ctrl->m_editParent, &point );
editRect.left = point.x;
editRect.top  = point.y;

point.x = editRect.right;
point.y = editRect.bottom;
ScreenToClient( m_ctrl->m_editParent, &point );
editRect.right  = point.x;
editRect.bottom = point.y;

GetClientRect(m_ctrl->m_editParent, &parentRect);

dc = GetDC( m_editWnd );

oldFont = NULL;
font    = (HFONT) SendMessage( m_editWnd, WM_GETFONT, 0, 0 );
if ( font != NULL )
    oldFont = (HFONT) SelectObject( dc, font );

CopyRect(&fmtRect,&editRect);
fmtRect.right = parentRect.right;
fmtRect.left +=2;

/*
--------------------------------------------------------------------------
The following modifies fmtRect.right and fmtRect.bottom to fit the string.
Nothing is drawn.
-------------------------------------------------------------------------- */
if ( len == 0 )
    len = 1;
::DrawText( dc, string, len, &fmtRect, DT_CALCRECT|DT_WORDBREAK );

fmtRect.left -= 2;

if ( oldFont != NULL )
    SelectObject(dc, oldFont);

ReleaseDC( m_editWnd, dc );

if ( fmtRect.bottom >= parentRect.bottom )
    fmtRect.bottom = parentRect.bottom - 1;

if ( fmtRect.right > editRect.right )
    {
    m_ctrl->GetCellFromPoint( fmtRect.right+1, fmtRect.top+1, &col, &row, &rect );
    if ( rect.right > parentRect.right )
        rect.right = parentRect.right;

    fmtRect.right = rect.right -1;
    }

if ( fmtRect.bottom > editRect.bottom )
    {
    fmtRect.right = parentRect.right;
    m_ctrl->GetCellFromPoint(fmtRect.left +1,fmtRect.bottom+1,&col,&row,&rect);
    fmtRect.bottom = rect.bottom -1;
    }

if ( fmtRect.bottom >= parentRect.bottom )
    fmtRect.bottom = parentRect.bottom - 1;

if ( fmtRect.bottom < editRect.bottom )
    fmtRect.bottom = editRect.bottom;

if ( fmtRect.right < editRect.right )
    fmtRect.right = editRect.right;


if ( fmtRect.right != editRect.right || fmtRect.bottom != editRect.bottom )
    {
    MoveWindow( m_editWnd, fmtRect.left, fmtRect.top, fmtRect.right - fmtRect.left, fmtRect.bottom - fmtRect.top, TRUE);
    ValidateRect( GetParent(m_editWnd), &fmtRect );
    m_currentHeight = fmtRect.bottom - fmtRect.top;
    }

delete [] string;
}

/***********************************************************************
*                           SetAutoSize                                *
***********************************************************************/
BOOL CUGEdit::SetAutoSize( BOOL state )
{
BOOL oldsize;

oldsize = m_autoSize;

if ( state )
    m_autoSize = TRUE;
else
    m_autoSize = FALSE;

return oldsize;
}

/***********************************************************************
*                             OnGetDlgCode                             *
***********************************************************************/
UINT CUGEdit::OnGetDlgCode( void )
{
if ( GetKeyState(VK_TAB) < 0 )
    return DLGC_WANTTAB;
else if ( GetKeyState(VK_RETURN) < 0 )
    return  DLGC_WANTALLKEYS;
else if ( GetKeyState(VK_ESCAPE) < 0 )
    return  DLGC_WANTALLKEYS;
return UG_SUCCESS;
}

FARPROC lpfnOldEdit;

/***********************************************************************
*                          CreateEditWindow                            *
***********************************************************************/
BOOL CUGEdit::CreateEditWindow( DWORD dwStyle, RECT rect, HWND parent, UINT nID )
{
FARPROC   lpfnNewEdit;
HINSTANCE hInstance;

if(parent == NULL)
    return UG_ERROR;

hInstance = (HINSTANCE) GetWindowLong( parent, GWL_HINSTANCE );

/*
-----------------
Create the window
----------------- */
m_editWnd = CreateWindow(
    _T("EDIT"),
    _T(""),
    dwStyle,
    rect.left, rect.top,
    rect.right, rect.bottom,
    parent,
    (HMENU)nID,
    hInstance,
    NULL
    );

if (m_editWnd == NULL)
    return UG_ERROR;

lpfnOldEdit = (FARPROC)GetWindowLong(m_editWnd, GWL_WNDPROC);
lpfnNewEdit = MakeProcInstance((FARPROC)EditWndProc, hInstance);
SetWindowLong( m_editWnd, GWL_WNDPROC, (LONG)lpfnNewEdit );

SendMessage( m_editWnd, UG_THIS_PTR_EDIT, 0, (LPARAM)this );

return UG_SUCCESS;
}

/***********************************************************************
*                           EditWndProc                                *
***********************************************************************/
long CALLBACK CUGEdit::EditWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
CUGEdit * ep;

switch ( msg )
    {
    case WM_NCCREATE:
        SetWindowLong( hWnd, GWL_USERDATA, (LPARAM) NULL );
        return 0;

    case UG_THIS_PTR_EDIT:
        /*
        --------------------------------------
        Store the pointer to the calling class
        -------------------------------------- */
        SetWindowLong( hWnd, GWL_USERDATA, lParam );
        return 0;

    }

ep = (CUGEdit *) GetWindowLong( hWnd, GWL_USERDATA );
if ( ep == NULL )
    return CallWindowProc( lpfnOldEdit, hWnd, msg, wParam, lParam );

switch ( msg )
    {
    case WM_SETFOCUS:
        ep->OnSetFocus( (HWND) wParam );
        break;

    case WM_KILLFOCUS:
        ep->OnKillFocus( (HWND) wParam );
        return 0;

    case WM_SETCURSOR:
        ep->OnSetCursor( hWnd, (UINT)(LOWORD(lParam)), UINT(HIWORD(lParam)));
        return 0;

    case WM_GETDLGCODE:
        int retn;
        retn = ep->OnGetDlgCode();
        if ( retn == UG_SUCCESS )
            break;
        else
            return retn;

    case WM_MOUSEACTIVATE:
        return (ep->OnMouseActivate((HWND) wParam, (INT) LOWORD(lParam), (UINT) HIWORD(lParam)));

    case WM_CHAR:
        if ( (UINT)wParam == VK_TAB || (UINT)wParam == VK_RETURN || (UINT)wParam == VK_ESCAPE )
            return 0;
        else
            if (ep->OnChar((UINT)wParam, lParam) == FALSE)
                return 0;
        break;

    case WM_KEYDOWN:
        ep->OnKeyDown((UINT)wParam, lParam);
        if ( (UINT)wParam == VK_ESCAPE )
            return 0;
        break;

    default:
        break;
    }

return CallWindowProc( lpfnOldEdit, hWnd, msg, wParam, lParam );
}
