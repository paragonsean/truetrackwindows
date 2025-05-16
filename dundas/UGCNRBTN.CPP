#include <windows.h>
#include "ugctrl.h"

/***********************************************************************
*   Ultimate Grid 97                                                   *
*   Copyright 1994 - 1997 Dundas Software Ltd.                         *
*                                                                      *
*   CUGnrBtn                                                           *
*                                                                      *
*   This class is used to draw the cells corner button. The            *
*   corner button is the region at the top left of the grid            *
*   between the top and side headings. The width and height of         *
*   this object are determined by the width and height of the          *
*   side and top headings.                                             *
*                                                                      *
*   This class uses a cell located at col -1 by row -1 as its          *
*   source of information                                              *
*                                                                      *
*   This object is drawn using the celltype that the above cell        *
*   object points to                                                   *
*                                                                      *
*   This object gives the user the ability to size the width           *
*   and/or height of the side and top headings. This is done           *
*   when the mouse is moved to the edge and depressed, then            *
*   moved. Sizing nofications are make, which lets the                 *
*   programmer allow or dis-allow the sizing, or even set sizing       *
*   limits.                                                            *
*                                                                      *
***********************************************************************/
static TCHAR CornerButtonName[] = TEXT( "UGCnrBtn" );
static TCHAR EmptyString[] = TEXT( "" );

/***********************************************************************
*                            CUGCnrBtn                                 *
***********************************************************************/
CUGCnrBtn::CUGCnrBtn()
{
m_canSize      = FALSE;
m_isSizing     = FALSE;
m_sizingHeight = FALSE;
m_sizingWidth  = FALSE;
m_cnrBtnWnd    = 0;
m_ctrl         = 0;
m_GI           = 0;
}

/***********************************************************************
*                           ~CUGCnrBtn                                 *
***********************************************************************/
CUGCnrBtn::~CUGCnrBtn()
{
}

/***********************************************************************
*                              OnPaint                                 *
*                                                                      *
*   Paints the corner button object. The corner button object          *
*   uses a CUGCell located at position -1,-1 from the default          *
*   datasource then uses the cell type associated with this cell       *
*   to draw itself with.                                               *
*                                                                      *
***********************************************************************/
void CUGCnrBtn::OnPaint( void )
{
PAINTSTRUCT   ps;
HDC           dc;
RECT          rect;
CUGCellType * cellType;
CUGCell       cell;
HFONT         oldfont;

dc = BeginPaint( m_cnrBtnWnd, &ps );

m_ctrl->OnScreenDCSetup(dc,NULL,UG_TOPHEADING);

GetClientRect(m_cnrBtnWnd, &rect);
SetBkColor(dc, GetSysColor(COLOR_BTNFACE));

m_ctrl->GetCellIndirect( -1, -1, &cell );

oldfont = 0;
if ( m_ctrl->m_GI->m_defFont )
    oldfont = (HFONT) SelectObject( dc, m_ctrl->m_GI->m_defFont );

/*
----------------------------------
Get the cell type to draw the cell
---------------------------------- */
if ( cell.IsPropertySet(UGCELL_CELLTYPE_SET) )
    cellType = m_ctrl->GetCellType( cell.GetCellType() );
else
    cellType = m_ctrl->GetCellType( -1 );

cellType->OnDraw( dc, &rect, -1, -1, &cell, 0, 0 );

if ( oldfont )
    SelectObject( dc, oldfont );

EndPaint( m_cnrBtnWnd, &ps );
}

/***********************************************************************
*                              Update                                  *
*    This function is called by the grid when certian properties       *
*    are changed and when the grid is resized. This function           *
*    must repaint itself if a change was made that will affect         *
*    this object                                                       *
***********************************************************************/
void CUGCnrBtn::Update()
{
InvalidateRect( m_cnrBtnWnd, NULL, TRUE );
UpdateWindow( m_cnrBtnWnd );
}

/***********************************************************************
*                              Moved                                   *
*   This function is called whenever the current cell in the           *
*   grid is moved or the drag cell is moved. If the movement           *
*   affects this cell then it should be repainted                      *
***********************************************************************/
void CUGCnrBtn::Moved()
{
}

/***********************************************************************
*                           OnLButtonDblClk                            *
*  This function is called during a double click on this object.       *
*  This function will then notify the main grid class of the event.    *
*  Params                                                              *
*       nFlags  - mouse button flags                                   *
*       point   - point where mouse was clicked                        *
***********************************************************************/
void CUGCnrBtn::OnLButtonDblClk( UINT nFlags, POINT point )
{
BOOL processed;

processed = FALSE;
m_ctrl->OnCB_DClicked( &m_GI->m_cnrBtnRect, &point, processed );
}

