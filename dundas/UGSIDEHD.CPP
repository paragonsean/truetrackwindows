#include <windows.h>

#include "UGCtrl.h"

/***********************************************************************
*                           CUGSideHdg                                 *
***********************************************************************/
CUGSideHdg::CUGSideHdg()
{
    //init the varialbes
    m_isSizing  = FALSE;
    m_canSize   = FALSE;

    //set the row height focus rect to be offscreen
    m_focusRect.top = -1;
    m_focusRect.bottom = -1;
}

/***********************************************************************
*                          ~CUGSideHdg                                 *
***********************************************************************/
CUGSideHdg::~CUGSideHdg()
{
//perform clean up
}


/***********************************************************************
*                             OnPaint                                  *
***********************************************************************/
void CUGSideHdg::OnPaint()
{
PAINTSTRUCT paintstru;
HDC dc = BeginPaint(m_sideHdgWnd, &paintstru);

m_drawHint.AddHint(m_GI->m_numberSideHdgCols * -1,0,0,m_GI->m_numberRows);

DrawCellsIntern(dc);

m_drawHint.ClearHints();

EndPaint(m_sideHdgWnd, &paintstru);
}

/***********************************************************************
*                            DrawCellsIntern                           *
***********************************************************************/
void CUGSideHdg::DrawCellsIntern(HDC dc)
{

    RECT rect = {0,0,0,0};
    RECT cellRect;
    CUGCell cell;
    CUGCellType * cellType;
    int dcID;
    HFONT orgFont;

    int x,col;
    long y,row;

    m_bottomRow = m_GI->m_numberRows;

    m_ctrl->OnScreenDCSetup(dc,NULL,UG_SIDEHEADING);

    if(m_GI->m_defFont != NULL)
        orgFont = (HFONT)SelectObject(dc, m_GI->m_defFont);

    for(y = 0; y <m_GI->m_numberRows; y++){

        if(y == m_GI->m_numLockRows)
            y = m_GI->m_topRow;

        row = y;

        rect.top = rect.bottom;
        rect.bottom += m_ctrl->GetRowHeight(row);
        rect.right = 0;

        if(rect.top == rect.bottom)
            continue;

        for(x = (m_GI->m_numberSideHdgCols*-1) ;x <0;x++){

            col = x;
            row = y;

            rect.left = rect.right;
            rect.right  += GetSHColWidth(col);

            //draw if invalid
            if(m_drawHint.IsInvalid(col,row) != FALSE){

                CopyRect(&cellRect,&rect);

                m_ctrl->GetCellIndirect(col,row,&cell);

                if(cell.IsPropertySet(UGCELL_JOIN_SET)){
                    GetCellRect(col,row,&cellRect);
                    m_ctrl->GetJoinStartCell(&col,&row,&cell);
                    if(m_drawHint.IsValid(col,row))
                        continue;
                    m_drawHint.SetAsValid(col,row);
                }

                dcID = SaveDC(dc);

                cellType = m_ctrl->GetCellType(cell.GetCellType());
                cellType->OnDraw(dc,&cellRect,col,row,&cell,0,0);

                RestoreDC(dc, dcID);
            }
        }
        if(rect.bottom > m_GI->m_gridHeight)
            break;
    }


    m_bottomRow = row;

    if(rect.bottom  < m_GI->m_gridHeight){
        rect.top = rect.bottom;
        rect.bottom = m_GI->m_gridHeight;
        rect.left = 0;
        rect.right = m_GI->m_sideHdgWidth;
        SetBkColor(dc, m_GI->m_hdgDefaults->GetBackColor());
        ExtTextOut(dc, 0,0,ETO_OPAQUE,&rect,_T(""),0,NULL);
    }
    if(m_GI->m_defFont != NULL)
        if (orgFont != NULL)
            SelectObject(dc, orgFont);
}

/***********************************************************************
*                                Update                                *
***********************************************************************/
void CUGSideHdg::Update(){

    //calc the last col width
    int width = 0;
   int x;
    for(x = -1;x > (m_GI->m_numberSideHdgCols*-1);x--){
        width += GetSHColWidth(x);
    }
    width = m_GI->m_sideHdgWidth - width;
    if(width <0 )
        width = 0;
    m_ctrl->SetSH_ColWidth(x,width);

    //redraw the window
    InvalidateRect(m_sideHdgWnd, NULL, TRUE);
    UpdateWindow(m_sideHdgWnd);
}

