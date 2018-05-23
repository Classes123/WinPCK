
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
#include "CharsCodeConv.h"

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
	SetLastError(0);
	return szMessage;
}

void CPckControlCenter::PreInsertLogToList(const int _loglevel, const char *_logtext)
{
	CAnsi2Ucs cA2U;
	PreInsertLogToList(_loglevel, cA2U.GetString(_logtext));
}

void CPckControlCenter::PreInsertLogToList(const int _loglevel, const WCHAR *_logtext)
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

#define define_one_PrintLog(_loglvchar, _loglevel)	\
void CPckControlCenter::PrintLog##_loglvchar(const char *_text, ...)\
{\
	va_list	ap;\
	va_start(ap, _text);\
	PrintLog##_loglvchar(_text, ap);\
	va_end(ap);\
}\
void CPckControlCenter::PrintLog##_loglvchar(const wchar_t *_text, ...)\
{\
	va_list	ap;\
	va_start(ap, _text);\
	PrintLog##_loglvchar(_text, ap);\
	va_end(ap);\
}\
void CPckControlCenter::PrintLog##_loglvchar(const char *_text, va_list ap)\
{\
	char _maintext[8192];\
	_vsnprintf(_maintext, sizeof(_maintext), _text, ap);\
	PreInsertLogToList(_loglevel, _maintext);\
}\
void CPckControlCenter::PrintLog##_loglvchar(const wchar_t *_text, va_list ap)\
{\
	wchar_t _maintext[8192];\
	_vsnwprintf(_maintext, sizeof(_maintext), _text, ap);\
	PreInsertLogToList(_loglevel, _maintext);\
}

define_one_PrintLog(I, LOG_IMAGE_INFO)
define_one_PrintLog(W, LOG_IMAGE_WARNING)
define_one_PrintLog(E, LOG_IMAGE_ERROR)
define_one_PrintLog(D, LOG_IMAGE_DEBUG)
define_one_PrintLog(N, LOG_IMAGE_NOTICE)

void CPckControlCenter::PrintLogEL(const char *_maintext, const char *_file, const char *_func, const long _line)
{
	CAnsi2Ucs cA2U;
	m_dwLastError = GetLastError();
	PrintLogEL(cA2U.GetString(_maintext), _file, _func, _line);
}

void CPckControlCenter::PrintLogEL(const wchar_t *_maintext, const char *_file, const char *_func, const long _line)
{

	PrintLog(LOG_IMAGE_ERROR, L"%s (%S ���������� %S ����:%d)", _maintext, _file, _func, _line); 

	if(0 == m_dwLastError) {
		m_dwLastError = GetLastError();
		if(0 != m_dwLastError)
			PrintLog(LOG_IMAGE_ERROR, GetErrorMsg(GetLastError()));
	} else {
		PrintLog(LOG_IMAGE_ERROR, GetErrorMsg(m_dwLastError));
		m_dwLastError = 0;
	}
	assert(FALSE);

}

void CPckControlCenter::PrintLogEL(const char *_fmt, const char *_maintext, const char *_file, const char *_func, const long _line)
{
	char szPrintf[8192];
	sprintf_s(szPrintf, _fmt, _maintext);
	PrintLogEL(szPrintf, _file, _func, _line);

}
void CPckControlCenter::PrintLogEL(const char *_fmt, const wchar_t *_maintext, const char *_file, const char *_func, const long _line)
{
	CUcs2Ansi cU2A;
	PrintLogEL(_fmt, cU2A.GetString(_maintext), _file, _func, _line);
}


void CPckControlCenter::PrintLog(const char chLevel, const char *_fmt, va_list ap)
{
	char _maintext[8192];
	_vsnprintf(_maintext, sizeof(_maintext), _fmt, ap);

	PreInsertLogToList(chLevel, _maintext);
}

void CPckControlCenter::PrintLog(const char chLevel, const wchar_t *_fmt, va_list ap)
{
	//CUcs2Ansi cU2A;
	wchar_t _maintext[8192];
	_vsnwprintf(_maintext, sizeof(_maintext), _fmt, ap);

	PreInsertLogToList(chLevel, _maintext);
}

void CPckControlCenter::PrintLog(const char chLevel, const char *_fmt, ...)
{
	va_list	ap;
	va_start(ap, _fmt);
	PrintLog(chLevel, _fmt, ap);
	va_end(ap);
}

void CPckControlCenter::PrintLog(const char chLevel, const wchar_t *_fmt, ...)
{
	va_list	ap;
	va_start(ap, _fmt);
	PrintLog(chLevel, _fmt, ap);
	va_end(ap);
}

#undef define_one_PrintLog