/***********************************************************************
*                          OnLButtonDown                               *
*   This function is called when the left mouse button was just        *
*   pressed over this object. This function will then notify the       *
*   main grid class of the event. If the mouse is at the edge of       *
*   the cell where user resizing can take please then resizing         *
*   is enabled (if the user resize option is enabled)                  *
*   Params                                                             *
*       nFlags  - mouse button flags                                   *
*       point   - point where mouse was clicked                        *
***********************************************************************/
void CUGCnrBtn::OnLButtonDown( UINT nFlags, POINT point )
{
if ( m_canSize )
    {
    m_isSizing = TRUE;
    SetCapture( m_cnrBtnWnd );
    }
else
    {
    m_ctrl->OnCB_LClicked( 1, &m_GI->m_cnrBtnRect, &point );
    }
}

/***********************************************************************
*                              OnLButtonUp                             *
*   This function is called when the left mouse button was just        *
*   released over this object. This function will then notify          *
*   the main grid class of the event. If resizing was in               *
*   progress then it is terminated and a notification on the           *
*   resize is made to the main grid.                                   *
*   Params                                                             *
*       nFlags  - mouse button flags                                   *
*       point   - point where mouse was clicked                        *
***********************************************************************/
void CUGCnrBtn::OnLButtonUp(UINT nFlags, POINT point)
{
if ( m_isSizing )
    {
    m_isSizing = FALSE;
    ReleaseCapture();
    }

m_ctrl->OnCB_LClicked( 0, &m_GI->m_cnrBtnRect, &point );
}

/***********************************************************************
*                             OnMouseMove                              *
*   This function is called when the mouse is moved over this          *
*   object. If user resizing is allowed then this function check       *
*   to see if the mouse is located at a position where resizing        *
*   can take place, and sets a m_canSize flag. If user resizing        *
*   has been started then this function resizes the top or side        *
*   heading, according to the position of the mouse.                   *
*   Params                                                             *
*       nFlags  - mouse button flags                                   *
*       point   - point where mouse was clicked                        *
***********************************************************************/
void CUGCnrBtn::OnMouseMove( UINT nFlags, POINT point )
{
RECT rect;
int  height;
int  width;

/*
---------------------------------------------------
Check to see if the mouse is over a separation line
--------------------------------------------------- */
if( m_isSizing == FALSE && (nFlags&MK_LBUTTON)==0 && m_GI->m_userSizingMode > 0 )
    {
    m_canSize = FALSE;
    m_sizingWidth = FALSE;
    m_sizingHeight = FALSE;

    GetClientRect( m_cnrBtnWnd, &rect );
    if ( point.x > (rect.right-4) && m_ctrl->OnCanSizeSideHdg() )
        {
        m_canSize = TRUE;
        m_sizingWidth = TRUE;
        SetCursor(m_GI->m_WEResizseCursor);
        }
    else if ( point.y > (rect.bottom-4) && m_ctrl->OnCanSizeTopHdg() )
        {
        m_canSize = TRUE;
        m_sizingHeight = TRUE;
        SetCursor(m_GI->m_NSResizseCursor);
        }
    }
else if ( m_isSizing )
    {
    if ( m_sizingHeight )
        {
        if ( point.y < 0 )
            point.y = 0;

        height = point.y;
        m_ctrl->OnTopHdgSizing( &height );

        m_ctrl->SetTH_Height( height );
        m_ctrl->AdjustComponentSizes();
        }
    else
        {
        if ( point.x < 0 )
            point.x = 0;
        width = point.x;
        m_ctrl->OnSideHdgSizing( &width );
        m_ctrl->SetSH_Width( width );
        m_ctrl->AdjustComponentSizes();
        }
    }
}

/***********************************************************************
*                           OnRButtonDown                              *
*   This function is called during a right mouse down event.           *
*   This function will then notify the main grid class of the          *
*   event. If popup menus are enabled then this function calls         *
*   the grids StartMenu function                                       *
*   Params                                                             *
*       nFlags  - mouse button flags                                   *
*       point   - point where mouse was clicked                        *
***********************************************************************/
void CUGCnrBtn::OnRButtonDown( UINT nFlags, POINT point )
{
BOOL processed;

processed = FALSE;
m_ctrl->OnCB_RClicked( 1, &m_GI->m_cnrBtnRect, &point, processed );

if ( m_GI->m_enablePopupMenu )
    {
    ClientToScreen(m_cnrBtnWnd, &point);
    m_ctrl->StartMenu( -1, -1, &point, UG_CORNERBUTTON );
    }
}

