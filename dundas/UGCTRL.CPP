#include <windows.h>
#include "ugctrl.h"

/***********************************************************************
*   Ultimate Grid 97                                                   *
*   Copyright 1994 - 1997 Dundas Software Ltd.                         *
*                                                                      *
*   This is the main grid class. It contains and/or controls all       *
*   of the other grid clases. CUGCtrl - stands for C++ Ultimate        *
*   Grid Control. This is the class that other grids are usually       *
*   derived from, since all of the grid's notifications are            *
*   contained in this class. For simple and many complex grid          *
*   applications this is the only class that needs to be used          *
*   and worked with directly.                                          *
*                                                                      *
*   Instructions                                                       *
*                                                                      *
*   Generaly a derived class is created from this class and used       *
*   in other applications. A derived class is neccessary if any        *
*   of the grid's notifications are to be used                         *
*                                                                      *
*   Being derived from CWnd allows the grid to be used anywhere        *
*   a CWnd can be used, plus all CWnd functions are available as       *
*   well.                                                              *
*                                                                      *
*   This class automatically creates the grids child components        *
*    (grid, top heading, side heading, scrollbars, tabs, and           *
*    corner button)                                                    *
*                                                                      *
*   This class also creates a default memory manager datasource        *
*   and registers the grids standard cell types                        *
*                                                                      *
*   When an instance of this class (or a class derived from            *
*   this) is created, a windows still needs to be created (this        *
*   is the same for any CWnd derived class). Call the classes          *
*   CreateGrid to create the window. Or call AttachGrid to             *
*   attach the grid to a control in a dialog box.                      *
*                                                                      *
*   When this class creates or attaches to a window it's OnSetup       *
*   member is automatically called. This is a good spot for            *
*   initial setup of the grid.                                         *
*                                                                      *
***********************************************************************/

#ifdef UNICODE
static UINT ClipFormat = CF_UNICODETEXT;
#else
static UINT ClipFormat = CF_TEXT;
#endif

const static TCHAR CRChar  = TEXT( '\r' );
const static TCHAR LFChar  = TEXT( '\n' );
const static TCHAR TabChar = TEXT( '\t' );

const static TCHAR LFString[]    = TEXT( "\n" );
const static TCHAR TabString[]   = TEXT( "\t" );
const static TCHAR EmptyString[] = TEXT( "" );

/***********************************************************************
*                                CUGCtrl                               *
***********************************************************************/
CUGCtrl::CUGCtrl()
{

/*
-----------
Data source
----------- */
m_dataSrcList          = 0;
m_dataSrcListLength    = 0;
m_dataSrcListMaxLength = 0;

m_fontList          = new CUGPtrList;
m_bitmapList        = new CUGPtrList;
m_cellTypeList      = new CUGPtrList;
m_cellStylesList    = new CUGPtrList;
m_validateList      = new CUGPtrList;
m_displayFormatList = new CUGPtrList;
m_CUGGrid           = new CUGGrid;
m_CUGTopHdg         = new CUGTopHdg;
m_CUGSideHdg        = new CUGSideHdg;
m_CUGVScroll        = new CUGVScroll;
m_CUGHScroll        = new CUGHScroll;
m_CUGCnrBtn         = new CUGCnrBtn;
m_CUGTab            = new CUGTab;

/*
------------------
Scroll hint window
------------------ */
#ifdef UG_ENABLE_SCROLLHINTS
m_CUGHint = new CUGHint;
#endif

#ifdef UG_ENABLE_PRINTING
m_CUGPrint = new CUGPrint;
#endif

if ( !m_fontList || !m_bitmapList || !m_cellTypeList || !m_cellStylesList
     || !m_validateList || !m_displayFormatList || !m_CUGGrid || !m_CUGTopHdg
     || !m_CUGSideHdg || !m_CUGVScroll || !m_CUGHScroll || !m_CUGCnrBtn || !m_CUGTab )
    {
    return;
    }

/*
--------------
Control window
-------------- */
m_ctrlWnd = NULL;

/*
---------------
Tracking window
--------------- */
m_trackingWnd = NULL;

/*
------------------
Set up the 3D pens
------------------ */
m_threeDLightPen = CreatePen(PS_SOLID,1,RGB(255,255,255));
m_threeDDarkPen  = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW));

/*
----------
Popup menu
---------- */
m_menu = CreatePopupMenu();

/*
-------------------------------------------------------------------
Set up the 1 sheet and get the GridInfo class pointer to that sheet
------------------------------------------------------------------- */
m_currentSheet = 0;
m_numberSheets = 0;
m_GIList       = NULL;
m_GI           = NULL;
SetNumberSheets( 1 );
m_GI           = m_GIList[0];

/*
--------------------------
Setup inter-class pointers
-------------------------- */
m_CUGGrid->m_ctrl    = this;
m_CUGGrid->m_GI      = m_GI;
m_CUGTopHdg->m_ctrl  = this;
m_CUGTopHdg->m_GI    = m_GI;
m_CUGSideHdg->m_ctrl = this;
m_CUGSideHdg->m_GI   = m_GI;
m_CUGCnrBtn->m_ctrl  = this;
m_CUGCnrBtn->m_GI    = m_GI;
m_CUGVScroll->m_ctrl = this;
m_CUGVScroll->m_GI   = m_GI;
m_CUGHScroll->m_ctrl = this;
m_CUGHScroll->m_GI   = m_GI;
m_CUGTab->m_ctrl     = this;
m_CUGTab->m_GI       = m_GI;

#ifdef UG_ENABLE_SCROLLHINTS
m_CUGHint->m_ctrl       = this;
#endif

#ifdef UG_ENABLE_PRINTING
    m_CUGPrint->m_ctrl  = this;
    m_CUGPrint->m_GI    = m_GI;
#endif

m_defEditCtrl.m_ctrl    = this;

m_editInProgress = FALSE;

/*
---------------------------
Add the standard cell types
--------------------------- */
AddCellType( &m_normalCellType );
AddCellType( &m_dropListType   );
AddCellType( &m_arrowType      );

/*
---------------------------------------------
Add a NULL datasource to the beginning of the
data source list, since index 0 is not used
--------------------------------------------- */
AddDataSource(NULL);

m_enableUpdate = TRUE;

m_findDialogRunning = FALSE;
m_findDialogStarted = FALSE;
m_findInAllCols = FALSE;

m_tabSizing = FALSE;
}

/***********************************************************************
*                               ~CUGCtrl                               *
***********************************************************************/
CUGCtrl::~CUGCtrl()
{
int     i;
HFONT   font;
HBITMAP bitmap;

m_ctrlWnd = 0;

if ( m_dataSrcList )
    {
    delete[] m_dataSrcList;
    m_dataSrcList = 0;
    }

/*
--------------------
Delete the font list
-------------------- */
for ( i=0; i<m_fontList->GetMaxCount(); i++ )
    {
    font = (HFONT) m_fontList->GetPointer(i);
    if ( font )
        DeleteObject( font );
    }

if ( m_fontList )
    {
    delete m_fontList;
    }

/*
----------------------
Delete the bitmap list
---------------------- */
for ( i=0; i<m_bitmapList->GetMaxCount(); i++ )
    {
    bitmap = (HBITMAP) m_bitmapList->GetPointer(i);
    if ( bitmap )
        DeleteObject( bitmap );
    }

if ( m_bitmapList )
    delete m_bitmapList;

if ( m_cellTypeList )
    delete m_cellTypeList;

if ( m_cellStylesList )
    delete m_cellStylesList;

if ( m_validateList )
    delete m_validateList;

if ( m_displayFormatList )
    delete m_displayFormatList;

if ( m_CUGGrid )
    delete m_CUGGrid;

if ( m_CUGTopHdg )
    delete m_CUGTopHdg;

if ( m_CUGSideHdg )
    delete m_CUGSideHdg;

if ( m_CUGVScroll )
    delete m_CUGVScroll;

if ( m_CUGHScroll )
    delete m_CUGHScroll;

if ( m_CUGCnrBtn )
    delete m_CUGCnrBtn;

/*
------------------
Scroll hint window
------------------ */
#ifdef UG_ENABLE_SCROLLHINTS
    if ( m_CUGHint )
        delete m_CUGHint;
#endif

#ifdef UG_ENABLE_PRINTING
    if ( m_CUGPrint )
        delete m_CUGPrint;
#endif

if ( m_CUGTab )
    delete m_CUGTab;

if ( m_threeDLightPen )
    DeleteObject( m_threeDLightPen );

if ( m_threeDDarkPen )
    DeleteObject( m_threeDDarkPen );

if ( m_menu )
    DestroyMenu(m_menu);

if ( m_GIList )
    {
    for ( i=0; i<m_numberSheets; i++ )
        {
        if ( m_GIList[i] )
            delete m_GIList[i];
        }
    delete[] m_GIList;
    }
}

/***********************************************************************
*                          CLEAR_DEFAULT_FONT                          *
***********************************************************************/
void CUGCtrl::clear_default_font()
{

if ( m_GI->m_defFont )
    {
    /*
    ---------------------------------------------------------
    Only delete the font object if it is not in the font list
    --------------------------------------------------------- */
    if ( m_fontList->GetPointerIndex(m_GI->m_defFont) == -1 )
        DeleteObject( m_GI->m_defFont );

    m_GI->m_defFont = 0;
    }
}

/***********************************************************************
*                           OnSysColorChange                           *
*   This function is called when system colors are changed It          *
*   then notifies all of the cell type classes, so they can            *
*   update themselves                                                  *
***********************************************************************/
void CUGCtrl::OnSysColorChange()
{
int i;
int n;

n = m_cellTypeList->GetCount();

for ( i=0; i<n; i++ )
    {
    if ( m_cellTypeList->GetPointer(i) )
        {
        ((CUGCellType *)m_cellTypeList->GetPointer(i))->OnSystemChange();
        }
    }
}

/***********************************************************************
*                              OnHScroll                               *
*   Pass all scroll messages over to the scroll child window           *
***********************************************************************/
void CUGCtrl::OnHScroll( UINT nSBCode, UINT nPos, HWND hScrollBar )
{
if ( hScrollBar == m_CUGHScroll->m_hScrollWnd )
    m_CUGHScroll->HScroll( nSBCode, nPos );
}

/***********************************************************************
*                              OnVScroll                               *
*   Pass all scroll messages over to the scroll child window           *
***********************************************************************/
void CUGCtrl::OnVScroll( UINT nSBCode, UINT nPos, HWND vScrollBar )
{
if ( vScrollBar == m_CUGVScroll->m_vScrollWnd )
    m_CUGVScroll->VScroll( nSBCode, nPos );
}

/***********************************************************************
*                              OnSize                                  *
*   Sizes all the child windows whenever the main window sizes.        *
*   This includes the Grid, Headings and Scroll bars, tabs, etc.       *
***********************************************************************/
void CUGCtrl::OnSize( UINT nType, int cx, int cy )
{
InvalidateRect( m_ctrlWnd, NULL, TRUE );

EnableUpdate( FALSE );
AdjustComponentSizes();
EnableUpdate( TRUE );
}

/***********************************************************************
*                             EditCancel                               *
*             Cancels any editing if it is in progress                 *
***********************************************************************/
int CUGCtrl::EditCancel()
{
if ( m_editInProgress )
    {
    m_editInProgress = FALSE;

    if ( GetFocus() == m_GI->m_editCtrl )
        SetFocus( m_CUGGrid->m_gridWnd );
    /*
    ---------------------
    Hide the edit control
    --------------------- */
    ShowWindow(m_GI->m_editCtrl, SW_HIDE);
    }

return UG_SUCCESS;
}

/***********************************************************************
*                          EditCtrlFinished                            *
*   This function is to be called by all edit controls that are        *
*   used within the grid when editing is finished or canceled.         *
*   Params                                                             *
*       string      - the string that is in the edit control           *
*       cancelFlag  - edit cancel state                                *
*       continueFlag- edit continue state                              *
*       continueCol - continue col, if being continued                 *
*       continueRow - continue row, if being continued                 *
*   Return                                                             *
*       TRUE - if successful                                           *
*       FALSE - if the current editing must continue                   *
***********************************************************************/
int CUGCtrl::EditCtrlFinished(LPCTSTR string,BOOL cancelFlag,
                              BOOL continueFlag,int continueCol,
                              long continueRow)
{


if ( !m_editInProgress )
    cancelFlag = TRUE;

/*
------------------------------
Call the OnEditFinished notify
------------------------------ */
if ( OnEditFinish(m_editCol, m_editRow, m_editCtrl, string, cancelFlag) )
    {
    ShowWindow( m_editCtrl, SW_HIDE );

    if ( !cancelFlag )
        QuickSetText( m_editCol, m_editRow, string );

    /*
    -----------------------------------------------
    Check to see if editing continues in a new cell
    ----------------------------------------------- */
    if ( continueFlag && m_editParent == m_CUGGrid->m_gridWnd )
        {
        if ( continueCol < 0 )
            continueCol = m_GI->m_numberCols - 1;
        else if ( continueCol >= m_GI->m_numberCols )
            continueCol = 0;
        if ( continueRow < 0 )
            continueRow = m_GI->m_numberRows - 1;
        else if ( continueRow >= m_GI->m_numberRows )
            continueRow = 0;

        if ( OnEditContinue(m_editCol, m_editRow, &continueCol, &continueRow) )
            {
            StartEdit( continueCol, continueRow, 0 );
            return TRUE;
            }
        }

    m_editInProgress = FALSE;
    }

if ( m_editInProgress == FALSE)
    return TRUE;

return FALSE;
}

/***********************************************************************
*                        AdjustComponentSizes                          *
*   Sizes all of the child windows to match the parent window          *
*   size. A OnAdjustComponentSizes notification is called so           *
*   that the calculated sizes can be modified                          *
***********************************************************************/
void CUGCtrl::AdjustComponentSizes()
{
int i;
int vScrollWidth;
int hScrollHeight;
int origTabWidth;
int maxWidth;
int maxHeight;
int height;
int width;

RECT rect;

if ( GetFocus() != m_CUGGrid->m_gridWnd )
    SetFocus( m_CUGGrid->m_gridWnd );

vScrollWidth  = m_GI->m_vScrollWidth;
hScrollHeight = m_GI->m_hScrollHeight;
origTabWidth  = m_GI->m_tabWidth;

GetClientRect( m_ctrlWnd, &rect );

/*
---------------------------------------------
Check to see if the hScrollbar is to be shown
--------------------------------------------- */
maxWidth = rect.right - m_GI->m_sideHdgWidth;
if ( m_GI->m_tabWidth > 0 )
    {
    if ( m_GI->m_tabWidth > maxWidth-30 )
        m_GI->m_tabWidth = (maxWidth-30);
    if ( m_GI->m_tabWidth < 4 )
        m_GI->m_tabWidth = 4;
    }

width = 0;
for ( i=0; i<m_GI->m_numberCols; i++ )
    {
    width += GetColWidth(i);
    if ( width > maxWidth )
        break;
    }

/*
---------------------------------------------
Check to see if the vScrollbar is to be shown
--------------------------------------------- */
maxHeight = rect.bottom - m_GI->m_topHdgHeight;
height    = 0;
for ( i=0; i<m_GI->m_numberRows; i++ )
    {
    height += GetRowHeight(i);
    if ( height > maxHeight )
        break;
    }

if ( height < maxHeight)
    {
    if ( width < maxWidth )
        m_GI->m_vScrollWidth = 0;
    else if ( height < maxHeight-hScrollHeight )
        m_GI->m_vScrollWidth = 0;
    }

if ( width < maxWidth && m_GI->m_tabWidth <= 0 )
    {
    if ( height < maxHeight )
        m_GI->m_hScrollHeight = 0;
    else if ( width < maxWidth-vScrollWidth )
        m_GI->m_hScrollHeight = 0;
    }

/*
----------------------
Setup the vScroll Rect
---------------------- */
m_GI->m_vScrollRect.left    = rect.right - m_GI->m_vScrollWidth;
m_GI->m_vScrollRect.top     = 0;
m_GI->m_vScrollRect.right   = rect.right;
m_GI->m_vScrollRect.bottom  = rect.bottom - m_GI->m_hScrollHeight;

/*
----------------------
Setup the hScroll Rect
---------------------- */
m_GI->m_hScrollRect.left    = m_GI->m_tabWidth;
m_GI->m_hScrollRect.top     = rect.bottom - m_GI->m_hScrollHeight;
m_GI->m_hScrollRect.right   = rect.right - m_GI->m_vScrollWidth;
m_GI->m_hScrollRect.bottom  = rect.bottom;

/*
-----------------
Setup the tabRect
----------------- */
m_GI->m_tabRect.left        = 0;
m_GI->m_tabRect.top         = rect.bottom - m_GI->m_hScrollHeight;
m_GI->m_tabRect.right       = m_GI->m_tabWidth;
m_GI->m_tabRect.bottom      = rect.bottom;

/*
------------------------
Setup the topHeadingRect
------------------------ */
m_GI->m_topHdgRect.left     = m_GI->m_sideHdgWidth;
m_GI->m_topHdgRect.top      = 0;
m_GI->m_topHdgRect.right    = rect.right - m_GI->m_vScrollWidth;
m_GI->m_topHdgRect.bottom   = m_GI->m_topHdgHeight;

/*
--------------------------
Setup the side headingRect
-------------------------- */
m_GI->m_sideHdgRect.left    = 0;
m_GI->m_sideHdgRect.top     = m_GI->m_topHdgHeight;
m_GI->m_sideHdgRect.right   = m_GI->m_sideHdgWidth;
m_GI->m_sideHdgRect.bottom  = rect.bottom - m_GI->m_hScrollHeight;

/*
---------------------------
Setup the corner buttonRect
--------------------------- */
m_GI->m_cnrBtnRect.left     = 0;
m_GI->m_cnrBtnRect.top      = 0;
m_GI->m_cnrBtnRect.right    = m_GI->m_sideHdgWidth;
m_GI->m_cnrBtnRect.bottom   = m_GI->m_topHdgHeight;

/*
------------------
Setup the GridRect
------------------ */
m_GI->m_gridRect.left       = m_GI->m_sideHdgWidth;
m_GI->m_gridRect.top        = m_GI->m_topHdgHeight;
m_GI->m_gridRect.right      = m_GI->m_topHdgRect.right;
m_GI->m_gridRect.bottom     = m_GI->m_sideHdgRect.bottom;
m_GI->m_gridWidth = m_GI->m_gridRect.right - m_GI->m_gridRect.left;
m_GI->m_gridHeight = m_GI->m_gridRect.bottom - m_GI->m_gridRect.top;

/*
------------------------
Call the nofity function
------------------------ */
OnAdjustComponentSizes(&m_GI->m_gridRect,&m_GI->m_topHdgRect,
    &m_GI->m_sideHdgRect,&m_GI->m_cnrBtnRect,&m_GI->m_vScrollRect,
    &m_GI->m_hScrollRect,&m_GI->m_tabRect);

if ( m_tabSizing )
    {
    MoveWindow(m_CUGHScroll->m_hScrollWnd,
        m_GI->m_hScrollRect.left, m_GI->m_hScrollRect.top,
        m_GI->m_hScrollRect.right - m_GI->m_hScrollRect.left,
        m_GI->m_hScrollRect.bottom - m_GI->m_hScrollRect.top,
        TRUE);
    MoveWindow(m_CUGTab->m_tabWnd,
        m_GI->m_tabRect.left, m_GI->m_tabRect.top,
        m_GI->m_tabRect.right - m_GI->m_tabRect.left,
        m_GI->m_tabRect.bottom - m_GI->m_tabRect.top,
        TRUE);
    m_CUGTab->Update();
    m_CUGHScroll->Update();
    ValidateRect(m_ctrlWnd, NULL);
    }
else
    {
    MoveWindow(m_CUGGrid->m_gridWnd,
        m_GI->m_gridRect.left, m_GI->m_gridRect.top,
        m_GI->m_gridRect.right - m_GI->m_gridRect.left,
        m_GI->m_gridRect.bottom - m_GI->m_gridRect.top, TRUE);
    MoveWindow(m_CUGTopHdg->m_topHdgWnd,
        m_GI->m_topHdgRect.left, m_GI->m_topHdgRect.top,
        m_GI->m_topHdgRect.right - m_GI->m_topHdgRect.left,
        m_GI->m_topHdgRect.bottom - m_GI->m_topHdgRect.top, TRUE);
    MoveWindow(m_CUGSideHdg->m_sideHdgWnd,
        m_GI->m_sideHdgRect.left, m_GI->m_sideHdgRect.top,
        m_GI->m_sideHdgRect.right - m_GI->m_sideHdgRect.left,
        m_GI->m_sideHdgRect.bottom - m_GI->m_sideHdgRect.top, TRUE);
    MoveWindow(m_CUGCnrBtn->m_cnrBtnWnd,
        m_GI->m_cnrBtnRect.left, m_GI->m_cnrBtnRect.top,
        m_GI->m_cnrBtnRect.right - m_GI->m_cnrBtnRect.left,
        m_GI->m_cnrBtnRect.bottom - m_GI->m_cnrBtnRect.top, TRUE);
    MoveWindow(m_CUGVScroll->m_vScrollWnd,
        m_GI->m_vScrollRect.left, m_GI->m_vScrollRect.top,
        m_GI->m_vScrollRect.right - m_GI->m_vScrollRect.left,
        m_GI->m_vScrollRect.bottom - m_GI->m_vScrollRect.top, TRUE);
    MoveWindow(m_CUGHScroll->m_hScrollWnd,
        m_GI->m_hScrollRect.left, m_GI->m_hScrollRect.top,
        m_GI->m_hScrollRect.right - m_GI->m_hScrollRect.left,
        m_GI->m_hScrollRect.bottom - m_GI->m_hScrollRect.top, TRUE);
    MoveWindow(m_CUGTab->m_tabWnd,
        m_GI->m_tabRect.left, m_GI->m_tabRect.top,
        m_GI->m_tabRect.right - m_GI->m_tabRect.left,
        m_GI->m_tabRect.bottom - m_GI->m_tabRect.top, TRUE);

    CalcTopRow();
    CalcLeftCol();

    Update();
    }

/*
----------------------------------------------------------
Make sure that the scroll dimensions are put back to their
original values
---------------------------------------------------------- */
m_GI->m_vScrollWidth = vScrollWidth;
m_GI->m_hScrollHeight = hScrollHeight;

m_GI->m_tabWidth = origTabWidth;

m_GI->m_lastTopRow = m_GI->m_topRow;
m_GI->m_lastLeftCol = m_GI->m_leftCol;
}

/***********************************************************************
*                          CreateChildWindows                          *
*   This function creates all the child windows and passes them        *
*   a pointer to this class as well as a pointer to the internal       *
*   data structure (child windows include the grid, headings and       *
*   scroll bars).                                                      *
***********************************************************************/
BOOL CUGCtrl::CreateChildWindows()
{

RECT rect ={0,0,0,0};

m_CUGGrid->CreateGridWindow(WS_CHILD | WS_VISIBLE, rect, m_ctrlWnd, UG_GRID);
m_CUGTopHdg->CreateTopHdgWindow(WS_CHILD | WS_VISIBLE, rect, m_ctrlWnd, UG_TOPHEADING);
m_CUGSideHdg->CreateSideHdgWindow(WS_CHILD | WS_VISIBLE, rect, m_ctrlWnd, UG_SIDEHEADING);
m_CUGCnrBtn->CreateCnrBtnWindow(WS_CHILD | WS_VISIBLE, rect, m_ctrlWnd, UG_CORNERBUTTON );
m_CUGTab->CreateTabWindow(WS_CHILD | WS_VISIBLE, rect, m_ctrlWnd, UG_TAB);

m_CUGVScroll->CreateVScrollWindow(SBS_VERT | WS_CHILD | WS_VISIBLE, rect, m_ctrlWnd, UG_VSCROLL);
m_CUGHScroll->CreateHScrollWindow(SBS_HORZ | WS_CHILD | WS_VISIBLE, rect, m_ctrlWnd, UG_HSCROLL);

/*
------------------------------
Setup the default edit control
------------------------------ */
m_defEditCtrl.CreateEditWindow(WS_CHILD | ES_MULTILINE | ES_AUTOVSCROLL, rect, m_ctrlWnd, UG_EDITID);

m_GI->m_editCtrl = m_defEditCtrl.m_editWnd;

/*
--------------------------
Size all the child windows
-------------------------- */
AdjustComponentSizes();

return UG_SUCCESS;
}

/***********************************************************************
*                               OnPaint                                *
*   This routine is responsible for painting any area that is          *
*   not covered by one of the child windows (such as the bottom        *
*   right corner)                                                      *
***********************************************************************/
void CUGCtrl::OnPaint()
{
HBRUSH brush;
HDC    dc;
RECT   rect;

dc = GetDC( m_ctrlWnd );

GetClientRect( m_ctrlWnd, &rect );

brush = CreateSolidBrush( GetSysColor(COLOR_BTNFACE) );
FillRect( dc, &rect, brush );
DeleteObject( brush );

ReleaseDC( m_ctrlWnd, dc );
ValidateRect( m_ctrlWnd, NULL );

RedrawAll();
m_CUGVScroll->Update();
m_CUGHScroll->Update();
}

/***********************************************************************
*                            OnGetDlgCode                              *
***********************************************************************/
UINT CUGCtrl::OnGetDlgCode()
{
return DLGC_WANTALLKEYS|DLGC_WANTARROWS;
}