/***********************************************************************
*                                Moved                                 *
***********************************************************************/
void CUGSideHdg::Moved()
{

    if(m_GI->m_topRow == m_GI->m_lastTopRow &&
      m_GI->m_currentRow == m_GI->m_lastRow)
        return;

    if(GetUpdateRect(m_sideHdgWnd, NULL, TRUE)){
        UpdateWindow(m_sideHdgWnd);
        return;
    }

    int yScroll = 0;
    long y;
    HDC pDC;
    BOOL redrawAll = FALSE;

    //if the top row has changed then calc the scroll distance
    if(m_GI->m_topRow != m_GI->m_lastTopRow){

        if(m_GI->m_topRow > m_GI->m_lastTopRow){
            for(y = m_GI->m_lastTopRow;y < m_GI->m_topRow; y++){
                yScroll += m_ctrl->GetRowHeight(y);
                if(yScroll > m_GI->m_gridHeight){
                    redrawAll = TRUE;
                    break;
                }
            }
        }
        else{
            for(y = m_GI->m_topRow;y < m_GI->m_lastTopRow; y++){
                yScroll -= m_ctrl->GetRowHeight(y);
                if(yScroll < -m_GI->m_gridHeight){
                    redrawAll = TRUE;
                    break;
                }
            }
        }
        if(redrawAll){
            yScroll = 0;
            //redraw the whole heading
            m_drawHint.AddToHint(m_GI->m_numberSideHdgCols * -1,0,0,m_GI->m_numberRows);
        }
    }

    //create the device context
    pDC = GetDC(m_sideHdgWnd);

    //do scrolling of the window here
    if(yScroll != 0){
        RECT scrollRect;
        GetClientRect(m_sideHdgWnd, &scrollRect);
        scrollRect.top += m_GI->m_lockRowHeight;
        ScrollDC(pDC, 0,-yScroll,&scrollRect,&scrollRect,NULL,NULL);

        //add the draw hints for the area the was uncovered by the scroll
        if(yScroll >0)
            m_drawHint.AddToHint(m_GI->m_numberSideHdgCols * -1,m_bottomRow,0,m_GI->m_numberRows);
        else if(yScroll < 0)
            m_drawHint.AddToHint(m_GI->m_numberSideHdgCols * -1,0,0,m_GI->m_lastTopRow);
    }

    //add the last and current cells
    m_drawHint.AddToHint(m_GI->m_numberSideHdgCols * -1,m_GI->m_lastRow,0,m_GI->m_lastRow);
    m_drawHint.AddToHint(m_GI->m_numberSideHdgCols * -1,m_GI->m_currentRow,0,m_GI->m_currentRow);

    //call the grids main drawing routine
    DrawCellsIntern(pDC);

    //close the device context
    ReleaseDC(m_sideHdgWnd, pDC);

    //clear the draw hints
    m_drawHint.ClearHints();
}

/***********************************************************************
*                             OnLButtonDown                            *
***********************************************************************/
void CUGSideHdg::OnLButtonDown(UINT nFlags, POINT point)
{

    int col;
    long row;
    RECT rect;

    //cancel editing if it is in progress
    m_ctrl->EditCancel();

    if(m_canSize){
        m_isSizing = TRUE;
        SetCapture(m_sideHdgWnd);
        if(GetFocus() != m_ctrl->m_CUGGrid->m_gridWnd)
            SetFocus(m_ctrl->m_CUGGrid->m_gridWnd);
    }
    else if(GetCellFromPoint(&point,&col,&row,&rect) == UG_SUCCESS){

        //send a notification to the cell type
        BOOL processed = m_ctrl->GetCellType(col,row)->OnLClicked(col,row,0,&rect,&point);
        //send a notification to the main grid class
        m_ctrl->OnSH_LClicked(col,row,1,&rect,&point,processed);
    }

}

/***********************************************************************
*                          OnLButtonUp                                 *
***********************************************************************/
void CUGSideHdg::OnLButtonUp(UINT nFlags, POINT point){

    int col;
    long row;
    RECT rect;

    if(m_isSizing){
        m_isSizing = FALSE;
        m_focusRect.top = -1;
        m_focusRect.bottom = -1;

        ReleaseCapture();
        if(m_colOrRowSizing == 0){  //col sizing
            m_ctrl->OnSideHdgSized(&m_GI->m_sideHdgWidth);
        }
        else{
            int height = m_ctrl->GetRowHeight(m_sizingColRow);
            m_ctrl->OnRowSized(m_sizingColRow,&height);
            m_ctrl->SetRowHeight(m_sizingColRow,height);

        }
        m_ctrl->AdjustComponentSizes();
    }
    else if(GetCellFromPoint(&point,&col,&row,&rect) == UG_SUCCESS){

        //send a notification to the cell type
        BOOL processed = m_ctrl->GetCellType(col,row)->OnLClicked(col,row,0,&rect,&point);
        //send a notification to the main grid class
        m_ctrl->OnSH_LClicked(col,row,0,&rect,&point,processed);
    }

}

