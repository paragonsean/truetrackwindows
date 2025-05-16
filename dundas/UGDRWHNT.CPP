#include <windows.h>

/***********************************************************************
*   Ultimate Grid 97                                                   *
*   Copyright 1994 - 1997 Dundas Software Ltd.                         *
*                                                                      *
*   CUGDrawHint                                                        *
*                                                                      *
*   This class is used internally by the grid to keep track of         *
*   which cells need redrawing The grid draws its cells in an          *
*   extremely optimized manner which gives it is great speed.          *
*                                                                      *
*   This is the class which helps the optimization process by          *
*   maintaining a list of cells that need to be redrawn.               *
*                                                                      *
*   Cells are added to this list by the grid when movement is          *
*   made and/or changes to the grid are made. Only the cells           *
*   that are affected are added.                                       *
*                                                                      *
*   When the grid is going to redraw itself it calls this              *
*   classes IsInvalid function to see if the cell really needs         *
*   to be redrawn                                                      *
*                                                                      *
***********************************************************************/

#include "UGCtrl.h"

/***********************************************************************
*                           CUGDrawHint                                *
***********************************************************************/
CUGDrawHint::CUGDrawHint()
{
m_List  = NULL;
m_VList = NULL;
}

/***********************************************************************
*                          ~CUGDrawHint                                *
***********************************************************************/
CUGDrawHint::~CUGDrawHint()
{
ClearHints();
}

/***********************************************************************
*                            SetAsValid                                *
***********************************************************************/
void CUGDrawHint::AddHint( int col, long row )
{
AddHint( col, row, col, row );
}

/***********************************************************************
*                            SetAsValid                                *
***********************************************************************/
void CUGDrawHint::AddHint( int startCol, long startRow, int endCol, long endRow )
{
UGDrwHintList   * next;
UGDrwHintList   * newhint;

next = m_List;
newhint = new UGDrwHintList;

if ( m_List != NULL )
    {
    while(next->next != NULL)
        next = next->next;
    next->next = newhint;
    }
else
    {
    m_List = newhint;
    m_minCol = startCol;
    m_minRow = startRow;
    m_maxCol = endCol;
    m_maxRow = endRow;
    }

newhint->next     = NULL;
newhint->startCol = startCol;
newhint->startRow = startRow;
newhint->endCol   = endCol;
newhint->endRow   = endRow;

if(startCol < m_minCol)
    m_minCol = startCol;

if(endCol > m_maxCol)
    m_maxCol = endCol;

if(startRow < m_minRow)
    m_minRow = startRow;

if(endRow > m_maxRow)
    m_maxRow = endRow;
}

/***********************************************************************
*                            SetAsValid                                *
***********************************************************************/
void CUGDrawHint::AddToHint( int col, long row )
{
AddToHint(col,row,col,row);
}

/***********************************************************************
*                            SetAsValid                                *
***********************************************************************/
void CUGDrawHint::AddToHint( int startCol, long startRow, int endCol, long endRow )
{
if ( m_List == NULL )
    AddHint(startCol,startRow,endCol,endRow);

if ( m_List->startCol > startCol )
    m_List->startCol = startCol;
if ( m_List->endCol < endCol )
    m_List->endCol = endCol;
if ( m_List->startRow > startRow )
    m_List->startRow = startRow;
if ( m_List->endRow < endRow )
    m_List->endRow = endRow;

if ( startCol < m_minCol )
    m_minCol = startCol;
if ( endCol > m_maxCol )
    m_maxCol = endCol;
if ( startRow < m_minRow )
    m_minRow = startRow;
if ( endRow > m_maxRow )
    m_maxRow = endRow;
}

/***********************************************************************
*                            SetAsValid                                *
***********************************************************************/
void CUGDrawHint::ClearHints()
{
UGDrwHintList  * current;
UGDrwHintList  * next;
UGDrwHintVList * currentV;
UGDrwHintVList * nextV;

/*
----------------------
Clear the Invalid list
---------------------- */
current = m_List;

while ( current )
    {
    next = current->next;
    delete current;
    current = next;
    }

m_List = NULL;

/*
--------------------
Clear the valid list
-------------------- */
currentV = m_VList;

while ( currentV )
    {
    nextV = currentV->next;
    delete currentV;
    currentV = nextV;
    }

m_VList = NULL;
}

/***********************************************************************
*                            SetAsValid                                *
***********************************************************************/
int CUGDrawHint::IsInvalid( int col, long row )
{
UGDrwHintList * current;

if ( m_List == NULL )
    return FALSE;

if ( col < m_minCol || col > m_maxCol )
    return FALSE;

if ( row < m_minRow || row > m_maxRow )
    return FALSE;

current = m_List;

/*
----------------------
Check the invalid list
---------------------- */
while ( current )
    {
    /*
    -----------------------------------------------------------
    Check to see if the item in the list covers a greater range
    ----------------------------------------------------------- */
    if ( col >= current->startCol && col <= current->endCol )
        {
        if ( row >= current->startRow && row <= current->endRow )
            return TRUE;
        }
    current = current->next;
    }

/*
---------------------------------------------------------
If the item is not in the invalid list assume it is valid
--------------------------------------------------------- */
return FALSE;
}

/***********************************************************************
*                            SetAsValid                                *
***********************************************************************/
int CUGDrawHint::IsValid( int col, long row )
{
UGDrwHintVList * currentV;

if ( !m_VList )
    return FALSE;

currentV = m_VList;

/*
--------------------
Check the valid list
-------------------- */
while ( currentV )
    {
    if ( col == currentV->Col && row == currentV->Row )
        return TRUE;
    currentV = currentV->next;
    }

return FALSE;
}

/***********************************************************************
*                            SetAsValid                                *
***********************************************************************/
int CUGDrawHint::GetTotalRange( int * startCol, long * startRow, int * endCol, long * endRow )
{
UGDrwHintList * current;

if ( !m_List )
    {
    /*
    --------------------------------------------------------------
    Put in def values just in case the return value is not checked
    -------------------------------------------------------------- */
    *startCol = 0;
    *startRow = 0;
    *endCol   = 0;
    *endRow   = 0;
    return FALSE;
    }

*startCol = m_List->startCol;
*startRow = m_List->startRow;
*endCol   = m_List->endCol;
*endRow   = m_List->endRow;

current = m_List;

while ( current )
    {
    /*
    -----------------------------------------------------------
    Check to see if the item in the list covers a greater range
    ----------------------------------------------------------- */
    if ( *startCol > current->startCol )
        *startCol = current->startCol;

    if ( *startRow > current->startRow)
        *startRow = current->startRow;

    if ( *endCol < current->endCol )
        *endCol = current->endCol;

    if ( *endRow < current->endRow )
        *endRow = current->endRow;

    current = current->next;
    }


return TRUE;
}

/***********************************************************************
*                            SetAsValid                                *
***********************************************************************/
void CUGDrawHint::SetAsValid( int col, long row )
{

UGDrwHintVList  * nextV;
UGDrwHintVList  * newhintV;

nextV    = m_VList;
newhintV = new UGDrwHintVList;

if ( m_VList != NULL )
    {
    while(nextV->next != NULL)
        nextV = nextV->next;
    nextV->next = newhintV;
    }
else
    {
    m_VList = newhintV;
    }

newhintV->next = NULL;
newhintV->Col  = col;
newhintV->Row  = row;
}
