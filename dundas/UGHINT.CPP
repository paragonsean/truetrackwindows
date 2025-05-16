#include <windows.h>

#include "UGCtrl.h"

#ifdef UG_ENABLE_SCROLLHINTS

/***********************************************************************
*                               CUGHint                                *
***********************************************************************/
CUGHint::CUGHint()
{
m_text        = NULL;
m_defaultFont = CreateFont(14,0,0,0,500,0,0,0,0,0,0,0,0,TEXT("Arial"));
m_hFont       = m_defaultFont;

}

/***********************************************************************
*                              ~CUGHint                                *
***********************************************************************/
CUGHint::~CUGHint()
{

if ( m_defaultFont != NULL )
    {
    DeleteObject( m_defaultFont );
    m_defaultFont = NULL;
    }

m_hFont = NULL;

if ( m_text != NULL )
    {
    delete [] m_text;
    m_text = NULL;
    }
}

/***********************************************************************
*                               OnPaint                                *
***********************************************************************/
void CUGHint::OnPaint()
{
HDC   dc;
HFONT oldfont = NULL;
RECT  r;

dc = GetDC( m_hintWnd );

if ( m_hFont == NULL )
    m_hFont = m_defaultFont;

if ( m_hFont != NULL )
    oldfont = (HFONT) SelectObject( dc, m_hFont );

GetClientRect( m_hintWnd, &r );

::SetTextColor( dc, m_textColor );
SetBkColor( dc, m_backColor );

ExtTextOut( dc, 2, 1, ETO_OPAQUE|ETO_CLIPPED, &r, m_text, lstrlen(m_text), NULL );

if ( oldfont != NULL )
    SelectObject( dc, oldfont );

MoveToEx( dc, 0, 0, NULL );

LineTo( dc, r.right-1, 0 );
LineTo( dc, r.right-1, r.bottom-1 );
LineTo( dc, 0,         r.bottom-1 );
LineTo( dc, 0,         0 );

ReleaseDC( m_hintWnd, dc );
ValidateRect( m_hintWnd, NULL );
}

/***********************************************************************
*                               OnCreate                               *
***********************************************************************/
void CUGHint::OnCreate()
{
HDC dc;
SIZE sz;

/*
------------------
Init the variables
------------------ */
CopyString( m_text, TEXT("") );  //display text
m_textColor = RGB(0,0,0);        //text color
m_backColor = RGB(255,255,224);  //background color
m_windowAlign = UG_ALIGNLEFT;    //UG_ALIGNLEFT,UG_ALIGNRIGHT,UG_ALIGNCENTER
                                 //UG_ALIGNTOP,UG_ALIGNBOTTOM,UG_ALIGNVCENTER
m_textAlign   = UG_ALIGNLEFT;    //UG_ALIGNLEFT,UG_ALIGNRIGHT,UG_ALIGNCENTER

/*
-------------------
Get the font height
------------------- */
dc = GetDC( m_hintWnd );
GetTextExtentPoint32( dc, TEXT("X"), 1, &sz );
m_fontHeight = sz.cy;

ReleaseDC( m_hintWnd, dc );
}

/***********************************************************************
*                               SetFont                                *
***********************************************************************/
int CUGHint::SetFont( HFONT font )
{
HDC dc;
SIZE s;

if ( font != NULL )
    m_hFont = font;
else
    m_hFont = m_defaultFont;

/*
-------------------
Get the font height
------------------- */
dc = GetDC( m_hintWnd );
if ( m_hFont != NULL )
    SelectObject( dc, m_hFont );

GetTextExtentPoint32(dc, TEXT("Xy"), 2, &s);
m_fontHeight = s.cy + 3;
ReleaseDC( m_hintWnd, dc );

return UG_SUCCESS;
}

/***********************************************************************
*                             SetWindowAlign                           *
*  align = UG_ALIGNLEFT or UG_ALIGNRIGHT or UG_ALIGNCENTER             *
*          + UG_ALIGNTOP or UG_ALIGNBOTTOM or UG_ALIGNVCENTER          *
***********************************************************************/
int CUGHint::SetWindowAlign(int align )
{
m_windowAlign = align;
return UG_SUCCESS;
}

/***********************************************************************
*                         SetTextAlign                                 *
*     align = UG_ALIGNLEFT or UG_ALIGNRIGHT or UG_ALIGNCENTER          *
***********************************************************************/
int CUGHint::SetTextAlign(int align)
{
m_textAlign = align;
return UG_SUCCESS;
}

/***********************************************************************
*                           SetTextColor                               *
***********************************************************************/
int CUGHint::SetTextColor(COLORREF color)
{
m_textColor = color;
return UG_SUCCESS;
}

/***********************************************************************
*                          SetBackColor                                *
***********************************************************************/
int CUGHint::SetBackColor(COLORREF color)
{
m_backColor = color;
return UG_SUCCESS;
}

/***********************************************************************
*                            SetText                                   *
***********************************************************************/
int CUGHint::SetText(LPCTSTR string,int update)
{
CopyString(m_text, string);
if ( update )
    InvalidateRect(m_hintWnd, NULL, TRUE);

return UG_SUCCESS;
}