/***********************************************************************
*                             OnSetFocus                               *
*              Sets the focus to the grid child window                 *
***********************************************************************/
void CUGCtrl::OnSetFocus(HWND hOldWnd)
{
SetFocus( m_CUGGrid->m_gridWnd );
}

/***********************************************************************
*                             CreateGrid                               *
***********************************************************************/
BOOL CUGCtrl::CreateGrid( DWORD dwStyle, const RECT & rect, HWND hWndParent, UINT nID )
{

CreateCtrlWindow( dwStyle, rect, hWndParent, nID );

if ( m_ctrlWnd )
    {
    OnSetup();
    OnSheetSetup( 0 );
#ifdef UG_ENABLE_HINTS
    m_CUGGrid->CreateToolTipWindow(m_CUGGrid->m_gridWnd);
#endif
#ifdef UG_ENABLE_SCROLLHINTS
    m_CUGHint->CreateHintWindow(WS_CHILD, rect, m_ctrlWnd, 8008);
#endif
    return UG_SUCCESS;
    }

return UG_ERROR;
}

/***********************************************************************
*                             AttachGrid                               *
*   Attaches a 'CUG30' window from a dialog to the class Make          *
*   sure the InitCUGCtrl(); function has been called before the        *
*   dialog is opened, otherwise it will fail                           *
***********************************************************************/
BOOL CUGCtrl::AttachGrid( HWND hWnd, UINT ID )
{
long style;

/*
---------------------------------------------------------------------
if ( GetDlgItem(hWnd,ID) )
    {
    style = GetWindowLong( hWnd, GWL_STYLE );
    style |= WS_CLIPCHILDREN | WS_TABSTOP|WS_BORDER;
    SetWindowLong( hWnd, GWL_STYLE, style );

    m_ctrlWnd = GetDlgItem( hWnd, ID );
    SetWindowLong( m_ctrlWnd, GWL_WNDPROC, (long) CtrlWndProc );
    SendMessage( m_ctrlWnd, UG_THIS_PTR_CTRL, 0, (LPARAM) this );
    CreateChildWindows();
    OnSetup();
--------------------------------------------------------------------- */

hWnd = GetDlgItem( hWnd,ID );

if ( hWnd )
    {
    style = GetWindowLong( hWnd, GWL_STYLE );
    style |= WS_CLIPCHILDREN | WS_TABSTOP|WS_BORDER;
    SetWindowLong( hWnd, GWL_STYLE, style );

    m_ctrlWnd = hWnd;
    SetWindowLong( m_ctrlWnd, GWL_WNDPROC, (long) CtrlWndProc );
    SendMessage( m_ctrlWnd, UG_THIS_PTR_CTRL, 0, (LPARAM) this );
    CreateChildWindows();
    OnSetup();

#ifdef UG_ENABLE_HINTS
    m_CUGGrid->CreateToolTipWindow(m_CUGGrid->m_gridWnd);
#endif

    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                             CalcTopRow                               *
*   Calculates the Max top row allowed then adjusts the TopRow         *
*   if needed (m_maxTopRow). This function is called internally        *
***********************************************************************/
void CUGCtrl::CalcTopRow()
{
long oldMaxRow;
long row;
int  height;

oldMaxRow = m_GI->m_maxTopRow;

/*
--------------------
Calc the max top row
-------------------- */
height = m_GI->m_gridHeight - m_GI->m_lockRowHeight;
m_GI->m_maxTopRow = m_GI->m_numLockRows;
for ( row=m_GI->m_numberRows; row>= 0; row-- )
    {
    height -= GetRowHeight( row );
    if ( height < 0 )
        {
        m_GI->m_maxTopRow = row + 1;
        if ( m_GI->m_maxTopRow >= m_GI->m_numberRows )
            m_GI->m_maxTopRow = m_GI->m_numberRows - 1;
        break;
        }
    }

/*
-----------------------------------------
Make sure that the max row is not below 0
----------------------------------------- */
if ( m_GI->m_maxTopRow < m_GI->m_numLockRows )
    m_GI->m_maxTopRow = m_GI->m_numLockRows;

if ( m_GI->m_topRow < m_GI->m_numLockRows )
    m_GI->m_topRow = m_GI->m_numLockRows;

/*
----------------------------
Adjust the top row if needed
---------------------------- */
if ( m_GI->m_topRow > m_GI->m_maxTopRow )
    m_GI->m_topRow = m_GI->m_maxTopRow;

/*
--------------------------------------------------------
Check to see if the vscrollbar should be shown or hidden
-------------------------------------------------------- */
if ( oldMaxRow == 0 && m_GI->m_maxTopRow > 0 )
    AdjustComponentSizes();

if ( oldMaxRow > 0 && m_GI->m_maxTopRow == 0 )
    AdjustComponentSizes();
}

/***********************************************************************
*                            CalcLeftCol                               *
*  Calculates the MAX left column then adjusts the left column         *
*  if needed (m_maxLeftCol). This function is called internally        *
***********************************************************************/
void CUGCtrl::CalcLeftCol()
{

long oldMaxCol;
int  col;
int  width;

oldMaxCol = m_GI->m_maxLeftCol;

/*
---------------------
Calc the max left col
--------------------- */
width              = m_GI->m_gridWidth - m_GI->m_lockColWidth;
m_GI->m_maxLeftCol = m_GI->m_numLockCols;

for ( col=m_GI->m_numberCols; col>=0; col-- )
    {
    width -= GetColWidth( col );
    if ( width < 0 )
        {
        m_GI->m_maxLeftCol = col + 1;
        if ( m_GI->m_maxLeftCol >= m_GI->m_numberCols )
            m_GI->m_maxLeftCol = m_GI->m_numberCols - 1;
        break;
        }
    }

/*
---------------------------------------------
Check to make sure the values are not below 0
--------------------------------------------- */
if ( m_GI->m_maxLeftCol < m_GI->m_numLockCols )
    m_GI->m_maxLeftCol = m_GI->m_numLockCols;

if ( m_GI->m_leftCol < m_GI->m_numLockCols )
    m_GI->m_leftCol = m_GI->m_numLockCols;

/*
--------------------------------
Adjust the left column if needed
-------------------------------- */
if ( m_GI->m_leftCol > m_GI->m_maxLeftCol )
    m_GI->m_leftCol = m_GI->m_maxLeftCol;

/*
--------------------------------------------------------
Check to see if the vscrollbar should be shown or hidden
-------------------------------------------------------- */
if ( oldMaxCol == 0 && m_GI->m_maxLeftCol > 0 )
    AdjustComponentSizes();

if ( oldMaxCol > 0 && m_GI->m_maxLeftCol == 0 )
    AdjustComponentSizes();
}

/***********************************************************************
*                             AdjustTopRow                             *
*  Adjusts the top row so that that current row is in within           *
*  view. This function is called internally                            *
***********************************************************************/
void CUGCtrl::AdjustTopRow()
{
long row;
int  height;

/*
-------------------------------------------
If the top row is greater then set it equal
------------------------------------------- */
if ( m_GI->m_topRow > m_GI->m_dragRow )
    {
    if ( m_GI->m_dragRow >= m_GI->m_numLockRows )
        m_GI->m_topRow = m_GI->m_dragRow;
    else
        m_GI->m_topRow = m_GI->m_numLockRows;
    }


/*
-----------------------------------
If the top row is equal then return
----------------------------------- */
if ( m_GI->m_topRow == m_GI->m_dragRow )
    return;

/*
-----------------------------------------
If less then check to see if it is within
the height of the grid for distance
----------------------------------------- */
if ( m_GI->m_uniformRowHeightFlag == FALSE )
    {
    height = m_GI->m_gridHeight - m_GI->m_lockRowHeight;
    for ( row=m_GI->m_dragRow; row>=m_GI->m_topRow; row-- )
        {
        height -= GetRowHeight( row );
        if ( height < 0 )
            {
            m_GI->m_topRow = row + 1;
            break;
            }
        }
    }
else
    {
    height = m_GI->m_gridHeight - m_GI->m_lockRowHeight;
    row    = m_GI->m_dragRow - height / m_GI->m_defRowHeight;
    if ( m_GI->m_topRow < row )
        m_GI->m_topRow = row;
    }
}

/***********************************************************************
*                           AdjustLeftCol                              *
*  Adjusts the left column so that that current col is in view.        *
*  This function is called internally                                  *
***********************************************************************/
void CUGCtrl::AdjustLeftCol()
{
int col;
int width;

/*
------------------------------------------------
If the left col row is greater then set it equal
------------------------------------------------ */
if ( m_GI->m_leftCol > m_GI->m_dragCol )
    {
    if ( m_GI->m_dragCol >= m_GI->m_numLockCols )
        m_GI->m_leftCol = m_GI->m_dragCol;
    else
        m_GI->m_leftCol = m_GI->m_numLockCols;
    }

/*
------------------------------------
If the left col is equal then return
------------------------------------ */
if ( m_GI->m_leftCol == m_GI->m_dragCol )
    return;

/*
-----------------------------------------
If less then check to see if it is within
the width of the grid for distance
----------------------------------------- */
width = m_GI->m_gridWidth - m_GI->m_lockColWidth;
for ( col=m_GI->m_dragCol; col>=m_GI->m_leftCol; col-- )
    {
    width -= GetColWidth(col);
    if ( width < 0 )
        {
        m_GI->m_leftCol = col +1;
        break;
        }
    }
}

/***********************************************************************
*                              Update                                  *
*   General purpose update, updates are sent to all child              *
*   windows - each child window is then responsible for checking       *
*   all variables that affect it.                                      *
***********************************************************************/
void CUGCtrl::Update()
{
if ( !m_enableUpdate )
    return;

m_CUGGrid->Update();
m_CUGSideHdg->Update();
m_CUGCnrBtn->Update();
m_CUGTab->Update();
m_CUGTopHdg->Update();
m_CUGVScroll->Update();
m_CUGHScroll->Update();

}

/***********************************************************************
*                               Moved                                  *
*                      Movement notify function                        *
*   This function checks to see what moved in the grid then            *
*   notifies the appropriate child windows, Processes movements        *
*   for the multiple-selection class, and stores the current           *
*   cell                                                               *
***********************************************************************/
void CUGCtrl::Moved()
{
BOOL cellChange;

/*
----------------------------
Call the multiselect manager
---------------------------- */
if ( m_GI->m_multiSelectFlag && ! m_editInProgress )
    {
    if ( m_GI->m_moveType == 0 ) //keyboard
        m_GI->m_multiSelect->OnKeyMove(m_GI->m_dragCol,m_GI->m_dragRow);
    else if ( m_GI->m_moveType == 1 )  //left click
        m_GI->m_multiSelect->OnLClick(m_GI->m_dragCol,m_GI->m_dragRow,m_GI->m_moveFlags);
    else if ( m_GI->m_moveType == 2 )  //right click
        m_GI->m_multiSelect->OnRClick(m_GI->m_dragCol,m_GI->m_dragRow,m_GI->m_moveFlags);
    else    //mouse move
        m_GI->m_multiSelect->OnMouseMove(m_GI->m_dragCol,m_GI->m_dragRow,m_GI->m_moveFlags);

    m_GI->m_multiSelect->GetOrigCell(&m_GI->m_currentCol,&m_GI->m_currentRow);
    }
else
    {
    m_GI->m_currentCol = m_GI->m_dragCol;
    m_GI->m_currentRow = m_GI->m_dragRow;
    }

/*
--------------------
Send notify messages
-------------------- */
cellChange = FALSE;

/*
----------
Col change
---------- */
if ( m_GI->m_lastCol != m_GI->m_currentCol )
    {
    m_GI->m_defDataSource->OnColChange( m_GI->m_lastCol, m_GI->m_currentCol );
    OnColChange( m_GI->m_lastCol, m_GI->m_currentCol );
    cellChange = TRUE;
    }

/*
----------
Row change
---------- */
if ( m_GI->m_lastRow != m_GI->m_currentRow)
    {
    m_GI->m_defDataSource->OnRowChange( m_GI->m_lastRow, m_GI->m_currentRow );
    OnRowChange( m_GI->m_lastRow, m_GI->m_currentRow );
    cellChange = TRUE;
    }

/*
-----------
Cell change
----------- */
if ( cellChange )
    {
    OnCellChange( m_GI->m_lastCol, m_GI->m_currentCol, m_GI->m_lastRow, m_GI->m_currentRow );
    }

/*
---------------
Left col change
--------------- */
if ( m_GI->m_lastLeftCol != m_GI->m_leftCol )
    {
    OnLeftColChange( m_GI->m_lastLeftCol, m_GI->m_leftCol );
    }

/*
--------------
Top row change
-------------- */
if ( m_GI->m_lastTopRow != m_GI->m_topRow)
    {
    OnTopRowChange( m_GI->m_lastTopRow, m_GI->m_topRow );
    }

/*
-----------------------------------
Move the tracking window if need be
----------------------------------- */
MoveTrackingWindow();

/*
---------------------------------
Notify the cell types of the move
--------------------------------- */
GetCellIndirect( m_GI->m_lastCol, m_GI->m_lastRow, &m_cell );
GetCellType(m_cell.GetCellType())->OnKillFocus( m_GI->m_lastCol, m_GI->m_lastRow, &m_cell );

GetCellIndirect( m_GI->m_currentCol, m_GI->m_currentRow, &m_cell );
GetCellType(m_cell.GetCellType())->OnSetFocus( m_GI->m_currentCol, m_GI->m_currentRow, &m_cell );

/*
---------------------------------
Notify the components of the move
--------------------------------- */
m_CUGGrid->Moved();
m_CUGSideHdg->Moved();
m_CUGTopHdg->Moved();
m_CUGVScroll->Moved();
m_CUGHScroll->Moved();
m_CUGCnrBtn->Moved();

/*
----------------------------
Update the last... variables
---------------------------- */
m_GI->m_lastTopRow  = m_GI->m_topRow;
m_GI->m_lastRow     = m_GI->m_currentRow;
m_GI->m_lastLeftCol = m_GI->m_leftCol;
m_GI->m_lastCol     = m_GI->m_currentCol;
}

/***********************************************************************
*                           GetCellType                                *
*   Returns the pointer to the specified cell type. When a cell        *
*   type is added to the grid it is given an index number. This        *
*   index can be used to look up its coresponding pointer              *
*                                                                      *
*   Params                                                             *
*       type - index value to look up                                  *
*                                                                      *
*   Returns a pointer to a cell type, if the index is not              *
*   found then the default cell type pointer is returned               *
***********************************************************************/
CUGCellType * CUGCtrl::GetCellType( int type )
{
CUGCellType * ct;

if ( type <= 0 )
    return &m_normalCellType;

ct = (CUGCellType*) m_cellTypeList->GetPointer( type );
if ( ct == NULL )
    return &m_normalCellType;
else
    return ct;
}

/***********************************************************************
*                           GetCellType                                *
***********************************************************************/
int CUGCtrl::GetCellType( CUGCellType * type )
{
return m_cellTypeList->GetPointerIndex( (void*) type );
}

/***********************************************************************
*                           GetCellType                                *
*   Returns the cell type for a given cell                             *
*                                                                      *
*   Params                                                             *
*       col,row - cell to return the cell type of                      *
*                                                                      *
*   Returns a pointer to a cell type, if the cell is not               *
*   found then the default cell type pointer is returned               *
***********************************************************************/
CUGCellType * CUGCtrl::GetCellType( int col, long row )
{

if ( GetCellIndirect(col,row,&m_cell) == UG_SUCCESS )
    {
    if ( m_cell.IsPropertySet(UGCELL_CELLTYPE_SET) )
        return GetCellType( m_cell.GetCellType() );
    else
        return &m_normalCellType;
    }

return GetCellType(-1);
}

/***********************************************************************
*                             VerifyTopRow                             *
*   Checks to see that the new given position of the top row is        *
*   valid, and will adjust it if ness.                                 *
***********************************************************************/
int CUGCtrl::VerifyTopRow( long * newRow )
{

if ( *newRow > m_GI->m_maxTopRow )
    *newRow = m_GI->m_maxTopRow;

if ( *newRow < m_GI->m_numLockRows )
    *newRow = m_GI->m_numLockRows;

return UG_SUCCESS;
}

/***********************************************************************
*                           VerifyCurrentRow                           *
***********************************************************************/
int CUGCtrl::VerifyCurrentRow( long* newRow )
{
long rowsfound;
int  startCol;
int  endCol;
long startRow;
long endRow;

if ( *newRow >= m_GI->m_numberRows )
    {

    /*
    ------------------
    Perform hit bottom
    ------------------ */
    rowsfound = 0;
    m_GI->m_defDataSource->OnHitBottom( m_GI->m_numberRows, *newRow - m_GI->m_numberRows + 1, &rowsfound );
    OnHitBottom( m_GI->m_numberRows, *newRow - m_GI->m_numberRows + 1, rowsfound );

    if ( m_GI->m_dragRow == (m_GI->m_numberRows-1) )
        return 1;
    else
        *newRow = m_GI->m_numberRows -1;
    }

if ( *newRow < 0 )
    *newRow = 0;

if ( m_GI->m_enableJoins )
    {
    GetCell( m_GI->m_dragCol, *newRow, &m_cell );
    if ( m_cell.IsPropertySet(UGCELL_JOIN_SET) )
        {
        startCol = m_GI->m_dragCol;
        startRow = *newRow;
        GetJoinRange( &startCol, &startRow, &endCol, &endRow );
        if ( m_GI->m_moveType == 0 )
            {
            /*
            --------
            Keyboard
            -------- */
            if ( *newRow > m_GI->m_dragRow )
                {
                if ( m_GI->m_dragRow >= startRow)
                    {
                    if ( endRow+1 < m_GI->m_numberRows )
                        *newRow = endRow+1;
                    }
                else
                    {
                    *newRow = startRow;
                    }
                }
            else
                {
                if ( m_GI->m_dragRow <= endRow)
                    {
                    if ( startRow -1 > 0 )
                        *newRow = startRow-1;
                    }
                }
            }
        else
            {
            *newRow = startRow;
            }
        }
    }

return UG_SUCCESS;
}

/***********************************************************************
*                           VerifyLeftCol                              *
***********************************************************************/
int CUGCtrl::VerifyLeftCol( int * newCol )
{

if ( *newCol > m_GI->m_maxLeftCol )
    *newCol = m_GI->m_maxLeftCol;

if ( *newCol < m_GI->m_numLockCols )
    *newCol = m_GI->m_numLockCols;

return UG_SUCCESS;
}

/***********************************************************************
*                         VerifyCurrentCol                             *
***********************************************************************/
int CUGCtrl::VerifyCurrentCol( int * newCol )
{
int  startCol;
int  endCol;
long startRow;
long endRow;

if ( *newCol >= m_GI->m_numberCols )
    {
    if ( m_GI->m_dragCol == (m_GI->m_numberCols - 1) )
        return 1;
    else
        *newCol = m_GI->m_numberCols -1;
    }

if ( *newCol == m_GI->m_dragCol )
    return UG_SUCCESS;

if ( *newCol < 0 )
    *newCol = 0;

if ( m_GI->m_enableJoins )
    {
    GetCell( *newCol, m_GI->m_dragRow, &m_cell );
    if ( m_cell.IsPropertySet(UGCELL_JOIN_SET) )
        {
        startCol = *newCol;
        startRow = m_GI->m_dragRow;
        GetJoinRange( &startCol, &startRow, &endCol, &endRow );
        if ( m_GI->m_moveType == 0 )
            {
            /*
            --------
            Keyboard
            -------- */
            if ( *newCol > m_GI->m_dragCol )
                {
                if ( m_GI->m_dragCol >= startCol )
                    {
                    if ( endCol+1 < m_GI->m_numberCols )
                        *newCol = endCol+1;
                    }
                else
                    {
                    *newCol = startCol;
                    }
                }
            else
                {
                if ( m_GI->m_dragCol <= endCol )
                    {
                    if ( startCol -1 > 0 )
                        *newCol = startCol-1;
                    }
                }
            }
        else
            {
            *newCol = startCol;
            }
        }
    }

return UG_SUCCESS;
}

/***********************************************************************
*                            MoveTopRow                                *
*   Moves the grid's top row, calls the OnCanViewMove first to         *
*   see if the process is allowed Defines are avialable for this       *
*   function                                                           *
*                                                                      *
*   'flag'  0-lineup 1-linedown 2-pageup                               *
*           3-pagedown 4-top 5-bottom                                  *
*                                                                      *
* returns                                                              *
*   1           out of range                                           *
*   2           OnCanMove did not allow the procedure                  *
*   UG_SUCCESS  success                                                *
***********************************************************************/
int CUGCtrl::MoveTopRow( int flag )
{
int  height;
long row;

switch( flag )
    {

    case UG_LINEUP:
        m_CUGVScroll->m_trackRowPos = m_GI->m_topRow -1;
        return SetTopRow(m_GI->m_topRow -1);

    case UG_LINEDOWN:
        m_CUGVScroll->m_trackRowPos = m_GI->m_topRow +1;
        return SetTopRow(m_GI->m_topRow +1);

    case UG_TOP:
        m_CUGVScroll->m_trackRowPos = 0;
        return SetTopRow(0);

    case UG_BOTTOM:
        m_CUGVScroll->m_trackRowPos = m_GI->m_numberRows - 1;
        return SetTopRow(m_GI->m_numberRows - 1);

    case UG_PAGEUP:
        height = m_GI->m_gridHeight - m_GI->m_lockRowHeight;
        row    = m_GI->m_topRow;
        while ( height >= 0 )
            {
            if ( row >= 0 )
                height -= GetRowHeight( row );
            else
                height -= m_GI->m_defRowHeight;
            row--;
            }
        m_CUGVScroll->m_trackRowPos = row;
        return SetTopRow( row );

    case UG_PAGEDOWN:
        height = m_GI->m_gridHeight - m_GI->m_lockRowHeight;
        row    = m_GI->m_topRow;
        while ( height >= 0 )
            {
            if ( row < m_GI->m_maxTopRow )
                height -= GetRowHeight( row );
            else
                height -= m_GI->m_defRowHeight;
            row++;
            }
        m_CUGVScroll->m_trackRowPos = row;
        return SetTopRow(row);
    }
return UG_SUCCESS;
}

/***********************************************************************
*                           AdjustTopRow                               *
*  Adjusts the position of the top row by the given amount             *
*  (which can be positive or negative). OnCanViewMove is called        *
*  to see if the process is allowed                                    *
*                                                                      *
* returns                                                              *
*   1           out of range                                           *
*   2           OnCanMove did not allow the procedure                  *
*   UG_SUCCESS  success                                                *
***********************************************************************/
int CUGCtrl::AdjustTopRow( long adjust )
{
return SetTopRow( m_GI->m_topRow + adjust );
}

/***********************************************************************
*                         MoveCurrentRow                               *
*   Moves the grid's current row, calls the OnCanMove first to         *
*   see if the process is allowed                                      *
*                                                                      *
*   Defines are avialable for this function                            *
*   'flag'  0-lineup 1-linedown 2-pageup                               *
*           3-pagedown 4-top 5-bottom                                  *
*                                                                      *
*  returns                                                             *
*   1           out of range                                           *
*   2           OnCanMove did not allow the procedure                  *
*   UG_SUCCESS  success                                                *
***********************************************************************/
int CUGCtrl::MoveCurrentRow( int flag )
{
int  height;
long row;

switch ( flag )
    {

    case UG_LINEUP:
        return GotoRow( m_GI->m_dragRow - 1 );

    case UG_LINEDOWN:
        return GotoRow( m_GI->m_dragRow + 1 );

    case UG_TOP:
        return GotoRow( 0 );

    case UG_BOTTOM:
        return GotoRow( m_GI->m_numberRows - 1 );

    case UG_PAGEUP:
        height = m_GI->m_gridHeight - m_GI->m_lockRowHeight;
        row    = m_GI->m_dragRow;
        while ( height >= 0 )
            {
            if ( row >= 0 )
                height -= GetRowHeight( row );
            else
                height -= m_GI->m_defRowHeight;
            row --;
            }
        return GotoRow( row );

    case UG_PAGEDOWN:
        height = m_GI->m_gridHeight - m_GI->m_lockRowHeight;
        row    = m_GI->m_dragRow;
        while ( height >= 0 )
            {
            if ( row < m_GI->m_numberRows)
                height -= GetRowHeight( row );
            else
                height -= m_GI->m_defRowHeight;
            row++;
            }
        return GotoRow(row);
    }

return UG_SUCCESS;
}

/***********************************************************************
*                        AdjustCurrentRow                              *
*   Adjusts the position of the current row by the given               *
*   amount (which can be positive or negative).                        *
*   OnCanMove is called to see if the process is                       *
*   allowed                                                            *
*                                                                      *
* returns                                                              *
*   1           out of range                                           *
*   2           OnCanMove did not allow the procedure                  *
*   UG_SUCCESS  success                                                *
***********************************************************************/
int CUGCtrl::AdjustCurrentRow( long adjust )
{
return GotoRow( m_GI->m_dragRow + adjust );
}

/***********************************************************************
*                                 GotoRow                              *
*   Moves the current row to the given row. OnCanMove is called        *
*   to see if the process is allowed                                   *
*                                                                      *
* returns                                                              *
*   1           out of range                                           *
*   2           OnCanMove did not allow the procedure                  *
*   UG_SUCCESS  success                                                *
***********************************************************************/
int CUGCtrl::GotoRow( long row )
{

if ( m_GI->m_moveType == 0) // 0 => key
    {
    if ( GetKeyState(VK_SHIFT) >=0 && GetKeyState(VK_CONTROL) >=0 )
        m_GI->m_multiSelect->GetOrigCell( &m_GI->m_currentCol, &m_GI->m_currentRow );
    }

if ( row == m_GI->m_dragRow )
    return UG_SUCCESS;

if ( VerifyCurrentRow(&row) != UG_SUCCESS )
    return 1;

if ( row == m_GI->m_dragRow )
    return UG_SUCCESS;

if ( OnCanMove(m_GI->m_dragCol, m_GI->m_dragRow, m_GI->m_dragCol,row) == FALSE )
    return 2;

m_GI->m_dragRow = row;


AdjustTopRow();


Moved();

return UG_SUCCESS;
}

/***********************************************************************
*                              SetTopRow                               *
*   Moves the top row to the given position.                           *
*   OnCanViewMoved is called to see if the procedure                   *
*   is allowed                                                         *
*                                                                      *
* returns                                                              *
*   1           out of range                                           *
*   2           OnCanMove did not allow the procedure                  *
*   UG_SUCCESS  success                                                *
***********************************************************************/
int CUGCtrl::SetTopRow( long row )
{

if ( row == m_GI->m_topRow )
    return UG_SUCCESS;

if ( VerifyTopRow(&row) != UG_SUCCESS )
    return 1;

if ( OnCanViewMove(m_GI->m_leftCol,m_GI->m_topRow,m_GI->m_leftCol,row) == FALSE )
    return 2;

m_GI->m_topRow = row;

Moved();

return UG_SUCCESS;
}

/***********************************************************************
*                           MoveLeftCol                                *
*   Moves the grid's left column, calls the OnCanViewMove              *
*   first to see if the process is allowed                             *
*   Defines are avialable for this function                            *
*   'flag'  0-lineup 1-linedown 2-pageup                               *
*           3-pagedown 4-top 5-bottom                                  *
*                                                                      *
* returns                                                              *
*   1           out of range                                           *
*   2           OnCanMove did not allow the procedure                  *
*   UG_SUCCESS  success                                                *
***********************************************************************/
int CUGCtrl::MoveLeftCol( int flag )
{
int width;
int col;

switch ( flag )
    {

    case UG_LINEUP:
        m_CUGHScroll->m_trackColPos = m_GI->m_leftCol - 1;
        return SetLeftCol( m_GI->m_leftCol - 1 );

    case UG_LINEDOWN:
        m_CUGHScroll->m_trackColPos = m_GI->m_leftCol + 1;
        return SetLeftCol( m_GI->m_leftCol + 1 );

    case UG_TOP:
        m_CUGHScroll->m_trackColPos = 0;
        return SetLeftCol( 0 );

    case UG_BOTTOM:
        m_CUGHScroll->m_trackColPos = m_GI->m_numberCols - 1;
        return SetLeftCol( m_GI->m_numberCols - 1 );

    case UG_PAGEUP:
        width = m_GI->m_gridWidth - m_GI->m_lockColWidth;
        col   = m_GI->m_leftCol;
        while ( width >= 0 )
            {
            if ( col >= 0 )
                width -= GetColWidth( col );
            else
                width -= m_GI->m_defColWidth;
            col--;
            }
        m_CUGHScroll->m_trackColPos = col;
        return SetLeftCol( col );

    case UG_PAGEDOWN:
        width = m_GI->m_gridWidth - m_GI->m_lockColWidth;
        col   = m_GI->m_leftCol;
        while ( width >= 0 )
            {
            if ( col >= 0 )
                width -= GetColWidth( col );
            else
                width -= m_GI->m_defColWidth;
            col++;
            }
        m_CUGHScroll->m_trackColPos = col;
        return SetLeftCol( col );
    }

return UG_SUCCESS;
}

/***********************************************************************
*                         AdjustLeftCol                                *
*   Adjusts the position of the left column by the given               *
*   amount (which can be positive or negative).                        *
*   OnCanViewMove is called to see if the process is                   *
*   allowed                                                            *
*                                                                      *
* returns                                                              *
*   1           out of range                                           *
*   2           OnCanMove did not allow the procedure                  *
*   UG_SUCCESS  success                                                *
***********************************************************************/
int CUGCtrl::AdjustLeftCol( int adjust )
{
return SetLeftCol(m_GI->m_leftCol + adjust);
}

/***********************************************************************
*                          MoveCurrentCol                              *
*   Moves the grid's current column, calls the OnCanMove               *
*   first to see if the process is allowed                             *
*   Defines are avialable for this function                            *
*   'flag'  0-lineup 1-linedown 2-pageup                               *
*           3-pagedown 4-top 5-bottom                                  *
*                                                                      *
* returns                                                              *
*   1           out of range                                           *
*   2           OnCanMove did not allow the procedure                  *
*   UG_SUCCESS  success                                                *
***********************************************************************/
int CUGCtrl::MoveCurrentCol( int flag )
{
int col;
int width;

switch(flag)
    {
    case UG_LINEUP:
        return GotoCol( m_GI->m_dragCol -1 );

    case UG_LINEDOWN:
        return GotoCol( m_GI->m_dragCol + 1 );

    case UG_TOP:
        return GotoCol( 0 );

    case UG_BOTTOM:
        return GotoCol( m_GI->m_numberCols - 1);

    case UG_PAGEUP:
        width = m_GI->m_gridWidth - m_GI->m_lockColWidth;
        col   = m_GI->m_dragCol;
        while ( width >= 0 )
            {
            if ( col >= 0 )
                width -= GetColWidth(col);
            else
                width -= m_GI->m_defColWidth;
            col --;
            }
        return GotoCol( col );

    case UG_PAGEDOWN:
        width = m_GI->m_gridWidth - m_GI->m_lockColWidth;
        col   = m_GI->m_dragCol;
        while ( width >= 0 )
            {
            if ( col >= 0 )
                width -= GetColWidth( col );
            else
                width -= m_GI->m_defColWidth;
            col++;
            }
        return GotoCol(col);
    }

return UG_SUCCESS;
}

/***********************************************************************
*                          AdjustCurrentCol                            *
*   Adjusts the position of the current column by the given            *
*   amount (which can be positive or negative). OnCanMove is           *
*   called to see if the process is allowed                            *
*                                                                      *
* returns                                                              *
*   1           out of range                                           *
*   2           OnCanMove did not allow the procedure                  *
*   UG_SUCCESS  success                                                *
***********************************************************************/
int CUGCtrl::AdjustCurrentCol( int adjust )
{
return GotoCol( m_GI->m_dragCol + adjust );
}

/***********************************************************************
*                                GotoCol                               *
*   Moves the current column to the specified postion                  *
*   OnCanMove is called to see if the process is allowed               *
*                                                                      *
*  returns                                                             *
*    1           out of range                                          *
*    2           OnCanMove did not allow the procedure                 *
*    UG_SUCCESS  success                                               *
***********************************************************************/
int CUGCtrl::GotoCol(int col)
{

if ( m_GI->m_moveType == 0)
    {
    /*
    --------
    Keyboard
    -------- */
    if ( GetKeyState(VK_SHIFT) >= 0 && GetKeyState(VK_CONTROL) >= 0 )
        m_GI->m_multiSelect->GetOrigCell( &m_GI->m_currentCol, &m_GI->m_currentRow );
    }

if ( col == m_GI->m_dragCol )
    return UG_SUCCESS;

if ( VerifyCurrentCol(&col) != UG_SUCCESS )
    return 1;

if ( col == m_GI->m_dragCol )
    return UG_SUCCESS;

if ( OnCanMove(m_GI->m_dragCol, m_GI->m_dragRow,col, m_GI->m_dragRow) == FALSE )
    return 2;

m_GI->m_dragCol = col;

AdjustLeftCol();

Moved();

return UG_SUCCESS;
}

/***********************************************************************
*                               SetLeftCol                             *
*   Moves the left column to the specified postion                     *
*   OnCanViewMove is called to see if the process is allowed           *
* returns                                                              *
*   1           out of range                                           *
*   2           OnCanMove did not allow the procedure                  *
*   UG_SUCCESS  success                                                *
***********************************************************************/
int CUGCtrl::SetLeftCol( int col )
{

if ( col == m_GI->m_leftCol )
    return UG_SUCCESS;

if ( VerifyLeftCol(&col) != UG_SUCCESS )
    return 1;

if ( !OnCanViewMove(m_GI->m_leftCol, m_GI->m_topRow, col, m_GI->m_topRow) )
    return 2;

if ( m_GI->m_leftCol == col )
    return UG_SUCCESS;


m_GI->m_leftCol = col;
Moved();

return UG_SUCCESS;
}

/***********************************************************************
*                              GotoCell                                *
*   Moves the current row/column to the specified                      *
*   position                                                           *
*   OnCanMove is called to see if the process is allowed               *
*  returns                                                             *
*    1           out of range                                          *
*    2           OnCanMove did not allow the procedure                 *
*    UG_SUCCESS  success                                               *
***********************************************************************/
int CUGCtrl::GotoCell( int col, long row )
{

if ( col == m_GI->m_dragCol && row == m_GI->m_dragRow )
    return UG_SUCCESS;

if ( m_GI->m_enableJoins )
    GetJoinStartCell( &col, &row );

if ( !OnCanMove(m_GI->m_leftCol,m_GI->m_topRow,col,row) )
    return 2;

m_GI->m_dragCol = col;
m_GI->m_dragRow = row;

AdjustLeftCol();
AdjustTopRow();

Moved();

return UG_SUCCESS;
}

/***********************************************************************
*                         GetCellFromPoint                             *
*   Returns the column and row that lies over the given x,y            *
*   co-ordinates. The co-ordinates are relative to the top left        *
*   hand corner of the grid                                            *
*                                                                      *
* Return                                                               *
*   1           co-ords do not fall on a cell                          *
*   UG_SUCCESS  success                                                *
***********************************************************************/
int CUGCtrl::GetCellFromPoint(int x,int y,int *ptcol,long *ptrow)
{
RECT rect;
return GetCellFromPoint(x,y,ptcol,ptrow,&rect);
}

/***********************************************************************
*                          GetCellFromPoint                            *
*   Returns the column and row that lies over the given x,y            *
*   co-ordinates. The co-ordinates are relative to the top left        *
*   hand corner of the grid area (not including the top headings);     *
*                                                                      *
*   This function also retuns the rectangle of the cell that was       *
*   found at that location                                             *
*                                                                      *
* Return                                                               *
*   1           co-ords do not fall on a cell                          *
*   UG_SUCCESS  success                                                *
***********************************************************************/
int CUGCtrl::GetCellFromPoint(int px,int py,int *ptcol,long *ptrow,RECT *rect)
{
int  height;
int  ptsFound;
int  width;
int  x;
long y;

ptsFound = 0;

if ( px < 0 || px > m_GI->m_gridWidth )
    return 1;
if ( py < 0 || py > m_GI->m_gridHeight )
    return 1;

width   = 0;
height  = 0;

/*
------------
Find the col
------------ */
for ( x=0; x<m_GI->m_numberCols; x++ )
{

if (x == m_GI->m_numLockCols )
    x = m_GI->m_leftCol;

width += GetColWidth(x);

if ( width > px )
    {
    *ptcol = x;
    rect->right = width;
    rect->left = rect->right - GetColWidth(x);
    ptsFound ++;
    break;
    }
}

/*
------------
Find the row
------------ */
for ( y=0; y<m_GI->m_numberRows; y++ )
    {

    if ( y == m_GI->m_numLockRows )
        y = m_GI->m_topRow;

    height += GetRowHeight(y);

    if ( height > py)
        {
        *ptrow = y;
        rect->bottom = height;
        rect->top = rect->bottom - GetRowHeight(y);
        ptsFound ++;
        break;
        }
    }

/*
---------------------------------------
If the cell was not found then return 1
--------------------------------------- */
if ( ptsFound != 2 )
    return 1;

/*
------------------------------------------------------
If cell joining then check to see if the see is joined
------------------------------------------------------ */
if ( m_GI->m_enableJoins )
    {
    GetCell(x,y,&m_cell);
    if ( m_cell.IsPropertySet(UGCELL_JOIN_SET) )
        GetCellRect(ptcol,ptrow,rect);
    }
return UG_SUCCESS;
}

/***********************************************************************
*                    GetAbsoluteCellFromPoint                          *
*             Does not take cell joining into account                  *
***********************************************************************/
int CUGCtrl::GetAbsoluteCellFromPoint( int x, int y, int *ptcol, long *ptrow )
{
int  rt;
int  tempJoinFlag;
RECT rect;

tempJoinFlag        = m_GI->m_enableJoins;
rt                  = GetCellFromPoint(x,y,ptcol,ptrow,&rect);
m_GI->m_enableJoins = tempJoinFlag;
return rt;
}

/***********************************************************************
*                             GetCellRect                              *
*   Returns the rectangle of the given cell. Relative to the           *
*   top-left corner of the grid area (not including the top            *
*   headings);                                                         *
* Return                                                               *
*   1           co-ords out of range                                   *
*   UG_SUCCESS  success                                                *
***********************************************************************/
int CUGCtrl::GetCellRect( int col, long row, RECT * rect )
{
return GetCellRect(&col,&row,rect);
}

/***********************************************************************
                               GetCellRect
*                                                                      *
*   Purpose                                                            *
*       Returns the rectangle of the given cell relative               *
*       to the top-left corner of the grid area (not                   *
*       including the top/side headings). This function                *
*       takes cell joining and locked regions into account             *
*   Params                                                             *
*       col - (input) column to return the rect of                     *
*             (output) the start cell of a joined cell                 *
*             region if the specified col/row points to                *
*             a cell within the region                                 *
*       row - (input) row to return the rect of                        *
*             (output) the start cell of a joined cell                 *
*             region if the specified col/row points to                *
*             a cell witin the region                                  *
*       rect -  (output) returns the rectange of the specified         *
*               cell in pixels. Joins and locked regions are           *
*               taken into account                                     *
*   Return                                                             *
*       UG_SUCCESS  success                                            *
*       1           cell not visible                                   *
***********************************************************************/
int CUGCtrl::GetCellRect( int * col, long * row, RECT * rect )
{
int  x;
long y;
int  width;
int  height;
int  startCol;
int  endCol;
long startRow;
long endRow;

width    = 0;
height   = 0;
startCol = *col;
endCol   = *col;
startRow = *row;
endRow   = *row;

rect->left  = 0;
rect->top   = 0;
rect->right = 0;
rect->bottom    = 0;

/*
-----------------------------------------------------------------
If the specified cell is within a join then find the joined range
----------------------------------------------------------------- */
if ( m_GI->m_enableJoins )
    {
    if ( GetJoinRange(&startCol,&startRow,&endCol,&endRow) == UG_SUCCESS )
        {
        *col = startCol;
        *row = startRow;
        }
    }

/*
---------------
Find the column
--------------- */
if ( startCol >= m_GI->m_numLockCols )
    {
    /*
    -------------------------------------
    The col is not within the lock region
    ------------------------------------- */
    rect->left  = m_GI->m_lockColWidth;
    rect->right = m_GI->m_lockColWidth;
    }

for ( x=0; x<m_GI->m_numberCols; x++ )
    {
    if ( x == m_GI->m_numLockCols )
        x = m_GI->m_leftCol;

    if ( x == startCol )
        rect->left = width;

    width += GetColWidth(x);

    if ( x == endCol)
        {
        rect->right = width;
        break;
        }
    }

/*
------------
Find the row
------------ */
if ( startRow >= m_GI->m_numLockRows )
    {
    /*
    -------------------------------------
    The row is not within the lock region
    ------------------------------------- */
    rect->top       = m_GI->m_lockRowHeight;
    rect->bottom    = m_GI->m_lockRowHeight;
    }

for ( y=0; y<m_GI->m_numberRows; y++ )
    {
    if (y == m_GI->m_numLockRows)
        y = m_GI->m_topRow;

    if (y == startRow)
        rect->top = height;

    height += GetRowHeight(y);

    if ( y == endRow )
        {
        rect->bottom = height;
        break;
        }
    }

return UG_SUCCESS;
}

/***********************************************************************
*                            GetRangeRect                              *
*            returns the rectangle for a range of cells                *
***********************************************************************/
int CUGCtrl::GetRangeRect( int startCol, long startRow, int endCol, long endRow, RECT * rect )
{
int  x;
long y;
int  width;
int  height;

width  = 0;
height = 0;

rect->left      = - 10;
rect->top       = - 10;
rect->right     = m_GI->m_gridWidth + 10;
rect->bottom    = m_GI->m_gridHeight + 10;

if ( m_GI->m_leftCol > endCol )
    {
    rect->right = -10;
    return UG_SUCCESS;
    }

if (m_GI->m_topRow > endRow)
    {
    rect->bottom = -10;
    return UG_SUCCESS;
    }

/*
---------------
Find the column
--------------- */
if ( startCol >= m_GI->m_numLockCols && m_GI->m_numLockCols >0 )
    {
    /*
    -------------------------------------
    The col is not within the lock region
    ------------------------------------- */
    rect->left = m_GI->m_lockColWidth;
    }

for ( x=0; x<m_GI->m_numberCols; x++ )
    {
    if ( x == m_GI->m_numLockCols )
        x = m_GI->m_leftCol;

    if ( x == startCol )
        rect->left = width;

    width += GetColWidth(x);

    if ( x == endCol )
        {
        rect->right = width;
        break;
        }

    if ( width > m_GI->m_gridWidth )
        break;
    }

/*
------------
Find the row
------------ */
if ( startRow >= m_GI->m_numLockRows && m_GI->m_numLockRows >0 )
    {
    /*
    -------------------------------------
    The row is not within the lock region
    ------------------------------------- */
    rect->top = m_GI->m_lockRowHeight;
    }

for ( y=0; y<m_GI->m_numberRows; y++ )
    {
    if ( y == m_GI->m_numLockRows )
        y = m_GI->m_topRow;

    if ( y == startRow )
        rect->top = height;

    height += GetRowHeight(y);

    if ( y == endRow )
        {
        rect->bottom = height;
        break;
        }

    if (height > m_GI->m_gridHeight)
        break;
    }

return UG_SUCCESS;
}
/***********************************************************************
*                         GetJoinStartCell                             *
*   returns UG_SUCCESS if the specified cell is part of a block,       *
*   also returns the col and row of the starting cell of that block    *
***********************************************************************/
int CUGCtrl::GetJoinStartCell( int * col, long * row )
{
return GetJoinStartCell(col,row,&m_cell);
}

/***********************************************************************
*                         GetJoinStartCell                             *
*       returns UG_SUCCESS if the specified cell is part of            *
*       a block, also returns the col and row of the starting          *
*       cell of that block                                             *
***********************************************************************/
int CUGCtrl::GetJoinStartCell( int * col, long * row, CUGCell * cell )
{
int  startCol;
long startRow;
BOOL origin;

if ( m_GI->m_enableJoins == FALSE )
    return 1;

GetCellIndirect(*col,*row,cell);
if ( cell->IsPropertySet(UGCELL_JOIN_SET) == FALSE )
    return 2;


cell->GetJoinInfo(&origin,&startCol,&startRow);

if ( !origin )
    {
    *col += startCol;
    *row += startRow;
    GetCellIndirect(*col,*row,cell);
    }

return UG_SUCCESS;
}


/***********************************************************************
*                         GetJoinRange                                 *
*      returns UG_SUCCESS if the specified cell is part of             *
*      a block, also returns the col and row of the starting           *
*      and ending cells of that block                                  *
*      col and row point to a cell within the join on input            *
*      and contain the start cell on output if successful              *
***********************************************************************/
int CUGCtrl::GetJoinRange( int *col, long * row, int * col2, long * row2 )
{
int  startCol;
long startRow;
BOOL origin;

if ( m_GI->m_enableJoins == FALSE )
    return 1;

GetCell(*col,*row,&m_cell);
if ( m_cell.IsPropertySet(UGCELL_JOIN_SET) == FALSE )
    return 2;

m_cell.GetJoinInfo(&origin,&startCol,&startRow);

if ( !origin )
    {
    *col += startCol;
    *row += startRow;
    GetCell(*col,*row,&m_cell);
    }

m_cell.GetJoinInfo(&origin,col2,row2);
*col2+= *col;
*row2+= *row;

return UG_SUCCESS;
}

/***********************************************************************
*                           EnableJoins                                *
***********************************************************************/
int CUGCtrl::EnableJoins( BOOL state )
{
if (state)
    m_GI->m_enableJoins = TRUE;
else
    m_GI->m_enableJoins = FALSE;

return UG_SUCCESS;
}

/***********************************************************************
*                          EnableCellOverLap                           *
***********************************************************************/
int CUGCtrl::EnableCellOverLap( BOOL state )
{
if ( state )
    m_GI->m_enableCellOverLap = TRUE;
else
    m_GI->m_enableCellOverLap = FALSE;

return UG_SUCCESS;
}

/***********************************************************************
                           EnableColSwapping
***********************************************************************/
int CUGCtrl::EnableColSwapping( BOOL state )
{
if ( state )
    m_GI->m_enableColSwapping = TRUE;
else
    m_GI->m_enableColSwapping = FALSE;

return UG_SUCCESS;
}

/***********************************************************************
*                             StartEdit                                *
*                                                                      *
*   Starts editing mode - if the current cell is not readonly          *
*   and the cell type of the current cell allows editing               *
*                                                                      *
*   An OnStartEdit notification is also sent that can allow or         *
*   dis-allow the editing                                              *
*                                                                      *
* return                                                               *
*   1           editing not allowed                                    *
*   UG_SUCCESS  success                                                *
***********************************************************************/
int CUGCtrl::StartEdit()
{
return StartEdit( m_GI->m_currentCol, m_GI->m_currentRow, 0 );
}

/***********************************************************************
*                         StartEdit                                    *
*                                                                      *
*   Starts editing mode - if the current cell is not readonly          *
*   and the cell type of the current cell allows editing               *
*                                                                      *
*   An OnStartEdit notification is also sent that can allow or         *
*   dis-allow the editing 'key' allows to have a keystroke             *
*   stuffed into the edit control when is starts, useful for           *
*   when editing starts because of a OnCharDown notification           *
*                                                                      *
* return                                                               *
*   1           editing not allowed                                    *
*   UG_SUCCESS  success                                                *
***********************************************************************/
int CUGCtrl::StartEdit( int key )
{
return StartEdit(m_GI->m_currentCol,m_GI->m_currentRow,key);
}

/***********************************************************************
*                             StartEdit                                *
*                                                                      *
*   Starts editing mode - if the specified cell is not readonly        *
*   and the cell type of the specified cell allows editing             *
*                                                                      *
*   An OnStartEdit notification is also sent that can allow or         *
*   dis-allow the editing 'key' allows to have a keystroke             *
*   stuffed into the edit control when is starts, useful for           *
*   when editing starts because of a OnCharDown notification           *
*   'col','row' specifies the cell to edit                             *
*                                                                      *
* return                                                               *
*   1           editing not allowed                                    *
*   2           could not move to cell                                 *
*   UG_SUCCESS  success                                                *
***********************************************************************/
int CUGCtrl::StartEdit( int col, long row, int key )
{
int     startCol;
int     endCol;
long    startRow;
long    endRow;
RECT    rect;
CUGCell cell;
WPARAM  myfont;

m_editInProgress = TRUE;

/*
---------------------------
Clear any multiple sections
--------------------------- */
m_GI->m_multiSelect->GetTotalRange( &startCol, &startRow, &endCol, &endRow );
ClearSelections();
if ( startCol!=endCol || startCol!=col || startRow!=endRow || startRow!=row )
    RedrawRange(startCol,startRow,endCol,endRow);

if ( col != m_GI->m_currentCol || row != m_GI->m_currentRow )
    {
    if ( GotoCell(col,row) != UG_SUCCESS )
        {
        m_editInProgress = FALSE;
        return 2;
        }
    }

m_editCol = col;
m_editRow = row;
m_editCell.ClearAll();
GetCellIndirect(m_editCol,m_editRow,&m_editCell);

/*
------------------------------------------
Check to see if this cell type is editable
------------------------------------------ */
CUGCellType * ct;
if ( m_editCell.IsPropertySet(UGCELL_CELLTYPE_SET) )
    ct = GetCellType(m_editCell.GetCellType());
else
    ct = GetCellType(0);
if ( ct->CanTextEdit() == FALSE)
    {
    m_editInProgress = FALSE;
    return 1;
    }


if ( col >= 0 && row >= 0)
    {
    /*
    ------------
    Grid editing
    ------------ */
    GetCellRect(col,row,&rect);
    m_editParent = m_CUGGrid->m_gridWnd;
    }
if ( col < 0 && row >= 0 )
    {
    //side heading editing
    m_CUGSideHdg->GetCellRect(col,row,&rect);
    m_editParent = m_CUGSideHdg->m_sideHdgWnd;
    }
if ( col >= 0 && row < 0 )
    {
    //top heading editing
    m_CUGTopHdg->GetCellRect(col,row,&rect);
    m_editParent = m_CUGTopHdg->m_topHdgWnd;
    }

rect.right--;
rect.bottom--;

/*
-----------------
Get the edit area
----------------- */
ct->GetEditArea(&rect);

if ( m_editCell.GetReadOnly())
    {
    m_editInProgress = FALSE;
    return 1;
    }

SetWindowText(m_GI->m_editCtrl, m_editCell.GetText());

/*
--------------------------------------------------------
Set the pointer to the current edit control
only use this var from now on as it may point to another
control after OnEditStart is called
-------------------------------------------------------- */
m_editCtrl = m_GI->m_editCtrl;

if ( OnEditStart(m_editCol,m_editRow,&m_editCtrl) != FALSE )
    {
    m_editInProgress = TRUE;

    SetWindowText(m_editCtrl, m_editCell.GetText());

    if ( m_editCell.IsPropertySet(UGCELL_FONT_SET) )
        myfont = WPARAM( m_editCell.GetFont() );
    else if ( m_GI->m_defFont )
        myfont = WPARAM( m_GI->m_defFont );
    else
        myfont = NULL;

    if ( myfont != NULL )
        SendMessage( m_editCtrl, WM_SETFONT, myfont, MAKELPARAM(TRUE, 0) );
    SetParent( m_editCtrl, m_editParent);
    SetWindowPos( m_editCtrl, HWND_TOP, rect.left,rect.top, rect.right - rect.left, rect.bottom - rect.top,0);
    ShowWindow( m_editCtrl, SW_SHOW );
    SetActiveWindow( m_editCtrl );
    SetFocus( m_editCtrl );

    /*
    ---------------------
    Stuff a key stroke in
    --------------------- */
    if ( key > 0 )
        SendMessage( m_editCtrl, WM_CHAR, key, 0 );

    return UG_SUCCESS;
    }

m_editInProgress = FALSE;
return 1;
}

/***********************************************************************
*                          ContinueEdit                                *
*                                                                      *
*   If already in edit mode this function moves the editing to         *
*   another cell using the given adjustments if the specified          *
*   cell is not readonly and the cell type of the specified cell       *
*   allows editing An OnStartEdit notification is also sent that       *
*   can allow or dis-allow the editing                                 *
*                                                                      *
* return                                                               *
*   1           editing not allowed                                    *
*   UG_SUCCESS  success                                                *
***********************************************************************/
int CUGCtrl::ContinueEdit( int adjustCol, long adjustRow )
{
return UG_SUCCESS;
}

/***********************************************************************
*                            GetCell                                   *
*   Fills the given cell class with the information from the           *
*   cell at the given position                                         *
*                                                                      *
* return                                                               *
*       UG_SUCCESS  success                                            *
*       1           cell not found                                     *
***********************************************************************/
int CUGCtrl::GetCell( int col, long row, CUGCell * cell )
{

if ( col >= m_GI->m_numberCols )
    return UG_ERROR;

CUGDataSource * ds = NULL;

cell->SetPropertyFlags(0);

/*
-------------------------------------------------
Column translation only is performed on cols >= 0
------------------------------------------------- */
if ( col >=0 && col < m_GI->m_numberCols )
    ds = m_GI->m_colInfo[col].dataSource;

if ( ds == NULL )
    ds = m_GI->m_defDataSource;

/*
--------------------------------
Get the cell from the datasource
-------------------------------- */
if ( col >=0 && col < m_GI->m_numberCols )
    return ds->GetCell( m_GI->m_colInfo[col].colTranslation, row, cell );
else
    return ds->GetCell( col, row, cell );

}

/***********************************************************************
*                          GetCellIndirect                             *
*   Fills the given cell class with the information from the           *
*   cell at the given position, this function starts by filling        *
*   the cell with the default informaiton for the specified            *
*   column, then it over-writes any information that is 'set' in       *
*   the specified cell.                                                *
*                                                                      *
* return                                                               *
*       UG_SUCCESS  success                                            *
*       1           cell not found                                     *
***********************************************************************/
int CUGCtrl::GetCellIndirect(int col,long row,CUGCell *cell)
{

cell->SetPropertyFlags(0);

/*
----------------------
Get the column default
---------------------- */
if ( col >= 0 && row >= 0 && col < m_GI->m_numberCols )
    {
    if ( m_GI->m_colInfo[col].colDefault )
        cell->CopyInfoFrom(m_GI->m_colInfo[col].colDefault);
    }
else
    {
    cell->CopyInfoFrom(m_GI->m_hdgDefaults);
    }

/*
-------------------
Find the datasource
------------------- */
CUGDataSource * ds = NULL;
if (col >=0 && col < m_GI->m_numberCols)
    ds = m_GI->m_colInfo[col].dataSource;
if (ds == NULL)
    ds = m_GI->m_defDataSource;

/*
--------------------------------
Get the cell from the datasource
-------------------------------- */
if ( col >=0 && col < m_GI->m_numberCols )
    {
    ds->GetCell(m_GI->m_colInfo[col].colTranslation,row,cell);
    OnGetCell(m_GI->m_colInfo[col].colTranslation,row,cell);
    }
else{
    ds->GetCell(col,row,cell);
    OnGetCell(col,row,cell);
    }

/*
----------------
Use format class
---------------- */
if ( cell->IsPropertySet(UGCELL_FORMAT_SET) )
    cell->GetFormatClass()->ApplyDisplayFormat(cell);

/*
---------------------
Use cell style object
--------------------- */
if ( cell->IsPropertySet(UGCELL_STYLE_SET) )
    cell->GetCellStyle()->AddInfoTo(cell);

return UG_SUCCESS;
}

/***********************************************************************
*                               SetCell                                *
*   Copies the given cell to the cell at the specified position.       *
*   The information that is saved depends on the capabilities of       *
*   the data source. (ex. the memory manager saves all the             *
*   information, a basic database source will only save the            *
*   text).                                                             *
*                                                                      *
* return                                                               *
*   UG_SUCCESS  success                                                *
*   1           cell not found                                         *
***********************************************************************/
int CUGCtrl::SetCell( int col, long row, CUGCell * cell )
{

OnSetCell(col,row,cell);

/*
-------------------
Find the datasource
------------------- */
CUGDataSource * ds = NULL;
if (col >=0)
    ds = m_GI->m_colInfo[col].dataSource;
if (ds == NULL)
    ds = m_GI->m_defDataSource;

/*
------------------------------
set the cell in the datasource
------------------------------ */
if ( col >= 0 )
    ds->SetCell(m_GI->m_colInfo[col].colTranslation,row,cell);
else
    ds->SetCell(col,row,cell);

return UG_SUCCESS;
}

/***********************************************************************
*                          GETCOLTRANSLATION                           *
***********************************************************************/
int CUGCtrl::GetColTranslation(int col)
{

if ( col <0 || col >= m_GI->m_numberCols )
    return col;

return m_GI->m_colInfo[col].colTranslation;
}

/***********************************************************************
*                        SetColTranslation                             *
***********************************************************************/
int CUGCtrl::SetColTranslation(int col,int transCol)
{
if ( col <0 || col >= m_GI->m_numberCols )
    return 1;

m_GI->m_colInfo[col].colTranslation = transCol;

return UG_SUCCESS;
}

/***********************************************************************
*                            DeleteCell                                *
*   UG_SUCCESS  success                                                *
*   1           cell no found                                          *
***********************************************************************/
int CUGCtrl::DeleteCell(int col,long row)
{
CUGDataSource * ds;

/*
-------------------
Find the datasource
------------------- */
ds = 0;
if ( col >=0 )
    ds = m_GI->m_colInfo[col].dataSource;

if ( !ds )
    ds = m_GI->m_defDataSource;

return ds->DeleteCell(m_GI->m_colInfo[col].colTranslation,row);

}

/***********************************************************************
*                          SetColDefault                               *
*   Sets the column default values. These values are used when         *
*   the grid draws a cell, the defaults are retrieved first then       *
*   only the informtion that the cell being drawn contains is          *
*   copied overtop of the defauls                                      *
***********************************************************************/
int CUGCtrl::SetColDefault( int col, CUGCell * cell )
{
if ( col <0 || col >= m_GI->m_numberCols )
    return UG_ERROR;

cell->CopyInfoTo( m_GI->m_colInfo[col].colDefault );

return UG_SUCCESS;
}

/***********************************************************************
*                            SetGridDefault                            *
*   Sets the grid default values. These values are used when new       *
*   columns are made (SetNumberCols) they are copied to the new        *
*   column defaults (see above)                                        *
***********************************************************************/
int CUGCtrl::SetGridDefault( CUGCell *cell )
{
cell->CopyInfoTo(m_GI->m_gridDefaults);

return UG_SUCCESS;
}

/***********************************************************************
*                            GetColDefault                             *
*   copies the given column defaults into the supplied cell            *
***********************************************************************/
int CUGCtrl::GetColDefault( int col, CUGCell * cell )
{
if ( col <0 || col >= m_GI->m_numberCols )
    return UG_ERROR;

cell->CopyInfoFrom(m_GI->m_colInfo[col].colDefault);

return UG_SUCCESS;
}

/***********************************************************************
*                           GetGridDefault                             *
*   copies the grid defaults into the supplied cell                    *
***********************************************************************/
int CUGCtrl::GetGridDefault( CUGCell * cell )
{
cell->CopyInfoFrom(m_GI->m_gridDefaults);
return UG_SUCCESS;
}

/***********************************************************************
*                          SetHeadingDefault                           *
*   copies the given cell properties to the heading defaults.          *
*   Heading defaults are used when a top and side headings are         *
*   being drawn ( as well as the top corner button). These             *
*   values are applied first, then any properties set for the          *
*   heading cell being drawn are applied overtop                       *
***********************************************************************/
int CUGCtrl::SetHeadingDefault( CUGCell * cell )
{
m_GI->m_hdgDefaults->CopyInfoFrom(cell);
return UG_SUCCESS;
}

/***********************************************************************
*                          GetHeadingDefault                           *
*   Fills the given cell with the heading default properties           *
***********************************************************************/
int CUGCtrl::GetHeadingDefault(CUGCell *cell)
{
cell->CopyInfoFrom(m_GI->m_hdgDefaults);
return UG_SUCCESS;
}

/***********************************************************************
*                              JoinCells                               *
***********************************************************************/
int CUGCtrl::JoinCells( int startCol, long startRow, int endCol, long endRow )
{
int     x;
long    y;
CUGCell cell;

if ( startCol > endCol )
    return 1;

if ( startRow > endRow )
    return 1;

if ( endCol >= m_GI->m_numberCols )
    return 1;

if ( endRow >= m_GI->m_numberRows )
    return 1;

if ( startCol < (m_GI->m_numberSideHdgCols *-1) )
    return 1;

if ( startRow < (m_GI->m_numberTopHdgRows *-1) )
    return 1;


for ( y=startRow; y<=endRow; y++ )
    {
    for ( x = startCol; x<=endCol; x++ )
        {
        GetCell(x,y,&cell);
        cell.SetJoinInfo(FALSE,startCol-x,startRow-y);
        SetCell(x,y,&cell);
        }
    }

GetCell(startCol,startRow,&cell);
cell.SetJoinInfo(TRUE,endCol - startCol,endRow - startRow);
SetCell(startCol,startRow,&cell);

return UG_SUCCESS;
}

/***********************************************************************
*                            UnJoinCells                               *
***********************************************************************/
int CUGCtrl::UnJoinCells( int col, long row )
{
long    y;
int     x;
int     endCol;
long    endRow;
CUGCell cell;

if ( GetJoinRange(&col,&row,&endCol,&endRow) == UG_SUCCESS )
    {
    for ( y=row; y<=endRow; y++)
        {
        for ( x=col; x<=endCol; x++ )
            {
            GetCell(x,y,&cell);
            cell.ClearProperty(UGCELL_JOIN_SET);
            SetCell(x,y,&cell);
            }
        }
    }
return UG_SUCCESS;
}

/***********************************************************************
*                           QuickGetText                               *
***********************************************************************/
int CUGCtrl::QuickGetText(int col,long row,LPTSTR *string,int len)
{
GetCell(col,row,&m_cell);
CopyString (*string, m_cell.GetText(), len);

return UG_SUCCESS;
}

/***********************************************************************
*                           QuickGetText                               *
***********************************************************************/
LPCTSTR CUGCtrl::QuickGetText( int col, long row )
{
GetCell(col,row,&m_cell);
return m_cell.GetText();
}

/***********************************************************************
*                           QuickSetText                               *
***********************************************************************/
int CUGCtrl::QuickSetText( int col, long row, const TCHAR * string )
{
GetCell(col,row,&m_cell);
m_cell.SetText( string );
SetCell(col,row,&m_cell);
return UG_SUCCESS;
}

/***********************************************************************
*                           QuickSetMask                               *
***********************************************************************/
int CUGCtrl::QuickSetMask( int col, long row, const TCHAR * string )
{
GetCell( col, row,&m_cell );
m_cell.SetMask( string );
SetCell( col, row, &m_cell );
return UG_SUCCESS;
}

/***********************************************************************
*                         QuickSetLabelText                            *
***********************************************************************/
int CUGCtrl::QuickSetLabelText( int col, long row, const TCHAR * string )
{
GetCell(col,row,&m_cell);
m_cell.SetLabelText(string);
SetCell(col,row,&m_cell);
return UG_SUCCESS;
}

/***********************************************************************
*                         QuickSetTextColor                            *
***********************************************************************/
int CUGCtrl::QuickSetTextColor( int col, long row, COLORREF color )
{
GetCell(col,row,&m_cell);
m_cell.SetTextColor(color);
SetCell(col,row,&m_cell);
return UG_SUCCESS;
}

/***********************************************************************
*                         QuickSetHTextColor                           *
***********************************************************************/
int CUGCtrl::QuickSetHTextColor( int col, long row, COLORREF color )
{
GetCell(col,row,&m_cell);
m_cell.SetHTextColor(color);
SetCell(col,row,&m_cell);
return UG_SUCCESS;
}

/***********************************************************************
*                         QuickSetBackColor                            *
***********************************************************************/
int CUGCtrl::QuickSetBackColor( int col, long row, COLORREF color )
{
GetCell(col,row,&m_cell);
m_cell.SetBackColor(color);
SetCell(col,row,&m_cell);

return UG_SUCCESS;
}

/***********************************************************************
*                         QuickSetHBackColor                           *
***********************************************************************/
int CUGCtrl::QuickSetHBackColor( int col, long row, COLORREF color )
{
GetCell( col, row, &m_cell );
m_cell.SetHBackColor(color);
SetCell(col,row,&m_cell);

return UG_SUCCESS;
}

/***********************************************************************
*                         QuickSetBitmap                               *
***********************************************************************/
int CUGCtrl::QuickSetBitmap(int col,long row, HBITMAP bitmap)
{
GetCell( col, row, &m_cell );
m_cell.SetBitmap( bitmap );
SetCell( col, row, &m_cell );
return UG_SUCCESS;
}

/***********************************************************************
*                         QuickSetBitmap                               *
***********************************************************************/
int CUGCtrl::QuickSetBitmap( int col, long row, int index )
{
return QuickSetBitmap(col,row,GetBitmap(index));
}

/***********************************************************************
*                         QuickSetFont                                 *
***********************************************************************/
int CUGCtrl::QuickSetFont( int col, long row, HFONT font )
{

GetCell(col,row,&m_cell);
m_cell.SetFont(font);
SetCell(col,row,&m_cell);
return UG_SUCCESS;
}

/***********************************************************************
*                           QuickSetFont                               *
***********************************************************************/
int CUGCtrl::QuickSetFont( int col, long row, int index )
{
return QuickSetFont( col, row, GetFont(index) );
}

/***********************************************************************
*                          QuickSetAlignment                           *
***********************************************************************/
int CUGCtrl::QuickSetAlignment(int col,long row,int align)
{
GetCell(col,row,&m_cell);
m_cell.SetAlignment(align);
SetCell(col,row,&m_cell);
return UG_SUCCESS;
}

/***********************************************************************
*                           QuickSetBorder                             *
***********************************************************************/
int CUGCtrl::QuickSetBorder( int col, long row, int border )
{
GetCell(col,row,&m_cell);
m_cell.SetBorder(border);
SetCell(col,row,&m_cell);
return UG_SUCCESS;
}

/***********************************************************************
*                         QuickSetBorderColor                          *
***********************************************************************/
int CUGCtrl::QuickSetBorderColor( int col, long row, HPEN pen )
{
GetCell( col, row, &m_cell );
m_cell.SetBorderColor( pen );
SetCell( col, row, &m_cell );
return UG_SUCCESS;
}

/***********************************************************************
*                         QuickSetCellType                             *
***********************************************************************/
int CUGCtrl::QuickSetCellType( int col, long row, long type )
{
GetCell(col,row,&m_cell);
m_cell.SetCellType(type);
SetCell(col,row,&m_cell);
return UG_SUCCESS;
}

/***********************************************************************
*                         QuickSetCellTypeEx                           *
***********************************************************************/
int CUGCtrl::QuickSetCellTypeEx( int col, long row, long typeEx )
{
GetCell(col,row,&m_cell);
m_cell.SetCellTypeEx(typeEx);
SetCell(col,row,&m_cell);
return UG_SUCCESS;
}

/***********************************************************************
*                              QuickSetRange                           *
*   Sets a range of cells to use the same attributes as the            *
*   given cell                                                         *
*                                                                      *
*   Return                                                             *
*   UG_SUCCESS - success                                               *
*   otherwise the number of SetCell errors are returned                *
*   this may be caused if part or all of the range is                  *
*   outside of the grids total column/row range                        *
***********************************************************************/
int CUGCtrl::QuickSetRange( int startCol, long startRow, int endCol, long endRow, CUGCell * cell )
{
int  col;
int  error;
long row;

error = UG_SUCCESS;

for ( row=startRow; row<=endRow; row++ )
    {
    for ( col=startCol; col<=endCol; col++ )
        {
        if ( SetCell(col,row,cell) != UG_SUCCESS )
            error++;
        }
    }

return error;
}


/***********************************************************************
*                             SetNumberRow                             *
*   Sets the number of rows that are displayed in the grid. Up         *
*   to 64,000 rows are allowed with non-uniform row height. Up         *
*   to 2 billion rows are allowed with uniform row height. If          *
*   non-uniform row height is set and the number of row exced          *
*   65,000 then the grid automatically goes to uniform row height      *
*                                                                      *
***********************************************************************/
int CUGCtrl::SetNumberRows( long rows, BOOL redraw )
{
long  i;
long  oldNumRows;
int * tempRowHeights;

/*
--------------
Range checking
-------------- */
if (rows <0)
    return 1;

/*
----------------------------
Store the old number of rows
---------------------------- */
oldNumRows = m_GI->m_numberRows;

/*
-------------------------------------------------
Check to see if uniform row height should be used
------------------------------------------------- */
if ( rows > 64000 && m_GI->m_uniformRowHeightFlag == FALSE )
    SetUniformRowHeight(TRUE);

/*
------------------------------------------------------------
Store the number of rows and create the new row height array
------------------------------------------------------------ */
m_GI->m_numberRows = rows;
if ( m_GI->m_uniformRowHeightFlag == FALSE )
    {
    /*
    ----------------
    Create the array
    ---------------- */
    tempRowHeights = new int[rows];

    /*
    -----------------------------
    Copy the old row heights over
    ----------------------------- */
    for ( i=0; i<rows; i++ )
        {
        if ( i < oldNumRows )
            tempRowHeights[i] = m_GI->m_rowHeights[i];
        else
            tempRowHeights[i] = m_GI->m_defRowHeight;
        }

    /*
    -------------------------------
    Delete the old row height array
    ------------------------------- */
    if ( m_GI->m_rowHeights )
        delete[] m_GI->m_rowHeights;

    /*
    -----------------
    Use the new array
    ----------------- */
    m_GI->m_rowHeights = tempRowHeights;
    }

/*
----------------------------------------------------------
Make sure that the current grid positions are within range
---------------------------------------------------------- */
CalcTopRow();
if ( m_GI->m_currentRow >= rows )
    {
    m_GI->m_currentRow =  rows -1;
    m_GI->m_dragRow = m_GI->m_currentRow;
    }

SetLockRowHeight();

if ( m_GI->m_currentRow < 0 && rows > 0)
    GotoRow(0);

if ( redraw )
    RedrawAll();

return UG_SUCCESS;
}


/***********************************************************************
*                            GetNumberRow                              *
*                      Returns the number of rows                      *
***********************************************************************/
long CUGCtrl::GetNumberRows()
{
return m_GI->m_numberRows;
}

/***********************************************************************
*                           SetNumberCols                              *
*               Sets the number of columns [0,32,000]                  *
***********************************************************************/
int CUGCtrl::SetNumberCols( int cols, BOOL redraw )
{
int i;
int oldNumCols;
UGCOLINFO * tempColWidths;

/*
--------------
Range checking
-------------- */
if ( cols<0 || cols>32000 )
    return 1;

/*
----------------------------
Store the old number of cols
---------------------------- */
oldNumCols = m_GI->m_numberCols;

/*
-----------------------------------------------------------
Store the number of cols and create the new col width array
----------------------------------------------------------- */
m_GI->m_numberCols = cols;

/*
----------------
Create the array
---------------- */
tempColWidths   = new UGCOLINFO[cols];

/*
-----------------------------
Copy the col information over
----------------------------- */
for ( i=0; i<cols; i++ )
    {
    if ( i < oldNumCols )
        {
        tempColWidths[i].width = m_GI->m_colInfo[i].width;
        tempColWidths[i].dataSource = m_GI->m_colInfo[i].dataSource;
        tempColWidths[i].colDefault = m_GI->m_colInfo[i].colDefault;
        tempColWidths[i].colTranslation = m_GI->m_colInfo[i].colTranslation;
        }
    else
        {
        tempColWidths[i].width = m_GI->m_defColWidth;
        tempColWidths[i].dataSource = m_GI->m_defDataSource;
        tempColWidths[i].colDefault = new CUGCell;
        tempColWidths[i].colDefault->CopyInfoFrom(m_GI->m_gridDefaults);
        tempColWidths[i].colTranslation = i;
        }
    }

/*
--------------------------
Delete the old information
-------------------------- */
for ( i=cols; i<oldNumCols; i++ )
    {
    delete m_GI->m_colInfo[i].colDefault;
    }

/*
------------------------------
Delete the old col width array
------------------------------ */
if ( m_GI->m_colInfo )
    delete[] m_GI->m_colInfo;

m_GI->m_colInfo = tempColWidths;

/*
----------------------------------------------------------
Make sure that the current grid positions are within range
---------------------------------------------------------- */
CalcLeftCol();
if ( m_GI->m_currentCol >= cols)
    {
    m_GI->m_currentCol =  cols -1;
    m_GI->m_dragCol    = m_GI->m_currentCol;
    }

SetLockColWidth();

if ( m_GI->m_currentCol < 0 && cols > 0 )
    GotoCol(0);

if ( redraw )
    RedrawAll();

return UG_SUCCESS;
}

/***********************************************************************
*                        GetNumberCols                                 *
*   Returns the number of columns that are currently set               *
***********************************************************************/
int CUGCtrl::GetNumberCols()
{
return m_GI->m_numberCols;
}

/***********************************************************************
*                          SetColWidth                                 *
*   Sets the width of the given column in pixels                       *
*                                                                      *
*   return                                                             *
*       UG_SUCCESS  success                                            *
*       1           specifed col is out of range or a negative width   *
***********************************************************************/
int CUGCtrl::SetColWidth( int col, int width )
{

if ( col <0 || col >= m_GI->m_numberCols )
    {
    if ( col < 0 || col >= -m_GI->m_numberSideHdgCols )
        SetSH_ColWidth(col, width);
    return 1;
    }

m_GI->m_colInfo[col].width = width;

/*
----------------------------------------------------------
Check to see if a column within the lock is being changed,
then set the lock width
---------------------------------------------------------- */
if ( col < m_GI->m_numLockCols )
    SetLockColWidth();

return UG_SUCCESS;
}

/***********************************************************************
*                            GetColWidth                               *
*            Retrieves the width of the specied column.                *
*   Return                                                             *
*       UG_SUCCESS  success                                            *
*       1           specifed col is out of range                       *
***********************************************************************/
int CUGCtrl::GetColWidth( int col, int * width )
{
if ( col <0 || col >= m_GI->m_numberCols )
    return 1;

*width =  m_GI->m_colInfo[col].width;

return UG_SUCCESS;
}

/***********************************************************************
*                            GetColWidth                               *
***********************************************************************/
int CUGCtrl::GetColWidth( int col )
{
if ( col <0 || col >= m_GI->m_numberCols )
    return 1;

return m_GI->m_colInfo[col].width;
}

/***********************************************************************
*                           SetDefColWidth                             *
*   Sets the default column width. This value is used                  *
*   when new columns are created.                                      *
***********************************************************************/
int CUGCtrl::SetDefColWidth(int width)
{
if ( width < 0 )
    return 1;

m_GI->m_defColWidth = width;

return UG_SUCCESS;
}

/***********************************************************************
*                                BestFit                               *
*     Adjusts the widths of the secified columns                       *
*     so that they are the 'best fit' for the information              *
*     being displayed                                                  *
* Params                                                               *
*     startCol -  the start column in a range of cols                  *
*                 to be 'best fitted'                                  *
*     endCol -    the end column in the range of cols                  *
*     calcRange - the number of rows to base the calculation           *
*                 on. use 0 for all rows                               *
*     flag -      options for the best fit                             *
*                 bit1: include top headings (UG_BESTFIT_TOPHEADINGS)  *
*                 bit2: use average width (UG_BESTFIT_AVERAGE)         *
***********************************************************************/
int CUGCtrl::BestFit( int startCol, int endCol, int CalcRange, int flag )
{
CUGCell cell;
CUGCellType * cellType;
SIZE  size;
int   count;
int   startRow;
int   x;
long  y;
int   numCols;
int * bestWidth;
HDC   dc;

if ( startCol < (m_GI->m_numberSideHdgCols *-1) )
    return 1;

if ( endCol >= m_GI->m_numberCols )
    return 2;

if ( startCol > endCol )
    return 3;

count     = 0;
numCols   = endCol-startCol+1;
bestWidth = new int[numCols];
dc        = GetDC(m_ctrlWnd);

startRow = 0;
if ( flag & UG_BESTFIT_TOPHEADINGS )
    startRow -= m_GI->m_numberTopHdgRows;

/*
------------------------------------------------
Set the best width to 3 pixels (3 for min width)
------------------------------------------------ */
for ( x=0; x<numCols; x++ )
    bestWidth[x] = 3;

for ( y=startRow; y<m_GI->m_numberRows; y++ )
    {
    if ( y == m_GI->m_numLockRows )
        y = m_GI->m_topRow;

    for ( x=startCol; x<=endCol; x++ )
        {

        GetCellIndirect(x,y,&cell);

        if ( cell.IsPropertySet(UGCELL_JOIN_SET) == FALSE )
            {
            cellType = GetCellType(cell.GetCellType());
            cellType->GetBestSize(dc,&size,&cell);
            if (size.cx > bestWidth[x-startCol])
                bestWidth[x-startCol] = size.cx;
            }
        }

    count++;
    if ( count == CalcRange )
        break;
    }

/*
------------------------
Update the column widths
------------------------ */
for ( x=0; x<numCols; x++ )
    SetColWidth(startCol+x,bestWidth[x]);

delete[] bestWidth;

ReleaseDC(m_ctrlWnd, dc);

AdjustComponentSizes();

return UG_SUCCESS;
}

/***********************************************************************
*                            FitToWindow                               *
***********************************************************************/
int CUGCtrl::FitToWindow( int startCol, int endCol )
{

RECT  rect;
int   columnwidths;
int   loop;
int   newwidth;
int   screenwidth;
int   width;
float ratio;

/*
--------------
Range checking
-------------- */
if ( startCol < 0 || endCol < startCol || endCol >= m_GI->m_numberCols )
    return 1;

/*
-------------------------
Get the grid screen width
------------------------- */
GetClientRect(m_CUGGrid->m_gridWnd, &rect);
screenwidth = rect.right;

/*
---------------------------
Get the total column widths
--------------------------- */
columnwidths = 0;
for ( loop=startCol; loop<=endCol; loop++ )
    {
    GetColWidth(loop,&width);
    columnwidths += width;
    }

/*
-------------
Get the ratio
------------- */
ratio = ((float)screenwidth) / ((float)columnwidths);

/*
-----------------------
Reset the column widths
----------------------- */
for ( loop=startCol; loop<endCol; loop++ )
    {
    GetColWidth(loop,&width);
    newwidth = (int)((double)width*ratio+0.5);
    SetColWidth(loop,newwidth);
    screenwidth -= newwidth;
    }
SetColWidth(endCol,screenwidth);

Update();
return UG_SUCCESS;
}

/***********************************************************************
*                         SetDefRowHeight                              *
*   Sets the default row height. This value is used for all rows       *
*   when uniform row height is set. For non-uniform row height,        *
*   this value is used when a new row is created                       *
***********************************************************************/
int CUGCtrl::SetDefRowHeight( int height )
{
if ( height <1 || height > 1024 )
    return UG_ERROR;

m_GI->m_defRowHeight = height;

return UG_SUCCESS;
}

/***********************************************************************
*                      SetUniformRowHeight                             *
*                                                                      *
*   Sets the grid in uniform row height or no-uniform row height.      *
*                                                                      *
*   'flag'  TRUE:  then uniform row height is set                      *
*              all rows then become the same height which is set       *
*              by 'SetDefRowHeight' The height can still be            *
*              adjusted by the used, but all rows are adjusted         *
*              at the same time                                        *
*                                                                      *
*   'flag' FALSE: then non-uniform row height is set                   *
*               if the grid have less than 64001 rows A maximum        *
*               of 64000 rows are allowed with this mode. This         *
*               allows each row to have its own height                 *
*               'SetRowHeight'                                         *
***********************************************************************/
int CUGCtrl::SetUniformRowHeight( int flag )
{
long row;

if ( flag )
    {
    /*
    ---------------------
    Uniform row height on
    --------------------- */
    if ( m_GI->m_rowHeights )
        {
        delete[] m_GI->m_rowHeights;
        m_GI->m_rowHeights = NULL;
        }
    m_GI->m_uniformRowHeightFlag = TRUE;
    }

else
    {
    /*
    ----------------------
    Uniform row height off
    ---------------------- */
    if ( m_GI->m_rowHeights == NULL )
        {
        if ( m_GI->m_uniformRowHeightFlag && m_GI->m_numberRows >0 )
            {
            m_GI->m_rowHeights = new int[m_GI->m_numberRows];
            for ( row=0; row<m_GI->m_numberRows; row++ )
                m_GI->m_rowHeights[row] =m_GI->m_defRowHeight;
            }
        }
    m_GI->m_uniformRowHeightFlag = FALSE;
    }

return UG_SUCCESS;
}

/***********************************************************************
*                           SetRowHeight                               *
*   Sets the height of the specified row. If uniform row height        *
*   is specied then the default row height is set                      *
***********************************************************************/
int CUGCtrl::SetRowHeight( long row, int height )
{

if ( row < 0 || row >= m_GI->m_numberRows )
    return 0;

if ( m_GI->m_uniformRowHeightFlag )
    m_GI->m_defRowHeight = height;
else
    m_GI->m_rowHeights[row] = height;

SetLockRowHeight();

return UG_SUCCESS;
}

/***********************************************************************
*                             GetRowHeight                             *
*   Retrives the height of the specified row. If uniform row           *
*   height is specified then the default row height is returned        *
***********************************************************************/
int CUGCtrl::GetRowHeight( long row, int * height )
{
if ( row < 0 || row >= m_GI->m_numberRows )
    return 1;

*height = GetRowHeight(row);

return UG_SUCCESS;
}

/***********************************************************************
*                             GetRowHeight                             *
***********************************************************************/
int CUGCtrl::GetRowHeight( long row )
{

if ( row < 0 || row >= m_GI->m_numberRows )
        return 0;

if ( m_GI->m_uniformRowHeightFlag )
    return m_GI->m_defRowHeight;

return m_GI->m_rowHeights[row];
}

/***********************************************************************
*                             InsertRow                                *
***********************************************************************/
int CUGCtrl::InsertCol( int col )
{
int  rt;
CUGDataSource * ds;

/*
-------------------
Find the datasource
------------------- */
ds = 0;
if ( col >= 0 )
    ds = m_GI->m_colInfo[col].dataSource;

if ( !ds )
    ds = m_GI->m_defDataSource;

rt = ds->InsertCol(m_GI->m_colInfo[col].colTranslation);

if ( rt == UG_SUCCESS )
    SetNumberCols(GetNumberCols()+1);

return rt;
}

/***********************************************************************
*                            AppendCol                                 *
***********************************************************************/
int CUGCtrl::AppendCol()
{
int  rt;
CUGDataSource * ds;

/*
-------------------
Find the datasource
------------------- */
ds = m_GI->m_defDataSource;

rt = ds->AppendCol();

if ( rt == UG_SUCCESS )
    SetNumberCols( GetNumberCols()+1 );

return rt;
}

/***********************************************************************
*                            DeleteCol                                 *
***********************************************************************/
int CUGCtrl::DeleteCol( int col )
{
int  rt;
CUGDataSource * ds;

if ( GetNumberCols() <= 0 )
    return UG_ERROR;

/*
-------------------
Find the datasource
------------------- */
ds = 0;
if ( col >=0 )
    ds = m_GI->m_colInfo[col].dataSource;

if ( !ds )
    ds = m_GI->m_defDataSource;

rt = ds->DeleteCol(m_GI->m_colInfo[col].colTranslation);

if ( rt == UG_SUCCESS )
    SetNumberCols(GetNumberCols()-1);

return rt;
}

/***********************************************************************
*                            InsertRow                                 *
***********************************************************************/
int CUGCtrl::InsertRow( long row )
{
int  rt;
CUGDataSource * ds;

if ( row > m_GI->m_numberRows )
    return UG_ERROR;

/*
-------------------
Find the datasource
------------------- */
ds = m_GI->m_defDataSource;

rt = ds->InsertRow(row);
if ( rt == UG_SUCCESS )
    SetNumberRows(m_GI->m_numberRows + 1);

return rt;
}

/***********************************************************************
*                            AppendRow                                 *
***********************************************************************/
int CUGCtrl::AppendRow()
{
int rt;

rt = m_GI->m_defDataSource->AppendRow();
if ( rt == UG_SUCCESS)
    SetNumberRows(m_GI->m_numberRows + 1);

return rt;
}

/***********************************************************************
*                            DeleteRow                                 *
***********************************************************************/
int CUGCtrl::DeleteRow( long row )
{
int rt;

if ( row < 0 || row >= m_GI->m_numberRows )
    return UG_ERROR;

rt = m_GI->m_defDataSource->DeleteRow(row);
if ( rt == UG_SUCCESS )
    SetNumberRows(m_GI->m_numberRows - 1);

return rt;
}

/***********************************************************************
*                             FindDialog                               *
***********************************************************************/
int CUGCtrl::FindDialog()
{
if ( m_findDialogRunning == TRUE )
    return 1;

m_findDialogRunning               = TRUE;
m_findDialogStarted               = TRUE;
m_FindReplaceMsg                  = RegisterWindowMessage(FINDMSGSTRING);
m_findReplaceDialog.lStructSize   = sizeof(m_findReplaceDialog);
m_findReplaceDialog.hwndOwner     = m_ctrlWnd;
m_findReplaceDialog.Flags         = FR_FINDNEXT | FR_DOWN;
m_findstring[0]                   = '\0';
m_findReplaceDialog.lpstrFindWhat = m_findstring;
m_findReplaceDialog.wFindWhatLen  = 79;

if ( FindText(&m_findReplaceDialog) == NULL )
    return UG_ERROR;

return UG_SUCCESS;
}

/***********************************************************************
*                           ReplaceDialog                              *
***********************************************************************/
int CUGCtrl::ReplaceDialog()
{
if ( m_findDialogRunning == TRUE)
    return 1;

m_findDialogRunning                  = TRUE;
m_findDialogStarted                  = TRUE;
m_FindReplaceMsg                     = RegisterWindowMessage(FINDMSGSTRING);
m_findReplaceDialog.lStructSize      = sizeof(m_findReplaceDialog);
m_findReplaceDialog.hwndOwner        = m_ctrlWnd;
m_findReplaceDialog.Flags            = FR_FINDNEXT | FR_DOWN;
m_findstring[0]                      = '\0';
m_findReplaceDialog.lpstrFindWhat    = m_findstring;
m_findReplaceDialog.wFindWhatLen     = 79;
m_replacestring[0]                   = '\0';
m_findReplaceDialog.lpstrReplaceWith = m_replacestring;
m_findReplaceDialog.wReplaceWithLen  = 79;

if (ReplaceText(&m_findReplaceDialog) == NULL)
    return UG_ERROR;

return UG_SUCCESS;
}

/***********************************************************************
*                           FindInAllCols                              *
***********************************************************************/
int CUGCtrl::FindInAllCols( BOOL state )
{
if ( state )
    m_findInAllCols = TRUE;
else
    m_findInAllCols = FALSE;

return UG_SUCCESS;
}

/***********************************************************************
*                         ProcessFindDialog                            *
***********************************************************************/
long CUGCtrl::ProcessFindDialog( WPARAM wParam, LPARAM lParam )
{
//if ( m_FindReplaceWnd == NULL)
    //return 0;

LPFINDREPLACE lpfr;
int     col;
long    row;
TCHAR * string;
TCHAR * string2;
int     findFlags;

lpfr = (LPFINDREPLACE) lParam;
if ( lpfr->Flags & FR_DIALOGTERM )
    {
    m_findDialogStarted = FALSE;
    m_findDialogRunning = FALSE;
    return 1;
    }

col = m_GI->m_currentCol;
row = m_GI->m_currentRow;

string  = 0;
string2 = 0;

if ( lpfr->Flags & FR_FINDNEXT )
    CopyString(string, lpfr->lpstrFindWhat);

if ( lpfr->Flags & FR_REPLACE )
    CopyString(string2, lpfr->lpstrReplaceWith);

findFlags = 0;

if ( !(lpfr->Flags & FR_MATCHCASE) )
    findFlags |= UG_FIND_CASEINSENSITIVE;

if ( !(lpfr->Flags & FR_WHOLEWORD) )
    findFlags |= UG_FIND_PARTIAL;

if ( !(lpfr->Flags & FR_DOWN) )
    findFlags |= UG_FIND_UP;

if ( m_findInAllCols )
    findFlags |= UG_FIND_ALLCOLUMNS;

m_findDialogStarted = FALSE;
if ( FindNext(string,&col,&row,findFlags) !=UG_SUCCESS )
    {
    MessageBox(m_ctrlWnd, _T("Item not found"),_T("Not Found"),MB_OK);
    if (string )
        delete [] string;
    if (string2 )
        delete [] string2;
    return 0;
    }
else
    {
    GotoCell(col,row);
    }

if ( lpfr->Flags & FR_REPLACE && string2 )
    {
    QuickSetText(m_GI->m_currentCol,row,string2);
    }

if ( lpfr->Flags & FR_REPLACEALL && string2 )
    {
    do  {
        QuickSetText(m_GI->m_currentCol,row,string2);
        }while ( FindNext(string,&col,&row,findFlags) == UG_SUCCESS );
    }

GotoCell(col,row);

if ( string )
    delete [] string;

if ( string2 )
    delete [] string2;

return 0;
}

/***********************************************************************
*                             FindFirst                                *
***********************************************************************/
int CUGCtrl::FindFirst( LPTSTR string, int * col, long * row, long flags )
{
*col = m_GI->m_colInfo[*col].colTranslation;
return m_GI->m_defDataSource->FindFirst( string, col, row, flags );
}

/***********************************************************************
*                             FindNext                                 *
***********************************************************************/
int CUGCtrl::FindNext( LPTSTR string, int *col, long * row, int flags )
{
*col = m_GI->m_colInfo[*col].colTranslation;
return m_GI->m_defDataSource->FindNext( string, col, row, flags );
}

/***********************************************************************
*                             SortBy                                   *
***********************************************************************/
int CUGCtrl::SortBy( int col, int flag )
{
int cols[1];

cols[0] = col;

return SortBy( cols, 1, flag );
}

/***********************************************************************
*                             SortBy                                   *
***********************************************************************/
int CUGCtrl::SortBy( int * cols, int num, int flag )
{
int i;

/*
-------------------------------
Perform data column translation
------------------------------- */
for ( i=0; i<num; i++ )
    cols[i] = m_GI->m_colInfo[cols[i]].colTranslation;

return m_GI->m_defDataSource->SortBy(cols,num,flag);
}

/***********************************************************************
*                           SetTH_Height                               *
*   Sets the total height of the top heading                           *
*   A value of 0 hides the top heading                                 *
***********************************************************************/
int CUGCtrl::SetTH_Height( int height )
{
int    totalHeight;
int    i;
double adjust;

if ( height <0 || height >1024 )
    return 1;

m_GI->m_topHdgHeight = height;

/*
------------------------------------------
Adjust the height for each top heading row
------------------------------------------ */
totalHeight = 0;

/*
-------------------------
Find the total old height
------------------------- */
for ( i=0; i<m_GI->m_numberTopHdgRows; i++ )
    totalHeight += m_GI->m_topHdgHeights[i];

/*
-------------------------
Find the adjustment value
------------------------- */
adjust  = (double) height;
adjust /= (double) totalHeight;

/*
----------------------
Adjust each row height
---------------------- */
for ( i=0; i<m_GI->m_numberTopHdgRows; i++ )
    m_GI->m_topHdgHeights[i] = (int)(m_GI->m_topHdgHeights[i] * adjust +0.5);

AdjustComponentSizes();

return UG_SUCCESS;
}

/***********************************************************************
*                             GetTH_Height                             *
*                Returns the height of the top heading                 *
***********************************************************************/
int CUGCtrl::GetTH_Height()
{
return m_GI->m_topHdgHeight;
}

/***********************************************************************
*                             SetSH_Width                              *
*   Sets the total widht of the side heading A value of 0 hides        *
*   the side heading.                                                  *
***********************************************************************/
int CUGCtrl::SetSH_Width( int width )
{
if ( width <0 || width >1024 )
    return 1;

m_GI->m_sideHdgWidth = width;

/*
----------------
Adjust the width
---------------- */
int totalWidth = 0;
int loop;
double adjust;

/*
-------------------------
Find the total old height
------------------------- */
for ( loop=0; loop < m_GI->m_numberSideHdgCols; loop++ )
    totalWidth += m_GI->m_sideHdgWidths[loop];

/*
-------------------------
Find the adjustment value
------------------------- */
if (totalWidth == 0)
    adjust = 0.0;
else
    adjust = (double)width / (double)totalWidth;

/*
---------------------
Adjust each col width
--------------------- */
for ( loop=0; loop < m_GI->m_numberSideHdgCols; loop++ )
    m_GI->m_sideHdgWidths[loop] = (int)(m_GI->m_sideHdgWidths[loop] * adjust +0.5);

AdjustComponentSizes();

return UG_SUCCESS;
}

/***********************************************************************
*                            GetSH_Width                               *
*              Returns the width of the side heading                   *
***********************************************************************/
int CUGCtrl::GetSH_Width()
{
return m_GI->m_sideHdgWidth;
}

/***********************************************************************
*                            SetVS_Width                               *
*          Sets the width of the vertical scroll bar                   *
***********************************************************************/
int CUGCtrl::SetVS_Width(int width)
{

if (width <0 || width > 1024)
    return 1;

m_GI->m_vScrollWidth = width;

AdjustComponentSizes();

return UG_SUCCESS;
}

/***********************************************************************
*                             GetVS_Width                              *
*             returns the width of the vertical scroll bar             *
***********************************************************************/
int CUGCtrl::GetVS_Width()
{
return m_GI->m_vScrollWidth;
}

/***********************************************************************
*                            SetHVS_Height                             *
*            Sets the height of the horizontal scroll bar              *
***********************************************************************/
int CUGCtrl::SetHS_Height(int height){

    if (height <0 || height > 1024)
        return 1;

    m_GI->m_hScrollHeight = height;

    AdjustComponentSizes();

    return UG_SUCCESS;
}

/***********************************************************************
*                            GetHS_Height                              *
*        returns the height of the horizontal scroll bar               *
***********************************************************************/
int CUGCtrl::GetHS_Height()
{
return m_GI->m_hScrollHeight;
}

/***********************************************************************
*                          SetCurrentCellMode                          *
*   Sets the highlighting and focus mode of the current cell           *
*   value of 1 by default                                              *
*                                                                      *
*   'mode' if bit 0 ( value of 1) is set then the current cell         *
*           gets a focus rectangle                                     *
*                                                                      *
*          if bit 1 ( value of 2) is set then the current cell         *
*           is highlighted (a value of three is for both)              *
***********************************************************************/
int CUGCtrl::SetCurrentCellMode( int mode )
{
if ( mode < 0 || mode > 3 )
    return UG_ERROR;

m_GI->m_currentCellMode = mode;

return UG_SUCCESS;
}

/***********************************************************************
*                           SetHightlightRow                           *
*   Set the grid into rowhighlighting mode. FALSE by default           *
*                                                                      *
*   'mode'  TRUE: row highlighting mode - a whole row is               *
*           highlighted at a time - useful for multi-column list boxes *
*                                                                      *
*           FALSE: standard cell hightlighting                         *
***********************************************************************/
int CUGCtrl::SetHighlightRow( int mode )
{

if ( mode )
    {
    m_GI->m_highlightRowFlag = TRUE;

    mode = m_GI->m_multiSelect->GetSelectMode();
    if (mode&UG_MULTISELECT_CELL)
        mode -= UG_MULTISELECT_CELL;
    mode |= UG_MULTISELECT_ROW;

    m_GI->m_multiSelect->SelectMode(mode);
    }
else
    {
    m_GI->m_highlightRowFlag = FALSE;

    mode = m_GI->m_multiSelect->GetSelectMode();
    if (mode&UG_MULTISELECT_ROW)
        mode -= UG_MULTISELECT_ROW;
    mode |= UG_MULTISELECT_CELL;

    m_GI->m_multiSelect->SelectMode(mode);
    }

return UG_SUCCESS;
}

/***********************************************************************
*                          SetMultiSelectMode                          *
*       Turns multiple selection on and off, off by default            *
*       'mode'  TRUE or FALSE                                          *
***********************************************************************/
int CUGCtrl::SetMultiSelectMode( int mode )
{
m_GI->m_multiSelectFlag = mode;
/*
---------------------
0: off  1:cell  2:row
--------------------- */
if ( mode )
    {
    if ( m_GI->m_highlightRowFlag)
        return m_GI->m_multiSelect->SelectMode(2);
    else
        return m_GI->m_multiSelect->SelectMode(1);
    }

return m_GI->m_multiSelect->SelectMode(mode);
}

/***********************************************************************
*                            Set3DHeight                               *
*             Sets the height of the 3D border styles                  *
*   This value is used every time the border draws a rasied or         *
*   recessed border. A value of 1 is default                           *
***********************************************************************/
int CUGCtrl::Set3DHeight( int height )
{

if ( height <1 || height > 16 )
    return UG_ERROR;

m_GI->m_threeDHeight = height;

return UG_SUCCESS;
}

/***********************************************************************
*                            SetPaintMode                              *
*   Turns painting on/off.                                             *
*   Useful for making many changes to the grid's properties,           *
*   with paint mode off the grid will not try and update itself        *
***********************************************************************/
int CUGCtrl::SetPaintMode( int mode )
{
if ( mode )
    {
    m_GI->m_paintMode = TRUE;
    RedrawAll();
    }
else
    {
    m_GI->m_paintMode = FALSE;
    }

return UG_SUCCESS;
}

/***********************************************************************
*                             GetPaintMode                             *
***********************************************************************/
int CUGCtrl::GetPaintMode()
{
return m_GI->m_paintMode;
}

/***********************************************************************
*                            SetVScrollMode                            *
***********************************************************************/
int CUGCtrl::SetVScrollMode( int mode )
{
m_GI->m_vScrollMode = mode;
m_CUGVScroll->Update();

return UG_SUCCESS;
}

/***********************************************************************
*                            SetHScrollMode                            *
***********************************************************************/
int CUGCtrl::SetHScrollMode( int mode )
{
m_GI->m_hScrollMode = mode;
m_CUGHScroll->Update();

return UG_SUCCESS;
}

/***********************************************************************
*                            GetCurrentCol                             *
***********************************************************************/
int CUGCtrl::GetCurrentCol()
{
return m_GI->m_currentCol;
}

/***********************************************************************
*                            GetCurrentRow                             *
***********************************************************************/
long CUGCtrl::GetCurrentRow()
{
return m_GI->m_currentRow;
}

/***********************************************************************
*                             GetLeftCol                               *
*           Returns the left visible column within the grid            *
***********************************************************************/
int CUGCtrl::GetLeftCol()
{
return m_GI->m_leftCol;
}

/***********************************************************************
*                             GetRightCol                              *
*           Returns the right visible column within the grid           *
***********************************************************************/
int CUGCtrl::GetRightCol()
{
return m_GI->m_rightCol;

}

/***********************************************************************
*                              GetTopRow                               *
*             Returns the top visible row within the grid              *
***********************************************************************/
long CUGCtrl::GetTopRow()
{
return m_GI->m_topRow;
}

/***********************************************************************
*                             GetBottomRow                             *
*             Returns the bottom visible row within the grid           *
***********************************************************************/
long CUGCtrl::GetBottomRow()
{
return m_GI->m_bottomRow;
}

/***********************************************************************
*                               RedrawAll                              *
*                        Redraws all child windows                     *
***********************************************************************/
int CUGCtrl::RedrawAll()
{
Update();
return UG_SUCCESS;
}

/***********************************************************************
*                               RedrawCell                             *
***********************************************************************/
int CUGCtrl::RedrawCell( int col, long row )
{
if ( col < 0 && row < 0 )
    {
    InvalidateRect(m_CUGCnrBtn->m_cnrBtnWnd, NULL, TRUE);
    UpdateWindow(m_CUGCnrBtn->m_cnrBtnWnd);
    }
else if ( col < 0 )
    {
    InvalidateRect(m_CUGSideHdg->m_sideHdgWnd, NULL, TRUE);
    UpdateWindow(m_CUGSideHdg->m_sideHdgWnd);
    }
else if ( row < 0 )
    {
    InvalidateRect(m_CUGTopHdg->m_topHdgWnd, NULL, TRUE);
    UpdateWindow(m_CUGTopHdg->m_topHdgWnd);
    }
else
    {
    m_CUGGrid->RedrawCell(col,row);
    }

return UG_SUCCESS;
}

/***********************************************************************
*                              RedrawRow                               *
***********************************************************************/
int CUGCtrl::RedrawRow( long row )
{
m_CUGGrid->RedrawRow(row);
return UG_SUCCESS;
}

/***********************************************************************
*                             RedrawCol                                *
***********************************************************************/
int CUGCtrl::RedrawCol( int col )
{
m_CUGGrid->RedrawCol(col);
return UG_SUCCESS;
}

/***********************************************************************
*                            RedrawRange                               *
***********************************************************************/
int CUGCtrl::RedrawRange( int startCol, long startRow, int endCol, long endRow )
{
m_CUGGrid->RedrawRange(startCol,startRow,endCol,endRow);
return UG_SUCCESS;
}

/***********************************************************************
*                            AddCellType                               *
*   Adds a custom cell type to the grid's cell type list. Once         *
*   added a cell can use this type.                                    *
*                                                                      *
*    Returns the ID number of the cell type that was added. This       *
*   is the value to use when setting a cells 'celltype' property       *
*                                                                      *
*   NOTE: The grid does not delete the cell type when                  *
*   the grid is destroyed.                                             *
*                                                                      *
*   -1 error                                                           *
***********************************************************************/
long CUGCtrl::AddCellType( CUGCellType * celltype )
{
int i;

if (celltype == NULL)
    return -1;

i = m_cellTypeList->AddPointer(celltype);

if ( i == -1 )
    return -1;

celltype->m_ctrl = this;
celltype->SetID( i );

return i;
}

/***********************************************************************
*                          RemoveCellType                              *
*   Removes the specified cell type from the grid's cell type          *
*   list. It does not delete the cell type class                       *
***********************************************************************/
int CUGCtrl::RemoveCellType( int index )
{
m_cellTypeList->DeletePointer(index);

return UG_SUCCESS;
}

/***********************************************************************
*                          ClearSelections                             *
*             Clears any selections made in the grid                   *
***********************************************************************/
int CUGCtrl::ClearSelections()
{
m_GI->m_multiSelect->ClearAll();

m_GI->m_dragCol = m_GI->m_currentCol;
m_GI->m_dragRow = m_GI->m_currentRow;

return UG_SUCCESS;
}

/***********************************************************************
*                              Select                                  *
*   Selects a specified cell                                           *
*   Multiple selection must be turned on for this                      *
*   function to work                                                   *
***********************************************************************/
int CUGCtrl::Select( int col, long row )
{
m_GI->m_multiSelect->StartBlock(col,row);
m_GI->m_multiSelect->EndBlock(col,row);
RedrawCell(col,row);

return UG_SUCCESS;
}

/***********************************************************************
*                           IsSelected                                 *
***********************************************************************/
BOOL CUGCtrl::IsSelected( int col, long row, int * blockNum )
{
return m_GI->m_multiSelect->IsSelected(col,row,blockNum);
}

/***********************************************************************
*                           SelectRange                                *
*   Selects a specified range of cells                                 *
*   Multiple selection must be turned on for this function to work     *
***********************************************************************/
int CUGCtrl::SelectRange( int startCol, long startRow, int endCol, long endRow )
{
const static int mask = UG_MULTISELECT_CELL | UG_MULTISELECT_ROW;

if ( m_GI->m_multiSelectFlag & mask )
    {
    m_GI->m_multiSelect->StartBlock(startCol,startRow);
    m_GI->m_multiSelect->EndBlock(endCol,endRow);
    }

RedrawRange( startCol, startRow, endCol, endRow );
return UG_SUCCESS;
}

/***********************************************************************
*                        EnumFirstSelected                             *
*   Returns the co-ords of the first cell that has been                *
*   selected. the current cell is also included in this                *
*   enumeration ( if Multiple selection is off then the current        *
*   cell is returned)                                                  *
***********************************************************************/
int CUGCtrl::EnumFirstSelected( int * col, long * row )
{
return m_GI->m_multiSelect->EnumFirstSelected(col,row);
}

/***********************************************************************
*                         EnumNextSelected                             *
*   Returns the co-ords of the next cell that has been selected.       *
*   the current cell is also included in this enumeration              *
*   (Multiple selection mode must be set first)                        *
***********************************************************************/
int CUGCtrl::EnumNextSelected( int * col, long * row )
{
return m_GI->m_multiSelect->EnumNextSelected(col,row);
}

/***********************************************************************
*                           CopySelected                               *
*   Copies the grid's currently selected items to the clipboard        *
***********************************************************************/
int CUGCtrl::CopySelected()
{
return CopySelected(FALSE);
}

/***********************************************************************
*                           CopySelected                               *
*   Copies the grid's currently selected items to the clipboard        *
*   'cutFlag' if true then the selected items are cut from the grid    *
***********************************************************************/
int CUGCtrl::CopySelected( int cutFlag )
{
int rt;
TCHAR * clipString;

clipString = 0;

/*
------------------------------------------------
Get the selected items in a TAB delimited string
------------------------------------------------ */
CreateSelectedString(clipString,cutFlag);

/*
-------------------------------
Copy the items to the clipboard
------------------------------- */
rt = CopyToClipBoard(clipString);
if ( rt == UG_SUCCESS )
    {
    ClearSelections();
    RedrawAll();
    }

if (clipString )
    delete [] clipString;

return rt;
}

/***********************************************************************
*                        CreateSelectedString                          *
***********************************************************************/
void CUGCtrl::CreateSelectedString( LPTSTR & string, int cutFlag )
{
int     col;
int     rt;
long    lastrow;
long    row;
CUGCell cell;

/*
----------------------------------------------
Enum selected items and add them to the string
---------------------------------------------- */
rt = m_GI->m_multiSelect->EnumFirstSelected(&col,&row);

while ( rt == UG_SUCCESS )
    {
    /*
    ------------------------------------------
    Get the selected cell then copy the string
    ------------------------------------------ */
    GetCellIndirect(col,row,&cell);

    AddTwoString( string, cell.GetText() );

    /*
    ------------------
    Check the cut flag
    ------------------ */
    if ( cutFlag )
        {
        cell.ClearAll();
        SetCell( col, row, &cell );
        }

    /*
    ------------------------
    Update the last row flag
    ------------------------ */
    lastrow = row;

    /*
    ---------------------------
    Find the next selected item
    --------------------------- */
    rt = m_GI->m_multiSelect->EnumNextSelected(&col,&row);

    /*
    ---------------------------
    Add line feeds between rows
    --------------------------- */
    if ( rt == UG_SUCCESS )
        {
        if (row != lastrow )
            {
            AddTwoString( string, LFString );
            }
        else
            {
            /*
            -------------------------------
            Otherwise add tabs between cols
            ------------------------------- */
            AddTwoString( string, TabString );
            }
        }
    }
}

/***********************************************************************
*                            CutSelected                               *
*   Copies the grid's currently selected items to the clipboard        *
*   then removes the contents of the selected cells                    *
***********************************************************************/
int CUGCtrl::CutSelected()
{
return CopySelected(TRUE);
}

/***********************************************************************
*                           CopyToClipBoard                            *
*             Copies the specified string to the clipboard             *
***********************************************************************/
int CUGCtrl::CopyToClipBoard( TCHAR * string )
{
HGLOBAL hg;
LPTSTR  data;
int     size;

OpenClipboard( m_ctrlWnd );
EmptyClipboard();

size = lstrlen( string );

hg = GlobalAlloc( GMEM_ZEROINIT, size+1 );
if ( hg == NULL )
    return UG_ERROR;

data = (LPTSTR)GlobalLock(hg);

lstrcpy( data, string );

GlobalUnlock(hg);
SetClipboardData( ClipFormat, hg );

CloseClipboard();

return UG_SUCCESS;
}

/***********************************************************************
*                           CopyFromClipBoard                          *
*   Copies the contents from the clipboard to the specified string     *
***********************************************************************/
int CUGCtrl::CopyFromClipBoard( LPTSTR * string )
{
HGLOBAL hg;
LPTSTR data;

OpenClipboard( m_ctrlWnd );

hg = GetClipboardData( ClipFormat );
if ( hg == NULL )
    {
    CloseClipboard();
    return UG_ERROR;
    }

data = (LPTSTR) GlobalLock( hg );
CopyString( *string, data );

GlobalUnlock(hg);

CloseClipboard();

return UG_SUCCESS;
}

/***********************************************************************
*                               Paste                                  *
*   Pastes the contents of the clipboard into the grid starting from   *
*   the current cell.                                                  *
***********************************************************************/
int CUGCtrl::Paste()
{
TCHAR * string;

string = NULL;

if (CopyFromClipBoard(&string)!= UG_SUCCESS)
    return UG_ERROR;

Paste( string );

if ( string )
    delete [] string;

return UG_SUCCESS;
}

/***********************************************************************
*                                Paste                                 *
*   Pastes the contents of the clipboard into the grid starting        *
*   from the current cell                                              *
***********************************************************************/
int CUGCtrl::Paste( LPTSTR & string )
{

int     col;
long    row;
int     pos;
int     endpos;
int     len;
LPTSTR  sub;
LPTSTR  buf;
CUGCell cell;
TCHAR   endchar;
int     maxcol;
long    maxrow;

pos     = 0;
col     = m_GI->m_currentCol;
maxcol  = col;
row     = m_GI->m_currentRow;
maxrow  = row;

sub     = NULL;
buf     = NULL;

CopyString( buf, string );
len = lstrlen( string );

while ( pos < len )
    {
    /*
    ----------------------------------
    Check to see if the field is blank
    ---------------------------------- */
    if ( buf[pos] == TabChar || buf[pos] == CRChar || buf[pos] == LFChar )
        {
        endchar = buf[pos];
        endpos = pos;
        /*
        ----------------
        Set a blank cell
        ---------------- */
        GetCell(col,row,&cell);
        cell.SetText(_T(""));
        SetCell(col,row,&cell);
        }
    else
        {
        /*
        -------------------------------------------------------
        Find the end of the item then copy the item to the cell
        ------------------------------------------------------- */
        endpos = pos+1;
        while (endpos < len)
            {
            endchar = buf[endpos];
            if ( endchar == TabChar || endchar == CRChar || endchar == LFChar )
                break;
            endpos++;
            }

        /*
        -------------
        Copy the item
        ------------- */
        GetCell(col,row,&cell);
        CopyString(sub, &string[pos], endpos-pos);
        cell.SetText(sub);
        if ( sub )
            {
            delete [] sub;
            sub = NULL;
            }
        SetCell(col,row,&cell);

        /*
        -------------------------
        Store the max col and row
        ------------------------- */
        if (col > maxcol)
            maxcol = col;
        if (row > maxrow)
            maxrow = row;
        }

    if (sub ) {
        delete [] sub;
        sub = NULL;
    }

/*
--------------------------------------
Set up the position for the next field
-------------------------------------- */
if (endchar == _T('\t'))
    col++;

if (endchar == CRChar || endchar == LFChar )
    {
    col = m_GI->m_currentCol;
    row++;
    if ( buf[endpos] == CRChar && buf[endpos+1] == LFChar )
        endpos++;
    }

pos = endpos +1;
}

ClearSelections();
SelectRange(m_GI->m_currentCol,m_GI->m_currentRow,maxcol,maxrow);

RedrawAll();

if (buf )
    delete [] buf;

return UG_SUCCESS;
}

/***********************************************************************
*                          AddDataSource                               *
*                 return datasource index number                       *
***********************************************************************/
int CUGCtrl::AddDataSource( CUGDataSource * ds )
{
CUGDataSource ** temp;
int i;

if ( ds == NULL )
    return UG_ERROR;

ds->m_ctrl = this;

/*
----------------------------------------------
Alloc a larger DataSource List array if needed
---------------------------------------------- */
if ( m_dataSrcListLength == m_dataSrcListMaxLength )
    {
    m_dataSrcListMaxLength += 16;
    temp = new CUGDataSource*[m_dataSrcListMaxLength];
    for ( i=0; i<m_dataSrcListMaxLength; i++ )
        {
        if ( i < m_dataSrcListLength )
            temp[i] = m_dataSrcList[i];
        else
            temp[i] = NULL;
        }

    delete[] m_dataSrcList;
    m_dataSrcList = temp;
    }

for ( i=1; i<m_dataSrcListMaxLength; i++ )
    {
    if ( m_dataSrcList[i] == NULL )
        {
        m_dataSrcList[i] = ds;
        m_dataSrcListLength++;
        ds->SetID(i);
        return i;
        }
    }

return -1;
}

/***********************************************************************
*                          GetDataSource                               *
*       Returns the pointer to the datasource at the specified 0       *
*       based index. Datasources are added to the list through         *
*       the AddDataSource member function                              *
*   Params                                                             *
*       index - 0 based index of datasource                            *
*   Returns                                                            *
*       NULL - failure                                                 *
*       otherwise the pointer to the datasource                        *
***********************************************************************/
CUGDataSource * CUGCtrl::GetDataSource( int index )
{

if ( index <0 || index > m_dataSrcListLength )
    return NULL;

/*
-------------------------------------------------
Return the memory manager as the first datasource
------------------------------------------------- */
if ( index == 0 )
    return m_GI->m_CUGMem;
else
    return m_dataSrcList[index];
}

/***********************************************************************
*                        SetDefDataSource                              *
*   Sets the datasource for the grid to use as its default             *
*   datasource. The datasource must first be in the datasource         *
*   list (which the memory manager is by default, index 0).            *
*                                                                      *
*   Params                                                             *
*       index - index of datasource to use as the default              *
***********************************************************************/
int CUGCtrl::SetDefDataSource( int index )
{
int i;

if ( index < 0 || index > m_dataSrcListLength )
    return 1;

/*
------------------------------------------------------------------
If the index is 0 then use the memory manager that was created
in each sheet, otherwise use a datasource from the datasource list
------------------------------------------------------------------ */
m_GI->m_defDataSourceIndex = index;

if ( index == 0 )
    m_GI->m_defDataSource = m_GI->m_CUGMem;
else
    m_GI->m_defDataSource = m_dataSrcList[index];

for ( i=0; i<m_GI->m_numberCols; i++ )
    m_GI->m_colInfo[i].dataSource = m_GI->m_defDataSource;

return UG_SUCCESS;
}

/***********************************************************************
*                           GetDefDataSource                           *
*  Returns the index number of the default datasource for the grid     *
***********************************************************************/
int CUGCtrl::GetDefDataSource()
{
return m_GI->m_defDataSourceIndex;
}

/***********************************************************************
*                          RemoveDataSource                            *
*  Removes a datasource from the grids datasource list. This function  *
*  DOES NOT DELETE the datasource object that was in the list.         *
*   Params                                                             *
*       index -index of datasource to remove                           *
***********************************************************************/
int CUGCtrl::RemoveDataSource( int index )
{

if ( index <= 0 || index > m_dataSrcListLength )
    return UG_ERROR;

if ( m_dataSrcList[index] )
    {
    m_dataSrcList[index] = NULL;
    m_dataSrcListLength --;
    return UG_SUCCESS;
    }

return UG_ERROR;
}

/***********************************************************************
*                   SetGridUsingDataSource                             *
***********************************************************************/
int CUGCtrl::SetGridUsingDataSource( int index )
{
CUGDataSource * ds;
int  cols;
long rows;

if (index <0 || index >= m_dataSrcListMaxLength)
    return NULL;

ds= m_dataSrcList[index];

if (ds == NULL)
    return 1;

cols = ds->GetNumCols();
if ( cols < 1 )
    return 2;

SetNumberCols(cols,FALSE);

rows = ds->GetNumRows();
if ( rows >= 0 )
    SetNumberRows(rows,FALSE);

AdjustComponentSizes();

return UG_SUCCESS;
}

/***********************************************************************
*                          GetPopupMenu                                *
***********************************************************************/
HMENU CUGCtrl::GetPopupMenu()
{
return m_menu;
}

/***********************************************************************
*                             EmptyMenu                                *
***********************************************************************/
int CUGCtrl::EmptyMenu()
{
while ( DeleteMenu(m_menu, 0, MF_BYPOSITION) )
    ;

return UG_SUCCESS;
}

/***********************************************************************
*                            AddMenuItem                               *
***********************************************************************/
int CUGCtrl::AddMenuItem( int ID, LPCTSTR string )
{
if (lstrlen(string) == 0 || ID == -1)
    {
    if (AppendMenu(m_menu, MF_SEPARATOR, 0, _T("")) != TRUE )
        return UG_SUCCESS;
    }
else
    {
    if (AppendMenu(m_menu, MF_STRING, ID, string) != TRUE)
        return UG_SUCCESS;
    }

return UG_ERROR;
}

/***********************************************************************
*                         RemoveMenuItem                               *
***********************************************************************/
int CUGCtrl::RemoveMenuItem( int ID )
{
if ( DeleteMenu( m_menu, ID, MF_BYCOMMAND) != TRUE )
    return UG_SUCCESS;

return UG_ERROR;
}

/***********************************************************************
*                             EnableMenu                               *
***********************************************************************/
int CUGCtrl::EnableMenu( BOOL state )
{
if ( state )
    m_GI->m_enablePopupMenu = TRUE;
else
    m_GI->m_enablePopupMenu = FALSE;

return UG_SUCCESS;
}

#ifdef UG_ENABLE_HINTS
/***********************************************************************
*                            UseHints                                  *
***********************************************************************/
int CUGCtrl::UseHints( BOOL state )
{
if ( state )
    {
    m_GI->m_enableHints = TRUE;
    SendMessage(m_CUGGrid->m_gridWnd, TTM_ACTIVATE, TRUE, 0);
    }
else
    {
    m_GI->m_enableHints = FALSE;
    SendMessage(m_CUGGrid->m_gridWnd, TTM_ACTIVATE, FALSE, 0);
    }

return UG_SUCCESS;
}
#endif

#ifdef UG_ENABLE_SCROLLHINTS
/***********************************************************************
*                          UseVScrollHints                             *
***********************************************************************/
int CUGCtrl::UseVScrollHints( BOOL state )
{
if ( state )
    {
    m_GI->m_enableVScrollHints = TRUE;
    SendMessage(m_CUGVScroll->m_vScrollWnd, TTM_ACTIVATE, TRUE, 0);
    }
else
    {
    m_GI->m_enableVScrollHints = FALSE;
    SendMessage(m_CUGVScroll->m_vScrollWnd, TTM_ACTIVATE, FALSE, 0);
    }

return UG_SUCCESS;
}

/***********************************************************************
*                          UseHScrollHints                             *
***********************************************************************/
int CUGCtrl::UseHScrollHints( BOOL state )
{
if ( state )
    {
    m_GI->m_enableHScrollHints = TRUE;
    SendMessage(m_CUGHScroll->m_hScrollWnd, TTM_ACTIVATE, TRUE, 0);
    }
else
    {
    m_GI->m_enableHScrollHints = FALSE;
    SendMessage(m_CUGHScroll->m_hScrollWnd, TTM_ACTIVATE, FALSE, 0);
    }

return UG_SUCCESS;
}
#endif

/***********************************************************************
*                             AddTab                                   *
*    Adds a new tab to the grid to the end of the existing tabs.       *
*    This function does not resize the visible tab area. Use           *
*    SetTabWidth for setting the size.                                 *
*   Params                                                             *
*       label   - label to show in the tab                             *
*       ID      - ID number returned when the tab is                   *
*               clicked                                                *
***********************************************************************/
int CUGCtrl::AddTab( LPCTSTR label, long ID )
{
return m_CUGTab->AddTab(label,ID);
}

/***********************************************************************
*                            DeleteTab                                 *
***********************************************************************/
int CUGCtrl::DeleteTab( long ID )
{
return m_CUGTab->DeleteTab(ID);
}

/***********************************************************************
*                          SetTabWidth                                 *
***********************************************************************/
int CUGCtrl::SetTabWidth( int width )
{
m_GI->m_tabWidth = width;
return UG_SUCCESS;
}

/***********************************************************************
*                          SetCurrentTab                               *
***********************************************************************/
int CUGCtrl::SetCurrentTab( long ID )
{
return m_CUGTab->SetCurrentTab(ID);
}

/***********************************************************************
*                          GetCurrentTab                               *
***********************************************************************/
int CUGCtrl::GetCurrentTab()
{
return m_CUGTab->GetCurrentTab();
}

/***********************************************************************
*                        SetTabBackColor                               *
***********************************************************************/
int CUGCtrl::SetTabBackColor( long ID, COLORREF color )
{
return m_CUGTab->SetTabBackColor(ID,color);
}

/***********************************************************************
*                        SetTabTextColor                               *
***********************************************************************/
int CUGCtrl::SetTabTextColor( long ID, COLORREF color )
{
return m_CUGTab->SetTabTextColor(ID,color);
}

/***********************************************************************
*                          SetNumberSheets                             *
***********************************************************************/
int CUGCtrl::SetNumberSheets( int numSheets )
{
int i;
CUGGridInfo * origGI;

if ( numSheets < 1 || numSheets > 1024 )
    return UG_ERROR;

origGI = m_GI;

/*
--------------------
Create the new array
-------------------- */
CUGGridInfo ** temp = new CUGGridInfo *[numSheets];

/*
---------------------------------------------
Copy the existing gridinfo into the new array
--------------------------------------------- */
for ( i=0; i<m_numberSheets; i++ )
    {
    if ( i < numSheets )
        {
        temp[i] = m_GIList[i];
        }
    else
        {
        delete m_GIList[i];
        m_GIList[i] = NULL;
        }
    }

/*
------------------------------------------
Add new gridinfo elements to the new array
------------------------------------------ */
for ( i=m_numberSheets; i<numSheets; i++ )
    {
    temp[i] = new CUGGridInfo;

    /*
    ------------------------------
    Set up the default information
    ------------------------------ */
    temp[i]->m_multiSelect->m_ctrl   = this;
    temp[i]->m_CUGMem->m_ctrl        = this;
    temp[i]->m_multiSelect->m_GI     = temp[i];
    temp[i]->m_editCtrl              = m_defEditCtrl.m_editWnd;

    m_GI = temp[i];

    OnSheetSetup(i);
    }

/*
-------------------------------------------
Delete the old array, and use the new array
------------------------------------------- */
if ( m_GIList )
    delete[] m_GIList;

m_GIList = temp;

m_numberSheets = numSheets;

m_GI = origGI;

if ( m_currentSheet >= numSheets )
    SetSheetNumber( numSheets-1 );

return UG_SUCCESS;
}

/***********************************************************************
*                          GetNumberSheets                             *
***********************************************************************/
int CUGCtrl::GetNumberSheets()
{
return m_numberSheets;
}

/***********************************************************************
*                          SetSheetNumber                              *
***********************************************************************/
int CUGCtrl::SetSheetNumber( int index, BOOL update )
{
BOOL origValue;
CUGGridInfo * oldInfo;

/*
-----------------------------------
Check to see if the number is valid
----------------------------------- */
if ( index <0 || index >= m_numberSheets )
    return UG_ERROR;

/*
---------------------------------------------
Check to see if the display should be updated
--------------------------------------------- */
origValue = m_enableUpdate;
if ( !update )
    m_enableUpdate = FALSE;

SetFocus(m_CUGGrid->m_gridWnd);

/*
------------------------------------------------
Switch in the new CUGGridInfo Class
copy over the tab width to the new class
update all components so that they point to
the new CUGGridInfo class
update the multiSelect pointer so that it points
to the new CUGGridInfo class multiSelect class
------------------------------------------------ */

oldInfo          = m_GI;
m_GI             = m_GIList[index];
m_GI->m_tabWidth = oldInfo->m_tabWidth;

m_CUGGrid->m_GI         = m_GI;
m_CUGTopHdg->m_GI       = m_GI;
m_CUGSideHdg->m_GI      = m_GI;
m_CUGCnrBtn->m_GI       = m_GI;
m_CUGVScroll->m_GI      = m_GI;
m_CUGHScroll->m_GI      = m_GI;
m_CUGTab->m_GI          = m_GI;
#ifdef UG_ENABLE_PRINTING
m_CUGPrint->m_GI        = m_GI;
#endif

/*
------------------------------
Store the current sheet number
------------------------------ */
m_currentSheet = index;

m_GI->m_lastTopRow  = -1;
m_GI->m_lastLeftCol = -1;

/*
-----------------------------------------
Adjust the child window sizes - this also
causes the grid to re-draw
----------------------------------------- */
AdjustComponentSizes();

m_enableUpdate = origValue;

return UG_SUCCESS;
}

/***********************************************************************
*                          GetSheetNumber                              *
***********************************************************************/
int CUGCtrl::GetSheetNumber()
{
return m_currentSheet;
}

/***********************************************************************
*                          SetTH_NumberRows                            *
*    Sets the number of rows available in the top heading of the       *
*    grid. This function will not make the top heading disapear        *
*    if set to 0, use the SetTH_Height for this purpose.               *
*   Params                                                             *
*       rows - the number of rows to use in the top heading            *
***********************************************************************/
int CUGCtrl::SetTH_NumberRows( int rows )
{
int * temp;
int   i;

/*
--------------
Range checking
-------------- */
if ( rows < 1 || rows > 64 )
    return 1;

/*
-------------------------------
Copy the old height information
------------------------------- */
if ( m_GI->m_topHdgHeights )
    {
    temp = new int[rows];
    for ( i=0; i<rows; i++ )
        {
        if ( i < m_GI->m_numberTopHdgRows )
            temp[i] = m_GI->m_topHdgHeights[i];
        else
            temp[i] = m_GI->m_defRowHeight;
        }
    delete[] m_GI->m_topHdgHeights;
    m_GI->m_topHdgHeights = temp;
    }
else
    {
    m_GI->m_topHdgHeights = new int[rows];
    for ( i=0; i<rows; i++ )
        m_GI->m_topHdgHeights[i] = m_GI->m_defRowHeight;
    }

m_GI->m_numberTopHdgRows = rows;

return UG_SUCCESS;
}

/***********************************************************************
                           SetTH_RowHeight
    Sets the height of a row in the top heading
    Top heading rows start from -1 then -2 and so on
    -1 row is the one closest to the grid it self
***********************************************************************/
int CUGCtrl::SetTH_RowHeight(int row,int height){

    //translate the row number into a 0 based positive index
    row = (row * -1) -1;

    if (row <0 || row >= m_GI->m_numberTopHdgRows)
        return 1;

    if (height < 0 || height > 1024)
        return 2;

    m_GI->m_topHdgHeights[row] = height;

    return UG_SUCCESS;
}

/***********************************************************************
*                         SetSH_NumberCols                             *
***********************************************************************/
int CUGCtrl::SetSH_NumberCols( int cols )
{
int * temp;
int   i;

/*
--------------
Range checking
-------------- */
if (cols < 1 || cols > 64)
    return 1;

/*
------------------------------
Copy the old width information
------------------------------ */
if ( m_GI->m_sideHdgWidths )
    {
    temp = new int[cols];
    for ( i=0; i<cols; i++ )
        {
        if (i < m_GI->m_numberSideHdgCols)
            temp[i] = m_GI->m_sideHdgWidths[i];
        else
            temp[i] = m_GI->m_defColWidth;
        }
    delete[] m_GI->m_sideHdgWidths;
    m_GI->m_sideHdgWidths = temp;
    }
else
    {
    m_GI->m_sideHdgWidths = new int[cols];
    for ( i=0; i<cols; i++ )
        m_GI->m_sideHdgWidths[i] = m_GI->m_defColWidth;
    }

m_GI->m_numberSideHdgCols = cols;

return UG_SUCCESS;
}

/***********************************************************************
*                         SetSH_ColWidth                               *
***********************************************************************/
int CUGCtrl::SetSH_ColWidth( int col, int width )
{
/*
------------------------------------------------------
Translate the col number into a 0 based positive index
------------------------------------------------------ */
col = (col * -1) -1;

if ( col < 0 || col >= m_GI->m_numberSideHdgCols )
    return 1;

if ( width < 0 || width > 1024 )
    return 2;

m_GI->m_sideHdgWidths[col] = width;

return UG_SUCCESS;
}

/***********************************************************************
*                         SetLockRowHeight                             *
***********************************************************************/
void CUGCtrl::SetLockRowHeight()
{
int i;

if ( m_GI->m_numLockRows < 1 )
    return;

m_GI->m_lockRowHeight = 0;

for ( i=0; i<m_GI->m_numLockRows; i++ )
    m_GI->m_lockRowHeight += GetRowHeight(i);

}

/***********************************************************************
*                         SetLockColWidth                              *
***********************************************************************/
void CUGCtrl::SetLockColWidth()
{
int i;

if ( m_GI->m_numLockCols < 1 )
    return;

m_GI->m_lockColWidth = 0;

for ( i=0; i<m_GI->m_numLockCols; i++ )
    m_GI->m_lockColWidth += GetColWidth(i);
}

/***********************************************************************
*                           SetArrowCursor                             *
***********************************************************************/
int CUGCtrl::SetArrowCursor( HCURSOR cursor )
{
if ( cursor == NULL )
    return UG_ERROR;

m_GI->m_arrowCursor = cursor;

return UG_SUCCESS;
}

/***********************************************************************
*                          SetWESizingCursor                           *
***********************************************************************/
int CUGCtrl::SetWESizingCursor( HCURSOR cursor )
{
if ( cursor == NULL )
    return UG_ERROR;

m_GI->m_WEResizseCursor = cursor;

return UG_SUCCESS;
}

/***********************************************************************
*                          SetNSSizingCursor                           *
***********************************************************************/
int CUGCtrl::SetNSSizingCursor( HCURSOR cursor )
{
if ( cursor == NULL )
    return UG_ERROR;

m_GI->m_NSResizseCursor = cursor;

return UG_SUCCESS;
}

/***********************************************************************
*                              OnSetFocus                              *
***********************************************************************/
void CUGCtrl::OnSetFocus( int section )
{
}

/***********************************************************************
*                              OnKillFocus                             *
***********************************************************************/
void CUGCtrl::OnKillFocus( int section )
{
}

/***********************************************************************
*               VIRTUAL FUNTIONS    VIRTUAL FUNCTIONS                  *
***********************************************************************/

/***********************************************************************
*                              OnSetup                                 *
*   This function is called just after the grid window                 *
*   is created or attached to a dialog item.                           *
*   It can be used to initially setup the grid                         *
***********************************************************************/
void CUGCtrl::OnSetup()
{
}

/***********************************************************************
*                              OnSheetSetup                            *
***********************************************************************/
void CUGCtrl::OnSheetSetup( int sheetNumber )
{
}

/***********************************************************************
*                                 OnCanMove                            *
*  Sent when the current cell in the grid is about to move             *
*  A return of TRUE allows the move, a return of FALSE stops the move  *
***********************************************************************/
int CUGCtrl::OnCanMove( int oldcol, long oldrow, int & newcol, long & newrow )
{
return TRUE;
}

/***********************************************************************
*                               OnCanMove                              *
*  Sent when the top row or left column in the grid is about to move   *
*  A return of TRUE allows the move, a return of FALSE stops the move  *
***********************************************************************/
int CUGCtrl::OnCanViewMove( int oldcol, long oldrow, int newcol, long newrow )
{
return TRUE;
}

/***********************************************************************
*                              OnHitBottom                             *
***********************************************************************/
void CUGCtrl::OnHitBottom( long numrows, long rowspast, long rowsfound )
{
}

/***********************************************************************
*                               OnHitTop                               *
***********************************************************************/
void CUGCtrl::OnHitTop( long numrows, long rowspast )
{
}

/***********************************************************************
*                             OnCanSizeCol                             *
*   Sent when the user is over a separation line on the top heading    *
*   A return value of TRUE allows the possibiliy of    a resize        *
***********************************************************************/
int CUGCtrl::OnCanSizeCol( int col )
{
return TRUE;
}

/***********************************************************************
*                              OnColSizing                             *
*  Sent when the user is sizing a column. The column that is being     *
*  sized is given as well as the width. Plus the width can be modified *
*  at this point. This makes it easy to set min and max widths         *
***********************************************************************/
void CUGCtrl::OnColSizing( int col, int * width )
{
}

/***********************************************************************
*                               OnColSized                             *
*   This is sent when the user finished sizing the given column.       *
***********************************************************************/
void CUGCtrl::OnColSized( int col, int * width )
{
}

/***********************************************************************
*                              OnCanSizeRow                            *
*   Sent when the user is over a separation line on the side heading   *
*   A return value of TRUE allows the possibiliy of a resize           *
***********************************************************************/
int  CUGCtrl::OnCanSizeRow( long row )
{
return TRUE;
}

/***********************************************************************
*                                OnRowSizing                           *
* Sent when the user is sizing a row. The row that is being sized is   *
* given as well as the height. Plus the height can be modified         *
* at this point. This makes it easy to set min and max heights         *
***********************************************************************/
void CUGCtrl::OnRowSizing( long row, int * height )
{
}

/***********************************************************************
*                               OnRowSized                             *
*   This is sent when the user is finished sizing the given row.       *
***********************************************************************/
void CUGCtrl::OnRowSized( long row, int * height )
{
}

/***********************************************************************
*                           OnCanSizeSideHdg                           *
*   This is sent when the user moves into position for sizing the      *
*   width of the side heading. Return TRUE to allow the sizing         *
*   or FALSE to not allow it                                           *
***********************************************************************/
int CUGCtrl::OnCanSizeSideHdg()
{
return TRUE;
}

/***********************************************************************
*                             OnCanSizeTopHdg                          *
*   This is sent when the user moves into position for sizing          *
*   the height of the top heading return TRUE to allow the             *
*   sizing or FALSE to not allow it                                    *
***********************************************************************/
int CUGCtrl::OnCanSizeTopHdg()
{
return TRUE;
}

/***********************************************************************
*                        OnSideHdgSizing                               *
***********************************************************************/
int CUGCtrl::OnSideHdgSizing( int * width )
{
return TRUE;
}

/***********************************************************************
*                         OnTopHdgSizing                               *
***********************************************************************/
int CUGCtrl::OnTopHdgSizing( int * height )
{
return TRUE;
}

/***********************************************************************
                          OnSideHdgSized
***********************************************************************/
int CUGCtrl::OnSideHdgSized( int * width )
{
return TRUE;
}

/***********************************************************************
                           OnTopHdgSized
***********************************************************************/
int CUGCtrl::OnTopHdgSized( int * height )
{
return TRUE;
}

/***********************************************************************
                           OnColChange
              Sent whenever the current column changes
***********************************************************************/
void CUGCtrl::OnColChange( int oldcol,int newcol )
{
}

/***********************************************************************
                           OnRowChange
            Sent whenever the current row changes
************************************************************************/
void CUGCtrl::OnRowChange( long oldrow, long newrow )
{
}

/***********************************************************************
                             OnCellChange
    Sent whenever the current cell changes rows or columns
***********************************************************************/
void CUGCtrl::OnCellChange( int oldcol, int newcol, long oldrow, long newrow )
{
}

/***********************************************************************
                           OnLeftColChange
    Sent whenever the left visible column in the grid changes
***********************************************************************/
void CUGCtrl::OnLeftColChange( int oldcol, int newcol )
{
}

/***********************************************************************
                            OnTopRowChange
    Sent whenever the top visible row in the grid changes
***********************************************************************/
void CUGCtrl::OnTopRowChange( long oldrow, long newrow )
{
}

/***********************************************************************
                               OnLClicked
    Sent whenever the user clicks the left mouse
    button within the grid
    this message is sent when the button goes down
    then again when the button goes up

    'col' and 'row' are negitive if the area clicked
    in is not a valid cell
    'rect' the rectangle of the cell that was clicked in
    'point' the point where the mouse was clicked
    'updn'  TRUE if the button is down FALSE if the
            button just when up
***********************************************************************/
void CUGCtrl::OnLClicked( int col, long row, int updn, RECT * rect, POINT * point, BOOL processed )
{
}

/***********************************************************************
                               OnRClicked
  Sent whenever the user clicks the right mouse button within the grid
  This message is sent when the button goes down and then again when
  the button goes up

    'col' and 'row' are negitive if the area clicked in is
    not a valid cell
    'rect' the rectangle of the cell that was clicked in
    'point' the point where the mouse was clicked
    'updn'  TRUE if the button is down FALSE if the
            button just when up
***********************************************************************/
void CUGCtrl::OnRClicked( int col, long row, int updn, RECT * rect, POINT * point, BOOL processed )
{
}

/***********************************************************************
                                OnDClicked
    Sent whenever the user double clicks the left mouse
    button within the grid

    'col' and 'row' are negitive if the area clicked
    in is not a valid cell
    'rect' the rectangle of the cell that was clicked in
    'point' the point where the mouse was clicked
***********************************************************************/
void CUGCtrl::OnDClicked( int col, long row, RECT * rect, POINT * point, BOOL processed )
{
}

/***********************************************************************
                                OnMouseMove
***********************************************************************/
void CUGCtrl::OnMouseMove( int col, long row, POINT * point, UINT nFlags, BOOL processed )
{
}

/***********************************************************************
                               OnTH_LClicked
    Sent whenever the user clicks the left mouse
    button within the top heading
    this message is sent when the button goes down
    then again when the button goes up

    'col' is negitive if the area clicked in is not valid
    'updn'  TRUE if the button is down FALSE if the
            button just when up
***********************************************************************/
void CUGCtrl::OnTH_LClicked( int col, long row, int updn, RECT * rect, POINT * point, BOOL processed )
{
}

/***********************************************************************
                                OnTH_RClicked
    Sent whenever the user clicks the right mouse
    button within the top heading
    this message is sent when the button goes down
    then again when the button goes up

    'col' is negitive if the area clicked in is not valid
    'updn'  TRUE if the button is down FALSE if the
            button just when up
***********************************************************************/
void CUGCtrl::OnTH_RClicked( int col, long row, int updn, RECT * rect, POINT * point, BOOL processed )
{
}

/***********************************************************************
                               OnTH_LClicked
    Sent whenever the user double clicks the left mouse
    button within the top heading

    'col' is negitive if the area clicked in is not valid
***********************************************************************/
void CUGCtrl::OnTH_DClicked( int col, long row, RECT * rect, POINT * point, BOOL processed )
{
}

/***********************************************************************
                                OnSH_LClicked
    Sent whenever the user clicks the left mouse
    button within the side heading
    this message is sent when the button goes down
    then again when the button goes up

    'row' is negitive if the area clicked in is not valid
    'updn'  TRUE if the button is down FALSE if the
            button just when up
***********************************************************************/
void CUGCtrl::OnSH_LClicked( int col, long row, int updn, RECT * rect, POINT * point, BOOL processed )
{
}

/***********************************************************************
                              OnSH_RClicked
    Sent whenever the user clicks the right mouse
    button within the side heading
    this message is sent when the button goes down
    then again when the button goes up

    'row' is negitive if the area clicked in is not valid
    'updn'  TRUE if the button is down FALSE if the
            button just when up
***********************************************************************/
void CUGCtrl::OnSH_RClicked(int col,long row,int updn,RECT *rect,POINT *point,BOOL processed)
{
}

/***********************************************************************
                               OnSH_DClicked
    Sent whenever the user double clicks the left mouse
    button within the side heading

    'row' is negitive if the area clicked in is not valid
***********************************************************************/
void CUGCtrl::OnSH_DClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
}

/***********************************************************************
                                 OnCB_LClicked
    Sent whenever the user clicks the left mouse
    button within the top corner button
    this message is sent when the button goes down
    then again when the button goes up

    'updn'  TRUE if the button is down FALSE if the
            button just when up
***********************************************************************/
void CUGCtrl::OnCB_LClicked(int updn,RECT *rect,POINT *point,BOOL processed)
{
}

/***********************************************************************
                              OnCB_RClicked
    Sent whenever the user clicks the right mouse
    button within the top corner button
    this message is sent when the button goes down
    then again when the button goes up

    'updn'  TRUE if the button is down FALSE if the
            button just when up
***********************************************************************/
void CUGCtrl::OnCB_RClicked(int updn,RECT *rect,POINT *point,BOOL processed)
{
}

/***********************************************************************
                               OnCB_DClicked
    Sent whenever the user double clicks the left mouse
    button within the top corner button
***********************************************************************/
void CUGCtrl::OnCB_DClicked(RECT *rect,POINT *point,BOOL processed)
{
}

/***********************************************************************
                                  OnKeyDown
    Sent whenever the user types when the grid has
    focus. The keystroke can be modified here as well.
    (See WM_KEYDOWN for more information)
***********************************************************************/
void CUGCtrl::OnKeyDown(UINT *vcKey,BOOL processed)
{
}

/***********************************************************************
                                  OnCharDown
    Sent whenever the user types when the grid has
    focus. The keystroke can be modified here as well.
    (See WM_CHAR for more information)
***********************************************************************/
void CUGCtrl::OnCharDown( UINT *vcKey, BOOL processed )
{
}

/***********************************************************************
                                 OnGetCell
    This message is sent everytime the grid needs to
    draw a cell in the grid. At this point the cell
    class has been filled with the information to be
    used to draw the cell. The information can now be
    changed before it is used for drawing
***********************************************************************/
void CUGCtrl::OnGetCell( int col, long row, CUGCell * cell )
{

}

/***********************************************************************
                                OnSetCell
   This message is sent everytime the a cell is about to change.
***********************************************************************/
void CUGCtrl::OnSetCell( int col, long row, CUGCell * cell )
{
}

/***********************************************************************
                           OnCellTypeNotify
    This message is sent from a cell type , message
    depends on the cell type - check the information
    on the cell type classes
    - The ID of the cell type is given
***********************************************************************/
int CUGCtrl::OnCellTypeNotify(long ID,int col,long row,long msg,long param)
{
return TRUE;
}

/***********************************************************************
*                              OnEditStart                             *
*   This is called whenever the grid is ready to start editing a cell  *
*   plus the properties of the CEdit class can be modified             *
*   A return of TRUE allows the editing a return of FALSE stops editing*
***********************************************************************/
int CUGCtrl::OnEditStart( int col, long row, HWND * edit )
{
return TRUE;
}

/***********************************************************************
                               OnEditVerify
              This is sent when the editing is about to end
***********************************************************************/
int CUGCtrl::OnEditVerify(int col,long row, HWND edit,UINT *vcKey)
{
return TRUE;
}

/***********************************************************************
*                              OnEditFinish                            *
*                 This is sent when editing is finished                *
***********************************************************************/
int CUGCtrl::OnEditFinish( int col, long row, HWND edit, LPCTSTR string, BOOL cancelFlag)
{
return TRUE;
}

/***********************************************************************
*                              OnEditContinue                          *
***********************************************************************/
int CUGCtrl::OnEditContinue( int oldcol, long oldrow, int * newcol, long * newrow )
{
return TRUE;
}

/***********************************************************************
*                           OnMenuCommand                              *
***********************************************************************/
void CUGCtrl::OnMenuCommand( int col, long row, int section, int item )
{
}

/***********************************************************************
*                           OnMenuStart                                *
*      UG_SUCCESS => allow the menu to appear                          *
*      1          => do NOT allow the menu to appear.                  *
***********************************************************************/
int CUGCtrl::OnMenuStart( int col, long row, int section )
{
return TRUE;
}

/***********************************************************************
*                            HideCurrentCell                           *
***********************************************************************/
int CUGCtrl::HideCurrentCell()
{
return UG_SUCCESS;
}


/***********************************************************************
*                             OnScreenDCSetup                          *
*       dc - device context                                            *
*       db_dc - double buffer device context, may be NULL              *
***********************************************************************/
void CUGCtrl::OnScreenDCSetup( HDC dc, HDC db_dc, int section )
{
}

/***********************************************************************
*                              OnSortEveluate                          *
*          -1   cell1 <  cell2                                         *
*           0   cell1 == cell2                                         *
*           1   cell1 >  cell2                                         *
***********************************************************************/
int CUGCtrl::OnSortEvaluate( CUGCell * cell1, CUGCell * cell2, int flags )
{
BOOL have_cell1;
BOOL have_cell2;
CUGCell * ptr;

if ( flags & UG_SORT_DESCENDING )
    {
    ptr   = cell1;
    cell1 = cell2;
    cell2 = ptr;
    }

have_cell1 = cell1->IsPropertySet( UGCELL_TEXT_SET );
have_cell2 = cell2->IsPropertySet( UGCELL_TEXT_SET );

if ( !have_cell2 )
    {
    if ( !have_cell1 )
        return 0;
    return -1;
    }

if ( !have_cell1 )
    return 1;

return lstrcmp(cell1->GetText(), cell2->GetText() );
}

/***********************************************************************
*                      SetDoubleBufferMode                             *
***********************************************************************/
int CUGCtrl::SetDoubleBufferMode( BOOL mode )
{
m_GI->m_doubleBufferMode = mode;
m_CUGGrid->SetDoubleBufferMode(mode);

return UG_SUCCESS;
}

/***********************************************************************
*                                LockColumns                           *
*       Sets the number of columns to lock in place                    *
*       on the left side of the grid (so they do not                   *
*       scroll).                                                       *
*       The following variable are set:                                *
*           m_GI->m_numLockCols                                        *
*           m_GI->m_lockColWidth                                       *
*   Params                                                             *
*       numCols - the number of columns to lock                        *
*   Return                                                             *
*       UG_SUCCESS  - success                                          *
*       1           - numCols was out of range                         *
***********************************************************************/
int CUGCtrl::LockColumns( int numCols )
{
int col;

/*
-----------------
Do range checking
----------------- */
if ( numCols <0 || numCols > m_GI->m_numberCols )
    return 1;

/*
--------------------------------------------------------
If the new value is the same as the old then just return
-------------------------------------------------------- */
if ( numCols == m_GI->m_numLockCols )
    return UG_SUCCESS;

/*
--------------------------------
Set the number of locked columns
-------------------------------- */
m_GI->m_numLockCols = numCols;

/*
------------------
Set the lock width
------------------ */
m_GI->m_lockColWidth = 0;
for ( col=0; col<numCols; col++ )
    m_GI->m_lockColWidth += GetColWidth(col);

m_GI->m_leftCol = numCols;
m_GI->m_lastLeftCol = numCols;

CalcLeftCol();
return UG_SUCCESS;
}

/***********************************************************************
*                               LockRows                               *
*   Purpose                                                            *
*       Sets the number of rows to lock in place                       *
*       on the top of the grid (so they do not                         *
*       scroll).                                                       *
*       The following variable are set:                                *
*           m_GI->m_numLockRows                                        *
*           m_GI->m_lockRowHeight                                      *
*   Params                                                             *
*       numRows - the number of rows to lock                           *
***********************************************************************/
int CUGCtrl::LockRows( int numRows )
{
long row;

/*
--------------
Range checking
-------------- */
if ( numRows<0 || numRows > m_GI->m_numberRows )
    return 1;

/*
--------------------------------------------------------
If the new value is the same as the old then just return
-------------------------------------------------------- */
if ( numRows == m_GI->m_numLockRows )
    return UG_SUCCESS;

/*
-----------------------------
Set the number of locked rows
----------------------------- */
m_GI->m_numLockRows = numRows;

/*
-------------------
Set the lock height
------------------- */
m_GI->m_lockRowHeight = 0;
for ( row=0; row<numRows; row++ )
    m_GI->m_lockRowHeight += GetRowHeight(row);

m_GI->m_topRow = numRows;
m_GI->m_lastTopRow = numRows;

CalcTopRow();

return UG_SUCCESS;
}

/***********************************************************************
*                          OnTabSelected                               *
***********************************************************************/
void CUGCtrl::OnTabSelected( int ID )
{
}

/***********************************************************************
*                        OnAdjustComponentSizes                        *
***********************************************************************/
void CUGCtrl::OnAdjustComponentSizes( RECT * grid, RECT * topHdg, RECT * sideHdg, RECT * cnrBtn, RECT * vScroll, RECT * hScroll, RECT * tabs )
{
}

/***********************************************************************
*                         OnDrawFocusRect                              *
***********************************************************************/
void CUGCtrl::OnDrawFocusRect( HDC dc, RECT * rect )
{
rect->bottom--;
rect->right--;
DrawFocusRect(dc, rect);
}

/***********************************************************************
*                        OnGetDefBackColor                             *
***********************************************************************/
COLORREF CUGCtrl::OnGetDefBackColor( int section )
{

if ( section == UG_GRID )
    return GetSysColor(COLOR_WINDOW);
else
    return GetSysColor(COLOR_BTNFACE);

}

/***********************************************************************
*                            StartMenu                                 *
*   Starts the pop-up menu if enabled                                  *
*                                                                      *
*   col - col to start the menu for                                    *
*   row - row to start the menu for                                    *
*   point - point in screen co-ords                                    *
*   seciton - UG_GRID,UG_TOPHEADING,UG_SIDEHEADING                     *
*             UG_CORNERBUTTON,UG_TAB,UG_VSCROLL,UG_HSCROLL             *
* Return                                                               *
*   UG_SUCCESS  - success                                              *
*   1           - menu not enabled                                     *
*   2           - OnStartMenu did not allow the menu to appear         *
*   3           - menu failed                                          *
***********************************************************************/
int CUGCtrl::StartMenu( int col, long row, POINT * point, int section )
{

if ( !m_GI->m_enablePopupMenu )
    return 1;

GetJoinStartCell(&col,&row);

if ( OnMenuStart(col,row,section) == FALSE )
    return 2;

m_menuCol = col;
m_menuRow = row;
m_menuSection = section;
if ( !TrackPopupMenu(m_menu, TPM_LEFTALIGN|TPM_RIGHTBUTTON, point->x, point->y, 0, m_ctrlWnd,NULL) )
    return 3;

return UG_SUCCESS;
}

/***********************************************************************
*                            OnCommand                                 *
***********************************************************************/
BOOL CUGCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
int id;
int cmd;
int len;
HWND   w;
LPTSTR s;

id = LOWORD(  wParam );
cmd = HIWORD( wParam );

// TODO: Add your specialized code here and/or call the base class
if ( id == UG_EDITID && cmd == EN_CHANGE )
    {
    w = (HWND) lParam;
    len = GetWindowTextLength( w );
    if ( len > 0 )
        {
        s = new TCHAR[len + 2];
        GetWindowText( w, s, len+1 );
        QuickSetText( m_editCol, m_editRow, s );
        delete[] s;
        }
    }

/*
----------------------------------
Send a notification for menu items
---------------------------------- */
if ( id >= 1000 && lParam == NULL )
    OnMenuCommand( m_menuCol, m_menuRow, m_menuSection, id);

return UG_SUCCESS;
}

