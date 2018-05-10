
//////////////////////////////////////////////////////////////////////
// PckControlCenterLog.cpp: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�,������PCK������ݽ�������������
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2015.5.21
//////////////////////////////////////////////////////////////////////

#include "PckControlCenter.h"
#include <commctrl.h>
#include "resource.h"
#include <assert.h>

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4244 )

#if 0
char *GetErrorMsg(CONST DWORD dwError)
{
	static char szMessage[1024] = "����ԭ��";
	//strcpy(szMessage, "����ԭ��");

	char *lpszMessage = szMessage + 10;
	// retrieve a message from the system message table
	switch(dwError) {
	case 2:
		strcpy(lpszMessage, "ϵͳ�Ҳ���ָ�����ļ���");
		break;
	case 3:
		strcpy(lpszMessage, "ϵͳ�Ҳ���ָ����·����");
		break;
	case 4:
		strcpy(lpszMessage, "ϵͳ�޷����ļ���");
		break;
	case 5:
		strcpy(lpszMessage, "�ܾ����ʡ�");
		break;
	case 32:
		strcpy(lpszMessage, "��һ����������ʹ�ô��ļ��������޷����ʡ�");
		break;
	default:
		if(!FormatMessageA(
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			lpszMessage,
			1014,
			NULL)) {
			strcpy(lpszMessage, "δ֪����");
		}
	}
	SetLastError(0);
	return szMessage;
}
#endif

WCHAR *GetErrorMsg(CONST DWORD dwError)
{
	static WCHAR szMessage[1024] = L"����ԭ��";

	size_t len = wcslen(L"����ԭ��");
	WCHAR *lpszMessage = szMessage + len;
	// retrieve a message from the system message table
	//switch(dwError) {
	//case 2:
	//	strcpy(lpszMessage, "ϵͳ�Ҳ���ָ�����ļ���");
	//	break;
	//case 3:
	//	strcpy(lpszMessage, "ϵͳ�Ҳ���ָ����·����");
	//	break;
	//case 4:
	//	strcpy(lpszMessage, "ϵͳ�޷����ļ���");
	//	break;
	//case 5:
	//	strcpy(lpszMessage, "�ܾ����ʡ�");
	//	break;
	//case 32:
	//	strcpy(lpszMessage, "��һ����������ʹ�ô��ļ��������޷����ʡ�");
	//	break;
	//default:
		if(!FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			lpszMessage,
			1024 - len,
			NULL)) {
			wcscpy(lpszMessage, L"δ֪����");
		}
	//}
	SetLastError(0);
	return szMessage;
}

inline char *UCS2toA(const WCHAR *src, int max_len = -1)
{
	static char dst[8192];
	::WideCharToMultiByte(CP_ACP, 0, src, max_len, dst, 8192, "_", 0);
	return dst;
}

inline WCHAR *AtoUCS2(const char *src, int max_len = -1)
{
	static WCHAR dst[8192];
	::MultiByteToWideChar(CP_ACP, 0, src, max_len, dst, 8192);
	return dst;
}

void CPckControlCenter::PreInsertLogToList(const int _loglevel, const char *_logtext)
{
	PreInsertLogToList(_loglevel, AtoUCS2(_logtext));
}

