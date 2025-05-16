#include <windows.h>
#include "UGCtrl.h"

/***********************************************************************
*                                                                      *
*   This class hold all the properties of a cell. This class is        *
*   used internally and externally to set and retrieve cell            *
*   properties. Although this is the native format for cell            *
*   information datasouces may keep their own data in any              *
*   format, as long as they convert the data into CUGCell format       *
*   when the grid needs the information. The grid has many             *
*   functions which allow cell properties to be set and                *
*   retrieved without the need to use one of these object              *
*   directly (QuickSet and QuickGet functions) But to have the         *
*   greatest control, and performance the use of these objects         *
*   directly is required.                                              *
*                                                                      *
*   Each cell object has over 20 properties which can be set and       *
*   retrieved. Each property also has two states SET and NOT-SET       *
*   When a property is set a flag in the cell is set to indicate       *
*   that the property is SET. The reason for this SET/NOT-SET          *
*   design is to allow default styles for a cell. This way when        *
*   the grid is gathering cell information for display it can          *
*   get the cells default style(s) first (ie. column defaults,         *
*   cell styles, grid default, heading defaults), then retrieve        *
*   the cell properties next, and only the SET properties are          *
*   retrieved. This way the a cell can have default values but         *
*   can easily over-ride these values as well.                         *
*                                                                      *
*   The cells text (or string) property is also its data               *
*   property. This means that numbers, times, currency, etc. are       *
*   all stored in text format in this property. Even though the        *
*   cell stores all data as text, there is a flag which                *
*   indecates how the text should be treaded (text, time,              *
*   currency, numbers etc.).                                           *
*                                                                      *
***********************************************************************/

/***********************************************************************
*                               CUGCELL                                *
***********************************************************************/
CUGCell::CUGCell()
{
m_string       = 0;
m_mask         = 0;
m_label        = 0;
m_propSetFlags = 0;
m_format       = 0;
m_cellStyle    = 0;
m_dataType     = 0;
m_isReadOnly   = FALSE;
m_param        = 0;
m_cellType     = 0;
m_cellTypeEx   = 0;
m_textColor    = 0;
m_backColor    = 0;
m_hTextColor   = 0;
m_hBackColor   = 0;
m_borderStyle  = 0;
m_borderPen    = 0;
m_font         = 0;
m_bitmap       = 0;
m_alignment    = 0;
m_joinOrigin   = 0;
m_joinRow      = 0;
m_joinCol      = 0;

ClearAll();
}

/***********************************************************************
*                              ~CUGCELL                                *
***********************************************************************/
CUGCell::~CUGCell()
{
ClearAll();
}

/***********************************************************************
*                              ClearAll                                *
*     Set all the cell information properties to an "unused" state     *
***********************************************************************/
void CUGCell::ClearAll()
{

m_propSetFlags  = 0;

/*
-----------------
Clear the strings
----------------- */

if ( m_string )
    {
    delete[] m_string;
    m_string = 0;
    }

if ( m_mask )
    {
    delete[] m_mask;
    m_mask = 0;
    }

if ( m_label )
    {
    delete[] m_label;
    m_label = 0;
    }

}

/***********************************************************************
*                            SETDEFAULTINFO                            *
*             Sets up the cell with default information.               *
*  NOTE: when a cell is created there is no default information        *
***********************************************************************/
int CUGCell::SetDefaultInfo()
{
const unsigned long DEFAULT_FLAGS = UGCELL_TEXTCOLOR_SET  | UGCELL_BACKCOLOR_SET | UGCELL_HTEXTCOLOR_SET |
                                    UGCELL_HBACKCOLOR_SET | UGCELL_ALIGNMENT_SET;
ClearAll();

m_propSetFlags = DEFAULT_FLAGS;
m_format       = 0;
m_cellStyle    = 0;
m_textColor    = GetSysColor(COLOR_WINDOWTEXT);
m_backColor    = GetSysColor(COLOR_WINDOW);
m_hTextColor   = GetSysColor(COLOR_HIGHLIGHTTEXT);
m_hBackColor   = GetSysColor(COLOR_HIGHLIGHT);
m_alignment    = 0;

return UG_SUCCESS;
}

