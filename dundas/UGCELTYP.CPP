#include <windows.h>
#include "UGCtrl.h"

/***********************************************************************
*   Ultimate Grid 97                                                   *
*   Copyright 1994 - 1997 Dundas Software Ltd.                         *
*                                                                      *
*   Class                                                              *
*       CUGCellType                                                    *
*   Purpose                                                            *
*       standard text cell type class. This class is                   *
*       the default cell type for all cells in the grid                *
*       and is the most common one to use. Also this                   *
*       class is the base class for any other cell type                *
*       that the grid uses.                                            *
*   Instructions                                                       *
*       -this cell types ID number never changes so there              *
*       is a define for it's ID number:                                *
*           UGCT_NORMAL                                                *
*       -There are several extended styles for this type               *
*           UGCT_NORMALMULTILINE                                       *
*               - this style is used when word wrapping                *
*               and/or multiple lines are needed within                *
*               one cell                                               *
*           UGCT_NORMALELLIPSIS                                        *
*               - this style shows three periods at the                *
*               right of the cell if the text is too wide.             *
*           UGCT_NORMALLABELTEXT                                       *
*               - this style causes the cells label property           *
*               to be drawn instead of its text property               *
***********************************************************************/

static TCHAR EmptyString[] = TEXT( "" );

/***********************************************************************
*                            CUGCELLTYPE                               *
*   NOTE: New cell types must be registered with the                   *
*   grid before use.                                                   *
***********************************************************************/
CUGCellType::CUGCellType()
{

m_canTextEdit    = TRUE;
m_drawLabelText = FALSE;
m_canOverLap    = TRUE;
}

/***********************************************************************
*                           ~CUGCELLTYPE                               *
***********************************************************************/
CUGCellType::~CUGCellType()
{
}

/***********************************************************************
*                               SETID                                  *
***********************************************************************/
void CUGCellType::SetID( int ID )
{
m_ID =ID;
}

/***********************************************************************
*                               GETID                                  *
***********************************************************************/
int CUGCellType::GetID()
{
return m_ID;
}

/***********************************************************************
*                             GETNAME                                  *
***********************************************************************/
LPCTSTR CUGCellType::GetName()
{
return TEXT("Normal Cell Type (default)");
}

/***********************************************************************
*                             GETUGID                                  *
*       Returns a GUID for the cell type, this number is unique        *
*       for each cell type and never changes (unlike the               *
*       standard celltype ID number). This number can be used to       *
*       find the cell types standard ID number.                        *
***********************************************************************/
LPCUGID CUGCellType::GetUGID()
{

static const UGID ugid = {0x78a705c1, 0xf3e9, 0x11d0, 0x9c, 0x7b,
                          0x0, 0x80, 0xc8, 0x3f, 0x71, 0x2f};

return &ugid;
}

/***********************************************************************
*                            GETEDITAREA                               *
*                              virtual                                 *
*  Returns the editable area of a cell type using a given              *
*  rectangle which represents the whole celltype region.               *
*  Droplists for example do not use its whole area for editing,        *
*  as the droplist button on the right is not covered by the           *
*  edit control. Therefor a droplist will modify the righthand         *
*  side of the given rect.                                             *
***********************************************************************/
int CUGCellType::GetEditArea( RECT * rect )
{
return UG_SUCCESS;
}

/***********************************************************************
*                          CANTEXTEDIT                                 *
***********************************************************************/
BOOL CUGCellType::CanTextEdit()
{
return m_canTextEdit;
}

/***********************************************************************
*                            CANOVERLAP                                *
***********************************************************************/
BOOL CUGCellType::CanOverLap( CUGCell * cell )
{
if ( cell->GetCellTypeEx() == 0 && cell->IsPropertySet(UGCELL_JOIN_SET) == FALSE )
    return  m_canOverLap;
return FALSE;
}

/***********************************************************************
*                          SETTEXTEDITMODE                             *
***********************************************************************/
int CUGCellType::SetTextEditMode( int mode )
{

if ( mode == FALSE )
    m_canTextEdit = FALSE;
else
    m_canTextEdit = TRUE;

return UG_SUCCESS;
}

/***********************************************************************
*                        OnSystemChange                                *
*    This function is called for each cell type when system            *
*    settings change, such as screen resolution and or colors.         *

***********************************************************************/
int CUGCellType::OnSystemChange()
{
return UG_SUCCESS;
}