/***********************************************************************
*                         OnLButtonDblClk                              *
***********************************************************************/
void CUGSideHdg::OnLButtonDblClk(UINT nFlags, POINT point){

    int col;
    long row;
    RECT rect;

    if(GetCellFromPoint(&point,&col,&row,&rect) == UG_SUCCESS){

        //send a notification to the cell type
        BOOL processed = m_ctrl->GetCellType(col,row)->OnLClicked(col,row,0,&rect,&point);
        //send a notification to the main grid class
        m_ctrl->OnSH_DClicked(col,row,&rect,&point,processed);
    }
}

/***********************************************************************
*                         OnMouseMove                                  *
***********************************************************************/
void CUGSideHdg::OnMouseMove(UINT nFlags, POINT point){

    //check to see ifthe mouse is over a cell separation
    //but only if the mouse is not currently sizing
    if(m_isSizing == FALSE && (nFlags&MK_LBUTTON) == 0 &&
        m_GI->m_userSizingMode >0){

        m_canSize = FALSE;

        //side heading column width sizing
        int width = m_GI->m_sideHdgWidth;
        for(int col = 0; col < m_GI->m_numberSideHdgCols ;col++){

            if(point.x < width+3 && point.x > width-3){

                if(m_ctrl->OnCanSizeSideHdg() == FALSE)
                    return;

                m_canSize = TRUE;
                m_colOrRowSizing = 0;
                m_sizingColRow = col;
                m_sizingStartSize = m_GI->m_sideHdgWidths[col];
                m_sizingStartPos = point.x;
                m_sizingStartWidth = m_GI->m_sideHdgWidth;

                SetCursor(m_GI->m_WEResizseCursor);
                return;
            }
            width -= m_GI->m_sideHdgWidths[col];
        }

        //side heading row height sizing
        int height = 0;
        for(long row = 0;row < m_GI->m_numberRows;row++){
            if(row == m_GI->m_numLockRows)
                row = m_GI->m_topRow;

            height += m_ctrl->GetRowHeight(row);
            if(height > m_GI->m_gridHeight)
                break;

            if(point.y < height+3 && point.y > height-3){

                if(m_ctrl->GetRowHeight(row+1) == 0 && (row+1) < m_GI->m_numberRows)
                    row++;

                if(m_ctrl->OnCanSizeRow(row) == FALSE)
                    return;

                m_canSize = TRUE;
                m_colOrRowSizing = 1;
                m_sizingColRow = row;
                m_sizingStartSize = m_ctrl->GetRowHeight(row);
                m_sizingStartPos = point.y;
                if(m_GI->m_uniformRowHeightFlag){
                    m_sizingNumRowsDown =  point.y/ m_GI->m_defRowHeight;
                }
                SetCursor(m_GI->m_NSResizseCursor);
                return;
            }
        }

    }

    //perform the sizing
    else if(m_isSizing){
        if(m_colOrRowSizing == 0){  //col sizing
            int width = m_sizingStartSize +( point.x - m_sizingStartPos);
            if(width <0)
                width = 0;
            m_GI->m_sideHdgWidths[m_sizingColRow] = width;
            width = m_sizingStartWidth + (point.x - m_sizingStartPos);
            if(width <0)
                width = 0;

            m_ctrl->OnSideHdgSizing(&width);

            m_GI->m_sideHdgWidth = width;
            m_ctrl->AdjustComponentSizes();

        }
        else{                       //row sizing
            int height;
            if(m_GI->m_uniformRowHeightFlag){
                if(point.y <1)
                    point.y = 1;
                if(m_sizingNumRowsDown>0)
                    height = point.y / m_sizingNumRowsDown;
                else
                    height = point.y;
            }
            else{
                height = m_sizingStartSize+(point.y - m_sizingStartPos);
            }

            if(height < 0)
                height = 0 ;

            m_ctrl->OnRowSizing(m_sizingColRow,&height);

            m_ctrl->SetRowHeight(m_sizingColRow,height);

            if(m_GI->m_userSizingMode == 1){ // focus rect
                Update();
                HDC dc = GetDC(m_sideHdgWnd);
                DrawFocusRect(dc, &m_focusRect);
                m_focusRect.top = point.y-1;
                m_focusRect.bottom = point.y+1;
                m_focusRect.left = 0;
                m_focusRect.right = m_GI->m_gridWidth;
                DrawFocusRect(dc, &m_focusRect);
                ReleaseDC(m_sideHdgWnd, dc);

            }
            else{
                m_ctrl->RedrawAll();
            }
        }
    }
}

