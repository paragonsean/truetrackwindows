#include <windows.h>

/***********************************************************************
*  Ultimate Grid 97                                                    *
*  Copyright 1994 - 1997 Dundas Software Ltd.                          *
*                                                                      *
*  CUGPrtList                                                          *
*                                                                      *
*  This is a specialized pointer list that also has extra fields       *
*  for storing information on each pointer, such as a ID number        *
*  a GUID and even a string name.                                      *
*                                                                      *
*  This class will not delete the pointer attached to it when          *
*  the item is being deleted and/or destroyed. Therefore it is         *
*  the responsibilty of the calling program to destroy its own         *
*  pointers.                                                           *
*                                                                      *
***********************************************************************/

#include <ctype.h>
#include "UGCtrl.h"

/***********************************************************************
*                             CUGPtrList                               *
***********************************************************************/
CUGPtrList::CUGPtrList()
{
m_elementsUsed = 0;
m_maxElements  = 0;
m_arrayPtr     = NULL;
}

/***********************************************************************
*                            ~CUGPtrList                               *
***********************************************************************/
CUGPtrList::~CUGPtrList()
{
EmptyList();
}

/***********************************************************************
*                             AddPointer                               *
*             returns the 0 based index, -1 on error                   *
***********************************************************************/
int CUGPtrList::AddPointer( void * ptr, long param, UGID * id )
{

/*
----------------------------------------------
Check to see if more elements need to be added
---------------------------------------------- */
if ( m_elementsUsed >= m_maxElements )
    {
    if ( AddMoreElements() == UG_ERROR )
        return -1;
    }

for( int loop = 0;loop <m_maxElements; loop++ )
    {

    if ( m_arrayPtr[loop].isUsed == FALSE )
        {
        m_arrayPtr[loop].isUsed  = TRUE;
        m_arrayPtr[loop].pointer = ptr;
        m_arrayPtr[loop].param   = param;
        if ( id != NULL )
            memcpy( &m_arrayPtr[loop].id, id, sizeof(UGID) );

        m_elementsUsed++;

        return loop;
        }
    }

return -1;
}

/***********************************************************************
*                             UpdateParam                              *
***********************************************************************/
int CUGPtrList::UpdateParam( int index, long param )
{

if ( index <0 || index >= m_maxElements )
    return UG_ERROR;

if ( m_arrayPtr[index].isUsed == FALSE )
    return 2;

m_arrayPtr[index].param = param;

return UG_SUCCESS;
}

/***********************************************************************
*                              GetParam                                *
***********************************************************************/
long CUGPtrList::GetParam( int index )
{

if ( index <0 || index >= m_maxElements )
    return NULL;

return m_arrayPtr[index].param;
}

/***********************************************************************
*                            GetPointer                                *
***********************************************************************/
LPVOID CUGPtrList::GetPointer( int index )
{

if ( index <0 || index >= m_maxElements )
    return NULL;

if ( m_arrayPtr[index].isUsed == FALSE )
    return NULL;

return m_arrayPtr[index].pointer;
}

/***********************************************************************
*                           GetPointerIndex                            *
***********************************************************************/
int CUGPtrList::GetPointerIndex( void * ptr )
{

for ( int loop=0; loop<m_maxElements; loop++ )
    {

    if ( m_arrayPtr[loop].isUsed == FALSE )
        continue;

    if ( m_arrayPtr[loop].pointer == ptr )
        return loop;
    }

return -1;
}

/***********************************************************************
*                              DeletePointer                           *
***********************************************************************/
int CUGPtrList::DeletePointer( int index )
{

if ( index <0 || index >= m_maxElements )
    return UG_ERROR;

if ( m_arrayPtr[index].isUsed != FALSE )
    {
    m_arrayPtr[index].isUsed = FALSE;
    m_arrayPtr[index].pointer = NULL;
    m_elementsUsed--;
    }

return UG_SUCCESS;
}

/***********************************************************************
*                               EmptyList                              *
***********************************************************************/
int CUGPtrList::EmptyList()
{

if ( m_arrayPtr != NULL )
    {
    delete[] m_arrayPtr;
    m_arrayPtr = NULL;
    }

m_elementsUsed = 0;
m_maxElements = 0;

return UG_SUCCESS;
}

/***********************************************************************
*                               GetCount                               *
***********************************************************************/
int CUGPtrList::GetCount()
{
return m_elementsUsed;
}

/***********************************************************************
*                             GetMaxCount                              *
***********************************************************************/
int CUGPtrList::GetMaxCount()
{
return m_maxElements;
}

/***********************************************************************
*                             AddMoreElements                          *
***********************************************************************/
int CUGPtrList::AddMoreElements()
{
UGPtrList * temp;
int n;

n = m_maxElements + 20;

temp = new UGPtrList[n];


if ( temp == NULL )
    return UG_ERROR;

int i;

for ( i=0; i<m_maxElements; i++ )
    {
    temp[i].isUsed  = m_arrayPtr[i].isUsed;
    temp[i].pointer = m_arrayPtr[i].pointer;
    temp[i].param   = m_arrayPtr[i].param;
    memcpy( &temp[i].id, &m_arrayPtr[i].id, sizeof(UGID) );
    }

for( i=m_maxElements; i<n; i++)
    {
    temp[i].isUsed  = FALSE;
    temp[i].pointer = NULL;
    temp[i].param   = 0;
    }

if ( m_arrayPtr )
    delete[] m_arrayPtr;

m_arrayPtr    = temp;
m_maxElements = n;

return UG_SUCCESS;
}