/***********************************************************************
*                          OnLClicked                                  *
*       This function is called when the left mouse button is          *
*       clicked over a cell using this cell type. If this              *
*       function does something with this event then return TRUE       *
*       othrwise return FALSE.                                         *
*                                                                      *
*       col - column that was clicked in                               *
*       row - row that was clicked in                                  *
*       updn - TRUE if the mouse button just went down                 *
*            - FALSE if the mouse button just went up                  *
*       rect - rectangle of the cell that was clicked in               *
*       point - point where the mouse was clicked                      *
*   Return                                                             *
*       TRUE - if the event was processed                              *
*       FALSE - if the event was not                                   *
***********************************************************************/
BOOL CUGCellType::OnLClicked( int col, long row, int updn, RECT * rect, POINT * point )
{
return FALSE;
}

/***********************************************************************
*                          OnRClicked                                  *
*    This function is called when the right mouse button is            *
*    clicked over a cell using this cell type. If this function        *
*    does something with this event then return TRUE othrwise          *
*    return FALSE.                                                     *
*       col - column that was clicked in                               *
*       row - row that was clicked in                                  *
*       updn - TRUE if the mouse button just went down                 *
*            - FALSE if the mouse button just went up                  *
*       rect - rectangle of the cell that was clicked in               *
*       point - point where the mouse was clicked                      *
***********************************************************************/
BOOL CUGCellType::OnRClicked( int col, long row, int updn, RECT * rect, POINT * point )
{
return FALSE;
}

/***********************************************************************
*                          OnDClicked                                  *
*   This function is called when the left mouse button is double       *
*   clicked over a cell using this cell type. If this function         *
*   does something with this event then return TRUE othrwise           *
*   return FALSE.                                                      *
*       col - column that was clicked in                               *
*       row - row that was clicked in                                  *
*       rect - rectangle of the cell that was clicked in               *
*       point - point where the mouse was clicked                      *
***********************************************************************/
BOOL CUGCellType::OnDClicked( int col, long row, RECT * rect, POINT * point )
{
return FALSE;
}

/***********************************************************************
*                              OnKeyDown                               *
*   This function is called when a cell of this type has focus         *
*   and a key is pressed.(See WM_KEYDOWN) If this function does        *
*   something with this event then return TRUE othrwise return         *
*   FALSE.                                                             *
*       col - column that has focus                                    *
*       row - row that has focus                                       *
*       vcKey - pointer to the virtual key code,                       *
*               of the key that was pressed                            *
***********************************************************************/
BOOL CUGCellType::OnKeyDown( int col, long row, UINT * vcKey )
{
return FALSE;
}

/***********************************************************************
*                          OnCharDown                                  *
*   This function is called when a cell of this type has focus         *
*   and a printable key is pressed.(WM_CHARDOWN) If this               *
*   function does something with this event then return TRUE           *
*   othrwise return FALSE.                                             *
*       col - column that has focus                                    *
*       row - row that has focus                                       *
*       vcKey - pointer to the virtual key code, of the                *
*           key that was pressed                                       *
***********************************************************************/
BOOL CUGCellType::OnCharDown( int col, long row, UINT * vcKey )
{
return FALSE;
}

/***********************************************************************
*                          OnMouseMove                                 *
*   This function is called when the mouse  is over a cell of          *
*   this celltype. If this function does something with this           *
*   event then return TRUE othrwise return FALSE.                      *
*       col - column that the mouse is over                            *
*       row - row that the mouse is over                               *
*       point - point where the mouse is                               *
*       flags - mouse move flags (see WM_MOUSEMOVE)                    *
***********************************************************************/
BOOL CUGCellType::OnMouseMove( int col, long row, POINT * point, UINT flags )
{
return FALSE;
}

/***********************************************************************
*                              SetOption                               *
*   This function is a standard interface for setting extended         *
*   options for a cell type. What this function does is                *
*   determined on a celltype by celltype basis                         *
*       option - option ID number                                      *
*       param - value for the specified option                         *
*   Return                                                             *
*       UG_NA - if this function does not perform anything             *
*       UG_SUCCESS - success                                           *
*       UG_ERROR - error                                               *
***********************************************************************/
int CUGCellType::SetOption( long option, long param )
{
return UG_NA;
}

/***********************************************************************
*                             GetOption                                *
*   This function is a standard interface for getting extended         *
*   options for a cell type. What this function does is                *
*   determined on a celltype by celltype basis                         *
*       option - option ID number                                      *
*       param - value for the specified option                         *
*   Return                                                             *
*       UG_NA - if this function does not perform anything             *
*       UG_SUCCESS - success                                           *
*       UG_ERROR - error                                               *
***********************************************************************/
int CUGCellType::GetOption( long option, long * param )
{
return UG_NA;
}

