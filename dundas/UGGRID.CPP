#include <windows.h>

#include "UGCtrl.h"
#include <commctrl.h>

/***********************************************************************
*                               CUGGrid                                *
***********************************************************************/
CUGGrid::CUGGrid()
{

m_gridWnd           = 0;
m_tooltipWnd        = 0;
m_doubleBufferMode  = FALSE;
m_keyRepeatCount    = 0;
m_paintOverRide     = FALSE;
m_hasFocus          = FALSE;
m_cellTypeCapture   = FALSE;
m_bitmap            = 0;

}

/***********************************************************************
*                              ~CUGGrid                                *
***********************************************************************/
CUGGrid::~CUGGrid()
{
if ( m_bitmap != NULL )
    DeleteObject( m_bitmap );
}


/***********************************************************************
*                               ONPAINT                                *
*                                                                      *
*   This routine is responsible for gathering the information to       *
*   draw, get the selected state plus draw in an optomized fashion     *
*                                                                      *
***********************************************************************/
void CUGGrid::OnPaint()
{

PAINTSTRUCT paintstru;
HDC dc;

dc = BeginPaint( m_gridWnd, &paintstru );

/*
--------------------------------------
Redraw the cells in any invalid region
-------------------------------------- */
RECT rect;
if( GetClipBox(dc, &rect) != NULLREGION)
    {
    int startCol, endCol;
    long startRow, endRow;
    if(m_ctrl->GetCellFromPoint(rect.left, rect.top, &startCol, &startRow) != UG_SUCCESS)
        {
        startCol = 0;
        startRow = 0;
        }
    if(m_ctrl->GetCellFromPoint(rect.right, rect.bottom, &endCol, &endRow) != UG_SUCCESS)
        {
        endCol = m_GI->m_numberCols;
        endRow = m_GI->m_numberRows;

        }
    m_drawHint.AddHint(0, 0, m_GI->m_numberCols, m_GI->m_numberRows);
    }
else
    {
    m_drawHint.AddHint(0, 0, m_GI->m_numberCols, m_GI->m_numberRows);
    }

HDC     db_dc  = NULL;
HBITMAP bitmap = NULL;

if ( m_doubleBufferMode )
    {
    db_dc = CreateCompatibleDC(NULL);
    bitmap = (HBITMAP)SelectObject(db_dc, m_bitmap);
    }

DrawCellsIntern(&dc,db_dc);

if ( db_dc != NULL )
    {
    if ( bitmap != NULL )
        SelectObject( db_dc, bitmap );
    DeleteDC( db_dc );
    }

EndPaint( m_gridWnd, &paintstru );
}