/***********************************************************************
*                              SetMargin                               *
*   Purpose                                                            *
*       Sets the left/right margins that cell types                    *
*       will use when drawing cells.                                   *
*   Params                                                             *
*       pixels - pixels wide to set the margin to                      *
***********************************************************************/
int CUGCtrl::SetMargin( int pixels )
{

if ( pixels < 0 )
    return UG_ERROR;

m_GI->m_margin = pixels;

return UG_SUCCESS;
}

/***********************************************************************
*                             AddFont                                  *
***********************************************************************/
int CUGCtrl::AddFont( LPCTSTR fontName, int height, int weight )
{
return AddFont(height,0,0,0,weight,0,0,0,0,0,0,0,0,fontName);
}

/***********************************************************************
*                             AddFont                                  *
***********************************************************************/
int CUGCtrl::AddFont(int height,int width,int escapement,int orientation,
            int weight,BYTE italic,BYTE underLine,BYTE strikeOut,
            BYTE charSet,BYTE outputPrecision,BYTE clipPrecision,
            BYTE quality,BYTE pitchAndFamily,LPCTSTR fontName)
{

int  count;
int  i;
int  lfBaseSize;
LOGFONT lf;
LOGFONT lf2;
HFONT font;

lfBaseSize = sizeof(LOGFONT) - LF_FACESIZE;

/*
-------------------------------------------
Create a log font structure from the params
------------------------------------------- */
lf.lfHeight         = height;
lf.lfWidth          = width;
lf.lfEscapement     = escapement;
lf.lfOrientation    = orientation;
lf.lfWeight         = weight;
lf.lfItalic         = italic;
lf.lfUnderline      = underLine;
lf.lfStrikeOut      = strikeOut;
lf.lfCharSet        = charSet;
lf.lfOutPrecision   = outputPrecision;
lf.lfClipPrecision  = clipPrecision;
lf.lfQuality        = quality;
lf.lfPitchAndFamily = pitchAndFamily;

if ( lstrlen(fontName) < LF_FACESIZE )
    lstrcpy( lf.lfFaceName, fontName );
else
    lstrcpy( lf.lfFaceName, EmptyString );

/*
-----------------------------------------------------------
Check to see if a font with the same attribs already exists
else add the font
----------------------------------------------------------- */
count = m_fontList->GetCount();
for ( i=0; i<count; i++)
    {
    /*
    ------------------------------------
    Get the LOGFONT for an existing font
    ------------------------------------ */
    font = (HFONT) m_fontList->GetPointer(i);
    GetObject( font, sizeof(LOGFONT), &lf2 );

    /*
    --------------------
    Compare the LOGFONTs
    -------------------- */
    if ( memcmp(&lf, &lf2, lfBaseSize) == 0 )
        {
        if ( lstrcmp(lf.lfFaceName,lf2.lfFaceName) == 0 )
            {
            /*
            -------------------------------------------
            If the LOGFONTs are the same then just
            increment the use count andreturn a pointer
            to the existing font
            ------------------------------------------- */
            m_fontList->UpdateParam( i, m_fontList->GetParam(i)+1 );
            return i;
            }
        }
    }

/*
-----------------------------------------------------
If a matching font was not found above then create it
----------------------------------------------------- */
font = CreateFontIndirect( &lf );
return m_fontList->AddPointer( font );
}