/***********************************************************************
*                              OnSetFocus                              *
*  This function is called when a cell of this type receives focus.    *
*                                                                      *
*       col - column that just received focus                          *
*       row - row that just received focus                             *
*       cell - pointer to the cell object located at col/row           *
***********************************************************************/
void CUGCellType::OnSetFocus( int col, long row, CUGCell * cell )
{
}

/***********************************************************************
*                               OnKillFocus                            *
*  This function is called when a cell of this type loses focus.       *
*       col - column that just lost focus                              *
*       row - row that just lost focus                                 *
*       cell - pointer to the cell object located at col/row           *
***********************************************************************/
void CUGCellType::OnKillFocus( int col, long row, CUGCell * cell )
{
}

/***********************************************************************
*                                 OnDraw                               *
*   This function is called whenever the grid needs a cell to be       *
*   drawn. Detailed information of what needs to be drawn is           *
*   passed in as parameters.                                           *
*       dc      - device context to draw the cell with                 *
*       rect    - rectangle to draw the cell in                        *
*       col     - column that is being drawn                           *
*       row     - row that is being drawn                              *
*       cell    - cell that is being drawn                             *
*       selected- TRUE if the cell is selected, otherwise FALSE        *
*       current - TRUE if the cell is the current cell, otherwise FALSE*
***********************************************************************/
void CUGCellType::OnDraw( HDC dc, RECT * rect, int col, long row, CUGCell * cell, int selected, int current )
{
int     offset;
int     overlapCol;
RECT    rectOut;
CUGCell overlapCell;

if ( m_ctrl->m_GI->m_enableCellOverLap )
    {
    if ( CanOverLap(cell) )
        {
        if ( cell->GetTextLength() == 0 )
            {
            offset = GetCellOverlapInfo( dc, col, row, &overlapCol, &overlapCell );
            if ( offset != 0 )
                {
                DrawBorder( dc, rect, &rectOut, cell );
                DrawText(dc, &rectOut, offset, col, row, &overlapCell, selected, current );
                return;
                }
            }
        }
    }

DrawBorder( dc, rect, &rectOut, cell );
DrawText( dc, &rectOut, 0, col, row, cell, selected, current );
}

