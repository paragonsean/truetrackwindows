#include <windows.h>

/***********************************************************************
*   Ultimate Grid 97                                                   *
*   Copyright 1994 - 1997 Dundas Software Ltd.                         *
*                                                                      *
*   CUGTab                                                             *
*                                                                      *
*   This is a general purose tab control that is built into the        *
*   grid. It is most commonly used to switch grid sheets, but          *
*   can be used for other purposes.                                    *
*                                                                      *
*                                                                      *
*   This control can contain upto 64 tabs                              *
*                                                                      *
*   A notification is sent when a tab is selected, it does not         *
*   change grid sheets automatically. Although grid sheets can         *
*   be changed during this notification. This method gives the         *
*   tab control greater abilities.                                     *
*                                                                      *
*   Fonts and colors can be setup for the tab control                  *
*                                                                      *
***********************************************************************/

#include "ugctrl.h"

static TCHAR DefaultFontName[] = TEXT( "Arial" );

/***********************************************************************
*                              CUGTab                                  *
***********************************************************************/
CUGTab::CUGTab()
{
LOGFONT    lf;

m_tabCount    = 0;
m_currentTab  = 0;
m_tabOffset   = 0;
m_scrollWidth = GetSystemMetrics(SM_CXHSCROLL) *2;

lf.lfHeight          = 14;
lf.lfWidth           = 0;
lf.lfEscapement      = 0;
lf.lfOrientation     = 0;
lf.lfWeight          = FW_DONTCARE;
lf.lfItalic          = FALSE;
lf.lfUnderline       = FALSE;
lf.lfStrikeOut       = FALSE;
lf.lfOutPrecision    = 0;
lf.lfClipPrecision   = 0;
lf.lfQuality         = DEFAULT_QUALITY;
lf.lfPitchAndFamily  = 0;

if ( PRIMARYLANGID(GetUserDefaultLangID()) == LANG_JAPANESE )
    {
    lf.lfCharSet = SHIFTJIS_CHARSET;
    lstrcpy(lf.lfFaceName,TEXT("MS –¾’©"));
    }
else
    {
    lf.lfCharSet = ANSI_CHARSET;
    lstrcpy( lf.lfFaceName, DefaultFontName );
    }

m_defFont          = CreateFontIndirect( &lf );
m_font             = m_defFont;
m_resizeReady      = FALSE;
m_resizeInProgress = FALSE;
m_bestWidth        = 150;

for (int i=0; i<UTMAXTABS; i++)
    m_tabStrings[i] = 0;
}

/***********************************************************************
*                             ~CUGTab                                  *
***********************************************************************/
CUGTab::~CUGTab()
{
int i;

for ( i=0; i<UTMAXTABS; i++ )
    {
    if ( m_tabStrings[i] )
        {
        delete[] m_tabStrings[i];
        m_tabStrings[i] = 0;
        }
    }

if ( m_font )
    {
    if ( m_defFont != m_font )
        DeleteObject( m_font );
    m_font = 0;
    }

if ( m_defFont != NULL )
    DeleteObject(m_defFont);

m_defFont = 0;
}

/***********************************************************************
*                            GetTabItemWidth                           *
***********************************************************************/
int CUGTab::GetTabItemWidth( LPCTSTR string )
{
HFONT oldfont;
SIZE  size;

HDC dc = GetDC( m_tabWnd );

oldfont = (HFONT) SelectObject( dc, m_font );
GetTextExtentPoint32( dc, string, lstrlen(string), &size );
SelectObject( dc, oldfont );

ReleaseDC( m_tabWnd, dc );

return size.cx + 10;
}