/***********************************************************************
*                           DrawCellsIntern                            *
***********************************************************************/
void CUGGrid::DrawCellsIntern(HDC *dc, HDC db_dc)
{
RECT        rect = {0,0,0,0};
RECT        cellRect;

CUGCell     cell,tempCell;
LPTSTR      string = NULL;

RECT        focusRect = {-1,-1,-1,-1};
CUGCellType * cellType;

int         rightBlank  = -1;
int         bottomBlank = -1;

int         col,x;
long        row,y;

int         selectBlock;

int         dcID;
HDC         origDC;

HFONT       orgFont1 = NULL;
HFONT       orgFont2 = NULL;
HDC         hDC1, hDC2;

/*
----------------
Double buffering
---------------- */
if ( m_doubleBufferMode )
    {
    origDC = *dc;
    *dc =  db_dc;

    /*
    ---------------------------
    Get the default font to use
    --------------------------- */
    if ( m_GI->m_defFont != NULL )
        {
        orgFont2 = (HFONT) SelectObject( origDC, m_GI->m_defFont );
        orgFont1 = (HFONT) SelectObject( *dc,    m_GI->m_defFont );
        hDC2 = origDC;
        hDC1 = *dc;
        }
    }
else
    {
    /*
    ---------------------------
    Get the default font to use
    --------------------------- */
    if ( m_GI->m_defFont != NULL )
        {
        orgFont1 = (HFONT) SelectObject( *dc, m_GI->m_defFont );
        hDC1 = *dc;
        }
    }

m_ctrl->OnScreenDCSetup( origDC, db_dc, UG_GRID );


/*
-------------------------------------------------------
Set the right and bottom vars to point to the extremes,
if the right or bottom is sooner then they will be
updated in the main drawing loop
------------------------------------------------------- */
m_GI->m_rightCol  = m_GI->m_numberCols;
m_GI->m_bottomRow = m_GI->m_numberRows;

/*
----------------------------------------------------------------
Main draw loop, this loop goes through all visible cells and
checks to see if they need redrawing if they do then the cell is
retrieved and drawn
---------------------------------------------------------------- */
for ( y=0; y<m_GI->m_numberRows; y++ )
    {
    /*
    ----------------------------------
    Skip rows hidden under locked rows
    ---------------------------------- */
    if ( y == m_GI->m_numLockRows )
        y = m_GI->m_topRow;

    row = y;

    //calc the top bottom and right side of the rect
    //for the current cell to be drawn
    rect.top = rect.bottom;

    if(m_GI->m_uniformRowHeightFlag)
        rect.bottom += m_GI->m_defRowHeight;
    else
        rect.bottom += m_GI->m_rowHeights[row];

    if(rect.top == rect.bottom)
        continue;

    rect.right = 0;

    /*
    ---------------------------------------------
    Check all visible cells in the current row to
    see if they need drawing
    --------------------------------------------- */
    for ( x=0; x<m_GI->m_numberCols; x++ )
        {
        /*
        ----------------------------------
        Skip cols hidden under locked cols
        ---------------------------------- */
        if ( x == m_GI->m_numLockCols )
            x = m_GI->m_leftCol;

        row = y;
        col = x;

        /*
        ----------------------------------------
        Calc the left and right side of the rect
        ---------------------------------------- */
        rect.left   = rect.right;
        rect.right += m_GI->m_colInfo[col].width;

        if ( rect.left == rect.right )
            continue;

        /*
        -------------------------------------------
        Check to see if the cell need to be redrawn
        ------------------------------------------- */
        if ( m_drawHint.IsInvalid(col, row) != FALSE )
            {

            /*
            -----------------------------------------------
            Copy the rect, then use the cellRect from here
            this is done since the cellRect may be modified
            ----------------------------------------------- */
            CopyRect(&cellRect, &rect);

            /*
            --------------------
            Get the cell to draw
            -------------------- */
            m_ctrl->GetCellIndirect( col, row, &cell );

            /*
            ----------------------------------
            Check to see if the cell is joined
            ---------------------------------- */
            if ( cell.IsPropertySet(UGCELL_JOIN_SET) )
                {
                m_ctrl->GetCellRect( col, row, &cellRect );
                m_ctrl->GetJoinStartCell( &col, &row, &cell );
                if ( m_drawHint.IsValid(col, row) )
                    continue;
                m_drawHint.SetAsValid(col, row);
                }

            /*
            ----------------------------------
            Get the cell type to draw the cell
            ---------------------------------- */
            if(cell.IsPropertySet(UGCELL_CELLTYPE_SET)){
                cellType = m_ctrl->GetCellType(cell.GetCellType());
            }
            else{
                cellType = m_ctrl->GetCellType(-1);
            }


            dcID = SaveDC( *dc );

            /*
            --------------------------------------------------------------
            Draw the cell, check to see if it is 'current' and/or selected
            -------------------------------------------------------------- */
            if ( row == m_GI->m_currentRow && ( col == m_GI->m_currentCol || m_GI->m_highlightRowFlag) )
                {
                cellType->OnDraw(*dc, &cellRect, col, row, &cell, 0, 1);
                }
            else
                {
                if(m_GI->m_multiSelect->IsSelected(col, row, &selectBlock))
                    cellType->OnDraw( *dc, &cellRect, col, row, &cell, selectBlock+1, 0 );
                else
                    cellType->OnDraw( *dc, &cellRect, col, row, &cell, 0, 0 );
                }

            /*
            ---------------------------------------------------------
            Draw a black line at the right edge of the locked columns
            --------------------------------------------------------- */
            if ( m_GI->m_numLockCols > 0 )
                {
                if ( col == m_GI->m_leftCol )
                    {
                    SelectObject(*dc, GetStockObject(BLACK_PEN));
                    MoveToEx(*dc, cellRect.left-1, cellRect.top, NULL);
                    LineTo(*dc, cellRect.left-1, cellRect.bottom+1);
                    }
                else if ( col == m_GI->m_numLockCols-1 )
                    {
                    SelectObject(*dc, GetStockObject(BLACK_PEN));
                    MoveToEx(*dc, cellRect.right-1, cellRect.top, NULL);
                    LineTo(*dc, cellRect.right-1, cellRect.bottom+1);
                    }
                }

            /*
            --------------------------------------------------
            Draw a black line at the bottom of the locked rows
            -------------------------------------------------- */
            if ( m_GI->m_numLockRows > 0 )
                {
                if ( row == m_GI->m_topRow )
                    {
                    SelectObject(*dc, GetStockObject(BLACK_PEN));
                    MoveToEx(*dc, cellRect.left, cellRect.top-1, NULL);
                    LineTo(*dc, cellRect.right+1, cellRect.top-1);
                    }
                else if ( row == m_GI->m_numLockRows-1 )
                    {
                    SelectObject(*dc, GetStockObject(BLACK_PEN));
                    MoveToEx(*dc, cellRect.left, cellRect.bottom-1, NULL);
                    LineTo(*dc, cellRect.right+1, cellRect.bottom-1);
                    }
                }

            RestoreDC(*dc, dcID);
            }

        /*
        ---------------------------------------------------------------
        Check to see if the focus rect should be drawn this
        function should be called all the time even if it is off screen
        --------------------------------------------------------------- */
        if ( row == m_GI->m_currentRow && (col == m_GI->m_currentCol || m_GI->m_highlightRowFlag) )
            CopyRect( &focusRect, &cellRect );

        /*
        -----------------------------------------------------------
        Check to see if the right side of the rect is past the edge
        of the grid drawing area, if so then break
        ----------------------------------------------------------- */
        if ( rect.right > m_GI->m_gridWidth )
            {
            m_GI->m_rightCol = col;
            break;
            }

        }

    /*
    ---------------------------------------------------------
    Check to see if there is blank space on the right side of
    the grid drawing area
    --------------------------------------------------------- */
    if ( rect.right < m_GI->m_gridWidth && m_GI->m_rightCol == m_GI->m_numberCols )
        rightBlank = rect.right;

    /*
    ----------------------------------------------------------------
    Check to see if the bottom of the rect is past the bottom of the
    grid drawing area, if so then break
    ---------------------------------------------------------------- */
    if ( rect.bottom > m_GI->m_gridHeight )
        {
        m_GI->m_bottomRow = row;
        break;
        }

    /*
    --------------------
    Check for extra rows
    -------------------- */
    if ( y >= (m_GI->m_numberRows-1) )
        {
        long origNumRows = m_GI->m_numberRows;
        long newRow = y+1;
        m_ctrl->VerifyCurrentRow(&newRow);
        if ( m_GI->m_numberRows > origNumRows )
            m_drawHint.AddHint(0, y+1, m_GI->m_numberCols-1, y+1);
        }
    }

/*
--------------------
Clear the draw hints
-------------------- */
m_drawHint.ClearHints();

/*
--------------------------------------------------------------
Check to see if there is blank space on the bottom of the grid
drawing area
-------------------------------------------------------------- */
if ( rect.bottom < m_GI->m_gridHeight && m_GI->m_bottomRow == m_GI->m_numberRows )
    bottomBlank = rect.bottom;

/*
------------------------------------
Fill in the blank grid drawing areas
------------------------------------ */
if ( rightBlank>=0 || bottomBlank>=0 )
    {
    //HBRUSH brush = CreateSolidBrush( m_ctrl->OnGetDefBackColor(UG_GRID) ); 02-Dec-02 Jay
    HBRUSH brush = CreateSolidBrush( GetSysColor(COLOR_BTNFACE) );

    GetClientRect( m_gridWnd, &rect );

    if ( rightBlank >=0 )
        {
        rect.left = rightBlank;
        FillRect( *dc, &rect, brush );
        }

    if ( bottomBlank >=0 )
        {
        rect.left = 0;
        rect.top = bottomBlank;
        FillRect( *dc, &rect, brush );
        }

    DeleteObject( brush );
    }

/*
----------------
Double buffering
---------------- */
if ( m_doubleBufferMode )
    {
    *dc = origDC;
    BitBlt( *dc, 0, 0, m_GI->m_gridWidth, m_GI->m_gridHeight, db_dc, 0, 0, SRCCOPY );
    }

/*
----------------------------------------------
Draw the focus rect, if the flag was set above
---------------------------------------------- */
if ( m_hasFocus || m_ctrl->m_findDialogRunning )
    {
    if ( m_GI->m_highlightRowFlag )
        {
        focusRect.left = 0;
        if ( rect.right < m_GI->m_gridWidth )
            focusRect.right = rect.right;
        else
            focusRect.right = m_GI->m_gridWidth;
        }
    m_ctrl->OnDrawFocusRect( *dc, &focusRect );
    }

if ( string != NULL )
    delete [] string;

if ( orgFont1 != NULL )
    SelectObject( hDC1, orgFont1 );

if ( orgFont2 != NULL )
    SelectObject( hDC2, orgFont2 );
}