/***********************************************************************
*                              DrawText                                *
*  This function is the standard text drawing routine used by          *
*  this cell type and used by many others.                             *
*       dc      - device context to draw the cell with                 *
*       rect    - rectangle to draw the cell in                        *
*       col     - column that is being drawn                           *
*       row     - row that is being drawn                              *
*       cell    - cell that is being drawn                             *
*       selected- TRUE if the cell is selected, otherwise FALSE        *
*       current - TRUE if the cell is the current cell, otherwise FALSE*
***********************************************************************/
void CUGCellType::DrawText( HDC dc, RECT * rect, int offset, int col, long row, CUGCell * cell, int selected, int current )
{
int         loop;
int         left;
int         top;
SIZE        size;
COLORREF    backcolor;
TCHAR *     string;
int         stringLen;
long        cellTypeEx;
short       alignment;
HFONT       origFont;
int         oldleft;
long        properties;
int         overLapCol;
CUGCell     olCell;

string     = 0;
origFont   = 0;
oldleft    = rect->left;
properties = cell->GetPropertyFlags();
overLapCol = col;

/*
----------------------
Get the extended style
---------------------- */
if ( properties & UGCELL_CELLTYPEEX_SET )
    cellTypeEx = cell->GetCellTypeEx();
else
    cellTypeEx = 0;

/*
----------------------
Get the string to draw
---------------------- */
if ( !m_drawLabelText && cellTypeEx != UGCT_NORMALLABELTEXT )
    {
    if ( properties & UGCELL_STRING_SET )
        {
        CopyString( string, cell->GetText() );
        stringLen = cell->GetTextLength();
        }
    else
        {
        CopyString( string, EmptyString );
        stringLen = 0;
        }
    }
else
    {
    if(properties&UGCELL_LABEL_SET)
        {
        CopyString (string, cell->GetLabelText());
        stringLen = lstrlen(string);
        }
    else
        {
        CopyString (string, TEXT(""));
        stringLen = 0;
        }
    }

/*
---------------------
Check for overlapping
--------------------- */
if(overLapCol != col)
    {
    m_ctrl->GetCellIndirect(overLapCol,row,&olCell);
    cell->SetAlignment(olCell.GetAlignment());
    CopyString(string, olCell.GetText());
    stringLen = olCell.GetTextLength();
    }

if ( properties & UGCELL_ALIGNMENT_SET )
    alignment = cell->GetAlignment();
else
    alignment = 0;

/*
---------------
Select the font
--------------- */
if ( properties & UGCELL_FONT_SET )
    origFont = (HFONT) SelectObject( dc, cell->GetFont() );

/*
-------------------------------------
Check the selected and current states
------------------------------------- */
if ( selected || (current && m_ctrl->m_GI->m_currentCellMode & 2) )
    {
    SetTextColor( dc, cell->GetHTextColor() );
    SetBkColor( dc, cell->GetHBackColor() );
    backcolor = cell->GetHBackColor();
    }
else
    {
    SetTextColor( dc, cell->GetTextColor() );
    backcolor = cell->GetBackColor();
    SetBkColor( dc, backcolor );
    }

/*
-----------------
Check for bitmaps
----------------- */
if ( properties & UGCELL_BITMAP_SET )
    {
    oldleft = rect->left;
    rect->left += DrawBitmap( dc, cell->GetBitmap(), rect, backcolor );
    }

/*
--------------
Text alignment
-------------- */
if ( alignment )
    {
    if ( alignment & UG_ALIGNCENTER )
        {
        GetTextExtentPoint32( dc, string, stringLen, &size );
        left = rect->left + (rect->right - rect->left - size.cx) /2;
        }
    else if ( alignment & UG_ALIGNRIGHT )
        {
        GetTextExtentPoint32( dc, string, stringLen, &size );
        left = rect->right - size.cx - m_ctrl->m_GI->m_margin;
        }
    else
        {
        left = rect->left + m_ctrl->m_GI->m_margin + offset;
        }

    if ( alignment & UG_ALIGNVCENTER )
        {
        GetTextExtentPoint32( dc, string, stringLen, &size );
        top = rect->top + (rect->bottom - rect->top - size.cy) /2;
        }
    else if ( alignment & UG_ALIGNBOTTOM )
        {
        GetTextExtentPoint32( dc, string, stringLen, &size );
        top = rect->bottom - size.cy -1;
        }
    else
        {
        top = rect->top+1;
        }
    }
else
    {
    left = rect->left + m_ctrl->m_GI->m_margin + offset;
    top = rect->top + 1;
    }

if ( offset < 0 )
    rect->left--;

/*
-------------
Draw the text
------------- */
if ( cellTypeEx & (UGCT_NORMALMULTILINE | UGCT_NORMALELLIPSIS) )
    {
    if ( cellTypeEx & UGCT_NORMALMULTILINE )
        {
        /*
        --------------
        Clear the rect
        -------------- */
        ExtTextOut( dc, left,top, ETO_OPAQUE, rect, EmptyString, 0, NULL );
        /*
        -------------
        Draw the text
        ------------- */
        ::DrawText(dc, string,-1,rect,DT_WORDBREAK|((alignment&UG_ALIGNCENTER)?DT_CENTER:(alignment&UG_ALIGNRIGHT)?DT_RIGHT:DT_LEFT));
        }
    else
        {
        /*
        --------
        Ellipsis
        -------- */
        GetTextExtentPoint32(dc, string, stringLen, &size );
        if ( size.cx+6 > (rect->right - rect->left) )
            {
            GetTextExtentPoint32( dc, TEXT("..."), 3, &size );
            rect->right -= size.cx + 3;
            if ( rect->right > rect->left )
                ExtTextOut( dc, rect->left+3, top, ETO_OPAQUE|ETO_CLIPPED, rect, string, stringLen, NULL );
            rect->left = rect->right;
            if ( rect->left <= oldleft )
                rect->left = oldleft + 1;
            rect->right += size.cx +3;
            ExtTextOut( dc, rect->left+1, top, ETO_OPAQUE|ETO_CLIPPED, rect, TEXT("..."), 3, NULL );
            }
        else
            {
            ExtTextOut( dc, left, top, ETO_OPAQUE|ETO_CLIPPED, rect, string, stringLen, NULL );
            }
        }
    }
else
    {
    if ( overLapCol != col )
        {
        /*
        --------------
        Get the offset
        -------------- */
        for ( loop = col -1; loop >= overLapCol; loop-- )
            {
            left -= m_ctrl->GetColWidth( loop );
            }
        left += 1 ;
        }

    ExtTextOut( dc, left, top, ETO_OPAQUE|ETO_CLIPPED, rect, string, stringLen, NULL );
    }

/*
--------------
Reset the rect
-------------- */
rect->left = oldleft;
if ( string )
    {
    delete [] string;
    string = 0;
    }

if ( origFont )
    SelectObject( dc, origFont );
}