/***********************************************************************
*                            COPYINFOTO                                *
***********************************************************************/
int CUGCell::CopyInfoTo( CUGCell * dest )
{

dest->m_propSetFlags = m_propSetFlags;

return AddCellInfo( this, dest );
}

/***********************************************************************
*                            COPYINFOFROM                              *
***********************************************************************/
int CUGCell::CopyInfoFrom( CUGCell * sorc )
{

m_propSetFlags = sorc->m_propSetFlags;

return AddCellInfo( sorc, this );
}

/***********************************************************************
*                            COPYCELLINFO                              *
***********************************************************************/
int CUGCell::CopyCellInfo( CUGCell * sorc, CUGCell * dest )
{
dest->m_propSetFlags = sorc->m_propSetFlags;

return AddCellInfo( sorc, dest );
}

/***********************************************************************
*                             ADDINFOTO                                *
*       Copies only the properties that are set from this cell         *
*       to the destiination cell. NOTE: all properties have a          *
*       set and unset state. Their state is detirmined by a bit        *
*       int the m_propSetFlags member variable                         *
***********************************************************************/
int CUGCell::AddInfoTo( CUGCell * dest )
{
return AddCellInfo( this, dest );
}

/***********************************************************************
*                             ADDINFOFROM                              *
***********************************************************************/
int CUGCell::AddInfoFrom( CUGCell * sorc )
{
return AddCellInfo( sorc, this );
}

/***********************************************************************
*                             ADDCELLINFO                              *
*       Copies only the properties that are set in the source          *
*       cell to the dest cell object. NOTE: all properties have        *
*       a set and unset state. Their state is detirmined by a          *
*       bit int the m_propSetFlags member variable                     *
***********************************************************************/
int CUGCell::AddCellInfo( CUGCell * sorc, CUGCell * dest )
{

/*
----------
Properties
---------- */
dest->m_propSetFlags |= sorc->m_propSetFlags;

/*
----
Mask
---- */
if ( sorc->m_propSetFlags & UGCELL_STRING_SET )
    CopyString( dest->m_string, sorc->m_string );

if ( sorc->m_propSetFlags & UGCELL_MASK_SET )
    CopyString( dest->m_mask, sorc->m_mask );

/*
-----
Label
----- */
if ( sorc->m_propSetFlags & UGCELL_LABEL_SET )
    CopyString( dest->m_label, sorc->m_label );

/*
-------------------------------
Format and style class pointers
------------------------------- */
if ( sorc->m_propSetFlags & UGCELL_FORMAT_SET )
    dest->m_format = sorc->m_format;

if( sorc->m_propSetFlags & UGCELL_STYLE_SET )
    dest->m_cellStyle = sorc->m_cellStyle;

/*
---------
Data type
--------- */
if ( sorc->m_propSetFlags & UGCELL_DATATYPE_SET )
    dest->m_dataType = sorc->m_dataType;

/*
---------------------
General purpose value
--------------------- */
if ( sorc->m_propSetFlags & UGCELL_PARAM_SET )
    dest->m_param = sorc->m_param;

/*
---------
Cell type
--------- */
if ( sorc->m_propSetFlags & UGCELL_CELLTYPE_SET )
    dest->m_cellType = sorc->m_cellType;
if ( sorc->m_propSetFlags & UGCELL_CELLTYPEEX_SET )
    dest->m_cellTypeEx = sorc->m_cellTypeEx;

/*
------
Colors
------ */
if ( sorc->m_propSetFlags & UGCELL_TEXTCOLOR_SET )
    dest->m_textColor = sorc->m_textColor;
if ( sorc->m_propSetFlags & UGCELL_BACKCOLOR_SET )
    dest->m_backColor = sorc->m_backColor;
if ( sorc->m_propSetFlags & UGCELL_HTEXTCOLOR_SET )
    dest->m_hTextColor = sorc->m_hTextColor;
if ( sorc->m_propSetFlags & UGCELL_HBACKCOLOR_SET )
    dest->m_hBackColor = sorc->m_hBackColor;

/*
-------
Borders
------- */
if ( sorc->m_propSetFlags & UGCELL_BORDERSTYLE_SET )
    dest->m_borderStyle = sorc->m_borderStyle;
if ( sorc->m_propSetFlags & UGCELL_BORDERCOLOR_SET )
    dest->m_borderPen = sorc->m_borderPen;

/*
---------------
Font and bitmap
--------------- */
if ( sorc->m_propSetFlags & UGCELL_FONT_SET )
    dest->m_font = sorc->m_font;
if ( sorc->m_propSetFlags & UGCELL_BITMAP_SET )
    dest->m_bitmap = sorc->m_bitmap;

/*
---------
Alignment
--------- */
if ( sorc->m_propSetFlags & UGCELL_ALIGNMENT_SET )
    dest->m_alignment = sorc->m_alignment;

/*
----------
Join cells
---------- */
if ( sorc->m_propSetFlags & UGCELL_JOIN_SET )
    {
    dest->m_joinOrigin = sorc->m_joinOrigin;
    dest->m_joinCol    = sorc->m_joinCol;
    dest->m_joinRow    = sorc->m_joinRow;
    }

/*
---------
Read only
--------- */
if ( sorc->m_propSetFlags & UGCELL_READONLY_SET )
    dest->m_isReadOnly = sorc->m_isReadOnly;

return UG_SUCCESS;
}