void CUGGrid::RedrawCell(int col,long row)
{
m_drawHint.AddHint(col, row);
Moved();
}

void CUGGrid::RedrawRow(long row)
{
m_drawHint.AddHint(0, row, m_GI->m_numberCols-1, row);
Moved();
}

void CUGGrid::RedrawCol(int col)
{
m_drawHint.AddHint(col,0,col,m_GI->m_numberRows-1);
Moved();
}

void CUGGrid::RedrawRange(int startCol, long startRow, int endCol, long endRow)
{
m_drawHint.AddHint(startCol, startRow, endCol, endRow);
Moved();
}

/************************************************
Update
    This routine recalcs the internal variables
    plus causes the grid to re-draw itself
*************************************************/
void CUGGrid::Update()
{
InvalidateRect(m_gridWnd, NULL, TRUE);
UpdateWindow(m_gridWnd);
}

/***********************************************************************
*                               CUGGrid                                *
*                                Moved                                 *
***********************************************************************/
void CUGGrid::Moved()
{
int  xScroll = 0;
int  yScroll = 0;
int  x;
long y;
HDC  pDC;
HDC  screenDC;
HDC  db_DC;

if ( GetUpdateRect(m_gridWnd, NULL, FALSE) )
    {
    UpdateWindow(m_gridWnd);
    return;
    }

BOOL redrawAll = FALSE;

/*
---------------------------------------------------------
If the left col has changed then calc the scroll distance
--------------------------------------------------------- */
if ( m_GI->m_leftCol != m_GI->m_lastLeftCol )
    {
    if ( m_GI->m_leftCol > m_GI->m_lastLeftCol )
        {
        for( x=m_GI->m_lastLeftCol; x<m_GI->m_leftCol; x++ )
            {
            xScroll += m_ctrl->GetColWidth( x );
            if ( xScroll > m_GI->m_gridWidth )
                {
                redrawAll = TRUE;
                break;
                }
            }
        }
    else
        {
        for ( x=m_GI->m_leftCol; x<m_GI->m_lastLeftCol; x++ )
            {
            xScroll -= m_ctrl->GetColWidth( x );
            if ( xScroll < -m_GI->m_gridWidth )
                {
                redrawAll = TRUE;
                break;
                }
            }
        }

    if ( redrawAll )
        {
        xScroll = 0;
        /*
        ---------------------
        Redraw the whole grid
        --------------------- */
        m_drawHint.AddHint(0, 0, m_GI->m_numberCols, m_GI->m_numberRows);
        }
    }

/*
--------------------------------------------------------
If the top row has changed then calc the scroll distance
-------------------------------------------------------- */
if ( m_GI->m_topRow != m_GI->m_lastTopRow )
    {
    if ( m_GI->m_topRow > m_GI->m_lastTopRow )
        {
        for ( y=m_GI->m_lastTopRow; y<m_GI->m_topRow; y++ )
            {
            yScroll += m_ctrl->GetRowHeight( y );
            if ( yScroll > m_GI->m_gridHeight )
                {
                redrawAll = TRUE;
                break;
                }
            }
        }
    else
        {
        for ( y=m_GI->m_topRow; y<m_GI->m_lastTopRow; y++ )
            {
            yScroll -= m_ctrl->GetRowHeight( y );
            if ( yScroll < -m_GI->m_gridHeight )
                {
                redrawAll = TRUE;
                break;
                }
            }
        }

    if(redrawAll)
        {
        yScroll = 0;
        /*
        ---------------------
        Redraw the whole grid
        --------------------- */
        m_drawHint.AddHint( 0, 0, m_GI->m_numberCols, m_GI->m_numberRows );
        }
    }

/*
-------------
Create the dc
------------- */
HBITMAP orgBitmap = NULL;
screenDC = GetDC( m_gridWnd );

if ( m_doubleBufferMode )
    {
    pDC = CreateCompatibleDC( NULL );
    orgBitmap = (HBITMAP) SelectObject( pDC, m_bitmap );
    db_DC = pDC;
    }
else
    {
    pDC = screenDC;
    }

/*
-------------------------------
Do scrolling of the window here
------------------------------- */
if ( xScroll != 0 || yScroll != 0 )
    {
    RECT scrollRect;
    GetClientRect( m_gridWnd, &scrollRect );
    if ( xScroll == 0 )
        scrollRect.top += m_GI->m_lockRowHeight;
    if ( yScroll == 0 )
        scrollRect.left += m_GI->m_lockColWidth;
    ScrollDC( pDC, -xScroll, -yScroll, &scrollRect, &scrollRect, NULL, NULL );

    /*
    ---------------------------------------------------------------
    Add the draw hints for the area the was uncovered by the scroll
    --------------------------------------------------------------- */
    if ( xScroll > 0 )
        m_drawHint.AddHint( m_GI->m_rightCol, 0, m_GI->m_numberCols, m_GI->m_numberRows);
    else if ( xScroll < 0 )
        m_drawHint.AddHint( 0, 0, m_GI->m_lastLeftCol, m_GI->m_numberRows);
    if ( yScroll > 0 )
        m_drawHint.AddHint( 0, m_GI->m_bottomRow, m_GI->m_numberCols, m_GI->m_numberRows);
    else if ( yScroll < 0 )
        m_drawHint.AddHint( 0, 0, m_GI->m_numberCols, m_GI->m_lastTopRow);
}

/*
------------------------------------------------------
Add the last and current cells - add row support later
------------------------------------------------------ */
if ( m_GI->m_highlightRowFlag )
    {
    m_drawHint.AddHint( 0, m_GI->m_lastRow, m_GI->m_numberCols, m_GI->m_lastRow );
    m_drawHint.AddHint( 0, m_GI->m_currentRow, m_GI->m_numberCols, m_GI->m_currentRow);
    }
else
    {
    m_drawHint.AddHint( m_GI->m_lastCol, m_GI->m_lastRow);
    m_drawHint.AddHint( m_GI->m_currentCol, m_GI->m_currentRow);
    }

/*
-----------------------------------
Call the grids main drawing routine
----------------------------------- */
DrawCellsIntern( &screenDC, db_DC );

if ( pDC != NULL && m_doubleBufferMode )
    {
    if ( orgBitmap != NULL )
        SelectObject( pDC, orgBitmap );
    DeleteDC(pDC);
    }

/*
------------------------
Close the device context
------------------------ */
ReleaseDC(m_gridWnd, screenDC);
}