/***********************************************************************
*                         MoveHintWindow                               *
***********************************************************************/
int CUGHint::MoveHintWindow( int x, int y, int width )
{
HDC  dc;
RECT r;
SIZE s;

/*
-------------------------------------------------------------------
Get the width of the string and reset the specified width if needed
------------------------------------------------------------------- */
dc = GetDC( m_hintWnd );

/*
-----------------------
TD first select font...
----------------------- */
if ( m_hFont != NULL )
    {
    SelectObject(dc,  m_hFont);
    GetTextExtentPoint32( dc, m_text, lstrlen(m_text), &s );
    if ( (s.cx+4) > width )
        width = s.cx+4;
    ReleaseDC( m_hintWnd, dc );
    }

/*
-------------------------
Set up the horizontal pos
------------------------- */
if ( m_windowAlign & UG_ALIGNCENTER )
    {
    r.left = x-(width/2);
    r.right = x+width;
    }
else if ( m_windowAlign & UG_ALIGNRIGHT )
    {
    r.left = x-width;
    r.right = x;
    }
else  //left
    {
    r.left = x;
    r.right = x+width;
    }

/*
-----------------------
Set up the vertical pos
----------------------- */
if ( m_windowAlign & UG_ALIGNVCENTER )
    {
    r.top    = y-(m_fontHeight/2);
    r.bottom = r.top+m_fontHeight;
    }
else if ( m_windowAlign & UG_ALIGNBOTTOM )
    {
    r.top    = y-m_fontHeight;
    r.bottom = y;
    }
else    //top
    {
    r.top = y;
    r.bottom = y+m_fontHeight;
    }

/*
-------------------------------------------
Make sure the position is within the parent
------------------------------------------- */
RECT parentRect;
int dif;

GetWindowRect( GetParent(m_hintWnd), &parentRect );

if ( r.left < 0 )
    {
    dif = 0 - r.left;
    r.left  += dif;
    r.right += dif;
    }
if ( r.top < 0 )
    {
    dif = 0 - r.top;
    r.top    += dif;
    r.bottom += dif;
    }
if ( r.right > parentRect.right )
    {
    dif = r.right - parentRect.right;
    r.right -= dif;
    r.left  -= dif;
    }

if ( r.bottom > parentRect.bottom )
    {
    dif = r.bottom - parentRect.bottom;
    r.top    -= dif;
    r.bottom -= dif;
    }

MoveWindow( m_hintWnd, r.left, r.top, r.right - r.left, r.bottom - r.top, TRUE );
SendMessage( m_hintWnd, WM_PAINT, 0, 0 );

return UG_SUCCESS;
}

/***********************************************************************
*                              Hide                                    *
***********************************************************************/
int CUGHint::Hide()
{
ShowWindow(m_hintWnd, SW_HIDE);

// TD solves (workaround) grid update problem.
InvalidateRect( m_ctrl->m_CUGGrid->m_gridWnd, NULL, TRUE );
return UG_SUCCESS;
}

/***********************************************************************
*                              Show                                    *
***********************************************************************/
int CUGHint::Show()
{

if ( IsWindowVisible(m_hintWnd) == FALSE )
    ShowWindow( m_hintWnd, SW_SHOW );

return UG_SUCCESS;
}

/***********************************************************************
*                        CreateHintWindow                              *
***********************************************************************/
BOOL CUGHint::CreateHintWindow(DWORD dwStyle, RECT rect, HWND hWndParent, UINT nID)
{

if ( hWndParent == NULL )
    return UG_ERROR;

HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWndParent,GWL_HINSTANCE);

WNDCLASS wndclass;

/*
----------------------------------------------------
Register this new window class for the ultimate grid
---------------------------------------------------- */
wndclass.style          = CS_HREDRAW | CS_VREDRAW;
wndclass.lpfnWndProc    = HintWndProc;
wndclass.cbClsExtra     = 0;
wndclass.cbWndExtra     = 20;
wndclass.hInstance      = hInstance ;
wndclass.hIcon          = NULL;
wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
wndclass.hbrBackground  = NULL;
wndclass.lpszMenuName   = NULL;
wndclass.lpszClassName  = _T("UGHint");

if ( !GetClassInfo(hInstance,_T("UGHint"), &wndclass) )
    {
    RegisterClass( &wndclass );
    if ( !GetClassInfo(hInstance,_T("UGHint"),&wndclass) )
        return UG_ERROR;
    }

dwStyle = WS_POPUP;
m_hintWnd = CreateWindowEx(
    WS_EX_TOPMOST,
    TEXT("UGHint"),
    TEXT(""),
    dwStyle,
    rect.left, rect.top, rect.right, rect.bottom,
    hWndParent,
    NULL,
    hInstance,
    NULL);

if ( m_hintWnd == NULL )
    return UG_ERROR;

OnCreate();

SendMessage( m_hintWnd, UG_THIS_PTR_HINT, 0, (LPARAM)this );

return UG_SUCCESS;
}

/***********************************************************************
*                            HintWndProc                               *
***********************************************************************/
long CALLBACK CUGHint::HintWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{

switch(message)
    {
    case WM_NCCREATE:
        SetWindowLong(hWnd, GWL_USERDATA, (LPARAM)NULL);
        return 1;

    case UG_THIS_PTR_HINT:
        /*
        --------------------------------------
        Store the pointer to the calling class
        -------------------------------------- */
        SetWindowLong(hWnd, GWL_USERDATA, lParam);
        return 1;

    }

CUGHint * _this = (CUGHint *)GetWindowLong(hWnd, GWL_USERDATA);
if (_this == NULL)
    return (DefWindowProc(hWnd, message, wParam, lParam));

switch(message)
    {
    case WM_PAINT:
        _this->OnPaint();
        return 0;

    default:
        return (DefWindowProc(hWnd, message, wParam, lParam));
    }

}
#endif
