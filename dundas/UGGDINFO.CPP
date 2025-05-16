#include <windows.h>

/***********************************************************************
*   Ultimate Grid 97                                                   *
*   Copyright 1994 - 1997 Dundas Software Ltd.                         *
*                                                                      *
*   CUGGridInfo                                                        *
*                                                                      *
*   This class contains all of the grid specific properties. An        *
*   instance of this class is created for every grid sheet that        *
*   is created. This way every sheet can have its own                  *
*   properties.                                                        *
*                                                                      *
*   Since all of the grid's properties are stored in CUGGridInfo       *
*   classes, it makes it quite simple to serialize the                 *
*   information                                                        *
*                                                                      *
*   The CUGCtrl class holds an array of these classes with one         *
*   element for every grid sheet. (the grid by default has one         *
*   sheet)                                                             *
*                                                                      *
*   The properties found in this class should be set using the         *
*   functions found in the CUGCtrl class. By setting properties        *
*   this way, the grid will only set the properties for the            *
*   currently selected sheet.                                          *
*                                                                      *
***********************************************************************/

#include "UGCtrl.h"

/***********************************************************************
*                             CUGGRIDINFO                              *
***********************************************************************/
CUGGridInfo::CUGGridInfo()
{

/*
------------------
Column information
------------------ */
m_numberCols        = 0;
m_currentCol        = -1;
m_lastCol           = -1;
m_leftCol           = 0;
m_lastLeftCol       = 0;
m_maxLeftCol        = 0;
m_defColWidth       = 75;
m_colInfo           = NULL;
m_rightCol          = 0;
m_dragCol           = -1;

/*
---------------
Row information
--------------- */
m_numberRows        = 0;
m_currentRow        = -1;
m_lastRow           = -1;
m_topRow            = 0;
m_lastTopRow        = 0;
m_maxTopRow         = 0;
m_rowHeights        = NULL;
m_defRowHeight      = 20;
m_uniformRowHeightFlag  = FALSE;
m_bottomRow         = 0;
m_dragRow           = -1;

/*
-------------------
Heading information
------------------- */
m_numberTopHdgRows = 1;
m_topHdgHeights    = new int[1];
m_topHdgHeights[0] = 20;

m_numberSideHdgCols = 1;
m_sideHdgWidths     = new int[1];
m_sideHdgWidths[0]  = 25;

/*
--------
Defaults
-------- */
m_gridDefaults = new CUGCell;
m_hdgDefaults  = new CUGCell;
m_hdgDefaults->SetBackColor(GetSysColor(COLOR_BTNFACE));
m_hdgDefaults->SetBorder(UG_BDR_RAISED);
m_hdgDefaults->SetAlignment(UG_ALIGNCENTER|UG_ALIGNVCENTER);

/*
-----
Sizes
----- */
m_topHdgHeight  = 20;       //pixels
m_sideHdgWidth  = 40;       //pixels
m_vScrollWidth  = GetSystemMetrics(SM_CXVSCROLL);       //pixels
m_hScrollHeight = GetSystemMetrics(SM_CYHSCROLL);       //pixels
m_tabWidth      = 0;        //pixels
m_showHScroll   = TRUE;     //TRUE or FALSE
m_showVScroll   = TRUE;     //TRUE or FALSE

/*
------------
Highlighting
------------ */
m_highlightRowFlag  = FALSE;    //TRUE or False
m_multiSelectFlag   = FALSE;    //TRUE or False
m_currentCellMode   = 1;        //mode(bits) 1:focus rect 2:highlight

/*
-------------
Other options
------------- */
m_mouseScrollFlag   = TRUE;     //TRUE or FALSE
m_threeDHeight      = 1;        // 1 - n
m_paintMode         = TRUE;     //if false then do not paint
m_enablePopupMenu   = FALSE;    //TRUE or FALSE

#ifdef UG_ENABLE_HINTS
    m_enableHints       = FALSE;    //TRUE or FALSE
#endif

#ifdef UG_ENABLE_SCROLLHINTS
    m_enableVScrollHints = FALSE;   //TRUE or FALSE
    m_enableHScrollHints = FALSE;   //TRUE or FALSE
#endif

m_userSizingMode     = 1;        //0 -off 1-normal 2-update on the fly
m_userBestSizeFlag   = TRUE;
m_enableJoins        = TRUE;
m_enableCellOverLap  = TRUE;
m_enableColSwapping  = FALSE;
m_enableExcelBorders = TRUE;


/*
----------
Scrollbars
---------- */
m_vScrollMode = 0;        // 0-normal 2- tracking 3-joystick
m_hScrollMode = 0;        // 0-normal 2- tracking

/*
--------
Balistic
-------- */
m_ballisticMode     = 0;        //0- off 1-increment 2-squared 3- cubed
m_ballisticDelay    = 200;      //slow scroll delay
m_ballisticKeyMode  = 0;        //0- off
m_ballisticKeyDelay = 0;        //slow scroll delay

/*
-------------
Double buffer
------------- */
m_doubleBufferMode  = FALSE;

/*
----------------------
Column and row locking
---------------------- */
m_numLockCols       = 0;
m_numLockRows       = 0;
m_lockColWidth      = 0;
m_lockRowHeight     = 0;


/*
-----------------------------
Zooming multiplication factor
----------------------------- */
m_zoomMultiplier    = 0.0;
m_zoomOn            = FALSE;

m_defFont            = NULL;
m_CUGMem             = new CUGMem;
m_defDataSource      = m_CUGMem;
m_defDataSourceIndex = 0;
m_moveType           = 0;  //keyboard - default
m_multiSelect        = new CUGMultiSelect;
m_arrowCursor        = LoadCursor(NULL,IDC_ARROW);
m_WEResizseCursor    = LoadCursor(NULL,IDC_SIZEWE);
m_NSResizseCursor    = LoadCursor(NULL,IDC_SIZENS);
m_margin             = 4;
m_CUGOverlay         = new CUGPtrList;

/*
--------------------------------------------------------
Default border pen - used when using excel style borders
-------------------------------------------------------- */
m_defBorderPen = CreatePen(PS_SOLID,1,RGB(220,220,220));

m_trackingWndMode = 0;  //normal mode 1-stay close
}

/***********************************************************************
*                             ~CUGGRIDINFO                             *
***********************************************************************/
CUGGridInfo::~CUGGridInfo()
{

if ( m_colInfo != NULL )
    {
    int i;
    for( i=0; i<m_numberCols; i++ )
        {
        if ( m_colInfo[i].colDefault != NULL )
            delete m_colInfo[i].colDefault;
        }
    delete[] m_colInfo;
    }

if ( m_rowHeights != NULL )
    delete[] m_rowHeights;

if ( m_topHdgHeights != NULL )
    delete[] m_topHdgHeights;

if ( m_sideHdgWidths != NULL )
    delete[] m_sideHdgWidths;

if ( m_gridDefaults != NULL )
    delete m_gridDefaults;

if ( m_hdgDefaults != NULL )
    delete m_hdgDefaults;

if ( m_multiSelect != NULL )
    delete m_multiSelect;

if ( m_CUGOverlay != NULL )
    delete m_CUGOverlay;

if ( m_CUGMem != NULL )
    delete m_CUGMem;

if ( m_defBorderPen != NULL )
    DeleteObject( m_defBorderPen );

}