/************************************************
OnKeyDown
    Passes all keystrokes to a callback routine
    then processes them, if the callback allows
*************************************************/
void CUGGrid::OnKeyDown(UINT nChar, UINT nFlags)
{

    m_GI->m_moveType = 0;   //key(default)

    int increment = 1; //default number of units to move

    //keyboard balistics
    if(m_GI->m_ballisticKeyMode > 0){

        m_keyRepeatCount++;

        int value = (m_keyRepeatCount / m_GI->m_ballisticKeyMode);
        increment = value*value*value + 1;

        if(m_GI->m_ballisticKeyDelay >0){
            if(value == 0 && m_keyRepeatCount >1){
                #ifdef WIN32
                Sleep(m_GI->m_ballisticKeyDelay);
                #endif
            }
        }
    }


    //send a notify message to the cell type class
    BOOL processed = m_ctrl->GetCellType(m_GI->m_currentCol, m_GI->m_currentRow)->
        OnKeyDown(m_GI->m_currentCol, m_GI->m_currentRow, &nChar);

    //send a keydown notify message
    m_ctrl->OnKeyDown(&nChar, processed);

    //process the keystroke
    if(nChar == VK_LEFT)
        m_ctrl->GotoCol(m_GI->m_dragCol - increment);
    else if(nChar == VK_RIGHT)
        m_ctrl->GotoCol(m_GI->m_dragCol + increment);
    else if(nChar == VK_UP)
        m_ctrl->GotoRow(m_GI->m_dragRow - increment);
    else if(nChar == VK_DOWN)
        m_ctrl->GotoRow(m_GI->m_dragRow + increment);
    else if(nChar == VK_PRIOR)
        m_ctrl->MoveCurrentRow(UG_PAGEUP);
    else if(nChar == VK_NEXT)
        m_ctrl->MoveCurrentRow(UG_PAGEDOWN);
    else if(nChar == VK_HOME){
        if(GetKeyState(VK_CONTROL) <0)
            m_ctrl->MoveCurrentRow(UG_TOP);
        else
            m_ctrl->MoveCurrentCol(UG_TOP);
    }
    else if(nChar == VK_END){
        if(GetKeyState(VK_CONTROL) <0)
            m_ctrl->MoveCurrentRow(UG_BOTTOM);
        else
            m_ctrl->MoveCurrentCol(UG_BOTTOM);
    }
}

/************************************************
OnLButtonDown
    Finds the cell that was clicked in
    Sends a notification
    updates the current cells position
    Captures the mouse focus
*************************************************/
void CUGGrid::OnLButtonDown(UINT nFlags, POINT point){

    if(m_ctrl->m_editInProgress)
        return;

    int col = -1;
    long row = -1;
    RECT rect;
    BOOL processed = FALSE;

    SetCapture(m_gridWnd);

    //setup the move type flags
    m_GI->m_moveType = 1;   //lbutton
    m_GI->m_moveFlags = nFlags;

    //check to see what cell was clicked in, and move there
    if(m_ctrl->GetCellFromPoint(point.x, point.y, &col, &row, &rect) == UG_SUCCESS){

        m_ctrl->GotoCell(col, row);

        //send a notification to the cell type
        processed = m_ctrl->GetCellType(col, row)->OnLClicked(col, row, 1, &rect, &point);
        if(processed)
            m_cellTypeCapture = TRUE;
    }

    m_ctrl->OnLClicked(col, row, 1, &rect, &point, processed);

    m_GI->m_moveType = 0;   //key(default)
}

