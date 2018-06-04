//////////////////////////////////////////////////////////////////////
// ShowLogOnDlgListView.cpp: ����־��ʾ��listView��
// 
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2018.6.4
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include "ShowLogOnDlgListView.h"
#include "PckClassLog.h"
#include "resource.h"
#include <commctrl.h>
#include <stdio.h>

static HWND					m_hWndMain;
static HWND					m_hWndLogListWnd;
static int					m_LogListCount = 0;
static DWORD				m_dwLastError;

void SetLogListWnd(HWND _hWndList)
{
	m_hWndLogListWnd = _hWndList;
}

void SetLogMainWnd(HWND _hWnd)
{
	m_hWndMain = _hWnd;
}

void PreInsertLogToList(const int _loglevel, const WCHAR *_logtext)
{

	if(LOG_IMAGE_NOTICE == _loglevel) {
		::SendDlgItemMessageW(m_hWndMain, IDC_STATUS, SB_SETTEXTW, 4, (LPARAM)_logtext);
		::SendDlgItemMessageW(m_hWndMain, IDC_STATUS, SB_SETTIPTEXTW, 4, (LPARAM)_logtext);
	} else if(LOG_IMAGE_ERROR == _loglevel) {
		ShowWindow(GetParent(m_hWndLogListWnd), SW_SHOW);
	}

	wchar_t szPrintf[8192];
	size_t nTextLen = wcslen(_logtext);

	//����\n ������ڣ�\n������ݻ��У����µ��У���ʾ
	const wchar_t *lpString2Show = _logtext, *lpString2Search = _logtext, *lpStringTail = _logtext + nTextLen;
	int loglevel = _loglevel;
	while(0 != *lpString2Search) {

		if('\n' == *lpString2Search) {
			memcpy(szPrintf, lpString2Show, (lpString2Search - lpString2Show) * 2);
			*(szPrintf + (lpString2Search - lpString2Show)) = 0;
			_InsertLogIntoList(loglevel, szPrintf);
			lpString2Show = lpString2Search + 1;
			loglevel = LOG_IMAGE_EMPTY;
		}

		lpString2Search++;
	}
	if(0 != *lpString2Show) {
		memcpy(szPrintf, lpString2Show, (lpString2Search - lpString2Show) * 2);
		*(szPrintf + (lpString2Search - lpString2Show)) = 0;
		_InsertLogIntoList(loglevel, szPrintf);
	}
}

#define LEN_OF_PREFIX	9

void _InsertLogIntoList(const int _loglevel, const wchar_t *_logtext)
{

	LVITEMW	item;
	wchar_t szPrintf[8192 + 9];
	//size_t nLenOfPrefix = wcslen(szPrintf);
	wchar_t *lpPointToPaestString = szPrintf + LEN_OF_PREFIX;
	//123456789
	//09:06:17 WinPCK v1.22.9  is started.
	if(LOG_IMAGE_EMPTY == _loglevel) {
		wcsnset(szPrintf, ' ', LEN_OF_PREFIX);
	} else {
		SYSTEMTIME systime;
		GetLocalTime(&systime);
		swprintf_s(szPrintf, L"%02d:%02d:%02d ", systime.wHour, systime.wMinute, systime.wSecond);
	}

	wcscpy_s(lpPointToPaestString, 4096, _logtext);

	ZeroMemory(&item, sizeof(LVITEMW));

	item.iItem = INT_MAX;			//��0��ʼ
	item.iImage = _loglevel;
	//item.iSubItem = 0;
	item.mask = LVIF_TEXT | LVIF_IMAGE;
	item.pszText = szPrintf;

	if(-1 != (m_LogListCount = ::SendMessageW(m_hWndLogListWnd, LVM_INSERTITEMW, 0, (LPARAM)&item))) {
		ListView_EnsureVisible(m_hWndLogListWnd, m_LogListCount, 0);
		//++m_LogListCount;
	}
}
#undef LEN_OF_PREFIX