/***********************************************************************
*                                OnRButtonDown                         *
***********************************************************************/
void CUGSideHdg::OnRButtonDown(UINT nFlags, POINT point){

    int col;
    long row;
    RECT rect;

    if(GetCellFromPoint(&point,&col,&row,&rect) == UG_SUCCESS){
        //send a notification to the cell type
        BOOL processed = m_ctrl->GetCellType(col,row)->OnLClicked(col,row,0,&rect,&point);
        //send a notification to the main grid class
        m_ctrl->OnSH_RClicked(col,row,1,&rect,&point,processed);
    }

    if(m_GI->m_enablePopupMenu){
        ClientToScreen(m_sideHdgWnd, &point);
        m_ctrl->StartMenu(col,row,&point,UG_SIDEHEADING);
    }
}

/***********************************************************************
*                            OnRButtonUp                               *
***********************************************************************/
void CUGSideHdg::OnRButtonUp(UINT nFlags, POINT point)
{
int  col;
long row;
RECT rect;

if ( GetCellFromPoint(&point, &col, &row, &rect) == UG_SUCCESS )
    {
    //send a notification to the cell type
    BOOL processed = m_ctrl->GetCellType(col,row)->OnLClicked(col,row,0,&rect,&point);
    //send a notification to the main grid class
    m_ctrl->OnSH_RClicked(col,row,0,&rect,&point,processed);
    }
}

/***********************************************************************
*                             OnSetCursor                              *
***********************************************************************/
BOOL CUGSideHdg::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
{
// TODO: Add your message handler code here and/or call default
if(!m_canSize)
    SetCursor(m_GI->m_arrowCursor);
else if(m_colOrRowSizing == 0)
    SetCursor(m_GI->m_WEResizseCursor);
else
    SetCursor(m_GI->m_NSResizseCursor);

return 1;
}

/***********************************************************************
*                             GetCellRect                              *
***********************************************************************/
int CUGSideHdg::GetCellRect( int col, long row, RECT * rect )
{
return GetCellRect(&col,&row,rect);
}

/***********************************************************************
*                            GetCellRect                               *
***********************************************************************/
int CUGSideHdg::GetCellRect( int *col, long * row, RECT *rect )
{

    int x,y;
    int width   = 0;
    int height  = 0;

    int startCol    = *col;
    long startRow   = *row;
    int endCol      = *col;
    long endRow     = *row;

    rect->left      = 0;
    rect->top       = 0;
    rect->right     = m_GI->m_sideHdgWidth;
    rect->bottom    = 0;

    //if the specified cell is within a join then find the joined range
    if(m_GI->m_enableJoins)
        if(m_ctrl->GetJoinRange(&startCol,&startRow,&endCol,&endRow) == UG_SUCCESS){
            *col = startCol;
            *row = startRow;
        }


    //find the col
    for(x= (m_GI->m_numberSideHdgCols * -1);x < 0;x++){

        if(x == startCol)
            rect->left = width;

        width += GetSHColWidth(x);

        if(x == endCol){
            rect->right = width;
            break;
        }
    }

    //find the row
    if(startRow >= m_GI->m_numLockRows){
        rect->top = m_GI->m_lockRowHeight;
        rect->bottom = m_GI->m_lockRowHeight;
    }
    for(y= 0;y < m_GI->m_numberRows ;y++){

        if(y == m_GI->m_numLockRows)
            y = m_GI->m_topRow;

        if(y == startRow)
            rect->top = height;

        height += m_ctrl->GetRowHeight(y);

        if(y == endRow){
            rect->bottom = height;
            break;
        }
    }

    return UG_SUCCESS;
}