/***********************************************************************
*                            RemoveFont                                *
***********************************************************************/
int CUGCtrl::RemoveFont( int index )
{
int count;
HFONT font;

/*
-----------------------
Decrement the use count
----------------------- */
count = m_fontList->GetParam(index) - 1;
m_fontList->UpdateParam(index,count);

/*
------------------------------------
If the use count is 0 then delete it
------------------------------------ */
if ( count <= 0 )
    {
    font = (HFONT) m_fontList->GetPointer(index);
    if ( font )
        DeleteObject( font );

    return m_fontList->DeletePointer(index);
    }

return UG_SUCCESS;
}

/***********************************************************************
*                            ClearAllFonts                             *
***********************************************************************/
int CUGCtrl::ClearAllFonts()
{
m_fontList->EmptyList();
return UG_SUCCESS;
}

/***********************************************************************
*                              GetFont                                 *
***********************************************************************/
HFONT CUGCtrl::GetFont( int index )
{
return (HFONT) m_fontList->GetPointer(index);
}

/***********************************************************************
*                             SetDefFont                               *
***********************************************************************/
int CUGCtrl::SetDefFont( HFONT font )
{
if ( font == NULL )
    return 1;

if ( m_GI->m_defFont != font )
    {
    clear_default_font();
    m_GI->m_defFont = font;
    }

return UG_SUCCESS;
}

