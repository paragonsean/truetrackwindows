#include <windows.h>

/***********************************************
	Ultimate Grid 97
	Copyright 1994 - 1997 Dundas Software Ltd.

	Class 
		CUGVScroll
	Purpose
		This class controls the grid's verticle
		scrollbar and is responsible for updating
		it and making sure that it is using the 
		specified scroll mode
************************************************/

#include "UGCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CUGVScroll

CUGVScroll::CUGVScroll()
{
	m_lastMaxTopRow = -2;
	m_lastScrollMode = -2;
	m_lastNumLockRow = -2;
	m_trackRowPos = 0;
	m_vScrollWnd = NULL;
	//m_tooltipWnd = NULL;
}

CUGVScroll::~CUGVScroll()
{
}

/////////////////////////////////////////////////////////////////////////////
// CUGVScroll message handlers

/************************************************
*************************************************/
void CUGVScroll::Update(){
	Moved();
}

/************************************************
*************************************************/
void CUGVScroll::Moved(){

	//set the scroll range
	if(m_lastMaxTopRow != m_GI->m_maxTopRow || 
		m_lastScrollMode != m_GI->m_vScrollMode ||
		m_lastNumLockRow != m_GI->m_numLockRows){

		//set the last value vars
		m_lastMaxTopRow = m_GI->m_maxTopRow;
		m_lastScrollMode = m_GI->m_vScrollMode;
		m_lastNumLockRow = m_GI->m_numLockCols;

		//set up the scrollbar if the number of rows is less than 1000
		if(m_GI->m_maxTopRow <=1000){
			if(UG_SCROLLJOYSTICK == m_GI->m_vScrollMode){
				SCROLLINFO ScrollInfo;
				ScrollInfo.cbSize =sizeof(SCROLLINFO);
				ScrollInfo.fMask = SIF_PAGE | SIF_RANGE |SIF_POS;
				ScrollInfo.nPage = 0;
				ScrollInfo.nMin = 0;
				ScrollInfo.nMax = 2;
				ScrollInfo.nPos = 1;
				SetScrollInfo(m_vScrollWnd, SB_CTL, &ScrollInfo,FALSE);
			}
			else{
				SCROLLINFO ScrollInfo;
				ScrollInfo.cbSize =sizeof(SCROLLINFO);
				ScrollInfo.fMask = SIF_PAGE | SIF_RANGE;
				ScrollInfo.nPage = m_GI->m_gridHeight / m_GI->m_defRowHeight;
				ScrollInfo.nMin = m_GI->m_numLockRows;
				ScrollInfo.nMax = m_GI->m_maxTopRow + ScrollInfo.nPage -1;
				SetScrollInfo(m_vScrollWnd, SB_CTL, &ScrollInfo,FALSE);
			}
			m_multiRange = 1;
			m_multiPos	= 1;			
		}

		//set up the scrollbar if the number of rows is greater than 1000
		else{
			m_multiRange = (double)1000 / (double)m_GI->m_maxTopRow;
			m_multiPos   = (double)m_GI->m_maxTopRow / (double)1000;
			if(UG_SCROLLJOYSTICK==m_GI->m_vScrollMode){
				SetScrollRange(m_vScrollWnd, SB_CTL, 0,2,FALSE);
				m_multiRange = 1;
				m_multiPos	= 1;			
			}
			else{
				SCROLLINFO ScrollInfo;
				ScrollInfo.cbSize =sizeof(SCROLLINFO);
				ScrollInfo.fMask = SIF_PAGE | SIF_RANGE;
				ScrollInfo.nPage = 1;
				ScrollInfo.nMin = m_GI->m_numLockRows;
				ScrollInfo.nMax = 1000;
				SetScrollInfo(m_vScrollWnd, SB_CTL, &ScrollInfo,FALSE);
			}
		}
	}
	
	//set the scroll pos
	if(m_GI->m_lastTopRow != m_GI->m_topRow){
		if(UG_SCROLLJOYSTICK == m_GI->m_vScrollMode)
			SetScrollPos(m_vScrollWnd, SB_CTL, 1,TRUE);
		else
			SetScrollPos(m_vScrollWnd, SB_CTL, (int)(m_GI->m_topRow * m_multiRange), TRUE);
	}
	InvalidateRect(m_vScrollWnd, NULL, TRUE);
	UpdateWindow(m_vScrollWnd);
}