/***********************************************************************
*                             OnPrint                                  *
*   This function is called when the grid is going to be printed       *
*   on a printer. This function should perform any setup               *
*   required then call the celltypes OnDraw member                     *
*       dc      - device context to draw the cell with                 *
*       rect    - rectangle to draw the cell in                        *
*       bdrRect - outer retangle of the cells border                   *
*       col     - column that is being drawn                           *
*       row     - row that is being drawn                              *
*       cell    - cell that is being drawn                             *
*       overLapCol - column number of the cell to the left that        *
*               is overlapping this cell. If this value is the same    *
*               as 'col' then there is no overlapping                  *
*   Return                                                             *
*       0 - standard return value                                      *
*       1 - if the text overflowed and the cell type supports          *
*           cell overlapping                                           *
***********************************************************************/
void CUGCellType::OnPrint( HDC dc, RECT * rect, int col, long row, CUGCell * cell )
{
int     height;
double  scale;
HFONT   font;
HFONT   scaleFont;
LOGFONT lf;

if ( row >=0 )
    height = m_ctrl->GetRowHeight( row );
else
    height = m_ctrl->m_GI->m_topHdgHeights[((row * -1) -1)];

scale = ((double)(rect->bottom - rect->top)) / ((double)height);

if ( cell->GetFont() != NULL )
    font = cell->GetFont();
else
    font = m_ctrl->m_GI->m_defFont;

GetObject( font, sizeof(lf), &lf );

lf.lfHeight = (int) ((double)lf.lfHeight*scale);
scaleFont = CreateFontIndirect( &lf );

cell->SetFont( scaleFont );

OnDraw( dc, rect, col, row, cell, 0, 0 );
}

/***********************************************************************
*                             BitmapDisplay                            *
*   Draws a bitmap within the given rectangle The bitmap is            *
*   automatically resized to fit within rect. The bitmap is            *
*   placed on the left side of the rect the width of the sized         *
*   bitmap plus a margin is returned                                   *
*       hdc         device context handle                              *
*       hbitmap     bitmap handle                                      *
*       rect        bounding rectangle                                 *
*       Margin  top and bottom margins                                 *
*       return: width of the area drawn in                             *
***********************************************************************/
int CUGCellType::DrawBitmap( HDC dc, HBITMAP bitmap, RECT * rect, COLORREF backcolor )
{

BITMAP bm;
HBITMAP bmpOld;
HDC dcMemory;
long xin;
long xout;
long yin;
long yout;
long ratio;
int t;
int resize = TRUE;
int Margin = 2;


if (bitmap == NULL )
    return UG_ERROR;

/*
-----------------------
Get the bitmaps co-ords
----------------------- */
GetObject( bitmap, sizeof(BITMAP), &bm );

/*
---------------------------------------------------
calc the output co-ords , and maintain aspect ratio
--------------------------------------------------- */
yout  = (rect->bottom - rect->top - Margin*2)*100;
yin   =  bm.bmHeight;
xin   =  bm.bmWidth;
ratio = yout /yin;
xout  = xin *ratio /100;
yout /= 100;

/*
------------------------------------------------------------
If the height of the bitmap is less than the height of a row
then don't resize it
------------------------------------------------------------ */
if ( yin <= (rect->bottom - rect->top - Margin *2) )
    {
    xout   = xin;
    yout   = yin;
    resize = FALSE;
    }

if ( xout >= rect->right - rect->left - Margin *2 )
    xout = rect->right - rect->left - Margin *2;

/*
----------------------------------
Create a compatible memory context
---------------------------------- */
dcMemory = CreateCompatibleDC( dc );
bmpOld = (HBITMAP) SelectObject( dcMemory, bitmap );

/*
------------------------
Fill in background first
------------------------ */
SetBkColor( dc, backcolor );
t =rect->right;
rect->right = rect->left+(int)xout+Margin*2;
ExtTextOut( dc, 0, 0, ETO_OPAQUE, rect, TEXT(""), 0, NULL );
rect->right = t;

/*
---------------
Draw the bitmap
--------------- */
if(resize)
    {
    StretchBlt( dc, rect->left+Margin, rect->top+Margin, (int)xout, (int)yout, dcMemory,0,0, bm.bmWidth, bm.bmHeight, SRCCOPY );
    }
else
    {
    t= (int) ((rect->bottom - rect->top - yout)/2);
    BitBlt( dc, rect->left+Margin,rect->top+t, (int)xout, (int)yout, dcMemory,0,0,SRCCOPY);
    }

/*
------------------------------------
Remove the compatible memory context
------------------------------------ */
if ( bmpOld != NULL )
    SelectObject( dcMemory, bmpOld );
DeleteDC( dcMemory );

return Margin*2+(int)xout;
}