/***********************************************************************
*                              AddTab                                  *
***********************************************************************/
int CUGTab::AddTab(LPCTSTR string,long ID)
{

if ( m_tabCount < UTMAXTABS )
    {
    CopyString( m_tabStrings[m_tabCount], string );
    m_tabIDs[m_tabCount]        = ID;
    m_tabWidths[m_tabCount]     = GetTabItemWidth(string);
    m_tabTextColors[m_tabCount] = GetSysColor(COLOR_WINDOWTEXT);
    m_tabBackColors[m_tabCount] = GetSysColor(COLOR_BTNFACE);

    m_tabCount++;

    AdjustScrollBars();

    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                             InsertTab                                *
***********************************************************************/
int CUGTab::InsertTab( int pos, LPCTSTR string, long ID )
{
int i;

if ( pos < 0 || pos > m_tabCount )
    return FALSE;

if ( pos == m_tabCount )
    return AddTab( string,ID );

if ( m_tabCount < UTMAXTABS )
    {

    for( i=pos; i<m_tabCount-1; i++ )
        {
        CopyString(m_tabStrings[i+1], m_tabStrings[i]);
        m_tabIDs[i+1]        = m_tabIDs[i];
        m_tabWidths[i+1]     = m_tabWidths[i];
        m_tabTextColors[i+1] = m_tabTextColors[i];
        m_tabBackColors[i+1] = m_tabBackColors[i];
        }

    m_tabCount++;
    m_currentTab++;

    CopyString(m_tabStrings[pos], string);
    m_tabIDs[pos]           = ID;
    m_tabWidths[pos]        = GetTabItemWidth(string);
    m_tabTextColors[pos]    = GetSysColor(COLOR_WINDOWTEXT);
    m_tabBackColors[pos]    = GetSysColor(COLOR_BTNFACE);

    AdjustScrollBars();
    InvalidateRect(m_tabWnd, NULL, TRUE);

    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                           DeleteTab                                  *
***********************************************************************/
int CUGTab::DeleteTab( long ID )
{

for ( int loop=0; loop<m_tabCount; loop++ )
    {
    if ( m_tabIDs[loop] == ID )
        {
        for( int loop2=loop; loop2<m_tabCount-1; loop2++ )
            {
            CopyString(m_tabStrings[loop2], m_tabStrings[loop2+1]);
            m_tabIDs[loop2]         = m_tabIDs[loop2+1];
            m_tabWidths[loop2]      = m_tabWidths[loop2+1];
            m_tabTextColors[loop2]  = m_tabTextColors[loop2+1];
            m_tabBackColors[loop2]  = m_tabBackColors[loop2+1];
            }

        m_tabCount--;

        if ( m_currentTab == loop )
            SetCurrentTab( m_tabIDs[0] );
        else if ( m_currentTab > 0 )
            m_currentTab--;

        AdjustScrollBars();
        InvalidateRect(m_tabWnd, NULL, TRUE);

        return TRUE;
        }
    }
return FALSE;

}

/***********************************************************************
*                              GetTabCount                             *
***********************************************************************/
int CUGTab::GetTabCount()
{
return m_tabCount;
}

/***********************************************************************
*                               SetCurrentTab                          *
***********************************************************************/
int CUGTab::SetCurrentTab(long ID)
{
int width = 0;
int i;

for ( i=0; i<m_tabCount; i++ )
    {
    width += m_tabWidths[i];

    if ( m_tabIDs[i] == ID )
        {
        m_currentTab = i;
        MakeCurrentTabVisible();
        InvalidateRect( m_tabWnd, NULL, TRUE );
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                             GetCurrentTab                            *
***********************************************************************/
int CUGTab::GetCurrentTab()
{
return m_currentTab;
}

/***********************************************************************
*                              AdjustScrollBars                        *
***********************************************************************/
int CUGTab::AdjustScrollBars()
{

if ( m_tabCount < 1 )
    return 0;

/*
---------------------------------------------
Get the total width and setup the scroll bars
--------------------------------------------- */
int totalTabWidths = 0;

for( int i=0; i<m_tabCount; i++ )
    totalTabWidths += m_tabWidths[i];

RECT r;

GetClientRect( m_tabWnd, &r );
int width = r.right - m_scrollWidth;

int dif = totalTabWidths - width;

if ( dif > 0 )
    m_maxTabOffset = dif + 20;
else
    m_maxTabOffset = 0;

return 1;
}

/***********************************************************************
*                              OnTabSizing                             *
***********************************************************************/
void CUGTab::OnTabSizing(int width)
{
RECT r;
POINT point;
GetWindowRect(m_tabWnd, &r);
point.x = r.left;
point.y = r.top;
ScreenToClient(GetParent(m_tabWnd), &point);
r.left = point.x;
r.top = point.y;
point.x = r.right;
point.y = r.bottom;
ScreenToClient(GetParent(m_tabWnd), &point);
r.right = point.x;
r.bottom = point.y;


RECT parentRect;
GetClientRect(GetParent(m_tabWnd), &parentRect);

if(width < 5)
    width = 5;
if(width > parentRect.right - 15)
    width = parentRect.right - 15;

m_GI->m_tabWidth = width;

m_ctrl->m_tabSizing = TRUE;
m_ctrl->AdjustComponentSizes();
m_ctrl->m_tabSizing = FALSE;
}

/***********************************************************************
*                                UPDATE                                *
***********************************************************************/
void CUGTab::Update()
{
InvalidateRect( m_tabWnd, NULL, TRUE );
UpdateWindow( m_tabWnd );
}

/***********************************************************************
*                               ONPAINT                                *
***********************************************************************/
void CUGTab::OnPaint()
{
PAINTSTRUCT paintstru;

HDC dc = BeginPaint(m_tabWnd, &paintstru);

HFONT oldfont = (HFONT)SelectObject(dc, m_font);

HBRUSH whitebrush = (HBRUSH)CreateSolidBrush( RGB(255,255,255));

HPEN whitepen = CreatePen(PS_SOLID,1,RGB(255,255,255));
HPEN blackpen = CreatePen(PS_SOLID,1,RGB(0,0,0));
HPEN graypen = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNFACE));
HPEN darkgraypen = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW));

HBRUSH pBrush   = NULL;
HPEN   oldPen;
HBRUSH oldBrush = NULL;

POINT points[4];

RECT currentRect;

RECT r;
RECT clientRect;

GetClientRect( m_tabWnd, &r );
GetClientRect( m_tabWnd, &clientRect );

SetBkColor( dc, GetSysColor(COLOR_BTNFACE) );
ExtTextOut( dc, 0, 0, ETO_OPAQUE, &r, TEXT(""), 0, NULL );

r.left = m_scrollWidth - m_tabOffset;

r.bottom --;
r.right --;

/*
-------------
Draw each tab
------------- */
for ( int t=0; t<m_tabCount; t++ )
    {
    if ( pBrush != NULL )
        {
        DeleteObject(pBrush);
        pBrush = NULL;
        }

    r.right = r.left+ m_tabWidths[t];

    if ( t == m_currentTab )
        {
        if (t == 0)
            oldBrush = (HBRUSH)SelectObject(dc, whitebrush);
        else
            SelectObject(dc, whitebrush);
        SetBkColor(dc, RGB(255,255,255));
        CopyRect(&currentRect,&r);
        }
    else
        {
        pBrush = CreateSolidBrush( m_tabBackColors[t] );
        if ( t == 0 )
            oldBrush = (HBRUSH)SelectObject(dc, pBrush);
        else
            SelectObject(dc, pBrush);
        SetBkColor(dc, m_tabBackColors[t]);
        }

    /*
    ------------
    Draw the tab
    ------------ */
    points[0].x = r.left;
    points[0].y = r.top;
    points[1].x = r.left + 7;
    points[1].y = r.bottom;
    points[2].x = r.right + 2;
    points[2].y = r.bottom;
    points[3].x = r.right +9;
    points[3].y = r.top;
    Polygon(dc, points,4);

    /*
    -------------
    Draw the text
    ------------- */
    r.left+=10;
    r.top ++;
    SetTextColor(dc, m_tabTextColors[t]);
    ExtTextOut(dc, r.left,r.top,0,&r,m_tabStrings[t],
        lstrlen(m_tabStrings[t]), NULL);
    r.left -=10;
    r.top --;

    r.left = r.right;
    }

SelectObject( dc, whitebrush );
SetBkColor( dc, RGB(255,255,255) );
CopyRect( &r, &currentRect );

/*
------------
Draw the tab
------------ */
points[0].x = r.left;
points[0].y = r.top;
points[1].x = r.left + 7;
points[1].y = r.bottom;
points[2].x = r.right + 2;
points[2].y = r.bottom;
points[3].x = r.right +9;
points[3].y = r.top;
Polygon(dc, points,4);

//blank out the top line
oldPen = (HPEN)SelectObject(dc, whitepen);
MoveToEx(dc, r.left+1,r.top, NULL);
LineTo(dc, r.right+9,r.top);


//draw the text
r.left+=10;
r.top ++;
SetTextColor(dc, m_tabTextColors[m_currentTab]);
ExtTextOut(dc, r.left,r.top,0,&r,m_tabStrings[m_currentTab],
    lstrlen(m_tabStrings[m_currentTab]),NULL);

//draw the sizing bar
CopyRect(&r,&clientRect);
r.right --;
SelectObject(dc, blackpen);
MoveToEx(dc, r.right,r.top, NULL);
LineTo(dc, r.right,r.bottom);
SelectObject(dc, darkgraypen);
r.right --;
MoveToEx(dc, r.right,r.top, NULL);
LineTo(dc, r.right,r.bottom);
SelectObject(dc, graypen);
r.right --;
MoveToEx(dc, r.right,r.top, NULL);
LineTo(dc, r.right,r.bottom);
r.right --;
MoveToEx(dc, r.right,r.top, NULL);
LineTo(dc, r.right,r.bottom);
SelectObject(dc, whitepen);
r.right --;
MoveToEx(dc, r.right,r.top, NULL);
LineTo(dc, r.right,r.bottom);
SelectObject(dc, graypen);
r.right --;
MoveToEx(dc, r.right,r.top, NULL);
LineTo(dc, r.right,r.bottom);

SelectObject(dc, oldfont);

//draw the black line over the scroll buttons
SelectObject(dc, blackpen);
MoveToEx(dc, r.left,r.top, NULL);
LineTo(dc, r.left+m_scrollWidth+1,r.top);

InvalidateRect(m_ScrollBarWnd, &r, TRUE);
UpdateWindow(m_ScrollBarWnd);

EndPaint(m_tabWnd, &paintstru);

if (oldBrush != NULL)
    SelectObject(dc, oldBrush);
if (oldPen != NULL)
    SelectObject(dc, oldPen);
if(pBrush != NULL)
    DeleteObject(pBrush);
if (whitebrush != NULL)
    DeleteObject(whitebrush);
if (whitepen != NULL)
    DeleteObject(whitepen);
if (blackpen != NULL)
    DeleteObject(blackpen);
if (graypen != NULL)
    DeleteObject(graypen);
if (darkgraypen != NULL)
    DeleteObject(darkgraypen);
}

/***********************************************************************
*                                                                      *
***********************************************************************/
int CUGTab::OnCreate()
{

    // TODO: Add your specialized creation code here
    RECT r;
    GetClientRect(m_tabWnd, &r);
    r.right = m_scrollWidth;

    HINSTANCE hInstance = (HINSTANCE)GetWindowLong(m_tabWnd,GWL_HINSTANCE);

    m_ScrollBarWnd = CreateWindow(TEXT("SCROLLBAR"), TEXT(""), WS_CHILD|WS_VISIBLE,
        r.left, r.top, r.right, r.bottom,
        m_tabWnd, (HMENU)UTABSCROLLID, hInstance, NULL);

    return 0;
}


/***********************************************************************
*                           OnLButtonDown                              *
***********************************************************************/
void CUGTab::MakeCurrentTabVisible() 
{
RECT r;
int  left;
int  right;

if ( m_currentTab < 0 || m_currentTab >= m_tabCount ) 
    return;

left = m_scrollWidth - m_tabOffset;

GetClientRect( m_tabWnd, &r );
left -= 5;
if ( left < m_scrollWidth )
    {
    m_tabOffset -= (m_scrollWidth - left);
    if ( m_tabOffset < 0 )
        m_tabOffset = 0;
    }

right = left + m_tabWidths[m_currentTab];

right += 19;
if ( right > r.right )
    {
    m_tabOffset += (right - r.right);
    if(m_tabOffset > m_maxTabOffset)
        m_tabOffset = m_maxTabOffset;
    }

}

/***********************************************************************
*                           OnLButtonDown                              *
***********************************************************************/
void CUGTab::OnLButtonDown( UINT nFlags, POINT point )
{

/*
------------------------------------------
Check to see if the tabs are to be resized
------------------------------------------ */
if ( m_resizeReady )
    {
    SetCapture( m_tabWnd );
    m_resizeInProgress = TRUE;
    }
else
    {
    /*
    -----------------------------------
    Find the tab that the mouse is over
    ----------------------------------- */
    int left = m_scrollWidth - m_tabOffset;

    for ( int t=0; t<m_tabCount; t++ )
        {
        int right = left + m_tabWidths[t];

        /*
        ---------------------------------------------
        Check to see if the mouse is over a given tab
        --------------------------------------------- */
        if ( point.x >= left && point.x <= (right+2) )
            {
            m_currentTab = t;

            /*
            ------------------------------------------------
            Check to make sure that the tab is fully visible
            ------------------------------------------------ */
            MakeCurrentTabVisible();
            InvalidateRect(m_tabWnd, NULL, TRUE);

            /*
            -------------
            Send a notify
            ------------- */
            m_ctrl->OnTabSelected(m_tabIDs[t]);
            break;
            }

        left = right;
        }
    }
}

/***********************************************************************
*                               OnSize                                 *
***********************************************************************/
void CUGTab::OnSize(UINT nType, int cx, int cy)
{
    RECT r;
    GetClientRect(m_tabWnd, &r);
    r.right = m_scrollWidth;
    r.top++;

    MoveWindow(m_ScrollBarWnd, r.left, r.top,
        r.right, r.bottom, TRUE);

    AdjustScrollBars();
}

/***********************************************************************
*                             OnHScroll                                *
***********************************************************************/
void CUGTab::OnHScroll( UINT nSBCode, UINT nPos, HWND hScrollBar )
{
// TODO: Add your message handler code here and/or call default
if ( nSBCode == SB_LINEUP )
    {
    if ( m_tabOffset > 0 )
        {
        m_tabOffset -= 5;
        InvalidateRect(m_tabWnd, NULL, TRUE);
        }
    }

if ( nSBCode == SB_LINEDOWN )
    {
    if ( m_tabOffset < m_maxTabOffset-5 )
        {
        m_tabOffset += 5;
        InvalidateRect(m_tabWnd, NULL, TRUE);
        }
    }
}

/***********************************************************************
*                             OnMouseMove                              *
***********************************************************************/
void CUGTab::OnMouseMove(UINT nFlags, POINT point)
{
    // TODO: Add your message handler code here and/or call default

    //resize in progress
    if(m_resizeInProgress){
        OnTabSizing(point.x);
    }

    //check for resize position
    else{
        RECT r;
        GetClientRect(m_tabWnd, &r);
        if(point.x > r.right - 7){
            if(!m_resizeReady){
                m_resizeReady = TRUE;
            }
        }
        else if(m_resizeReady){
            m_resizeReady = FALSE;
        }
    }
}

/***********************************************************************
*                              OnSetCursor                             *
***********************************************************************/
BOOL CUGTab::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
{
// TODO: Add your message handler code here and/or call default
if(m_resizeReady)
    SetCursor(m_GI->m_WEResizseCursor);
else
    SetCursor(LoadCursor(NULL,IDC_ARROW));

return TRUE;
}

/***********************************************************************
*                              OnLButtonUp                             *
***********************************************************************/
void CUGTab::OnLButtonUp(UINT nFlags, POINT point)
{
    // TODO: Add your message handler code here and/or call default
    ReleaseCapture();
    m_resizeInProgress = FALSE;

}

/***********************************************************************
*                              OnRButtonDown                           *
***********************************************************************/
void CUGTab::OnRButtonDown(UINT nFlags, POINT point)
{
if ( m_GI->m_enablePopupMenu )
    {
    int tab = -1;
    /*
    ------------------------------
    Find the tab the mouse is over
    ------------------------------ */
    int left = m_scrollWidth - m_tabOffset;

    for ( int t=0; t<m_tabCount; t++ )
        {
        int right = left + m_tabWidths[t];
        /*
        ---------------------------------------------
        Check to see if the mouse is over a given tab
        --------------------------------------------- */
        if ( point.x >= left && point.x <= (right+2) )
            {
            tab = t;
            break;
            }
        left = right;
        }

    ClientToScreen( m_tabWnd, &point );
    m_ctrl->StartMenu( tab, 0, &point, UG_TAB );
    }

}

/***********************************************************************
*                                                                      *
***********************************************************************/
int CUGTab::SetTabBackColor(long ID,COLORREF color){

    for(int loop = 0;loop < m_tabCount; loop++){
        if(m_tabIDs[loop] == ID){
            m_tabBackColors[loop] = color;
            return TRUE;
        }
    }
    return FALSE;
}

/***********************************************************************
*                                                                      *
***********************************************************************/
int CUGTab::SetTabTextColor(long ID,COLORREF color){

    for(int loop = 0;loop < m_tabCount; loop++){
        if(m_tabIDs[loop] == ID){
            m_tabTextColors[loop] = color;
            return TRUE;
        }
    }
    return FALSE;
}

/***********************************************************************
*                                                                      *
***********************************************************************/
int CUGTab::SetTabFont(HFONT font){

    if(font != NULL)
        m_font = font;

    for(int loop = 0;loop < m_tabCount; loop++){
        m_tabWidths[loop] = GetTabItemWidth(m_tabStrings[loop]);
    }
    AdjustScrollBars();
    InvalidateRect(m_tabWnd, NULL, TRUE);

    return TRUE;
}


/***********************************************************************
*                                                                      *
***********************************************************************/
BOOL CUGTab::CreateTabWindow(DWORD dwStyle, RECT r, HWND parent, UINT nID)
{

if ( !parent )
    return UG_ERROR;

HINSTANCE hInstance = (HINSTANCE) GetWindowLong( parent, GWL_HINSTANCE );

WNDCLASS wc;

wc.style          = CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS;
wc.lpfnWndProc    = TabWndProc;
wc.cbClsExtra     = 0;
wc.cbWndExtra     = 20;
wc.hInstance      = hInstance ;
wc.hIcon          = NULL;
wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
wc.hbrBackground  = NULL;
wc.lpszMenuName   = NULL;
wc.lpszClassName  = TEXT("UGTab");

if ( !GetClassInfo(hInstance, TEXT("UGTab"), &wc) )
    {
    RegisterClass(&wc);
    if ( !GetClassInfo(hInstance, TEXT("UGTab"), &wc) )
        return UG_ERROR;
    }

dwStyle |= WS_CLIPCHILDREN;
m_tabWnd = CreateWindow(
   TEXT("UGTab"),
   TEXT(""),
   dwStyle,
   r.left, r.top, r.right, r.bottom,
   parent,
   (HMENU) nID,
   hInstance,
   NULL);

if ( !m_tabWnd )
    return UG_ERROR;

SendMessage( m_tabWnd, UG_THIS_PTR_TAB, 0, (LPARAM) this );

OnCreate();

return UG_SUCCESS;
}


/***********************************************************************
*                              TabWndProc                              *
***********************************************************************/
long CALLBACK CUGTab::TabWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{

switch( message )
    {
    case WM_NCCREATE:
        SetWindowLong( hWnd, GWL_USERDATA, (LPARAM)NULL );
        return 1;

    case UG_THIS_PTR_TAB:
        //store the pointer to the calling class
        SetWindowLong( hWnd, GWL_USERDATA, lParam );
        return 1;
    }

CUGTab * _this = (CUGTab *) GetWindowLong( hWnd, GWL_USERDATA );
if ( !_this )
    return DefWindowProc( hWnd, message, wParam, lParam );

POINT point = {(short)LOWORD(lParam), (short)HIWORD(lParam)};

switch( message )
    {
    case WM_PAINT:
        _this->OnPaint();
        return 0;

    case WM_SETCURSOR:
        _this->OnSetCursor(hWnd, (UINT)(LOWORD(lParam)), UINT(HIWORD(lParam)));
        return 0;

    case WM_SIZE:
        _this->OnSize(wParam, LOWORD(lParam), HIWORD(lParam));
        return 0;

    case WM_LBUTTONDOWN:
        _this->OnLButtonDown((UINT)wParam, point);
        return 0;

    case WM_LBUTTONUP:
        _this->OnLButtonUp((UINT)wParam, point);
        return 0;

    case WM_MOUSEMOVE:
        _this->OnMouseMove((UINT)wParam, point);
        return 0;

    case WM_RBUTTONDOWN:
        _this->OnRButtonDown((UINT)wParam, point);
        return 0;

    case WM_HSCROLL:
        _this->OnHScroll(LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
        return 0;

    default:
        return (DefWindowProc(hWnd, message, wParam, lParam));
    }

}
