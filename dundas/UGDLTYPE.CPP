#include <windows.h>

#include "UGCtrl.h"

#define UG_SETDROPLIST 100
#define UG_CLEARDROPLIST 101

const static TCHAR LFChar   = TEXT( '\n' );
const static TCHAR NullChar = TEXT( '\0' );

/***********************************************************************
*                           CUGDropListType                            *
***********************************************************************/
CUGDropListType::CUGDropListType()
{
m_pen      = CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNFACE));
m_brush    = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
m_btnWidth = GetSystemMetrics(SM_CXVSCROLL);
m_btnDown  = FALSE;
m_btnCol   = -1;
m_btnRow   = -1;
m_listBox  = new CUGLstBox;

m_canOverLap = FALSE;
}

/***********************************************************************
*                          ~CUGDropListType                            *
***********************************************************************/
CUGDropListType::~CUGDropListType()
{
if (m_pen != NULL)
    DeleteObject(m_pen);

if (m_brush != NULL)
    DeleteObject(m_brush);

delete m_listBox;
}

/***********************************************************************
*                              GetName                                 *
***********************************************************************/
LPCTSTR CUGDropListType::GetName()
{
return TEXT("Standard DropList");
}

/***********************************************************************
*                              GetUGID                                 *
***********************************************************************/
LPCUGID CUGDropListType::GetUGID()
{
static const UGID ugid = { 0x1667a6a0, 0xf746,
                            0x11d0, 0x9c, 0x7f,
                            0x0, 0x80, 0xc8, 0x3f,
                            0x71, 0x2f };

return &ugid;
}

/***********************************************************************
*                              GetUGID                                 *
***********************************************************************/
int CUGDropListType::GetEditArea( RECT * rect )
{
rect->right -=(m_btnWidth+1);

return TRUE;
}