/************************************************
*************************************************/
void CUGVScroll::VScroll(UINT nSBCode, UINT nPos) 
{

	if(GetFocus() != m_ctrl->m_CUGGrid->m_gridWnd)
		SetFocus(m_ctrl->m_CUGGrid->m_gridWnd);
	
	m_ctrl->m_GI->m_moveType = 4;

	switch(nSBCode){
		case SB_LINEDOWN:{
			m_ctrl->MoveTopRow(UG_LINEDOWN);
			break;
		}
		case SB_LINEUP:{
			m_ctrl->MoveTopRow(UG_LINEUP);
			break;
		}
		case SB_PAGEUP:{
			m_ctrl->MoveTopRow(UG_PAGEUP);
			break;
		}
		case SB_PAGEDOWN:{
			m_ctrl->MoveTopRow(UG_PAGEDOWN);
			break;
		}
		case SB_TOP:{
			m_ctrl->MoveTopRow(UG_TOP);
			break;
		}
		case SB_BOTTOM:{
			m_ctrl->MoveTopRow(UG_BOTTOM);
			break;
		}
		case SB_THUMBTRACK:{
			if(m_GI->m_vScrollMode==UG_SCROLLTRACKING){	//tracking
				m_ctrl->SetTopRow((long)((double)nPos * m_multiPos) + m_GI->m_numLockRows);
			}
			m_trackRowPos = (long)((double)nPos * m_multiPos) + m_GI->m_numLockRows;
			//scroll hint window
#ifdef UG_ENABLE_SCROLLHINTS
			if(m_GI->m_enableVScrollHints){

				TCHAR string[80];
				RECT rect;
				GetWindowRect(m_vScrollWnd, &rect);
				rect.top = HIWORD(GetMessagePos());
				/*POINT point;
				point.x = rect.left;
				point.y = rect.top;
				ScreenToClient(m_ctrl->m_ctrlWnd, &point);
				rect.left = point.x;
				rect.top = point.y;
				point.x = rect.right;
				point.y = rect.bottom;
				ScreenToClient(m_ctrl->m_ctrlWnd, &point);
				rect.right = point.x;
				rect.bottom = point.y;*/
				m_ctrl->m_CUGHint->SetWindowAlign(UG_ALIGNRIGHT|UG_ALIGNVCENTER);
				m_ctrl->m_CUGHint->SetTextAlign(UG_ALIGNCENTER);

				m_ctrl->OnVScrollHint(m_trackRowPos,string);
				m_ctrl->m_CUGHint->SetText(string,FALSE);
				//TD - set text before move window...
				m_ctrl->m_CUGHint->MoveHintWindow(rect.left-1,rect.top,40);
				m_ctrl->m_CUGHint->Show();
			}
#endif
			break;
		}
		case SB_ENDSCROLL:{
			break;
		}
		case SB_THUMBPOSITION:{

			#ifdef UG_ENABLE_SCROLLHINTS
			if(m_GI->m_enableVScrollHints){
				m_ctrl->m_CUGHint->Hide();				
			}
			#endif

			m_ctrl->SetTopRow((long)((double)nPos * m_multiPos) + m_GI->m_numLockRows);
			m_trackRowPos = (long)((double)nPos * m_multiPos) + m_GI->m_numLockRows;
			break;
		}
	}
}


void CUGVScroll::OnRButtonDown(UINT nFlags, POINT point) 
{
	if(m_GI->m_enablePopupMenu){
		ClientToScreen(m_vScrollWnd, &point);
		m_ctrl->StartMenu(0,0,&point,UG_VSCROLL);
	}
}

/****************************************************
*****************************************************/
FARPROC lpfnOldvScroll;
BOOL CUGVScroll::CreateVScrollWindow(DWORD dwStyle, RECT rect, HWND hWndParent, UINT nID){

	if(hWndParent == NULL)
		return UG_ERROR;

	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWndParent,GWL_HINSTANCE);

	m_vScrollWnd = CreateWindow(_T("SCROLLBAR"), _T(""), dwStyle,
		rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,
		hWndParent, (HMENU)nID, hInstance, NULL);

	if (m_vScrollWnd == NULL) {
#ifdef UG_ENABLE_SCROLLHINTS
		m_GI->m_enableVScrollHints = 0;
#endif
		return UG_ERROR;
	}

    lpfnOldvScroll = (FARPROC)GetWindowLong(m_vScrollWnd, GWL_WNDPROC);
	FARPROC lpfnNewvScroll = MakeProcInstance((FARPROC)VScrollWndProc, hInstance);
	SetWindowLong(m_vScrollWnd, GWL_WNDPROC, (LONG)lpfnNewvScroll);
	
	SendMessage(m_vScrollWnd,UG_THIS_PTR_VSCROLL,0,(LPARAM)this);

	return UG_SUCCESS;
}

