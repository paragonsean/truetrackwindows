#include <windows.h>

/***********************************************************************
*   This class is used by the CUGDropList class as the listbox         *
*   that appears when the droplist is activated.                       *
*                                                                      *
*   This class is responsible for returning any selection make         *
*   in the listbox, and destroying itself when an item is              *
*   selected or when the selection process is canceled.                *
***********************************************************************/

#include "UGCtrl.h"

extern FARPROC lpfnOldListBox;

/***********************************************************************
*                            CUGLstBox                                 *
*                            CUGLstBox                                 *
***********************************************************************/
CUGLstBox::CUGLstBox()
{
m_listBoxWnd = NULL;
m_HasFocus   = FALSE;
m_col        = NULL;
m_row        = NULL;
m_selected   = FALSE;
m_ctrl       = NULL;
}

/***********************************************************************
*                            CUGLstBox                                 *
*                           ~CUGLstBox                                 *
***********************************************************************/
CUGLstBox::~CUGLstBox()
{
}

/***********************************************************************
*                            CUGLstBox                                 *
*                            OnSetFocus                                *
***********************************************************************/
void CUGLstBox::OnSetFocus( HWND hOldWnd )
{
ReleaseCapture();
SetCapture( m_listBoxWnd );

m_HasFocus = TRUE;
m_selected = FALSE;
}

/***********************************************************************
*                            CUGLstBox                                 *
*                           OnKillFocus                                *
***********************************************************************/
void CUGLstBox::OnKillFocus( HWND hNewWnd )
{
ReleaseCapture();

DestroyWindow( m_listBoxWnd );

m_HasFocus = FALSE;
if ( !m_selected )
    {
    *m_col = -2;
    *m_row = -2;
    }

m_selected = FALSE;

SendMessage( m_ctrl->m_CUGGrid->m_gridWnd, WM_PAINT, 0, 0L );
}

/***********************************************************************
*                              CUGLstBox                               *
*                           OnMouseActivate                            *
***********************************************************************/
int CUGLstBox::OnMouseActivate( HWND hDesktopWnd, UINT nHitTest, UINT message )
{
return MA_ACTIVATE;
}

/***********************************************************************
*                              CUGLstBox                               *
*                             OnLButtonUp                              *
***********************************************************************/
void CUGLstBox::OnLButtonUp( HWND hWnd, UINT nFlags, POINT point )
{
ReleaseCapture();

/*
-------------------------------------------------------------------------
Update the string in the current cell if the list box had a selected item
------------------------------------------------------------------------- */
if ( SendMessage(hWnd, LB_GETCURSEL, 0, 0) != LB_ERR )
    Select( hWnd );
}

/***********************************************************************
*                              CUGLstBox                               *
*                              OnKeyDown                               *
***********************************************************************/
void CUGLstBox::OnKeyDown( HWND hWnd, UINT nChar, UINT nFlags )
{

if ( nChar == VK_ESCAPE )
    DestroyWindow( hWnd );

if ( nChar == VK_RETURN )
    {
    if ( SendMessage(hWnd, LB_GETCURSEL, 0, 0) != LB_ERR )
        {
        Select(hWnd);
        }
    else
        {
        m_HasFocus = FALSE;
        DestroyWindow(hWnd);
        }
    }
}

/***********************************************************************
*                              CUGLstBox                               *
*                               Select                                 *
***********************************************************************/
void CUGLstBox::Select( HWND hWnd )
{

CUGCell cell;
LPTSTR string;
int col  = *m_col;
long row = *m_row;

m_selected = TRUE;

/*
-----------------------
Get the string selected
----------------------- */
int index = SendMessage(hWnd, LB_GETCURSEL, 0, 0);
int len = SendMessage(hWnd, LB_GETTEXTLEN, index, 0);

string = new TCHAR[len + 1];

SendMessage(hWnd, LB_GETTEXT, index, (LPARAM)string);

/*
--------------------------------
Notify the user of the selection
-------------------------------- */
if ( m_ctrl->OnCellTypeNotify(m_cellTypeId,col,row, UGCT_DROPLISTSELECT,(long)&string) != FALSE )
    {
    m_ctrl->GetCell( col, row, &cell );
    cell.SetText( string );
    m_ctrl->SetCell( col, row, &cell );
    m_ctrl->RedrawCell( col, row );
    }

m_HasFocus = FALSE;
if ( m_col != NULL )
    *m_col = -1;

if ( m_row != NULL )
    *m_row = -1;

SetFocus(GetParent(hWnd));

if ( string != NULL )
    delete[] string;
}

/***********************************************************************
*                              CUGLstBox                               *
*                            ListBoxWndProc                            *
***********************************************************************/
long CALLBACK CUGLstBox::ListBoxWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

switch ( msg )
    {
    case WM_NCCREATE:
        SetWindowLong( hWnd, GWL_USERDATA, (LPARAM) NULL );
        return 0;

    case UG_THIS_PTR_LISTBOX:
        /*
        --------------------------------------
        Store the pointer to the calling class
        -------------------------------------- */
        SetWindowLong( hWnd, GWL_USERDATA, lParam );
        return 0;
    }

CUGLstBox * lb = (CUGLstBox *) GetWindowLong( hWnd, GWL_USERDATA );
if ( lb == NULL )
    return CallWindowProc( lpfnOldListBox, hWnd, msg, wParam, lParam );

POINT point = {(short)LOWORD(lParam), (short)HIWORD(lParam)};

switch (msg)
    {
    case WM_GETDLGCODE:
        return DLGC_WANTMESSAGE;

    case WM_KILLFOCUS:
        lb->OnKillFocus( (HWND) wParam );
        return 0;

    case WM_SETFOCUS:
        lb->OnSetFocus( (HWND) wParam );
        break;

    case WM_MOUSEACTIVATE:
        lb->OnMouseActivate( (HWND) wParam, point.x, point.y );
        return  MA_ACTIVATE;

    case WM_LBUTTONUP:
        lb->OnLButtonUp( hWnd, (UINT) wParam, point );
        return 0;

    case WM_CHAR:
        return 0;

    case WM_KEYDOWN:
        if ( wParam == VK_ESCAPE || wParam == VK_RETURN )
            {
            lb->OnKeyDown( hWnd, (UINT) wParam, lParam );
            return 0;
            }
        break;

    default:
        break;
    }

return CallWindowProc( lpfnOldListBox, hWnd, msg, wParam, lParam );
}