/***********************************************************************
*                           GetCellFromPoint                           *
***********************************************************************/
int CUGSideHdg::GetCellFromPoint( POINT * point, int * col, long * row, RECT * rect )
{

int ptsFound = 0;

int x;
long y;

rect->left      =0;
rect->top       =0;
rect->right     =0;
rect->bottom    =0;

/*
------------
Find the row
------------ */
for(y=0;y<m_GI->m_numberRows;y++)
    {
    if ( y == m_GI->m_numLockRows )
        y = m_GI->m_topRow;

    rect->bottom += m_ctrl->GetRowHeight(y);

    if(rect->bottom > point->y)
        {
        rect->top = rect->bottom - m_ctrl->GetRowHeight(y);
        ptsFound ++;
        *row = y;
        break;
        }
    }

/*
------------
Find the col
------------ */
for ( x=m_GI->m_numberSideHdgCols * -1; x<0; x++ )
    {
    rect->right += GetSHColWidth(x);

    if ( rect->right > point->x )
        {
        rect->left = rect->right - m_GI->m_sideHdgWidths[x];
        ptsFound ++;
        *col = x;
        break;
        }
    }

if ( ptsFound == 2 )
    return UG_SUCCESS;

*col = -1;
*row = -1;
return 1;
}

/***********************************************************************
*                               GetSHColWidth                          *
***********************************************************************/
int CUGSideHdg::GetSHColWidth(int col)
{

    //translate the row number into a 0 based positive index
    col = (col * -1) -1;

    if(col <0 || col > m_GI->m_numberSideHdgCols)
        return 0;

    return m_GI->m_sideHdgWidths[col];
}

/***********************************************************************
*                            CreateSideHdgWindow                       *
***********************************************************************/
BOOL CUGSideHdg::CreateSideHdgWindow(DWORD dwStyle, RECT rect, HWND hWndParent, UINT nID)
{

    if(hWndParent == NULL)
        return UG_ERROR;

    HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWndParent,GWL_HINSTANCE);

    WNDCLASS wndclass;

    //register this new window class for the ultimate grid
    wndclass.style          = CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS;
    wndclass.lpfnWndProc    = SideHdgWndProc;
    wndclass.cbClsExtra     = 0;
    wndclass.cbWndExtra     = 20;
    wndclass.hInstance      = hInstance ;
    wndclass.hIcon          = NULL;
    wndclass.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground  = NULL;
    wndclass.lpszMenuName   = NULL;
    wndclass.lpszClassName  = _T("UGSideHdg");

    if(!GetClassInfo(hInstance,_T("UGSideHdg"),&wndclass)) {
        RegisterClass(&wndclass);
        if(!GetClassInfo(hInstance,_T("UGSideHdg"),&wndclass))
            return UG_ERROR;
    }

    //create the window
    m_sideHdgWnd = CreateWindow(_T("UGSideHdg"), _T(""), dwStyle,
        rect.left, rect.top, rect.right, rect.bottom,
        hWndParent, (HMENU)nID, hInstance, NULL);

    if (m_sideHdgWnd == NULL)
        return UG_ERROR;

    SendMessage(m_sideHdgWnd,UG_THIS_PTR_SIDEHDG,0,(LPARAM)this);
    UpdateWindow(m_sideHdgWnd);

    return UG_SUCCESS;
}

/***********************************************************************
*                              SideHdgWndProc                          *
***********************************************************************/
long CALLBACK CUGSideHdg::SideHdgWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{

switch(message)
    {
    case WM_NCCREATE:
        SetWindowLong(hWnd, GWL_USERDATA, (LPARAM)NULL);
        return 1;
    case UG_THIS_PTR_SIDEHDG:
        //store the pointer to the calling class
        SetWindowLong(hWnd, GWL_USERDATA, lParam);
        return 1;
    }

CUGSideHdg * _this = (CUGSideHdg *)GetWindowLong(hWnd, GWL_USERDATA);
if (_this == NULL)
    return (DefWindowProc(hWnd, message, wParam, lParam));

//POINT point = {LOWORD(lParam), HIWORD(lParam)};
POINT point = {(short)LOWORD(lParam), (short)HIWORD(lParam)};

switch(message)
    {
    case WM_PAINT:
        _this->OnPaint();
        return 0;
    case WM_SETCURSOR:
        _this->OnSetCursor(hWnd, (UINT)(LOWORD(lParam)), UINT(HIWORD(lParam)));
        return 0;
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
        return 0;
    case WM_RBUTTONDOWN:
        _this->OnRButtonDown((UINT)wParam, point);
        return 0;
    case WM_RBUTTONUP:
        _this->OnRButtonUp((UINT)wParam, point);
        return 0;
    default:
        return (DefWindowProc(hWnd, message, wParam, lParam));
    }

}