/***************************************************
****************************************************/
long CALLBACK CUGVScroll::VScrollWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam){

	switch(message){
		case WM_NCCREATE:
			SetWindowLong(hWnd, GWL_USERDATA, (LPARAM)NULL);
			return 0;
		case UG_THIS_PTR_VSCROLL:
			//store the pointer to the calling class
			SetWindowLong(hWnd, GWL_USERDATA, lParam);
			return 0;
	}

	CUGVScroll * _this = (CUGVScroll *)GetWindowLong(hWnd, GWL_USERDATA);
	if (_this == NULL)
		return (CallWindowProc(lpfnOldvScroll, hWnd, message, wParam, lParam));

	//POINT point = {LOWORD(lParam), HIWORD(lParam)};
	POINT point = {(short)LOWORD(lParam), (short)HIWORD(lParam)};

	switch(message){
		case WM_RBUTTONDOWN:
			_this->OnRButtonDown((UINT)wParam, point);
			return 0;
/*
#ifdef UG_ENABLE_SCROLLHINTS
		case WM_MOUSEMOVE:
			_this->ToolTipMessage(hWnd, message, wParam, lParam);
			break;
		case WM_NOTIFY:
			_this->OnToolTip(hWnd, wParam, lParam);
			return 0;	  
#endif
*/
		default:
			break;
	}
	//return (DefWindowProc(hWnd, message, wParam, lParam));
	return (CallWindowProc(lpfnOldvScroll, hWnd, message, wParam, lParam));
}

#ifdef UG_ENABLE_SCROLLHINTS
/************************************************
*************************************************/
/*BOOL CUGVScroll::CreateToolTipWindow(HWND hWndParent){

	/*if (!m_GI->m_enableVScrollHints)
		return 1;*/

/*	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hWndParent,GWL_HINSTANCE);

	InitCommonControls();

	RECT rect;
	GetClientRect(m_vScrollWnd, &rect);

	m_tooltipWnd = CreateWindowEx(0, TOOLTIPS_CLASS, NULL, WS_POPUP | TTS_ALWAYSTIP,
		rect.left, rect.top, rect.right, rect.bottom,
		hWndParent, NULL, hInstance, NULL);

	if (m_tooltipWnd != NULL) {
		TOOLINFO ti;
		ZeroMemory(&ti, sizeof(ti));
		ti.cbSize = sizeof(ti);
		ti.uFlags = 0;
		ti.hwnd = m_vScrollWnd;
		ti.lpszText = LPSTR_TEXTCALLBACK;
		ti.rect = rect;
		SendMessage(m_tooltipWnd, TTM_ADDTOOL, 0, (LPARAM)&ti);
	}
	else {
		m_GI->m_enableVScrollHints = FALSE;
		return UG_ERROR;
	}
	return UG_SUCCESS;
}

/************************************************
*************************************************/
/*void CUGVScroll::ToolTipMessage(HWND hWnd,UINT message, WPARAM wParam,LPARAM lParam) {

	static short swit = 0;

	if (!m_GI->m_enableVScrollHints)
		return;
	
	if (m_GI->m_enableVScrollHints && m_tooltipWnd) {
		
		RECT rect;
		GetClientRect(m_vScrollWnd, &rect);

		TOOLINFO ti;
		ZeroMemory(&ti, sizeof(ti));
		ti.cbSize = sizeof(ti);
		ti.uFlags = 0;
		ti.hwnd = m_vScrollWnd;
		ti.lpszText = LPSTR_TEXTCALLBACK;
		ti.rect = rect;
		if (swit)
			SendMessage(m_tooltipWnd, TTM_DELTOOL, 0, (LPARAM)&ti);
		else
			swit = 1;
		SendMessage(m_tooltipWnd, TTM_ADDTOOL, 0, (LPARAM)&ti);
	}

	MSG   msg;
	
	//we need to fill out a message structure and pass it to the tooltip 
	//with the TTM_RELAYEVENT message
	msg.hwnd = hWnd;
	msg.message = message;
	msg.wParam = wParam;
	msg.lParam = lParam;
	GetCursorPos(&msg.pt);
	msg.time = GetMessageTime();
	
	SendMessage(m_tooltipWnd, TTM_RELAYEVENT, 0, (LPARAM)&msg);
}

/************************************************
*************************************************/
/*int CUGVScroll::OnToolTip(HWND hWnd, WPARAM wParam, LPARAM lParam) {

	if (!m_GI->m_enableVScrollHints)
		return 1;

	LPTOOLTIPTEXT pTTT = (LPTOOLTIPTEXT)lParam;

	if (pTTT->hdr.code == TTN_NEEDTEXT) {
		POINT point;
		TCHAR string[80];
		RECT rect;

		GetCursorPos(&point);
		ScreenToClient(m_vScrollWnd, &point);
		GetClientRect(m_vScrollWnd, &rect);
		if(PtInRect(&rect, point)) {
			m_ctrl->OnVScrollHint(m_trackRowPos,string);
			lstrcpy(pTTT->szText,string);
		}
	}
	return UG_SUCCESS;
}
*/
#endif

