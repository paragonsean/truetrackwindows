#include <windows.h>

/***********************************************
Ultimate Grid 97
Copyright 1994 - 1997 Dundas Software Ltd.

Class
    CUGHScroll

This class controls the grid's horizontal scrollbar and
is responsible for updating it and making sure that it
is using the specified scroll mode
**********************************************/

#include "UGCtrl.h"

FARPROC lpfnOldhScroll;

/***********************************************************************
*                            CUGHScroll                                *
***********************************************************************/
CUGHScroll::CUGHScroll()
{
m_hScrollWnd = NULL;
m_lastMaxLeftCol = -1;
m_lastNumLockCols = -1;
m_trackColPos = 0;
}

/***********************************************************************
*                           ~CUGHScroll                                *
***********************************************************************/
CUGHScroll::~CUGHScroll()
{
}

/***********************************************************************
*                              Update                                  *
***********************************************************************/
void CUGHScroll::Update()
{
Moved();
}

/***********************************************************************
*                               Moved                                  *
***********************************************************************/
void CUGHScroll::Moved()
{

/*
------------------------------------------------
Update the range if the max left col has changed
or if the number of locked columns has changed
------------------------------------------------ */
if ( m_lastMaxLeftCol != m_GI->m_maxLeftCol || m_lastNumLockCols != m_GI->m_numLockCols)
    {
    m_lastMaxLeftCol  = m_GI->m_maxLeftCol;
    m_lastNumLockCols = m_GI->m_numLockCols;

    /*
    --------------------
    Set the scroll range
    -------------------- */
    SCROLLINFO ScrollInfo;
    ScrollInfo.cbSize = sizeof(SCROLLINFO);
    ScrollInfo.fMask  = SIF_PAGE | SIF_RANGE;
    ScrollInfo.nPage  = (m_GI->m_gridWidth  - m_GI->m_lockColWidth) / m_GI->m_defColWidth;
    ScrollInfo.nMin   = 0;
    ScrollInfo.nMax   = (m_GI->m_maxLeftCol - m_GI->m_numLockCols) + ScrollInfo.nPage -1;
    SetScrollInfo(m_hScrollWnd, SB_CTL, &ScrollInfo, FALSE);
    }

/*
------------------
Set the scroll pos
------------------ */
if ( m_GI->m_lastLeftCol != m_GI->m_leftCol )
    SetScrollPos(m_hScrollWnd, SB_CTL, m_GI->m_leftCol - m_GI->m_numLockCols,TRUE);

InvalidateRect(m_hScrollWnd, NULL, TRUE);
UpdateWindow(m_hScrollWnd);
}

/***********************************************************************
*                               HScroll                                *
***********************************************************************/
void CUGHScroll::HScroll( UINT nSBCode, UINT nPos )
{

if ( GetFocus() != m_ctrl->m_CUGGrid->m_gridWnd )
    SetFocus(m_ctrl->m_CUGGrid->m_gridWnd);

m_ctrl->m_GI->m_moveType = 4;

switch ( nSBCode )
    {
    case SB_LINEDOWN:
        m_ctrl->MoveLeftCol(UG_LINEDOWN);
        break;

    case SB_LINEUP:
        m_ctrl->MoveLeftCol(UG_LINEUP);
        break;

    case SB_PAGEUP:
        m_ctrl->MoveLeftCol(UG_PAGEUP);
        break;

    case SB_PAGEDOWN:
        m_ctrl->MoveLeftCol(UG_PAGEDOWN);
        break;

    case SB_TOP:
        m_ctrl->MoveLeftCol(UG_TOP);
        break;

    case SB_BOTTOM:
        m_ctrl->MoveLeftCol(UG_BOTTOM);
        break;

    case SB_THUMBTRACK:
        if ( m_GI->m_hScrollMode == 1)  //tracking
            m_ctrl->SetLeftCol( nPos + m_GI->m_numLockCols );

        m_trackColPos = nPos + m_GI->m_numLockCols;
        #ifdef UG_ENABLE_SCROLLHINTS
            if ( m_GI->m_enableHScrollHints )
                {

                TCHAR string[80];
                RECT r;
                GetWindowRect( m_hScrollWnd, &r );
                r.left = LOWORD( GetMessagePos() );

                m_ctrl->m_CUGHint->SetWindowAlign( UG_ALIGNRIGHT | UG_ALIGNVCENTER );
                m_ctrl->m_CUGHint->SetTextAlign( UG_ALIGNCENTER );

                m_ctrl->OnHScrollHint( m_trackColPos, string );
                m_ctrl->m_CUGHint->SetText( string, FALSE );

                /*
                ------------------------------
                Set text before move window...
                ------------------------------ */
                m_ctrl->m_CUGHint->MoveHintWindow( r.left-1, r.top, 20 );
                m_ctrl->m_CUGHint->Show();
                }
            #endif

        break;


    case SB_ENDSCROLL:
        break;

    case SB_THUMBPOSITION:
        #ifdef UG_ENABLE_SCROLLHINTS
            if ( m_GI->m_enableHScrollHints )
                m_ctrl->m_CUGHint->Hide();
            #endif

        m_ctrl->SetLeftCol(nPos+m_GI->m_numLockCols);
        break;

    }
}

