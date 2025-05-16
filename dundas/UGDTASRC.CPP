#include <windows.h>

/***********************************************************************
*                                                                      *
*  This is the interface that the grid uses to retrieve its data       *
*  as well as setting data. This abstract interface allows the         *
*  grid to connect to any type of data. Datasources can be             *
*  practically anything                                                *
*                                                                      *
*       i.e.    arrays                                                 *
*               linked lists                                           *
*               databases                                              *
*               flat files                                             *
*               real-time feeds                                        *
*               sensors                                                *
*               calculations                                           *
*                                                                      *
*  This is a base class which all other datasource must be             *
*  derived from. By defining a standard interface to the data,         *
*  an abstract layer is created which allows the uderlying data        *
*  to come from any source, plus allows the datasource to be           *
*  changed without any re-writing of code.                             *
*                                                                      *
*  The minimum number of functions that need to be over-ridden         *
*  is only ONE, GetCell. GetCell is called by the grid when it         *
*  needs information for display. The more functions that are          *
*  used in a derived class makes for better control of the data        *
*  being supplied.                                                     *
*                                                                      *
*  new functions can be added to derived datasource, but they          *
*  can never be called by the grid directly. even though the           *
*  grid generally works on a cell by cell basis, many datasource       *
*  (such as databases) tend to work on a row by row basis. To          *
*  allow data to be written to the datasource in this manner           *
*  transaction writing can be used within a datasource by              *
*  over-ridding the transaction functions. If a derived                *
*  datasource cannot return the number of rows that is contains        *
*  acuratly, then over-ride the datasouce OnHitBottom function.        *
*  This allows the grid to ask the datasource for new rows on          *
*  the fly.                                                            *
*                                                                      *
*  Stanard return values from a datasource are                         *
*     UG_SUCCESS  - success (0)                                        *
*     UG_NA       - not available (-1)                                 *
*     1 and up    - error codes                                        *
*                                                                      *
***********************************************************************/

#include "UGCtrl.h"

/***********************************************************************
*                            CUGDataSource                             *
*                            CUGDataSource                             *
***********************************************************************/
CUGDataSource::CUGDataSource()
{
m_ctrl = NULL;
m_ID   = -1;
}

/***********************************************************************
*                            CUGDataSource                             *
*                           ~CUGDataSource                             *
***********************************************************************/
CUGDataSource::~CUGDataSource()
{
}

/***********************************************************************
*                            CUGDataSource                             *
*                               SetID                                  *
***********************************************************************/
void CUGDataSource::SetID( long ID )
{
m_ID = ID;
}

/***********************************************************************
*                            CUGDataSource                             *
*                               GetID                                  *
***********************************************************************/
long CUGDataSource::GetID()
{
return m_ID;
}