/***********************************************************************
*                             SetDefFont                               *
***********************************************************************/
int CUGCtrl::SetDefFont( int index )
{
HFONT font;

font = (HFONT) m_fontList->GetPointer(index);
return SetDefFont( font );
}

/***********************************************************************
*                              AddBitmap                               *
***********************************************************************/
int CUGCtrl::AddBitmap( UINT resourceID, LPCTSTR resourceName )
{

HBITMAP bitmap;

if ( resourceName == NULL )
    bitmap = LoadBitmap((HINSTANCE)GetWindowLong(m_ctrlWnd,GWL_HINSTANCE), MAKEINTRESOURCE(resourceID));
else
    bitmap = LoadBitmap((HINSTANCE)GetWindowLong(m_ctrlWnd,GWL_HINSTANCE), resourceName);

if ( bitmap == NULL )
    {
    return UG_ERROR;
    }

m_bitmapList->AddPointer(bitmap);

return UG_SUCCESS;
}

/***********************************************************************
*                              AddBitmap                               *
***********************************************************************/
int CUGCtrl::AddBitmap( LPCTSTR fileName )
{
return UG_SUCCESS;
}

/***********************************************************************
*                           RemoveBitmap                               *
***********************************************************************/
int CUGCtrl::RemoveBitmap( int index )
{
return m_bitmapList->DeletePointer(index);
}