/*
-------------------------------------
Flags for the following two functions
------------------------------------- */
const unsigned long STRING_SET_FLAGS = UGCELL_STRING_SET | UGCELL_DATATYPE_SET;

/***********************************************************************
*                               SETTEXT                                *
***********************************************************************/
int CUGCell::SetText( const TCHAR * sorc )
{

CopyString( m_string, sorc );
m_dataType = UGCELLDATA_STRING;
m_propSetFlags |= STRING_SET_FLAGS;

return UG_SUCCESS;
}

/***********************************************************************
*                             APPENDTEXT                               *
***********************************************************************/
int CUGCell::AppendText( const TCHAR * text )
{

if ( (m_propSetFlags & UGCELL_STRING_SET) == 0 )
    return UG_ERROR;

AddTwoString( m_string, text );
m_dataType  = UGCELLDATA_STRING;
m_propSetFlags |= STRING_SET_FLAGS;

return UG_SUCCESS;
}

/***********************************************************************
*                             GETTEXT                                  *
***********************************************************************/
TCHAR * CUGCell::GetText()
{

if ( m_propSetFlags & UGCELL_STRING_SET )
    return m_string;

return 0;
}

/***********************************************************************
*                          GETTEXTLENGTH                               *
***********************************************************************/
int CUGCell::GetTextLength()
{
int n;

n = 0;
if ( (m_propSetFlags & UGCELL_STRING_SET) && m_string )
    n = lstrlen( m_string );

return n;
}

/***********************************************************************
*                             SETMASK                                  *
***********************************************************************/
int CUGCell::SetMask( const TCHAR * text )
{
CopyString( m_mask, text );
m_propSetFlags |= UGCELL_MASK_SET;

return UG_SUCCESS;
}

/***********************************************************************
*                             GETMASK                                  *
***********************************************************************/
TCHAR * CUGCell::GetMask()
{
if ( m_propSetFlags & UGCELL_MASK_SET )
    return m_mask;

return 0;
}

/***********************************************************************
*                           SETLABELTEXT                               *
***********************************************************************/
int CUGCell::SetLabelText( const TCHAR * text )
{

if ( !text || lstrlen(text) < 1 )
    {
    if ( m_label )
        {
        delete[] m_label;
        m_label = 0;
        }
    m_propSetFlags &= ~UGCELL_LABEL_SET;
    }
else
    {
    CopyString( m_label, text );
    m_propSetFlags |= UGCELL_LABEL_SET;
    }

return UG_SUCCESS;
}

/***********************************************************************
*                           GETLABELTEXT                               *
***********************************************************************/
TCHAR * CUGCell::GetLabelText()
{
if ( m_propSetFlags & UGCELL_LABEL_SET )
    return m_label;

return NULL;
}