void CPckControlCenter::PreInsertLogToList(const int _loglevel, const WCHAR *_logtext)
{

	SYSTEMTIME systime;

	if(LOG_IMAGE_NOTICE == _loglevel) {
		::SendDlgItemMessageW(m_hWndMain, IDC_STATUS, SB_SETTEXTW, 4, (LPARAM)_logtext);
		::SendDlgItemMessageW(m_hWndMain, IDC_STATUS, SB_SETTIPTEXTW, 4, (LPARAM)_logtext);
	}

	wchar_t szPrintf[8192];
	//GetLocalTime(&systime);

	size_t nTextLen = wcslen(_logtext);
	//swprintf_s(szPrintf, L"%02d:%02d:%02d ", systime.wHour, systime.wMinute, systime.wSecond);

	//size_t nLenOfPrefix = wcslen(szPrintf);
	//wchar_t *lpPointToPaestString = szPrintf + nLenOfPrefix;

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

void CPckControlCenter::SetLogListWnd(HWND _hWndList)
{
	m_hWndLogListWnd = _hWndList;
}

#define LEN_OF_PREFIX	9
#if 0
void CPckControlCenter::_InsertLogIntoList(const int _loglevel, const char *_logtext)
{
	//PreInsertLogToList(_loglevel, AtoUCS2(_logtext));
	_InsertLogIntoList(_loglevel, AtoUCS2(_logtext));
	//LVITEMA	item;
	//SYSTEMTIME systime;

	//if(LOG_IMAGE_NOTICE == _loglevel) {
	//	::SendDlgItemMessageA(m_hWndMain, IDC_STATUS, SB_SETTEXTA, 4, (LPARAM)_logtext);
	//	::SendDlgItemMessageA(m_hWndMain, IDC_STATUS, SB_SETTIPTEXTA, 4, (LPARAM)_logtext);
	//}

	//char szPrintf[4096];

	//GetLocalTime(&systime);

	//sprintf_s(szPrintf, "%02d:%02d:%02d %s", systime.wHour, systime.wMinute, systime.wSecond, _logtext);


	//ZeroMemory(&item, sizeof(LVITEMA));

	//item.iItem = INT_MAX;			//��0��ʼ
	//item.iImage = _loglevel;
	////item.iSubItem = 0;
	//item.mask = LVIF_TEXT | LVIF_IMAGE;
	//item.pszText = szPrintf;

	//if(-1 != (m_LogListCount = ::SendMessageA(m_hWndLogListWnd, LVM_INSERTITEMA, 0, (LPARAM)&item))) {
	//	ListView_EnsureVisible(m_hWndLogListWnd, m_LogListCount, 0);
	//	//++m_LogListCount;
	//}

}
#endif

void CPckControlCenter::_InsertLogIntoList(const int _loglevel, const wchar_t *_logtext)
{

	LVITEMW	item;
	//

	//if(LOG_IMAGE_NOTICE == _loglevel) {
	//	::SendDlgItemMessageW(m_hWndMain, IDC_STATUS, SB_SETTEXTW, 4, (LPARAM)_logtext);
	//	::SendDlgItemMessageW(m_hWndMain, IDC_STATUS, SB_SETTIPTEXTW, 4, (LPARAM)_logtext);
	//}

	//wchar_t szPrintf[4096];

	//

	//swprintf_s(szPrintf, L"%02d:%02d:%02d %s", systime.wHour, systime.wMinute, systime.wSecond, _logtext);


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

void CPckControlCenter::PrintLogI(const char *_logtext)
{
	PreInsertLogToList(LOG_IMAGE_INFO, _logtext);
}

void CPckControlCenter::PrintLogW(const char *_logtext)
{
	PreInsertLogToList(LOG_IMAGE_WARNING, _logtext);
}

void CPckControlCenter::PrintLogE(const char *_logtext)
{
	PreInsertLogToList(LOG_IMAGE_ERROR, _logtext);
	ShowWindow(GetParent(m_hWndLogListWnd), SW_SHOW);
}

void CPckControlCenter::PrintLogD(const char *_logtext)
{
	PreInsertLogToList(LOG_IMAGE_DEBUG, _logtext);
}

void CPckControlCenter::PrintLogN(const char *_logtext)
{
	PreInsertLogToList(LOG_IMAGE_NOTICE, _logtext);
}


void CPckControlCenter::PrintLogI(const wchar_t *_logtext)
{
	PreInsertLogToList(LOG_IMAGE_INFO, _logtext);
}

void CPckControlCenter::PrintLogW(const wchar_t *_logtext)
{
	PreInsertLogToList(LOG_IMAGE_WARNING, _logtext);
}

void CPckControlCenter::PrintLogE(const wchar_t *_logtext)
{
	PreInsertLogToList(LOG_IMAGE_ERROR, _logtext);
	ShowWindow(GetParent(m_hWndLogListWnd), SW_SHOW);
}

void CPckControlCenter::PrintLogD(const wchar_t *_logtext)
{
	PreInsertLogToList(LOG_IMAGE_DEBUG, _logtext);
}

void CPckControlCenter::PrintLogN(const wchar_t *_logtext)
{
	PreInsertLogToList(LOG_IMAGE_NOTICE, _logtext);
}

void CPckControlCenter::PrintLogE(const char *_maintext, const char *_file, const char *_func, const long _line)
{
	m_dwLastError = GetLastError();
	PrintLogE(AtoUCS2(_maintext), _file, _func, _line);
}

void CPckControlCenter::PrintLogE(const wchar_t *_maintext, const char *_file, const char *_func, const long _line)
{
	wchar_t szPrintf[8192];
	swprintf_s(szPrintf, L"%s (%S ���������� %S ����:%d)", _maintext, _file, _func, _line);
	PrintLogE(szPrintf);
	if(0 == m_dwLastError) {
		m_dwLastError = GetLastError();
		if(0 != m_dwLastError)
			PrintLogE(GetErrorMsg(GetLastError()));
	} else {
		PrintLogE(GetErrorMsg(m_dwLastError));
		m_dwLastError = 0;
	}
	assert(FALSE);

}

void CPckControlCenter::PrintLogE(const char *_fmt, const char *_maintext, const char *_file, const char *_func, const long _line)
{
	char szPrintf[8192];
	sprintf_s(szPrintf, _fmt, _maintext);
	PrintLogE(szPrintf, _file, _func, _line);

}
void CPckControlCenter::PrintLogE(const char *_fmt, const wchar_t *_maintext, const char *_file, const char *_func, const long _line)
{
	m_dwLastError = GetLastError();
	PrintLogE(_fmt, UCS2toA(_maintext), _file, _func, _line);
}


void CPckControlCenter::PrintLog(const char chLevel, const char *_maintext)
{
	switch(chLevel) {
	case LOG_FLAG_ERROR:
		PrintLogE(_maintext);
		break;
	case LOG_FLAG_WARNING:
		PrintLogW(_maintext);
		break;
	case LOG_FLAG_INFO:
		PrintLogI(_maintext);
		break;
	case LOG_FLAG_NOTICE:
		PrintLogN(_maintext);
		break;
#ifdef _DEBUG
	case LOG_FLAG_DEBUG:
		PrintLogD(_maintext);
		break;
#endif
	default:
		PrintLogI(_maintext);
	}
	//PrintLogE(szPrintf);
}

void CPckControlCenter::PrintLog(const char chLevel, const wchar_t *_maintext)
{
	PrintLog(chLevel, UCS2toA(_maintext));
}

void CPckControlCenter::PrintLog(const char chLevel, const char *_fmt, const char *_maintext)
{
	char szPrintf[8192];
	sprintf_s(szPrintf, _fmt, _maintext);
	PrintLog(chLevel, szPrintf);

}
void CPckControlCenter::PrintLog(const char chLevel, const char *_fmt, const wchar_t *_maintext)
{
	PrintLog(chLevel, _fmt, UCS2toA(_maintext));
}