/************************************************
OnLButtonUp
    Finds the cell that was clicked in
    Sends a notification
    Releases the mouse focus
*************************************************/
void CUGGrid::OnLButtonUp(UINT nFlags, POINT point){

    int col = -1;
    long row = -1;
    RECT rect;
    BOOL processed = FALSE;

    if(m_ctrl->GetCellFromPoint(point.x, point.y, &col, &row, &rect) == UG_SUCCESS){

        //send a notification to the cell type
        processed = m_ctrl->GetCellType(col,row)->OnLClicked(col, row, 0, &rect, &point);
    }

    m_ctrl->OnLClicked(col, row, 0, &rect, &point, processed);

    ReleaseCapture();

    m_cellTypeCapture = FALSE;
}

/************************************************
OnLButtonDblClk
    Finds the cell that was clicked in
    Sends a notification
*************************************************/
void CUGGrid::OnLButtonDblClk(UINT nFlags, POINT point){

    int col = -1;
    long row = -1;
    RECT rect;
    BOOL processed = FALSE;

    if(m_ctrl->GetCellFromPoint(point.x, point.y, &col, &row, &rect) == UG_SUCCESS){

        //send a notification to the cell type
        processed = m_ctrl->GetCellType(col, row)->OnDClicked(col, row, &rect, &point);
    }

    m_ctrl->OnDClicked(col, row, &rect, &point, processed);
}