/***********************************************************************
*                          ClearAllBitmaps                             *
***********************************************************************/
int CUGCtrl::ClearAllBitmaps()
{
m_bitmapList->EmptyList();
return UG_SUCCESS;
}

/***********************************************************************
*                              GetBitmap                               *
***********************************************************************/
HBITMAP CUGCtrl::GetBitmap( int index )
{
return (HBITMAP) m_bitmapList->GetPointer(index);
}

/***********************************************************************
*                           OnColSwapStart                             *
***********************************************************************/
BOOL CUGCtrl::OnColSwapStart( int col )
{
return TRUE;
}

/***********************************************************************
*                           OnCanColSwap                               *
***********************************************************************/
BOOL CUGCtrl::OnCanColSwap( int fromCol, int toCol )
{
return TRUE;
}

/***********************************************************************
*                           MoveColPosition                            *
***********************************************************************/
int CUGCtrl::MoveColPosition( int fromCol, int toCol, BOOL insertBefore )
{
int fTransCol;
int fWidth;
int i;
CUGDataSource * fDataSource;
CUGCell * fColDefault;

/*
-----------------------------
Check the instert before flag
----------------------------- */
if ( !insertBefore )
    toCol++;

/*
--------------
Range checking
-------------- */
if ( fromCol <0 || fromCol >= m_GI->m_numberCols )
    return 1;
if ( toCol <0 || toCol > m_GI->m_numberCols )
    return 1;

/*
----------------------------------------------------
Check to see if the col position needs to be changed
---------------------------------------------------- */
if ( fromCol == toCol || fromCol+1 == toCol )
    return UG_SUCCESS;

if ( fromCol < toCol )
    toCol --;

/*
---------------------------------------------------------------------
Store the column info for the fromCol - since it will be over-written
--------------------------------------------------------------------- */
fTransCol   = m_GI->m_colInfo[fromCol].colTranslation;
fWidth      = m_GI->m_colInfo[fromCol].width;
fDataSource = m_GI->m_colInfo[fromCol].dataSource;
fColDefault = m_GI->m_colInfo[fromCol].colDefault;

/*
-----------------------------------------------------
Shift the column information over to the new position
----------------------------------------------------- */
if ( fromCol < toCol )
    {
    /*
    -------------------------------------------------------------
    Shift everything bwtween the to and from cols down one column
    ------------------------------------------------------------- */
    for ( i=fromCol; i<toCol; i++ )
        {
        m_GI->m_colInfo[i].colTranslation = m_GI->m_colInfo[i+1].colTranslation;
        m_GI->m_colInfo[i].width         = m_GI->m_colInfo[i+1].width;
        m_GI->m_colInfo[i].dataSource    = m_GI->m_colInfo[i+1].dataSource;
        m_GI->m_colInfo[i].colDefault    = m_GI->m_colInfo[i+1].colDefault;
        }
    }
else
    {
    /*
    -----------------------------------------------------------
    Shift everything bwtween the to and from cols up one column
    ----------------------------------------------------------- */
    for ( i=fromCol; i>toCol; i-- )
        {
        m_GI->m_colInfo[i].colTranslation = m_GI->m_colInfo[i-1].colTranslation;
        m_GI->m_colInfo[i].width         = m_GI->m_colInfo[i-1].width;
        m_GI->m_colInfo[i].dataSource    = m_GI->m_colInfo[i-1].dataSource;
        m_GI->m_colInfo[i].colDefault    = m_GI->m_colInfo[i-1].colDefault;
        }
    }

/*
------------------------------------------------------
Copy the prev. saved col informtion into the to column
------------------------------------------------------ */
m_GI->m_colInfo[toCol].colTranslation = fTransCol;
m_GI->m_colInfo[toCol].width          = fWidth;
m_GI->m_colInfo[toCol].dataSource     = fDataSource;
m_GI->m_colInfo[toCol].colDefault     = fColDefault;

SetLockColWidth();

return UG_SUCCESS;
}