/***********************************************************************
*                               DrawBorder                             *
*   Draws a border using the given style. Defines                      *
*   for setting the style are available.                               *
*   'rect' is the area to draw the border in                           *
*   'rectout' returns the area inside the border                       *
*   'pen'  is the pen to use for drawing the first                     *
*   3 styles                                                           *
*                                                                      *
*  Style   Thin:       bit 0-left  1-top  2-right  3-bottom            *
*          Medium:     bit 4-left  5-top  6-right  7-bottom            *
*          Thick:      bit 8-left  9-top  10-right 11-bottom           *
*          3DRecess:   bit 12-left 13-top 14-right 15-bottom           *
*          3DRaised:   bit 16-left 17-top 18-right 19-bottom           *
***********************************************************************/
void CUGCellType::DrawBorder( HDC dc, RECT *rect, RECT *rectout, CUGCell * cell )
{
BOOL  excelBdr;
int   left;
int   top;
int   right;
int   bottom;
int   style;
long  props;
HPEN  pen;
HPEN  origPen;
HPEN  origPen2;

props    = cell->GetPropertyFlags();
excelBdr = m_ctrl->m_GI->m_enableExcelBorders;

if ( (props & UGCELL_BORDERSTYLE_SET) == 0 && !excelBdr )
    {
    CopyRect( rectout,rect );
    return;
    }

style   = cell->GetBorder();
pen     = cell->GetBorderColor();
origPen = NULL;

if ( (style & UG_BDR_RAISED)|| (style & UG_BDR_RECESSED) )
    excelBdr = 0;

left   = 0;
top    = 0;
right  = 0;
bottom = 0;

if ( pen != NULL )
    origPen2 = (HPEN) SelectObject( dc, pen );
else
    origPen2 = (HPEN) SelectObject( dc, m_ctrl->m_GI->m_defBorderPen );

if ( style&15 || excelBdr )
    {
    /*
    ----------
    Thin lines
    ---------- */
    if ( style & 1 )
        {
        /*
        ----
        Left
        ---- */
        MoveToEx(dc, rect->left,rect->top, NULL);
        LineTo(dc, rect->left,rect->bottom);
        left=1;
        }

    if ( style & 2 )
        {
        /*
        ---
        Top
        --- */
        MoveToEx( dc, rect->left, rect->top, NULL );
        LineTo( dc, rect->right, rect->top );
        top=1;
        }

    if ( style & 4 || excelBdr )
        {
        /*
        -----
        Right
        ----- */
        if ( (style&4) == 0 )
            origPen = (HPEN) SelectObject( dc, m_ctrl->m_GI->m_defBorderPen );

        MoveToEx( dc, rect->right-1, rect->top, NULL );
        LineTo( dc, rect->right-1, rect->bottom );
        right = -1;

        if ( (style&4) == 0 )
            if ( origPen != NULL )
                SelectObject( dc, origPen );
        }

    if ( style & 8 || excelBdr )
        {
        /*
        ------
        Bottom
        ------ */
        if ( (style & 8) == 0 )
            origPen = (HPEN) SelectObject( dc, m_ctrl->m_GI->m_defBorderPen );

        MoveToEx( dc, rect->left, rect->bottom-1, NULL);
        LineTo( dc, rect->right, rect->bottom-1);
        bottom=-1;

        if ( (style & 8) == 0 )
            if ( origPen != NULL )
                SelectObject( dc, origPen );
        }
    }

if ( style & 240 )
    {
    /*
    ------------
    Medium lines
    ------------ */
    if(style & 16)
        { //left
        MoveToEx(dc, rect->left,rect->top, NULL);
        LineTo(dc, rect->left,rect->bottom);
        MoveToEx(dc, rect->left+1,rect->top, NULL);
        LineTo(dc, rect->left+1,rect->bottom);
        left=2;
        }

    if(style & 32)
        { //top
        MoveToEx(dc, rect->left,rect->top, NULL);
        LineTo(dc, rect->right,rect->top);
        MoveToEx(dc, rect->left,rect->top+1, NULL);
        LineTo(dc, rect->right,rect->top+1);
        top=2;
        }
    if(style & 64)
        { //right
        MoveToEx(dc, rect->right-1,rect->top, NULL);
        LineTo(dc, rect->right-1,rect->bottom);
        MoveToEx(dc, rect->right-2,rect->top, NULL);
        LineTo(dc, rect->right-2,rect->bottom);
        right=-2;
        }
    if(style & 128)
        { //bottom
        MoveToEx(dc, rect->left,rect->bottom-1, NULL);
        LineTo(dc, rect->right,rect->bottom-1);
        MoveToEx(dc, rect->left,rect->bottom-2, NULL);
        LineTo(dc, rect->right,rect->bottom-2);
        bottom=-2;
        }
    }

if ( style & 3840 )
    { //thick lines
    if(style & 256)
        { //left
        MoveToEx(dc, rect->left,rect->top, NULL);
        LineTo(dc, rect->left,rect->bottom);
        MoveToEx(dc, rect->left+1,rect->top, NULL);
        LineTo(dc, rect->left+1,rect->bottom);
        MoveToEx(dc, rect->left+2,rect->top, NULL);
        LineTo(dc, rect->left+2,rect->bottom);
        left=3;
        }

    if(style & 512)
        { //top
        MoveToEx(dc, rect->left,rect->top, NULL);
        LineTo(dc, rect->right,rect->top);
        MoveToEx(dc, rect->left,rect->top+1, NULL);
        LineTo(dc, rect->right,rect->top+1);
        MoveToEx(dc, rect->left,rect->top+2, NULL);
        LineTo(dc, rect->right,rect->top+2);
        top=3;
        }
    if(style & 1024)
        { //right
        MoveToEx(dc, rect->right-1,rect->top, NULL);
        LineTo(dc, rect->right-1,rect->bottom);
        MoveToEx(dc, rect->right-2,rect->top, NULL);
        LineTo(dc, rect->right-2,rect->bottom);
        MoveToEx(dc, rect->right-3,rect->top, NULL);
        LineTo(dc, rect->right-3,rect->bottom);
        right=-3;
        }

    if(style & 2048)
        { //bottom
        MoveToEx(dc, rect->left,rect->bottom-1, NULL);
        LineTo(dc, rect->right,rect->bottom-1);
        MoveToEx(dc, rect->left,rect->bottom-2, NULL);
        LineTo(dc, rect->right,rect->bottom-2);
        MoveToEx(dc, rect->left,rect->bottom-3, NULL);
        LineTo(dc, rect->right,rect->bottom-3);
        bottom=-3;
        }
    }
if(style &4096)
    { //3D recessed
    int loop;
    /*
    ----------
    Dark color
    ---------- */
    SelectObject(dc, m_ctrl->m_threeDDarkPen);
    for(loop=0;loop<m_ctrl->m_GI->m_threeDHeight;loop++)
        {
        MoveToEx(dc, rect->left+loop,rect->bottom-loop-1, NULL);
        LineTo(dc, rect->left+loop,rect->top+loop);
        LineTo(dc, rect->right-loop-1,rect->top+loop);
        }

    /*
    -----------
    Light color
    ----------- */
    SelectObject(dc, m_ctrl->m_threeDLightPen);
    for(loop=0;loop<m_ctrl->m_GI->m_threeDHeight;loop++)
        {
        MoveToEx(dc, rect->right-loop-1,rect->top+loop, NULL);
        LineTo(dc, rect->right-loop-1,rect->bottom-loop-1);
        LineTo(dc, rect->left+loop,rect->bottom-loop-1);
        }

    left = m_ctrl->m_GI->m_threeDHeight;
    top = m_ctrl->m_GI->m_threeDHeight;
    right = -m_ctrl->m_GI->m_threeDHeight;
    bottom = -m_ctrl->m_GI->m_threeDHeight;

    }

if ( style &8192 )
    { //3D raised
    int loop;
    /*
    -----------
    Light color
    ----------- */
    SelectObject(dc, m_ctrl->m_threeDLightPen);
    for(loop=0;loop<m_ctrl->m_GI->m_threeDHeight;loop++)
        {
        MoveToEx(dc, rect->left+loop,rect->bottom-loop-1, NULL);
        LineTo(dc, rect->left+loop,rect->top+loop);
        LineTo(dc, rect->right-loop-1,rect->top+loop);
        }
    //dark color
    SelectObject(dc, m_ctrl->m_threeDDarkPen);
    for(loop=0;loop<m_ctrl->m_GI->m_threeDHeight;loop++)
        {
        MoveToEx(dc, rect->right-loop-1,rect->top+loop, NULL);
        LineTo(dc, rect->right-loop-1,rect->bottom-loop-1);
        LineTo(dc, rect->left+loop,rect->bottom-loop-1);
        }
    left = m_ctrl->m_GI->m_threeDHeight;
    top = m_ctrl->m_GI->m_threeDHeight;
    right = -m_ctrl->m_GI->m_threeDHeight;
    bottom = -m_ctrl->m_GI->m_threeDHeight;
    }

rectout->left   = rect->left + left;
rectout->top    = rect->top + top;
rectout->right  = rect->right + right;
rectout->bottom = rect->bottom + bottom;

if ( origPen2 != NULL )
    SelectObject( dc, origPen2 );

return;
}