/***********************************************************************
*                           SETDATATYPE                                *
*           UGCELLDATA_STRING is the only value allowed now            *
***********************************************************************/
int CUGCell::SetDataType( short type )
{

m_dataType  = type;
m_propSetFlags |= UGCELL_DATATYPE_SET;

return UG_SUCCESS;
}

/***********************************************************************
*                           GETDATATYPE                                *
***********************************************************************/
int CUGCell::GetDataType()
{
return m_dataType;
}

/***********************************************************************
*                           SETTEXTCOLOR                               *
***********************************************************************/
int CUGCell::SetTextColor( COLORREF color )
{

m_textColor = color;
m_propSetFlags |= UGCELL_TEXTCOLOR_SET;

return UG_SUCCESS;
}

/***********************************************************************
*                           GETTEXTCOLOR                               *
***********************************************************************/
COLORREF CUGCell::GetTextColor()
{

if ( m_propSetFlags & UGCELL_TEXTCOLOR_SET )
    return m_textColor;

return GetSysColor( COLOR_WINDOWTEXT );
}

/***********************************************************************
*                           SETBACKCOLOR                               *
***********************************************************************/
int CUGCell::SetBackColor( COLORREF color )
{

m_backColor = color;
m_propSetFlags |= UGCELL_BACKCOLOR_SET;

return UG_SUCCESS;
}

/***********************************************************************
*                           SETBACKCOLOR                               *
***********************************************************************/
COLORREF CUGCell::GetBackColor()
{
if ( m_propSetFlags & UGCELL_BACKCOLOR_SET )
    return m_backColor;

return GetSysColor( COLOR_WINDOW );
}

/***********************************************************************
*                           SETHTEXTCOLOR                              *
***********************************************************************/
int CUGCell::SetHTextColor( COLORREF color )
{

m_hTextColor = color;
m_propSetFlags |= UGCELL_HTEXTCOLOR_SET;

return UG_SUCCESS;
}

/***********************************************************************
*                           GETHTEXTCOLOR                              *
***********************************************************************/
COLORREF CUGCell::GetHTextColor()
{
if ( m_propSetFlags & UGCELL_HTEXTCOLOR_SET )
    return m_hTextColor;

return GetSysColor(COLOR_HIGHLIGHTTEXT);
}

/***********************************************************************
*                           SETHBACKCOLOR                              *
***********************************************************************/
int CUGCell::SetHBackColor( COLORREF color )
{

m_hBackColor = color;
m_propSetFlags |= UGCELL_HBACKCOLOR_SET;

return UG_SUCCESS;
}

/***********************************************************************
*                           GETHBACKCOLOR                              *
***********************************************************************/
COLORREF CUGCell::GetHBackColor()
{

if ( m_propSetFlags & UGCELL_HBACKCOLOR_SET )
    return m_hBackColor;

return GetSysColor( COLOR_HIGHLIGHT );
}

/***********************************************************************
*                            SETALIGNMENT                              *
*       Sets the alignment property of the cell. Verticle and          *
*       horizontal settings can be ORed together.                      *
*                                                                      *
*       Valid values are:                                              *
*           UG_ALIGNLEFT                                               *
*           UG_ALIGNRIGHT                                              *
*           UG_ALIGNCENTER                                             *
*           UG_ALIGNTOP                                                *
*           UG_ALIGNBOTTOM                                             *
*           UG_ALIGNVCENTER                                            *
***********************************************************************/
int CUGCell::SetAlignment( short align )
{
m_propSetFlags |= UGCELL_ALIGNMENT_SET;
m_alignment = align;

return UG_SUCCESS;
}

/***********************************************************************
*                            GETALIGNMENT                              *
***********************************************************************/
short CUGCell::GetAlignment()
{
if ( m_propSetFlags & UGCELL_ALIGNMENT_SET )
    return m_alignment;

return 0;
}

/***********************************************************************
*                             SETBORDER                                *
***********************************************************************/
int CUGCell::SetBorder( short style )
{

m_propSetFlags |= UGCELL_BORDERSTYLE_SET;
m_borderStyle = style;

return UG_SUCCESS;
}