#ifdef UG_ENABLE_PRINTING

/***********************************************************************
*                             PrintInit                                *
*   This function returns the number of pages needed to print the      *
*   entire range, using the current print scale and options            *
***********************************************************************/
int CUGCtrl::PrintInit( HDC hDC, PRINTDLG  pPD, int startCol, long startRow, int endCol, long endRow )
{
return m_CUGPrint->PrintInit(hDC,pPD,startCol,startRow,endCol,endRow);
}

/***********************************************************************
*                              PrintPage                               *
***********************************************************************/
int CUGCtrl::PrintPage( HDC hDC, int pageNum )
{
return m_CUGPrint->PrintPage(hDC,pageNum);
}

/***********************************************************************
*                           PrintSetMargin                             *
***********************************************************************/
int CUGCtrl::PrintSetMargin( int whichMargin, int size )
{
return m_CUGPrint->PrintSetMargin(whichMargin,size);
}

/***********************************************************************
*                           PrintSetScale                              *
***********************************************************************/
int CUGCtrl::PrintSetScale( double scale )
{
return m_CUGPrint->PrintSetScale(scale);
}

/***********************************************************************
*                           PrintSetOption                             *
***********************************************************************/
int CUGCtrl::PrintSetOption( int option, long param )
{
return m_CUGPrint->PrintSetOption(option,param);
}