/***********************************************************************
*                              OnRButtonDown                           *
***********************************************************************/
void CUGHScroll::OnRButtonDown( UINT nFlags, POINT point )
{
if ( m_GI->m_enablePopupMenu )
    {
    ClientToScreen(m_hScrollWnd, &point);
    m_ctrl->StartMenu(0,0,&point,UG_HSCROLL);
    }
}

/***********************************************************************
*                            CreateHScrollWindow                       *
***********************************************************************/
BOOL CUGHScroll::CreateHScrollWindow( DWORD dwStyle, RECT rect, HWND parent, UINT nID )
{

if ( parent == NULL )
    return UG_ERROR;

HINSTANCE hInstance = (HINSTANCE) GetWindowLong( parent, GWL_HINSTANCE );

m_hScrollWnd = CreateWindow(
    TEXT("SCROLLBAR"),
    TEXT(""),
    dwStyle,
    rect.left, rect.top, rect.right, rect.bottom,
    parent,
    (HMENU)nID,
    hInstance,
    NULL
    );

if ( m_hScrollWnd == NULL )
    {
    #ifdef UG_ENABLE_SCROLLHINTS
        m_GI->m_enableHScrollHints = 0;
        #endif
    return UG_ERROR;
    }

lpfnOldhScroll = (FARPROC) GetWindowLong( m_hScrollWnd, GWL_WNDPROC );
SetWindowLong( m_hScrollWnd, GWL_WNDPROC, (LONG) HScrollWndProc );

SendMessage( m_hScrollWnd, UG_THIS_PTR_HSCROLL, 0, (LPARAM) this );

return UG_SUCCESS;
}

/***********************************************************************
*                            HScrollWndProc                            *
***********************************************************************/
long CALLBACK CUGHScroll::HScrollWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{

switch ( message )
    {
    case WM_NCCREATE:
        SetWindowLong( hWnd, GWL_USERDATA, (LPARAM) NULL );
        return 0;

    case UG_THIS_PTR_HSCROLL:
        /*
        --------------------------------------
        Store the pointer to the calling class
        -------------------------------------- */
        SetWindowLong( hWnd, GWL_USERDATA, lParam );
        return 0;
    }

CUGHScroll * _this = (CUGHScroll *)GetWindowLong(hWnd, GWL_USERDATA);

if (_this == NULL)
    return CallWindowProc( lpfnOldhScroll, hWnd, message, wParam, lParam );

POINT point = {(short)LOWORD(lParam), (short)HIWORD(lParam)};

switch ( message )
    {
    case WM_RBUTTONDOWN:
        _this->OnRButtonDown( (UINT)wParam, point );
        return 0;

    default:
        break;
    }

return CallWindowProc( lpfnOldhScroll, hWnd, message, wParam, lParam );
}