/***********************************************************************
*                             OnRButtonUp                              *
*   This function is called during a right mouse up event. This        *
*   function will then notify the main grid class of the event.        *
*   Params                                                             *
*       nFlags  - mouse button flags                                   *
*       point   - point where mouse was clicked                        *
***********************************************************************/
void CUGCnrBtn::OnRButtonUp( UINT nFlags, POINT point )
{
BOOL processed;
processed = FALSE;
m_ctrl->OnCB_RClicked( 0, &m_GI->m_cnrBtnRect, &point, processed );
}

/***********************************************************************
*                              OnSetCursor                             *
*       This function sets the cursor to an arrow or a resize          *
*       cursor, depending on the resize state flags.                   *
***********************************************************************/
BOOL CUGCnrBtn::OnSetCursor( HWND hWnd, UINT nHitTest, UINT message )
{
if ( m_canSize )
    {
    if(m_sizingWidth && m_sizingHeight)
        SetCursor(LoadCursor(NULL,IDC_SIZENWSE));
    else if(m_sizingWidth)
        SetCursor(m_GI->m_WEResizseCursor);
    else if(m_sizingHeight)
        SetCursor(m_GI->m_NSResizseCursor);
    }
else
    SetCursor(m_GI->m_arrowCursor);

return TRUE;
}

/***********************************************************************
*                         CreateCnrBtnWindow                           *
***********************************************************************/
BOOL CUGCnrBtn::CreateCnrBtnWindow( DWORD dwStyle, RECT rect, HWND hWndParent, UINT nID )
{
HINSTANCE hInstance;
WNDCLASS  wc;

if ( !hWndParent )
    return UG_ERROR;

hInstance = (HINSTANCE) GetWindowLong( hWndParent, GWL_HINSTANCE );

/*
----------------------------------------------------
Register this new window class for the ultimate grid
---------------------------------------------------- */
wc.style          = CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS;
wc.lpfnWndProc    = CnrBtnWndProc;
wc.cbClsExtra     = 0;
wc.cbWndExtra     = 20;
wc.hInstance      = hInstance ;
wc.hIcon          = NULL;
wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
wc.hbrBackground  = NULL;
wc.lpszMenuName   = NULL;
wc.lpszClassName  = CornerButtonName;

if ( !GetClassInfo(hInstance, CornerButtonName, &wc) )
    {
    RegisterClass( &wc );
    if ( !GetClassInfo(hInstance, CornerButtonName, &wc) )
        return UG_ERROR;
    }

m_cnrBtnWnd = CreateWindow(
    CornerButtonName,
    EmptyString,
    dwStyle,
    rect.left, rect.top, rect.right, rect.bottom,
    hWndParent,
    (HMENU) nID,
    hInstance,
    NULL);

if ( !m_cnrBtnWnd )
    return UG_ERROR;

SendMessage( m_cnrBtnWnd, UG_THIS_PTR_CNRBTN, 0, (LPARAM) this );
UpdateWindow( m_cnrBtnWnd );

return UG_SUCCESS;
}

/***********************************************************************
*                         CnrBtnWndProc                                *
***********************************************************************/
long CALLBACK CUGCnrBtn::CnrBtnWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
CUGCnrBtn * bp;
POINT       p;

switch ( msg )
    {
    case WM_NCCREATE:
        SetWindowLong( hWnd, GWL_USERDATA, (LPARAM)NULL );
        return TRUE;

    case UG_THIS_PTR_CNRBTN:
        /*
        --------------------------------------
        Store the pointer to the calling class
        -------------------------------------- */
        SetWindowLong( hWnd, GWL_USERDATA, lParam );
        return 1;
    }

bp = (CUGCnrBtn *) GetWindowLong( hWnd, GWL_USERDATA );
if ( bp )
    {
    p.x = LOWORD(lParam);
    p.y = HIWORD(lParam);

    switch ( msg )
        {
        case WM_PAINT:
            bp->OnPaint();
            return 0;

        case WM_SETCURSOR:
            bp->OnSetCursor( hWnd, (UINT) p.x, (UINT) p.y );
            return 0;

        case WM_LBUTTONDBLCLK:
            bp->OnLButtonDblClk( (UINT) wParam, p );
            return 0;

        case WM_LBUTTONDOWN:
            bp->OnLButtonDown( (UINT) wParam, p );
            return 0;

        case WM_LBUTTONUP:
            bp->OnLButtonUp( (UINT) wParam, p );
            return 0;

        case WM_MOUSEMOVE:
            bp->OnMouseMove( (UINT) wParam, p );
            return 0;

        case WM_RBUTTONDOWN:
            bp->OnRButtonDown( (UINT) wParam, p );
            return 0;

        case WM_RBUTTONUP:
            bp->OnRButtonUp( (UINT) wParam, p );
            return 0;
        }
    }

return DefWindowProc( hWnd, msg, wParam, lParam );
}