/***********************************************************************
*                            CUGDataSource                             *
*                                Open                                  *
* Override this function to provide file loading to a data source.     *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::Open( LPCTSTR name, LPCTSTR option )
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                             SetPassword                              *
* Override this function if a password is needed for the data source   *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::SetPassword( LPCTSTR user, LPCTSTR pass )
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                               Close                                  *
*  Override this function if a close command is needed.                *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::Close()
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                                Save                                  *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::Save()
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                               SaveAs                                 *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::SaveAs( LPCTSTR name, LPCTSTR option )
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                             GetNumRows                               *
*                                                                      *
*  Return UG_NA       not available                                    *
*         number of rows                                               *
*                                                                      *
***********************************************************************/
long CUGDataSource::GetNumRows()
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                             GetNumCols                               *
*                                                                      *
*  Return  UG_NA       not available                                   *
*          number of columns                                           *
*                                                                      *
***********************************************************************/
int CUGDataSource::GetNumCols()
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                             GetColName                               *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::GetColName( int col, LPTSTR * string )
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                            GetColFromName                            *
*  Returns the column number for a named column                        *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::GetColFromName( LPCTSTR name, int * col )
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                             GetColType                               *
*                                                                      *
*   Returns the default data type of a column                          *
*   type =  0-string 1-bool 2-short 3-long 4-float                     *
*           5-double 6-currency 7-data 8-time                          *
*           8-blob 9-graphic 10-ole                                    *
*                                                                      *
***********************************************************************/
int CUGDataSource::GetColType( int col, int * type )
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                              AppendRow                               *
*   Appends a new row at the end of the current data                   *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::AppendRow()
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                              AppendRow                               *
***********************************************************************/
int CUGDataSource::AppendRow( CUGCell ** cellList, int numCells )
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                              InsertRow                               *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::InsertRow(long row)
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                              AppendCol                               *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::AppendCol()
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                              InsertCol                               *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::InsertCol(int col)
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                              DeleteRow                               *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::DeleteRow( long row )
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                              DeleteCol                               *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::DeleteCol(int col)
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                               Empty                                  *
*   Empties all the data in the data source                            *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::Empty()
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                             DeleteCell                               *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::DeleteCell( int col, long row )
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                               GetCell                                *
*                                                                      *
*      Updates the given cell with 'set' information for the           *
*      specified cell. (only information that is 'set' should be       *
*      updated, since every data property has an 'unset' state).       *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::GetCell(int col,long row,CUGCell *cell)
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                               SetCell                                *
*                                                                      *
*      Sets the cell - including all 'unset' information.              *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::SetCell(int col,long row,CUGCell *cell)
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                              FindFirst                               *
*   Returns the first row that contained the specified string in       *
*   the specified column                                               *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::FindFirst( LPTSTR string, int *col, long *row, long flags )
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                              FindNext                                *
***********************************************************************/
int CUGDataSource::FindNext( LPTSTR string, int *col, long * row, int flags )
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                              SortBy                                  *
*                                                                      *
*   Sorts (or indexes) the data by the specified column                *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::SortBy( int col, int flags )
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                              SortBy                                  *
*                                                                      *
*                                                                      *
*   Sorts the data starting with the specified column then sub-sorts   *
*   it up to 'num' cols                                                *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::SortBy( int * cols, int num, int flags )
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                              SetOption                               *
*   Datasource dependant function. Used to set data source             *
*   specific information and modes of operation                        *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::SetOption( int option, long param1, long param2 )
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                         GetPrevNonBlankCol                           *
***********************************************************************/
int CUGDataSource::GetPrevNonBlankCol(int *col,long row)
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                           StartTransaction                           *
***********************************************************************/
int CUGDataSource::StartTransaction()
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                          CancelTransaction                           *
***********************************************************************/
int CUGDataSource::CancelTransaction()
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                          FinishTransaction                           *
***********************************************************************/
int CUGDataSource::FinishTransaction()
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                              GetOption                               *
*                                                                      *
*   Datasource dependant function. Used to get data source             *
*   specific information and modes of operation                        *
*                                                                      *
*   Return: UG_NA       not available                                  *
*           UG_SUCCESS  success                                        *
*           1...        error codes (data source dependant)            *
*                                                                      *
***********************************************************************/
int CUGDataSource::GetOption( int option, long & param1, long & param2 )
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                             OnHitBottom                              *
***********************************************************************/
int CUGDataSource::OnHitBottom( long numrows, long rowspast, long * rowsfound )
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                              OnHitTop                                *
***********************************************************************/
int CUGDataSource::OnHitTop(long numrows,long rowspast,long *rowsfound)
{
return UG_NA;
}

/***********************************************************************
*                            CUGDataSource                             *
*                             OnRowChange                              *
***********************************************************************/
void CUGDataSource::OnRowChange(long oldRow,long newRow)
{
}

/***********************************************************************
*                            CUGDataSource                             *
*                             OnColChange                              *
***********************************************************************/
void CUGDataSource::OnColChange(int oldCol,int newCol)
{
}