/************************************************
OnMouseMove
    Finds the cell that the mouse is in and if the
    left mouse button is down then the current cell
    postion is updated
    Sends optional notification messages
    Also scrolls the current cell if the mouse is
    outside the window area
*************************************************/
void CUGGrid::OnMouseMove(UINT nFlags, POINT point){

    int col;
    long row;

    if(m_ctrl->m_editInProgress)
        return;

    if(m_cellTypeCapture){
        col = m_GI->m_currentCol;
        row = m_GI->m_currentRow;
        //send a notification to the cell type
        BOOL processed = m_ctrl->GetCellType(col, row)->OnMouseMove(col, row, &point, nFlags);
        //send a notification to the main grid class
        m_ctrl->OnMouseMove(col, row, &point, nFlags, processed);

        return;
    }


    m_GI->m_moveType = 3;   //mouse move
    m_GI->m_moveFlags = nFlags;

    //check to see if the mouse is over a cell
    if(m_ctrl->GetCellFromPoint((short)point.x, (short)point.y, &col, &row) == UG_SUCCESS){
        //send a notification to the cell type
        BOOL processed = m_ctrl->GetCellType(col, row)->OnMouseMove(col, row, &point, nFlags);
        //send a notification to the main grid class
        m_ctrl->OnMouseMove(col, row, &point, nFlags, processed);
    }

    if(nFlags & MK_LBUTTON){

        MSG msg;
        int moved = FALSE;

        while(1){
            //if the mouse is off the left side
            if((short)point.x < 0){
                //if ballistic mode
                if(m_GI->m_ballisticMode){
                    int increment = (int)pow(((short)point.x * -1)/ m_GI->m_defColWidth +1,
                        m_GI->m_ballisticMode);
                    m_ctrl->GotoCol(m_GI->m_dragCol - increment);
                    if(increment == 1)
                        Sleep(m_GI->m_ballisticDelay);
                }
                else
                    m_ctrl->MoveCurrentCol(UG_LINEUP);
                moved = TRUE;
            }
            //if the mouse is off the right side
            else if((short)point.x > m_GI->m_gridWidth){
                //if ballistic mode
                if(m_GI->m_ballisticMode){
                    int increment = (int)pow(((short)point.x - m_GI->m_gridWidth) /
                        m_GI->m_defColWidth +1, m_GI->m_ballisticMode);
                    m_ctrl->GotoCol(m_GI->m_dragCol + increment);
                    if(increment == 1)
                        Sleep(m_GI->m_ballisticDelay);
                }
                else
                    m_ctrl->MoveCurrentCol(UG_LINEDOWN);
                moved = TRUE;
            }
            //if the mouse is above the top
            if((short)point.y < 0){
                //if ballistic mode
                if(m_GI->m_ballisticMode){
                    long increment = (long)pow(((short)point.y* -1) / m_GI->m_defRowHeight +1,
                        m_GI->m_ballisticMode);
                    m_ctrl->GotoRow(m_GI->m_dragRow - increment);
                    if(increment == 1)
                        Sleep(m_GI->m_ballisticDelay);
                }
                else
                    m_ctrl->MoveCurrentRow(UG_LINEUP);
                moved = TRUE;
            }
            //if the mouse is below the bottom
            else if((short)point.y > m_GI->m_gridHeight){
                //if ballistic mode
                if(m_GI->m_ballisticMode){
                    long increment = (long)pow(((short)point.y-m_GI->m_gridHeight) /
                        m_GI->m_defRowHeight +1, m_GI->m_ballisticMode);
                    m_ctrl->GotoRow(m_GI->m_dragRow + increment);
                    if(increment == 1)
                        Sleep(m_GI->m_ballisticDelay);
                }
                else
                    m_ctrl->MoveCurrentRow(UG_LINEDOWN);
                moved = TRUE;
            }

            //if the moved flag was not set then the mouse must be within
            //the grid area itself
            if(!moved){
                if(m_ctrl->GetCellFromPoint(point.x, point.y, &col, &row) == UG_SUCCESS){

                    //send a notification to the cell type

                    //
                    if(m_GI->m_numLockCols == 0 && m_GI->m_numLockRows == 0)
                        m_ctrl->GotoCell(col,row);
                    else{
                        if(m_GI->m_dragCol > m_GI->m_numLockCols && col < m_GI->m_numLockCols)
                            m_ctrl->MoveCurrentCol(UG_LINEUP);
                        else
                            m_ctrl->GotoCol(col);

                        if(m_GI->m_dragRow > m_GI->m_numLockRows && row < m_GI->m_numLockRows)
                            m_ctrl->MoveCurrentRow(UG_LINEUP);
                        else{
                            m_ctrl->GotoRow(row);
                        }
                    }
                }
            }

            moved = FALSE;

            //check for messages, if there are not then scroll some more
            while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){
                if(msg.message == WM_MOUSEMOVE || msg.message == WM_LBUTTONUP){

                    m_GI->m_moveType = 0;   //key(default)

                    return;
                }
                GetMessage(&msg, NULL, 0, 0);
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    m_GI->m_moveType = 0;   //key - default
}

/************************************************
OnRButtonDown
    Finds the cell that the mouse is in
    Sends a notification
    Pops up a menu if one exists
*************************************************/
void CUGGrid::OnRButtonDown(UINT nFlags, POINT point){

    if(m_ctrl->m_editInProgress)
        return;

    int col = -1;
    long row = -1;
    BOOL processed = FALSE;
    RECT rect;

    m_GI->m_moveType = 2;   //2button
    m_GI->m_moveFlags = nFlags;

    if(m_ctrl->GetCellFromPoint(point.x, point.y, &col, &row, &rect) == UG_SUCCESS){
        m_ctrl->GotoCell(col, row);

        //send a notification to the cell type
        processed = m_ctrl->GetCellType(col, row)->OnRClicked(col, row, 1, &rect, &point);
    }


    m_ctrl->OnRClicked(col, row, 1, &rect, &point, processed);


    m_GI->m_moveType = 0;   //key(default)

    if(m_GI->m_enablePopupMenu){
        ClientToScreen(m_gridWnd, &point);
        m_ctrl->StartMenu(col, row, &point, UG_GRID);
    }

    m_GI->m_moveType = 0;//key - default
}

/************************************************
OnRButtonUp
    Finds the cell that the mouse is in
    Sends a notification
    Destroys the menu if it was enabled
*************************************************/
void CUGGrid::OnRButtonUp(UINT nFlags, POINT point){

    int col = -1;
    long row = -1;
    RECT rect;
    BOOL processed = FALSE;

    if(m_ctrl->GetCellFromPoint(point.x, point.y, &col, &row, &rect) == UG_SUCCESS){

        //send a notification to the cell type
        processed = m_ctrl->GetCellType(col, row)->OnRClicked(col, row, 0, &rect, &point);
    }

    m_ctrl->OnRClicked(col, row, 0, &rect, &point, processed);
}

/************************************************
OnChar
    Sends all messages to the main class
*************************************************/
void CUGGrid::OnChar(UINT nChar, UINT nFlags)
{
BOOL processed;

/*
------------------------------------
Send a notification to the cell type
------------------------------------ */
processed = m_ctrl->GetCellType(m_GI->m_currentCol, m_GI->m_currentRow)->OnCharDown(m_GI->m_currentCol, m_GI->m_currentRow, &nChar);
m_ctrl->OnCharDown( &nChar, processed );
}

/***********************************************************************
*                              ONKILLFOCUS                             *
***********************************************************************/
void CUGGrid::OnKillFocus(HWND hNewWnd)
{
m_hasFocus = FALSE;

RedrawCell(m_GI->m_currentCol, m_GI->m_currentRow);
m_ctrl->OnKillFocus(UG_GRID);
}

/***********************************************************************
*                              ONSETFOCUS                              *
***********************************************************************/
void CUGGrid::OnSetFocus( HWND hOldWnd )
{
m_hasFocus        = TRUE;
m_cellTypeCapture = FALSE;
m_keyRepeatCount  = 0;

m_ctrl->OnSetFocus( UG_GRID );
RedrawCell(m_GI->m_currentCol,m_GI->m_currentRow);
}

/***********************************************************************
*                            OnMouseActivate                           *
*   Activates the grid                                                 *
***********************************************************************/
int CUGGrid::OnMouseActivate(HWND hDesktopWnd, UINT nHitTest, UINT message)
{
SetFocus(m_gridWnd);
return MA_ACTIVATE;
}

/***********************************************************************
*                          SetDoubleBufferMode                         *
***********************************************************************/
int CUGGrid::SetDoubleBufferMode( BOOL mode )
{

if ( m_bitmap != 0 )
    {
    DeleteObject( m_bitmap );
    m_bitmap = 0;
    }

m_doubleBufferMode = mode;

if ( mode )
    {
    HDC  dc;
    RECT r;

    dc = GetDC( m_gridWnd );
    GetClientRect( m_gridWnd, &r );
    if ( r.right == 0 )
        r.right =1;
    if ( r.bottom == 0 )
        r.bottom =1;

    m_bitmap = CreateCompatibleBitmap( dc, r.right, r.bottom );
    if ( m_bitmap == 0 )
        {
        m_doubleBufferMode = FALSE;
        return 1;
        }

    /*
    ----------------
    Clear the bitmap
    ---------------- */
    HBITMAP oldbitmap = (HBITMAP) SelectObject( dc, m_bitmap );
    HBRUSH  brush     = CreateSolidBrush( m_ctrl->OnGetDefBackColor(UG_GRID) );
    FillRect( dc, &r, brush );
    DeleteObject( brush );
    if ( oldbitmap != NULL )
        SelectObject( dc, oldbitmap );

    ReleaseDC( m_gridWnd, dc);
    }

return UG_SUCCESS;
}

/***********************************************************************
*                               OnSize                                 *
***********************************************************************/
void CUGGrid::OnSize( UINT nType, int cx, int cy )
{
GetClientRect( m_gridWnd, &m_clientRect );

#ifdef UG_ENABLE_HINTS
if ( m_GI->m_enableHints && m_tooltipWnd )
    {
    RECT r;
    GetClientRect( m_gridWnd, &r );

    TOOLINFO ti;
    ZeroMemory( &ti, sizeof(ti) );
    ti.cbSize   = sizeof(ti);
    ti.uFlags   = 0;
    ti.hwnd     = m_gridWnd;
    ti.lpszText = LPSTR_TEXTCALLBACK;
    ti.rect     = r;
    SendMessage( m_tooltipWnd, TTM_ADDTOOL, 0, (LPARAM) &ti );
    }
#endif

if ( m_doubleBufferMode )
    SetDoubleBufferMode( m_doubleBufferMode );
}

#ifdef WM_MOUSEWHEEL
/***********************************************************************
*                               OnMouseWheel                           *
***********************************************************************/
BOOL CUGGrid::OnMouseWheel( UINT nFlags, short zDelta, POINT point )
{
// TODO: Add your message handler code here and/or call default
int distance;
distance = HIWORD( zDelta );
distance /= 120;

m_ctrl->SetTopRow(m_GI->m_topRow - (distance * 3));

return UG_SUCCESS;
}
#endif

/***********************************************************************
*                               OnSetCursor                            *
***********************************************************************/
BOOL CUGGrid::OnSetCursor( HWND hWnd, UINT nHitTest, UINT message )
{
SetCursor( m_GI->m_arrowCursor );
return 1;
}

/***********************************************************************
*                               OnKeyUp                                *
***********************************************************************/
void CUGGrid::OnKeyUp(UINT nChar, UINT nFlags)
{
m_keyRepeatCount = 0;
}

/***********************************************************************
*                              OnGetDlgCode                            *
***********************************************************************/
UINT CUGGrid::OnGetDlgCode()
{
HWND w;

w = GetFocus();

if ( w )
    {
    if ( IsChild(m_gridWnd, w) )
        return DLGC_WANTALLKEYS|DLGC_WANTARROWS;
    }

return -1;
}

/***********************************************************************
*                              OnDestroy                               *
***********************************************************************/
void CUGGrid::OnDestroy( HWND hWnd, WPARAM wParam, LPARAM lParam )
{
}

/***********************************************************************
*                           CreateGridWindow                           *
***********************************************************************/
BOOL CUGGrid::CreateGridWindow(DWORD dwStyle, RECT rect, HWND parent, UINT nID )
{

if ( parent == NULL )
    return UG_ERROR;

HINSTANCE hInstance = (HINSTANCE) GetWindowLong( parent, GWL_HINSTANCE );
WNDCLASS  wc;

/*
----------------------------------------------------
Register this new window class for the ultimate grid
---------------------------------------------------- */
wc.style          = CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS;
wc.lpfnWndProc    = GridWndProc;
wc.cbClsExtra     = 0 ;
wc.cbWndExtra     = 20;
wc.hInstance      = hInstance ;
wc.hIcon          = NULL;
wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
wc.hbrBackground  = NULL;
wc.lpszMenuName   = NULL;
wc.lpszClassName  = _T("UGGrid");

if ( !GetClassInfo(hInstance, TEXT("UGGrid"), &wc) )
    {
    RegisterClass( &wc );
    if ( !GetClassInfo(hInstance, TEXT("UGGrid"), &wc) )
        return UG_ERROR;
    }

/*
-----------------
Create the window
----------------- */
dwStyle |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
m_gridWnd = CreateWindow(
    TEXT("UGGrid"),
    TEXT(""),
    dwStyle,
    rect.left, rect.top, rect.right, rect.bottom,
    parent,
    (HMENU) nID,
    hInstance,
    NULL
    );

if ( m_gridWnd == NULL )
    return UG_ERROR;

SendMessage(m_gridWnd,UG_THIS_PTR_GRID,0,(LPARAM)this);

return UG_SUCCESS;
}

/***********************************************************************
*                             GridWndProc                              *
***********************************************************************/
long CALLBACK CUGGrid::GridWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{

switch ( message )
    {
    case WM_NCCREATE:
        SetWindowLong( hWnd, GWL_USERDATA, (LPARAM) NULL );
        return 1;

    case UG_THIS_PTR_GRID:
        /*
        --------------------------------------
        Store the pointer to the calling class
        -------------------------------------- */
        SetWindowLong( hWnd, GWL_USERDATA, lParam );
        return 1;
    }

CUGGrid * _this = (CUGGrid *)GetWindowLong(hWnd, GWL_USERDATA);

if (_this == NULL)
    return (DefWindowProc(hWnd, message, wParam, lParam));

POINT point = { (short)LOWORD(lParam), (short) HIWORD(lParam) };

switch( message )
    {
    case WM_SIZE:
        _this->OnSize(wParam, LOWORD(lParam), HIWORD(lParam));
        return 0;

    case WM_PAINT:
        _this->OnPaint();
        return 0;

    case WM_KILLFOCUS:
        _this->OnKillFocus((HWND)wParam);
        return 0;

    case WM_SETFOCUS:
        _this->OnSetFocus((HWND)wParam);
        return 0;

    case WM_SETCURSOR:
        _this->OnSetCursor(hWnd, (UINT)(LOWORD(lParam)), UINT(HIWORD(lParam)));
        return 0;

    case WM_GETDLGCODE:
        _this->OnGetDlgCode();
        return DLGC_WANTALLKEYS|DLGC_WANTARROWS;

#ifdef WM_MOUSEWHEEL
    case WM_MOUSEWHEEL:
        _this->OnMouseWheel(LOWORD(wParam), HIWORD(wParam), point);
        return 0;
#endif
    case WM_MOUSEACTIVATE:
        _this->OnMouseActivate(HWND(wParam), LOWORD(lParam), HIWORD(lParam));
        return  MA_ACTIVATE;

    case WM_LBUTTONDBLCLK:
        _this->OnLButtonDblClk((UINT)wParam, point);
        return 0;

    case WM_LBUTTONDOWN:
        _this->OnLButtonDown((UINT)wParam, point);
        return 0;

    case WM_LBUTTONUP:
        _this->OnLButtonUp((UINT)wParam, point);
        return 0;

    case WM_MOUSEMOVE:
        _this->OnMouseMove((UINT)wParam, point);
        _this->ToolTipMessage(hWnd, message, wParam, lParam);
        return 0;

    case WM_RBUTTONDOWN:
        _this->OnRButtonDown((UINT)wParam, point);
        return 0;

    case WM_RBUTTONUP:
        _this->OnRButtonUp((UINT)wParam, point);
        return 0;

    case WM_CHAR:
        _this->OnChar((UINT)wParam, lParam);
        return 0;

    case WM_KEYDOWN:
        _this->OnKeyDown((UINT)wParam, lParam);
        return 0;

    case WM_KEYUP:
        _this->OnKeyUp((UINT)wParam, lParam);
        return 0;

    case WM_NOTIFY:
        _this->OnToolTip(hWnd, wParam, lParam);
        return 0;

    case WM_DESTROY:
        _this->OnDestroy(hWnd, wParam, lParam);
        return 0;

    default:
        return (DefWindowProc(hWnd, message, wParam, lParam));
    }

}

#ifdef UG_ENABLE_HINTS
/***********************************************************************
*                         CreateToolTipWindow                          *
***********************************************************************/
BOOL CUGGrid::CreateToolTipWindow( HWND parent )
{

if ( !m_GI->m_enableHints )
    return 1;

HINSTANCE hInstance = (HINSTANCE) GetWindowLong( parent, GWL_HINSTANCE );

RECT rect;

GetClientRect( m_gridWnd, &rect );

m_tooltipWnd = CreateWindowEx(
    0,
    TOOLTIPS_CLASS,
    NULL,
    WS_POPUP | TTS_ALWAYSTIP,
    rect.left, rect.top, rect.right, rect.bottom,
    parent,
    NULL,
    hInstance,
    NULL
    );

if ( m_tooltipWnd != NULL )
    {
    TOOLINFO ti;
    ZeroMemory(&ti, sizeof(ti));
    ti.cbSize = sizeof(ti);
    ti.uFlags = 0;
    ti.hwnd = m_gridWnd;
    ti.lpszText = LPSTR_TEXTCALLBACK;
    ti.rect = rect;
    SendMessage(m_tooltipWnd, TTM_ADDTOOL, 0, (LPARAM)&ti);
    }
else
    {
    m_GI->m_enableHints = FALSE;
    return UG_ERROR;
    }
return UG_SUCCESS;
}

/***********************************************************************
*                            TOOLTIPMESSAGE                            *
***********************************************************************/
void CUGGrid::ToolTipMessage( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{

static short swit = 0;

if ( !m_GI->m_enableHints )
    return;

if ( m_GI->m_enableHints && m_tooltipWnd )
    {

    RECT rect;
    GetClientRect( m_gridWnd, &rect );

    TOOLINFO ti;
    ZeroMemory( &ti, sizeof(ti) );
    ti.cbSize   = sizeof(ti);
    ti.uFlags   = 0;
    ti.hwnd     = m_gridWnd;
    ti.lpszText = LPSTR_TEXTCALLBACK;
    ti.rect     = rect;
    if ( swit )
        SendMessage( m_tooltipWnd, TTM_DELTOOL, 0, (LPARAM) &ti );
    else
        swit = 1;

    SendMessage( m_tooltipWnd, TTM_ADDTOOL, 0, (LPARAM) &ti );
    }

/*
------------------------------------------------------------------
We need to fill out a message structure and pass it to the tooltip
with the TTM_RELAYEVENT message
------------------------------------------------------------------ */
MSG msg;
msg.hwnd    = hWnd;
msg.message = message;
msg.wParam  = wParam;
msg.lParam  = lParam;
GetCursorPos( &msg.pt );
msg.time = GetMessageTime();

SendMessage( m_tooltipWnd, TTM_RELAYEVENT, 0, (LPARAM) &msg );
}

/***********************************************************************
*                              ONTOOLTIP                               *
***********************************************************************/
int CUGGrid::OnToolTip( HWND hWnd, WPARAM wParam, LPARAM lParam )
{

if (!m_GI->m_enableHints)
    return 1;

LPTOOLTIPTEXT pTTT = (LPTOOLTIPTEXT) lParam;

if ( pTTT->hdr.code == TTN_NEEDTEXT )
    {
    int col;
    long row;
    POINT point;
    TCHAR string[80];
    RECT rect;

    GetCursorPos( &point );
    ScreenToClient( m_gridWnd, &point );
    GetClientRect( m_gridWnd, &rect );
    if ( PtInRect(&rect, point) )
        {
        m_ctrl->GetCellFromPoint( point.x, point.y, &col, &row );
        m_ctrl->OnHint( col, row, UG_GRID, string );
        lstrcpy( pTTT->szText,string );
        }
    }

return UG_SUCCESS;
}

#endif