/***********************************************************************
*                           PrintGetOption                             *
***********************************************************************/
int CUGCtrl::PrintGetOption( int option, long * param )
{
return m_CUGPrint->PrintGetOption(option,param);
}

#endif

/***********************************************************************
*                        SetNewTopHeadingClass                         *
***********************************************************************/
int CUGCtrl::SetNewTopHeadingClass( CUGTopHdg * topHeading )
{
if ( m_CUGTopHdg )
    delete m_CUGTopHdg;

m_CUGTopHdg = topHeading;

return UG_SUCCESS;
}

/***********************************************************************
*                        SetNewSideHeadingClass                        *
***********************************************************************/
int CUGCtrl::SetNewSideHeadingClass( CUGSideHdg * sideHeading )
{
if ( m_CUGSideHdg)
    delete m_CUGSideHdg;

m_CUGSideHdg = sideHeading;

return UG_SUCCESS;
}

/***********************************************************************
*                           SetNewGridClass                            *
***********************************************************************/
int CUGCtrl::SetNewGridClass( CUGGrid * grid )
{
if ( m_CUGGrid )
    delete m_CUGGrid;

m_CUGGrid = grid;

return UG_SUCCESS;
}

/***********************************************************************
*                        SetNewMultiSelectClass                        *
***********************************************************************/
int CUGCtrl::SetNewMultiSelectClass( CUGMultiSelect * multiSelect )
{
if ( m_GI->m_multiSelect )
    delete m_GI->m_multiSelect;

m_GI->m_multiSelect = multiSelect;
return UG_SUCCESS;
}

/***********************************************************************
*                          SetNewTabClass                              *
***********************************************************************/
int CUGCtrl::SetNewTabClass( CUGTab * tab )
{
if ( m_CUGTab )
    delete m_CUGTab;

m_CUGTab         = tab;
m_CUGTab->m_ctrl = this;
m_CUGTab->m_GI   = m_GI;

return UG_SUCCESS;
}

/***********************************************************************
*                         SetNewScrollClass                            *
***********************************************************************/
int CUGCtrl::SetNewVScrollClass( CUGVScroll * scroll )
{
if ( m_CUGVScroll )
    delete m_CUGVScroll;

m_CUGVScroll = scroll;

return UG_SUCCESS;
}

/***********************************************************************
*                         SetNewHScrollClass                           *
***********************************************************************/
int CUGCtrl::SetNewHScrollClass( CUGHScroll * scroll )
{
if ( m_CUGHScroll )
    delete m_CUGHScroll;

m_CUGHScroll = scroll;
return UG_SUCCESS;
}

/***********************************************************************
*                           SetNewEditClass                            *
***********************************************************************/
int CUGCtrl::SetNewEditClass( HWND edit )
{
m_GI->m_editCtrl = edit;

return UG_SUCCESS;
}

/***********************************************************************
*                            GetEditClass                              *
***********************************************************************/
HWND CUGCtrl::GetEditClass()
{
return m_GI->m_editCtrl;
}

/***********************************************************************
*                          AddOverlayObject                            *
***********************************************************************/
int CUGCtrl::AddOverlayObject( HWND wnd, int startCol, int startRow, int endCol, int endRow )
{
return UG_SUCCESS;
}

/***********************************************************************
*                          SetOverlayObjectPos                         *
***********************************************************************/
int CUGCtrl::SetOverlayObjectPos( int index,int startCol,int startRow,
                        int endCol, int endRow, int leftOffset,
                        int topOffset, int rightOffset, int bottomOffset )
{
return UG_SUCCESS;
}

/***********************************************************************
*                          GetOverlayObjectPos                         *
***********************************************************************/
int CUGCtrl::GetOverlayObjectPos( int index, int * startCol, int * startRow,
                        int * endCol, int * endRow, int * leftOffset,
                        int * topOffset, int* rightOffset,
                        int * bottomOffset )
{
return UG_SUCCESS;
}

/***********************************************************************
*                         GetOverlayObjectPos                          *
***********************************************************************/
int CUGCtrl::GetOverlayObjectPos( int index, int * startCol, int * startRow, int* endCol,int * endRow )
{
return UG_SUCCESS;
}

/***********************************************************************
*                        RemoveOverlayObject                           *
***********************************************************************/
int CUGCtrl::RemoveOverlayObject( int index )
{
return UG_SUCCESS;
}

/***********************************************************************
*                          GetOverlayObject                            *
***********************************************************************/
HWND CUGCtrl::GetOverlayObject( int index )
{
return UG_SUCCESS;
}

/***********************************************************************
*                          SetTrackingWindow                           *
***********************************************************************/
int CUGCtrl::SetTrackingWindow( HWND hWnd )
{
if ( hWnd )
    {
    m_trackingWnd = hWnd;
    return UG_SUCCESS;
    }

return UG_ERROR;
}

/***********************************************************************
*                      SetTrackingWindowMode                           *
***********************************************************************/
int CUGCtrl::SetTrackingWindowMode( int mode )
{
m_GI->m_trackingWndMode = mode;
return UG_SUCCESS;
}

/***********************************************************************
*                      MoveTrackingWindow                              *
***********************************************************************/
void CUGCtrl::MoveTrackingWindow()
{

RECT  tRect;
RECT  origTRect;
RECT  rect;
POINT point;
BOOL  moveWindow;
int   sWidth;
int   sHeight;
int   tWidth;
int   tHeight;
int   rightInc;
int   downInc;

if ( m_trackingWnd == NULL)
    return;

moveWindow = FALSE;
sWidth     = GetSystemMetrics( SM_CXSCREEN );
sHeight    = GetSystemMetrics( SM_CYSCREEN );

GetWindowRect(m_trackingWnd, &origTRect);
GetWindowRect(m_trackingWnd, &tRect);
point.x = tRect.left;
point.y = tRect.top;
ScreenToClient(m_CUGGrid->m_gridWnd, &point);
tRect.left = point.x;
tRect.top = point.y;
point.x = tRect.right;
point.y = tRect.bottom;
ScreenToClient(m_CUGGrid->m_gridWnd, &point);
tRect.right = point.x;
tRect.bottom = point.y;

GetCellRect(m_GI->m_dragCol,m_GI->m_dragRow,&rect);

if ( m_GI->m_trackingWndMode == 1)
    {
    /*
    ---------------
    Stay close mode
    --------------- */
    tWidth       = tRect.right - tRect.left;
    tHeight      = tRect.bottom - tRect.top;
    tRect.left   = rect.right +30;
    tRect.right  = tRect.left + tWidth;
    tRect.top    = rect.top;
    tRect.bottom = rect.top + tHeight;
    point.x      = tRect.left;
    point.y      = tRect.top;
    ClientToScreen( m_CUGGrid->m_gridWnd, &point );

    tRect.left = point.x;
    tRect.top  = point.y;
    point.x    = tRect.right;
    point.y    = tRect.bottom;
    ClientToScreen( m_CUGGrid->m_gridWnd, &point );

    tRect.right  = point.x;
    tRect.bottom = point.y;
    moveWindow   = TRUE;
    }

if ( tRect.top < rect.bottom && tRect.bottom > rect.top && tRect.left < rect.right && tRect.right > rect.left )
    {

    rightInc = rect.right - tRect.left + 5;
    downInc = rect.bottom - tRect.top + 5;

    if ( origTRect.right+rightInc < sWidth )
        {
        /*
        ----------------------------------------------------
        Move the tracking rect over to the right if possible
        ---------------------------------------------------- */
        GetWindowRect(m_trackingWnd, &tRect);
        tRect.left += rightInc;
        tRect.right += rightInc;
        moveWindow = TRUE;
        }
    else if ( origTRect.bottom + downInc < sHeight )
        {
        /*
        --------------------------------
        Else move the tracking rect down
        -------------------------------- */
        GetWindowRect(m_trackingWnd, &tRect);
        tRect.top += downInc;
        tRect.bottom += downInc;
        moveWindow = TRUE;
        }
    else
        {
        rightInc = rect.left - tRect.right  - 5;
        downInc  = rect.top  - tRect.bottom - 5;

        if ( origTRect.left+rightInc > 0 )
            {
            /*
            ----------------------------------
            Move the tracking rect to the left
            ---------------------------------- */
            GetWindowRect( m_trackingWnd, &tRect );
            tRect.left  += rightInc;
            tRect.right += rightInc;
            moveWindow = TRUE;
            }
        else if ( origTRect.bottom + downInc < sHeight )
            {
            /*
            -------------------------
            Move the tracking rect up
            ------------------------- */
            GetWindowRect( m_trackingWnd, &tRect );
            tRect.top    += downInc;
            tRect.bottom += downInc;
            moveWindow = TRUE;
            }
        }
    }

if ( moveWindow )
    {
    OnTrackingWindowMoved( &origTRect,&tRect );
    MoveWindow( m_trackingWnd, tRect.left, tRect.top, tRect.right - tRect.left, tRect.bottom - tRect.top,TRUE );
    UpdateWindow( m_trackingWnd );
    RedrawAll();
    }
}

/***********************************************************************
*                        OnTrackingWindowMoved                         *
***********************************************************************/
void CUGCtrl::OnTrackingWindowMoved( RECT * origRect, RECT * newRect )
{
}

/***********************************************************************
*                          EnableUpdate                                *
***********************************************************************/
int CUGCtrl::EnableUpdate( BOOL state )
{
m_enableUpdate = state;

return UG_SUCCESS;
}

#ifdef UG_ENABLE_HINTS
/***********************************************************************
*                              OnHint                                  *
***********************************************************************/
int CUGCtrl::OnHint( int col, long row, int section, LPTSTR string )
{
return TRUE;
}
#endif

#ifdef UG_ENABLE_SCROLLHINTS
/***********************************************************************
*                           OnVScrollHint                              *
***********************************************************************/
int CUGCtrl::OnVScrollHint( long row, LPTSTR string )
{
return TRUE;
}

/***********************************************************************
*                           OnHScrollHint                              *
***********************************************************************/
int CUGCtrl::OnHScrollHint( int col, LPTSTR string )
{
return TRUE;
}
#endif

/***********************************************************************
*                        SetUserSizingMode                             *
***********************************************************************/
int CUGCtrl::SetUserSizingMode( BOOL state )
{
m_GI->m_userSizingMode = state;

return UG_SUCCESS;
}

/***********************************************************************
*                        SetColDataSource                              *
***********************************************************************/
int CUGCtrl::SetColDataSource( int col, int dataSrcIndex )
{

if ( dataSrcIndex < 0 || dataSrcIndex > m_dataSrcListLength )
    return UG_ERROR;

return SetColDataSource( col, m_dataSrcList[dataSrcIndex] );
}

/***********************************************************************
*                       SetColDataSource                               *
***********************************************************************/
int CUGCtrl::SetColDataSource( int col, CUGDataSource * dataSrc )
{

if ( col < 0 || col >= m_GI->m_numberCols )
    return UG_ERROR;

if ( dataSrc == NULL )
    return UG_ERROR;

m_GI->m_colInfo[col].dataSource = dataSrc;
return UG_SUCCESS;
}

/***********************************************************************
*                         CreateCtrlWindow                             *
***********************************************************************/
BOOL CUGCtrl::CreateCtrlWindow(DWORD dwStyle, RECT rect, HWND hWndParent, UINT nID )
{
static TCHAR UGCtrlName[] = TEXT( "UGCtrl" );
HINSTANCE hInstance;
WNDCLASS wc;

if ( hWndParent == NULL )
    return UG_ERROR;

hInstance = (HINSTANCE) GetWindowLong( hWndParent, GWL_HINSTANCE );

/*
----------------------------------------------------
Register this new window class for the ultimate grid
---------------------------------------------------- */
wc.style          = CS_HREDRAW | CS_VREDRAW;
wc.lpfnWndProc    = CtrlWndProc;
wc.cbClsExtra     = 0;
wc.cbWndExtra     = 20;
wc.hInstance      = hInstance ;
wc.hIcon          = NULL;
wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
wc.hbrBackground  = NULL;
wc.lpszMenuName   = NULL;
wc.lpszClassName  = UGCtrlName;

if ( !GetClassInfo(hInstance,UGCtrlName, &wc) )
    {
    RegisterClass( &wc );
    if ( !GetClassInfo( hInstance, UGCtrlName, &wc) )
        return UG_ERROR;
    }

dwStyle |= WS_CLIPCHILDREN;
m_ctrlWnd = CreateWindow(
    UGCtrlName,
    EmptyString,
    dwStyle,
    rect.left, rect.top, rect.right, rect.bottom,
    hWndParent,
    (HMENU)nID,
    hInstance,
    NULL );

if ( m_ctrlWnd == NULL )
    return UG_ERROR;

SendMessage( m_ctrlWnd, UG_THIS_PTR_CTRL, 0, (LPARAM) this );

CreateChildWindows();

return UG_SUCCESS;
}

/***********************************************************************
*                            CTRLWNDPROC                               *
***********************************************************************/
long CALLBACK CUGCtrl::CtrlWndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
CUGCtrl * cp;

switch ( msg )
    {
    case WM_NCCREATE:
        SetWindowLong(hWnd, GWL_USERDATA, (LPARAM)NULL);
        return 1;

    case UG_THIS_PTR_CTRL:
        /*
        --------------------------------------
        Store the pointer to the calling class
        -------------------------------------- */
        SetWindowLong(hWnd, GWL_USERDATA, lParam);
        return 1;

    }

cp = (CUGCtrl *) GetWindowLong( hWnd, GWL_USERDATA );
if ( !cp )
    return DefWindowProc( hWnd, msg, wParam, lParam );

switch( msg )
    {
    case WM_COMMAND:
        cp->OnCommand( wParam, lParam );
        return 1;

    case WM_PAINT:
        cp->OnPaint();
        return 0;

    case WM_SETFOCUS:
        cp->OnSetFocus((HWND) wParam);
        return 0;

    case WM_SIZE:
        cp->OnSize( wParam, LOWORD(lParam), HIWORD(lParam) );
        return 0;

    case WM_SYSCOLORCHANGE:
        cp->OnSysColorChange();
        return 0;

    case WM_HSCROLL:
        cp->OnHScroll( LOWORD(wParam), HIWORD(wParam), (HWND)lParam );
        return 0;

    case WM_VSCROLL:
        cp->OnVScroll( LOWORD(wParam), HIWORD(wParam), (HWND)lParam );
        return 0;

    case WM_GETDLGCODE:
        cp->OnGetDlgCode();
        return DLGC_WANTALLKEYS|DLGC_WANTARROWS;

    default:
        if ( cp != NULL && msg == cp->m_FindReplaceMsg )
            {
            cp->ProcessFindDialog( wParam, lParam );
            return 0;
            }
        return DefWindowProc( hWnd, msg, wParam, lParam );
    }
}