/***********************************************************************
*                               GetBestSize                            *
*   Returns the best (nominal) size for a cell using this cell         *
*   type, with the given cell properties.                              *
*       dc      - device context to use to calc the size on            *
*       size    - return the best size in this param                   *
*       cell    - pointer to a cell object to use for the calc.        *
***********************************************************************/
void CUGCellType::GetBestSize(HDC dc, SIZE * size, CUGCell * cell)
{
HFONT origFont = NULL;

/*
---------------
Select the font
--------------- */
if ( cell->IsPropertySet(UGCELL_FONT_SET) )
    origFont = (HFONT) SelectObject( dc, cell->GetFont() );
else if ( m_ctrl->m_GI->m_defFont != NULL )
    origFont = (HFONT) SelectObject( dc, m_ctrl->m_GI->m_defFont );

/*
-----------------
Get the best size
----------------- */
SIZE s = {3,3};
if ( cell->IsPropertySet(UGCELL_TEXT_SET) )
    GetTextExtentPoint32( dc, cell->GetText(), cell->GetTextLength(), &s );

/*
-----------
Use margins
----------- */
s.cx += 6;
s.cy += 2;

/*
-----------------------------
Select back the original font
----------------------------- */
if ( origFont != NULL )
    SelectObject( dc, origFont );

*size = s;
}

