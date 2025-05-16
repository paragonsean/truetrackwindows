#include <windows.h>

/***********************************************************************
*                          Arrow Cell Type                             *
*                                                                      *
* CellTypeNumber:     UGCT_ARROW                                       *
* CellTypeExNumbers:  UGCT_ARROWLEFT      left pointing arrow          *
*                     UGCT_ARROWRIGHT     right pointing arrow         *
*                     UGCT_ARROWDLEFT     double left pointing arrow   *
*                     UGCT_ARROWDRIGHT    double right pointing arrow  *
*                                                                      *
* Text color determines the color of the arrow                         *
* back color determines the background color                           *
* alignment is always centered                                         *
***********************************************************************/

#include "UGCtrl.h"

static const TCHAR EmptyString[] = TEXT( "" );

/***********************************************************************
*                           CUGArrowType                               *
***********************************************************************/
CUGArrowType::CUGArrowType()
{
m_canTextEdit = FALSE;
}

/***********************************************************************
*                          ~CUGArrowType                               *
***********************************************************************/
CUGArrowType::~CUGArrowType()
{
}

/***********************************************************************
*                             OnDraw                                   *
***********************************************************************/
void CUGArrowType::OnDraw( HDC dc, RECT * rect, int col, long row, CUGCell * cell, int selected, int current )
{
HPEN pen;
HPEN oldpen;
int  style;
int  hcenter;
int  vcenter;

DrawBorder( dc, rect, rect, cell );

style = UGCT_ARROWRIGHT;

if ( cell->IsPropertySet(UGCELL_CELLTYPEEX_SET) )
    style = cell->GetCellTypeEx();

/*
-------------------
Draw the background
------------------- */
if ( selected || current )
    SetBkColor( dc, cell->GetHBackColor() );
else
    SetBkColor( dc, cell->GetBackColor() );

ExtTextOut( dc, 0, 0, ETO_OPAQUE, rect, EmptyString, 0, NULL );

/*
-----------------------------------
Check the width of the drawing area
----------------------------------- */
if ( (rect->right - rect->left) < 4 )
    return;

/*
---------------------------------
Set up the pen color to draw with
--------------------------------- */
pen    = CreatePen( PS_SOLID, 1, cell->GetTextColor() );
oldpen = (HPEN) SelectObject( dc, pen );

/*
----------------------------
Get the arrow center co-ords
---------------------------- */
vcenter = rect->top  + (rect->bottom - rect->top)/2;
hcenter = rect->left + (rect->right - rect->left)/2;

if ( style & UGCT_ARROWRIGHT || style == 0 )
    {
    /*
    ---------------------
    Right arrow (default)
    --------------------- */
    MoveToEx(dc, hcenter-1,vcenter-3, NULL);
    LineTo(dc, hcenter-1,vcenter+4);
    MoveToEx(dc, hcenter,vcenter-2, NULL);
    LineTo(dc, hcenter,vcenter+2);
    LineTo(dc, hcenter+2,vcenter);
    LineTo(dc, hcenter+1,vcenter-1);
    LineTo(dc, hcenter+1,vcenter+1);
    }
else if ( style & UGCT_ARROWLEFT )
    {
    /*
    ----------
    Left arrow
    ---------- */
    MoveToEx(dc, hcenter+1,vcenter-3, NULL);
    LineTo(dc, hcenter+1,vcenter+4);
    MoveToEx(dc, hcenter,vcenter-2, NULL);
    LineTo(dc, hcenter,vcenter+2);
    LineTo(dc, hcenter-2,vcenter);
    LineTo(dc, hcenter-1,vcenter-1);
    LineTo(dc, hcenter-1,vcenter+1);
    }
else if ( style&UGCT_ARROWDRIGHT )
    {
    /*
    ------------------
    Double right arrow
    ------------------ */
    MoveToEx(dc,hcenter+2,vcenter-3, NULL);
    LineTo(dc, hcenter+2,vcenter+4);
    MoveToEx(dc, hcenter+3,vcenter-2, NULL);
    LineTo(dc, hcenter+3,vcenter+2);
    LineTo(dc, hcenter+5,vcenter);
    LineTo(dc, hcenter+4,vcenter-1);
    LineTo(dc, hcenter+4,vcenter+1);

    MoveToEx(dc, hcenter-4,vcenter-3, NULL);
    LineTo(dc, hcenter-4,vcenter+4);
    MoveToEx(dc, hcenter-3,vcenter-2, NULL);
    LineTo(dc, hcenter-3,vcenter+2);
    LineTo(dc, hcenter-1,vcenter);
    LineTo(dc, hcenter-2,vcenter-1);
    LineTo(dc, hcenter-2,vcenter+1);
    }
else if ( style & UGCT_ARROWDLEFT )
    {
    /*
    -----------------
    Double left arrow
    ----------------- */
    MoveToEx(dc, hcenter+4,vcenter-3, NULL);
    LineTo(dc, hcenter+4,vcenter+4);
    MoveToEx(dc, hcenter+3,vcenter-2, NULL);
    LineTo(dc, hcenter+3,vcenter+2);
    LineTo(dc, hcenter+1,vcenter);
    LineTo(dc, hcenter+2,vcenter-1);
    LineTo(dc, hcenter+2,vcenter+1);

    MoveToEx(dc, hcenter-2,vcenter-3, NULL);
    LineTo(dc, hcenter-2,vcenter+4);
    MoveToEx(dc, hcenter-3,vcenter-2, NULL);
    LineTo(dc, hcenter-3,vcenter+2);
    LineTo(dc, hcenter-5,vcenter);
    LineTo(dc, hcenter-4,vcenter-1);
    LineTo(dc, hcenter-4,vcenter+1);
    }

SelectObject(dc, oldpen);
DeleteObject(pen);
}
