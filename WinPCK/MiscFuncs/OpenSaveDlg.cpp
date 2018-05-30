//////////////////////////////////////////////////////////////////////
// OpenSaveDlg.cpp: WinPCK ������������
// ��ʾ�ļ��򿪡�����Ի���
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2018.5.29
//////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include "OpenSaveDlg.h"
#include <tchar.h>
#include <shlobj.h>

#define DEFAULT_FILTER	"�����ļ�\0*.*\0\0"
#define __L(quote) L#quote

/******************************************************
ֻ��һ���ļ�
******************************************************/

BOOL OpenSingleFile(HWND hWnd, TCHAR * lpszFileName, LPCTSTR lpstrFilter, DWORD nFilterIndex)
{
	OPENFILENAME ofn;
	TCHAR szStrPrintf[MAX_PATH];
	*szStrPrintf = 0;

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	if('\\' == *(lpszFileName + lstrlen(lpszFileName) - 1)) {
		ofn.lpstrInitialDir = lpszFileName;
		ofn.lpstrFile = szStrPrintf;
	} else {
		ofn.lpstrFile = lpszFileName;
	}

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = (NULL == lpstrFilter) ? TEXT(DEFAULT_FILTER) : lpstrFilter;
	ofn.nFilterIndex = nFilterIndex;
	//ofn.lpstrFile         = lpszFileName;
	//ofn.lpstrInitialDir   = lpszFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ENABLESIZING/* | OFN_ALLOWMULTISELECT*/;

	if(!GetOpenFileName(&ofn)) {
		return FALSE;
	}

	if(NULL != ofn.lpstrInitialDir)
		_tcscpy_s(lpszFileName, MAX_PATH, szStrPrintf);

	return TRUE;

}


DWORD SaveFile(HWND hWnd, char * lpszFileName, LPCSTR lpszDefaultExt , LPCSTR lpstrFilter, DWORD nFilterIndex)
{
	OPENFILENAMEA ofn;

	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = (NULL == lpstrFilter) ? DEFAULT_FILTER : lpstrFilter;
	ofn.lpstrFile = lpszFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING;
	ofn.lpstrDefExt = lpszDefaultExt;
	ofn.nFilterIndex = nFilterIndex;

	//����lpstrDefExt���ַ�ֵ


	if(!GetSaveFileNameA(&ofn)) {
		return FALSE;
	}
	return ofn.nFilterIndex;

}

DWORD SaveFile(HWND hWnd, wchar_t * lpszFileName, LPCWSTR lpszDefaultExt, LPCWSTR lpstrFilter, DWORD nFilterIndex)
{
	OPENFILENAMEW ofn;

	ZeroMemory(&ofn, sizeof(OPENFILENAMEW));
	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = (NULL == lpstrFilter) ? __L(DEFAULT_FILTER) : lpstrFilter;
	ofn.lpstrFile = lpszFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLESIZING;
	ofn.lpstrDefExt = lpszDefaultExt;
	ofn.nFilterIndex = nFilterIndex;

	if(!GetSaveFileNameW(&ofn)) {
		return FALSE;
	}
	return ofn.nFilterIndex;

}

BOOL BrowseForFolderByPath(HWND hWnd, TCHAR * lpszPathName)
{

	BROWSEINFO					cBI;
	ITEMIDLIST					idl;
	LPITEMIDLIST				pidl = &idl;

	cBI.hwndOwner = hWnd;
	cBI.pidlRoot = 0;
	cBI.lpszTitle = TEXT("��ѡ��Ŀ¼");
	cBI.lpfn = BFFCALLBACK(&BFFCallBack);

	cBI.lParam = (LPARAM)lpszPathName;

	cBI.pszDisplayName = lpszPathName;

	cBI.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
	//OleInitialize(0);
	pidl = SHBrowseForFolder(&cBI);
	//OleUninitialize();
	if(NULL != pidl) {
		SHGetPathFromIDList(pidl, lpszPathName);
		CoTaskMemFree((VOID*)pidl);
		return TRUE;
	}
	return FALSE;
}

int CALLBACK BFFCallBack(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	//TCHAR szPath[MAX_PATH];
	//TCHAR szExePath[MAX_PATH];

	//ITEMIDLIST					idl;
	//LPITEMIDLIST				pidl = &idl;
	//BOOL bTemp;

	switch(uMsg) {
	case BFFM_INITIALIZED:
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		break;
	}

	return 0;
}