/***********************************************************************
*                           GetCellOverlapInfo                         *
*   Returns the best (nominal) size for a cell using this cell         *
*   type, with the given cell properties.                              *
*       dc      - device context to use to calc the size on            *
*       size    - return the best size in this param                   *
*       cell    - pointer to a cell object to use for the calc.        *
*   Return                                                             *
*       offset for drawing the overlapped cell                         *
***********************************************************************/
int CUGCellType::GetCellOverlapInfo( HDC dc, int col, long row, int * overlapCol, CUGCell * cell )
{
int         loop;
int         prevCol;
int         width;
SIZE        size;
HFONT       origFont;

origFont = 0;

if ( m_ctrl->m_GI->m_enableCellOverLap )
    {

    /*
    -----------------------------------------------------------------
    Check to see if there is a prev cell in the col that is not blank
    ----------------------------------------------------------------- */
    prevCol = col;
    if ( m_ctrl->m_GI->m_defDataSource->GetPrevNonBlankCol(&prevCol,row) == UG_SUCCESS )
        {
        /*
        ---------------------------------------------------
        Check to see if the text in the cell that was found
        is wider than the column that it is in
        --------------------------------------------------- */
        m_ctrl->GetCellIndirect( prevCol, row, cell );

        if ( cell->IsPropertySet(UGCELL_FONT_SET) )
            origFont = (HFONT) SelectObject( dc, cell->GetFont() );
        GetTextExtentPoint32( dc, cell->GetText(), cell->GetTextLength(), &size );
        size.cx += m_ctrl->m_GI->m_margin;

        /*
        -----------------------------------------------
        If the prev cell is wider than its column width
        and the cell allows overlapping
        ----------------------------------------------- */
        if ( size.cx > m_ctrl->GetColWidth(prevCol) && m_ctrl->GetCellType(cell->GetCellType())->CanOverLap(cell) != FALSE)
            {
            /*
            ----------------------------------------
            Check to see how many columns wide it is
            ---------------------------------------- */
            width = 0;
            for ( loop=prevCol; loop<col; loop++ )
                {
                if ( loop == m_ctrl->m_GI->m_numLockCols )
                    loop = m_ctrl->m_GI->m_leftCol;
                width += m_ctrl->GetColWidth( loop );
                if ( width > size.cx )
                    break;
                }

            /*
            -----------------------------------------------------------
            Check to see if the current cell is within the string width
            ----------------------------------------------------------- */
            if ( col == loop )
                {
                *overlapCol = prevCol;
                return -width;
                }
            }
        }
    }

if ( origFont != NULL )
    SelectObject( dc, origFont );

return 0;
}