/***********************************************************************
*                              OnDClicked                              *
***********************************************************************/
BOOL CUGDropListType::OnDClicked( int col, long row, RECT * rect, POINT * point )
{
if ( point->x > (rect->right - m_btnWidth) )
    return OnLClicked(col,row,1,rect,point);

return FALSE;
}
/***********************************************************************
*                              OnLClicked                              *
***********************************************************************/
BOOL CUGDropListType::OnLClicked( int col, long row, int updn, RECT * rect, POINT * point )
{

if ( updn )
    {
    if ( point->x > (rect->right - m_btnWidth) )
        {
        if ( col == m_btnCol && row == m_btnRow || m_btnCol == -2 )
            {
            m_btnCol = -1;
            m_btnRow = -1;
            }
        else
            {
            /*
            --------------------------------
            Copy the droplist button co-ords
            -------------------------------- */
            CopyRect(&m_btnRect,rect);
            m_btnRect.left = rect->right - m_btnWidth;

            /*
            -----------------
            Redraw the button
            ----------------- */
            m_btnDown = TRUE;
            m_btnCol = col;
            m_btnRow = row;
            m_ctrl->RedrawCell(m_btnCol,m_btnRow);

            /*
            -------------------
            Start the drop list
            ------------------- */
            StartDropList();
            return TRUE;
            }
        }
    else if ( m_btnCol == -2 )
        {
        m_btnCol = -1;
        m_btnRow = -1;
        return FALSE;
        }
    }
else if ( m_btnDown )
    {
    m_btnDown = FALSE;
    m_ctrl->RedrawCell(col,row);
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                              OnMouseMove                             *
***********************************************************************/
BOOL CUGDropListType::OnMouseMove( int col, long row, POINT * point, UINT flags )
{

if ( (flags&MK_LBUTTON) == FALSE )
    return TRUE;

if ( point->x >= m_btnRect.left && point->x <= m_btnRect.right )
    {
    if ( point->y >= m_btnRect.top && point->y <= m_btnRect.bottom )
        {
        if ( !m_btnDown )
            {
            m_btnDown = TRUE;
            m_ctrl->RedrawCell(m_btnCol,m_btnRow);
            }
        }
    else if ( m_btnDown )
        {
        m_btnDown = FALSE;
        m_ctrl->RedrawCell(m_btnCol,m_btnRow);
        }
    }
else if ( m_btnDown )
    {
    m_btnDown = FALSE;
    m_ctrl->RedrawCell(m_btnCol,m_btnRow);
    }

return FALSE;
}

/***********************************************************************
*                              OnKillFocus                             *
***********************************************************************/
void CUGDropListType::OnKillFocus( int col, long row, CUGCell * cell )
{
m_btnCol = -1;
m_btnRow = -1;
}

/***********************************************************************
*                              OnKeyDown                               *
***********************************************************************/
BOOL CUGDropListType::OnKeyDown( int col, long row, UINT * vcKey )
{

if ( *vcKey==VK_RETURN )
    {
    StartDropList();
    *vcKey =0;
    return TRUE;
    }

if ( *vcKey==VK_DOWN )
    {
    if ( GetKeyState(VK_CONTROL) < 0 )
        {
        StartDropList();
        *vcKey =0;
        return TRUE;
        }
    }

return FALSE;
}

/***********************************************************************
*                              OnCharDown                              *
***********************************************************************/
BOOL CUGDropListType::OnCharDown( int col, long row, UINT * vcKey )
{

if(*vcKey==VK_RETURN)
    return TRUE;

if(*vcKey==VK_DOWN)
    return TRUE;

return FALSE;
}

/***********************************************************************
*                               OnDraw                                 *
* current bit:  0-do nothing                                           *
*               1-highlight                                            *
***********************************************************************/
void CUGDropListType::OnDraw( HDC dc, RECT * rect, int col, long row, CUGCell * cell, int selected, int current )
{
DrawBorder(dc,rect,rect,cell);

RECT rectout;
HPEN oldpen;
int  left;
int  style;

left  = rect->left;
style = 0;

if ( cell->IsPropertySet(UGCELL_CELLTYPEEX_SET) )
    style = cell->GetCellTypeEx();

/*
------------------------------------------------
If the cell is not current and hide button is on
then dont draw the button
------------------------------------------------ */
if ( style&UGCT_DROPLISTHIDEBUTTON && !current )
    {
    CUGCellType::OnDraw(dc,rect,col,row,cell,selected,current);
    return;
    }

/*
------------------------------------
Draw the button side of the the cell
------------------------------------ */
rect->left = rect->right - m_btnWidth;

/*
------------------
Draw the 3D border
------------------ */
if ( m_btnDown && current )
    {
    cell->SetBorder(UG_BDR_RECESSED);
    DrawBorder(dc,rect,&rectout,cell);
    }
else
    {
    cell->SetBorder(UG_BDR_RAISED);
    DrawBorder(dc,rect,&rectout,cell);
    }

/*
------------------
Fill the border in
------------------ */
FillRect( dc, &rectout, m_brush );

/*
-----------------------------------------------------------
Make a line to separate the border from the rest ofthe cell
----------------------------------------------------------- */
oldpen = (HPEN) SelectObject( dc, m_pen );
MoveToEx( dc, rect->left-1,rect->top, NULL );
LineTo( dc, rect->left-1, rect->bottom );

/*
-------------------
Draw the down arrow
------------------- */
int x= ((m_btnWidth-5)/2) + rect->left;
int y = ((rect->bottom - rect->top -3)/2) + rect->top;
SelectObject(dc, GetStockObject(BLACK_PEN));

MoveToEx( dc, x, y, NULL );
LineTo( dc, x+5,y);

MoveToEx( dc, x+1,y+1, NULL);
LineTo( dc, x+4,y+1);

MoveToEx( dc, x+2,y+2, NULL);
LineTo( dc, x+2,y+1);

if ( oldpen != NULL )
    SelectObject( dc, oldpen );

/*
--------------------------------------------------
Draw the text in using the default drawing routine
-------------------------------------------------- */
rect->left =left;
rect->right -=(m_btnWidth+1);

CUGCellType::DrawText( dc, rect, 0, col, row, cell, selected, current );
}

/***********************************************************************
*                            StartDropList                             *
***********************************************************************/
FARPROC lpfnOldListBox;
int CUGDropListType::StartDropList()
{
RECT      r;
RECT      clientRect;
int       cell_height;
int       dif;
int       len;
int       pos;
int       startpos;
HFONT     font;
LPTSTR  * list;
int       nof_strings;
int       count;
LPTSTR    items;             //points to the string used to assemble the list
LPTSTR    string;
CUGCell   cell;
HINSTANCE hInstance;
FARPROC   lpfnNewListBox;

items  = NULL;
count  = 0;

/*
--------------------------------
Return false if it is already up
-------------------------------- */
if ( m_listBox->m_HasFocus )
    return 1;

/*
---------------------------
Get the current row and col
--------------------------- */
m_btnCol  = m_ctrl->GetCurrentCol();
m_btnRow = m_ctrl->GetCurrentRow();

/*
----------------------------------
Add the strings to the CStringList
---------------------------------- */
m_ctrl->GetCellIndirect(m_btnCol,m_btnRow,&cell);

string = NULL;
CopyString( string, cell.GetLabelText() );
CopyString( items, string );

len = lstrlen( items );
pos = 0;
nof_strings = 0;
while ( pos < len )
    {
    if ( items[pos] == LFChar )
        nof_strings++;
    pos++;
    }

if ( items[len - 1] != LFChar )
    nof_strings++;

if ( nof_strings > 15 )
    nof_strings = 15;

list = new LPTSTR[nof_strings+1];
memset(list, 0, sizeof(LPTSTR)*(nof_strings+1) );

pos      = 0;
startpos = 0;
while ( pos<len && count<=nof_strings )
    {
    if ( items[pos]== LFChar )
        {
        list[count] = new TCHAR[pos - startpos + 1];
        memcpy( list[count], &items[startpos], sizeof(TCHAR)*(pos - startpos) );
        list[count][pos - startpos] = NullChar;
        startpos = pos+1;
        count++;
        }
    pos++;
    }

if ( startpos < len )
    {
    list[count] = new TCHAR[len - startpos + 1];
    memcpy(list[count], &items[startpos], sizeof(TCHAR)*(len - startpos));
    list[count][len - startpos] = NullChar;
    }

/*
------------------------------------------------------------
Notify the user of the list, so it can be modified if needed
------------------------------------------------------------ */
m_ctrl->OnCellTypeNotify( m_ID, m_btnCol, m_btnRow, UGCT_DROPLISTSTART, (long)list );

/*
------------
Get the font
------------ */
font = NULL;
if ( cell.IsPropertySet(UGCELL_FONT_SET) )
    {
    font = cell.GetFont();
    }
else if ( m_ctrl->m_GI->m_defFont != NULL )
    {
    font = m_ctrl->m_GI->m_defFont;
    }

/*
---------------------------------
Get the rectangle for the listbox
--------------------------------- */
m_ctrl->GetCellRect( m_btnCol, m_btnRow, &r );
cell_height = r.bottom - r.top;
r.top       = r.bottom;
r.left     += 10;
r.right    += 10;
r.bottom   += cell_height * nof_strings + 2;

GetClientRect( m_ctrl->m_CUGGrid->m_gridWnd, &clientRect );
if ( r.bottom > clientRect.bottom )
    {
    dif = r.bottom - clientRect.bottom;
    r.bottom -= dif;
    r.top    -= dif;
    if ( r.top < 0 )
        r.top = 0;
    }

if ( r.right > clientRect.right )
    {
    dif = r.right - clientRect.right;
    r.right -= dif;
    r.left  -= dif;
    if ( r.left < 0 )
        r.left = 0;
    }

/*
-------------------
Create the CListBox
------------------- */
m_listBox->m_ctrl       = m_ctrl;
m_listBox->m_cellTypeId = m_ID;

hInstance = (HINSTANCE) GetWindowLong(m_ctrl->m_CUGGrid->m_gridWnd,GWL_HINSTANCE);

m_listBox->m_listBoxWnd = CreateWindow(
    TEXT("LISTBOX"),
    TEXT(""),
    WS_CHILD|WS_BORDER|WS_VSCROLL,
    r.left, r.top, r.right - r.left, r.bottom - r.top,
    m_ctrl->m_CUGGrid->m_gridWnd,
    (HMENU) UG_LISTBOXID,
    hInstance,
    NULL
    );

if ( m_listBox->m_listBoxWnd == NULL )
    {
    if ( items )
        delete [] items;

    if ( string )
        delete [] string;

    for ( count=0; count<nof_strings+1; count++ )
        {
        if ( list[count] )
            delete [] list[count];
        }

    if ( list )
        delete [] list;

    return UG_ERROR;
    }

lpfnOldListBox = (FARPROC) GetWindowLong(m_listBox->m_listBoxWnd, GWL_WNDPROC);
lpfnNewListBox = (FARPROC) m_listBox->ListBoxWndProc;
SetWindowLong( m_listBox->m_listBoxWnd, GWL_WNDPROC, (LONG)lpfnNewListBox );

SendMessage( m_listBox->m_listBoxWnd, UG_THIS_PTR_LISTBOX, 0, (LPARAM) m_listBox );

if ( font != NULL )
    SendMessage( m_listBox->m_listBoxWnd, WM_SETFONT, (WPARAM) font, FALSE );

/*
-------------------------
Add the items to the list
------------------------- */
count = 0;
while( list[count] != NULL )
    {
    SendMessage( m_listBox->m_listBoxWnd, LB_ADDSTRING, 0, (LONG)(LPTSTR) list[count] );
    count++;
    }

/*
--------------------------------------
UpdateWindow(m_listBox->m_listBoxWnd);
-------------------------------------- */
ShowWindow( m_listBox->m_listBoxWnd, SW_SHOW );
SetFocus( m_listBox->m_listBoxWnd );

/*
--------------------------------------
Give the list box pointers to the cell
-------------------------------------- */
m_listBox->m_col = &m_btnCol;
m_listBox->m_row = &m_btnRow;
m_btnDown = FALSE;

m_ctrl->RedrawCell(m_btnCol,m_btnRow);

if ( items )
    delete[] items;

if ( string )
    delete[] string;

for ( count=0; count<nof_strings+1; count++ )
    if ( list[count] )
        delete[] list[count];
if ( list )
    delete[] list;

return UG_SUCCESS;
}