/***********************************************************************
*                             GETBORDER                                *
*  Returns the cells border style property. The return value is        *
*  made up of bits which corresond to a style for each piece of        *
*  the border.                                                         *
*                                                                      *
*  Valid values are.                                                   *
*  UG_BDR_LTHIN UG_BDR_TTHIN UG_BDR_RTHIN UG_BDR_BTHIN                 *
*  UG_BDR_LMEDIUM UG_BDR_TMEDIUM UG_BDR_RMEDIUM UG_BDR_BMEDIUM         *
*  UG_BDR_LTHICK UG_BDR_TTHICK UG_BDR_RTHICK UG_BDR_BTHICK             *
*  UG_BDR_RECESSED UG_BDR_RAISED                                       *
***********************************************************************/
short CUGCell::GetBorder()
{
if ( m_propSetFlags & UGCELL_BORDERSTYLE_SET )
    return m_borderStyle;

return 0;
}

/***********************************************************************
*                             SETBORDERCOLOR                           *
*     The pen should use the PS_SOLID style and be 1 pixel wide.       *
***********************************************************************/
int CUGCell::SetBorderColor( HPEN pen )
{
if ( pen )
    {
    m_propSetFlags |= UGCELL_BORDERCOLOR_SET;
    m_borderPen = pen;

    return UG_SUCCESS;
    }

return UG_ERROR;
}

/***********************************************************************
*                             GETBORDERCOLOR                           *
***********************************************************************/
HPEN CUGCell::GetBorderColor()
{
if ( m_propSetFlags & UGCELL_BORDERCOLOR_SET )
    return m_borderPen;

return 0;
}

/***********************************************************************
*                               SETFONT                                *
***********************************************************************/
int CUGCell::SetFont( HFONT font )
{
if ( font )
    {
    m_propSetFlags |= UGCELL_FONT_SET;
    m_font = font;

    return UG_SUCCESS;
    }

return UG_ERROR;
}

/***********************************************************************
*                               GETFONT                                *
***********************************************************************/
HFONT CUGCell::GetFont()
{

if ( m_propSetFlags & UGCELL_FONT_SET )
    return m_font;

return 0;
}

/***********************************************************************
*                            SETBITMAP                                 *
*      Sets the bitmap property of the cell. The bitmap property       *
*      holds a pointer to a CBitmap object, but does not take          *
*      ownership. When the cell object is destroyed it does not        *
*      delete the bitmap object, it is the responsibility of the       *
*      calling function/application.                                   *
***********************************************************************/
int CUGCell::SetBitmap( HBITMAP bitmap )
{

if ( bitmap )
    {
    m_bitmap = bitmap;
    m_propSetFlags |= UGCELL_BITMAP_SET;

    return UG_SUCCESS;
    }

return UG_ERROR;
}

/***********************************************************************
*                            GETBITMAP                                 *
***********************************************************************/
HBITMAP CUGCell::GetBitmap()
{

if ( m_propSetFlags & UGCELL_BITMAP_SET )
    return m_bitmap;

return 0;
}

/***********************************************************************
*                            SETREADONLY                               *
***********************************************************************/
int CUGCell::SetReadOnly( BOOL state )
{

if ( state )
    m_isReadOnly = TRUE;
else
    m_isReadOnly = FALSE;

m_propSetFlags |= UGCELL_READONLY_SET;

return UG_SUCCESS;
}

/***********************************************************************
*                            GETREADONLY                               *
***********************************************************************/
BOOL CUGCell::GetReadOnly()
{

if ( m_propSetFlags & UGCELL_READONLY_SET )
    return m_isReadOnly;

return FALSE;
}

/***********************************************************************
*                            SETCELLTYPE                               *
***********************************************************************/
int CUGCell::SetCellType(int index)
{

m_propSetFlags |= UGCELL_CELLTYPE_SET;
m_cellType = index;

return UG_SUCCESS;
}

/***********************************************************************
*                            SETCELLTYPEEX                             *
***********************************************************************/
int CUGCell::SetCellTypeEx( long type )
{

m_propSetFlags |= UGCELL_CELLTYPEEX_SET;
m_cellTypeEx = type;

return UG_SUCCESS;
}

/***********************************************************************
*                            GETCELLTYPE                               *
***********************************************************************/
int CUGCell::GetCellType()
{

if ( m_propSetFlags & UGCELL_CELLTYPE_SET )
    return m_cellType;

return 0;
}

/***********************************************************************
*                            GETCELLTYPEEX                             *
***********************************************************************/
long CUGCell::GetCellTypeEx()
{
if ( m_propSetFlags & UGCELL_CELLTYPEEX_SET )
    return m_cellTypeEx;

return 0;
}

/***********************************************************************
*                            SETJOININFO                               *
*                                                                      *
*    Sets join information for a cell. If origin is TRUE then col      *
*    and row point to the last cell in the region. If origin is        *
*    FALSE then col and row point to the first cell in the             *
*    region. col and row are relative numbers. Ever cell in the        *
*    joined region need to have their join properties set.             *
*                                                                      *
*       row, col - relative to the start or end of join                *
***********************************************************************/
int CUGCell::SetJoinInfo( BOOL origin,int col,long row )
{

m_joinOrigin  = origin;
m_joinCol     = col;
m_joinRow     = row;

m_propSetFlags |= UGCELL_JOIN_SET;

return UG_SUCCESS;
}

/***********************************************************************
*                            GETJOININFO                               *
*       origin   - join region origin flag                             *
*       row, col - relative to the start or end of join                *
***********************************************************************/
int CUGCell::GetJoinInfo( BOOL * origin, int * col, long * row )
{

if ( m_propSetFlags & UGCELL_JOIN_SET )
    {
    *origin = m_joinOrigin;
    *col    = m_joinCol;
    *row    = m_joinRow;
    return UG_SUCCESS;
    }

return UG_ERROR;
}

/***********************************************************************
*                            ISPROPERTYSET                             *
***********************************************************************/
BOOL CUGCell::IsPropertySet( unsigned long flag )
{
if ( flag & m_propSetFlags )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                            SETPROPERTYFLAGS                          *
***********************************************************************/
int CUGCell::SetPropertyFlags( unsigned long flags )
{
m_propSetFlags = flags;

return UG_SUCCESS;
}

/***********************************************************************
*                            GETPROPERTYFLAGS                          *
***********************************************************************/
unsigned long CUGCell::GetPropertyFlags()
{
return m_propSetFlags;
}

/***********************************************************************
*                            CLEARPROPERTY                             *
***********************************************************************/
int CUGCell::ClearProperty( unsigned long propertyFlag )
{
if ( m_propSetFlags & propertyFlag )
    m_propSetFlags |= (~propertyFlag);

return UG_SUCCESS;
}

/***********************************************************************
*                            SETCELLSTYLE                              *
*                                                                      *
* Sets a pointer to another cell object that acts as a style for       *
* this cell. Therefore when properties are being retrieved the         *
* information found in this cell is used first.                        *
*                                                                      *
***********************************************************************/
int CUGCell::SetCellStyle( CUGCell * cell )
{

if ( cell )
    {
    m_propSetFlags |= UGCELL_STYLE_SET;
    m_cellStyle = cell;
    return UG_SUCCESS;
    }

return UG_ERROR;
}

/***********************************************************************
*                            GETCELLSTYLE                              *
***********************************************************************/
CUGCell * CUGCell::GetCellStyle()
{

if( m_propSetFlags & UGCELL_STYLE_SET )
    return m_cellStyle;

return 0;
}

/***********************************************************************
*                           SETFORMATCLASS                             *
***********************************************************************/
int CUGCell::SetFormatClass( CUGCellFormat * format )
{

if ( format )
    {
    m_propSetFlags |= UGCELL_FORMAT_SET;
    m_format = format;
    return UG_SUCCESS;
    }

return UG_ERROR;
}

/***********************************************************************
*                           GETFORMATCLASS                             *
***********************************************************************/
CUGCellFormat * CUGCell::GetFormatClass()
{

if( m_propSetFlags & UGCELL_FORMAT_SET )
    return m_format;

return 0;
}

