//////////////////////////////////////////////////////////////////////
// tAttrDlg.cpp: WinPCK �����̲߳���
// �Ի������
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2017.12.26
//////////////////////////////////////////////////////////////////////

#include "miscdlg.h"
#include <shlwapi.h>

TAttrDlg::TAttrDlg(void *_lpPckInfo, void *_lpRootInfo, QWORD _qwPckFileSize, wchar_t *_lpszPath, BOOL _isSearchMode, TWin *_win) : TDlg(IDD_DIALOG_ATTR, _win)
{
	lpPckInfo = _lpPckInfo;
	lpRootInfo = _lpRootInfo;
	lpszPath = _lpszPath;
	isSearchMode = _isSearchMode;
	qwPckFileSize = _qwPckFileSize;

}

//TAttrDlg::~TAttrDlg()
//{
//}

BOOL TAttrDlg::EvCreate(LPARAM lParam)
{
	//ultoa(lpPckNode->lpPckIndexTable->cFileIndex.dwFileCipherTextSize, szPrintf, 10));
#define PRINTF_BYTESIZE	32
#define PRINTF_SIZE		80

	wchar_t	*lpszFilename;
	wchar_t	*lpszwFilename;
	wchar_t	szPrintf[PRINTF_SIZE];
	wchar_t	szPrintfBytesize[PRINTF_BYTESIZE];
	LPCWSTR	lpszFormat = L"%s (%llu �ֽ�)";
	LPCWSTR	lpszFileFormat = L"%s (%u �ֽ�)";


	if(isSearchMode) {
		LPPCKINDEXTABLE	lpPckIndex = (LPPCKINDEXTABLE)lpPckInfo;
		wchar_t	szFilename[MAX_PATH_PCK_260];

		wcscpy(szFilename, lpPckIndex->cFileIndex.szwFilename);

		SetDlgItemTextW(IDC_EDIT_ATTR_TYPE, L"�ļ�");

		if(NULL != (lpszFilename = wcsrchr(szFilename, L'\\'))) {

			*lpszFilename++ = 0;
			SetDlgItemTextW(IDC_EDIT_ATTR_PATH, szFilename);

		} else if(NULL != (lpszFilename = wcsrchr(szFilename, L'/'))) {

			*lpszFilename++ = 0;
			SetDlgItemTextW(IDC_EDIT_ATTR_PATH, szFilename);

		} else {

			lpszFilename = lpPckIndex->cFileIndex.szwFilename;
			SetDlgItemTextA(IDC_EDIT_ATTR_PATH, "");
		}

		SetDlgItemTextW(IDC_EDIT_ATTR_NAME, lpszFilename);

		//ѹ����С
		_stprintf_s(szPrintf, PRINTF_SIZE, lpszFileFormat,
			StrFormatByteSizeW(lpPckIndex->cFileIndex.dwFileCipherTextSize, szPrintfBytesize, PRINTF_BYTESIZE),
			lpPckIndex->cFileIndex.dwFileCipherTextSize);

		SetDlgItemTextW(IDC_EDIT_ATTR_CIPHER, szPrintf);

		//ʵ�ʴ�С
		_stprintf_s(szPrintf, PRINTF_SIZE, lpszFileFormat,
			StrFormatByteSizeW(lpPckIndex->cFileIndex.dwFileClearTextSize, szPrintfBytesize, PRINTF_BYTESIZE),
			lpPckIndex->cFileIndex.dwFileClearTextSize);

		SetDlgItemTextW(IDC_EDIT_ATTR_SIZE, szPrintf);

		//ѹ����
		if(0 == lpPckIndex->cFileIndex.dwFileClearTextSize)
			SetDlgItemTextA(IDC_EDIT_ATTR_CMPR, "-");
		else {
			swprintf_s(szPrintf, PRINTF_SIZE, L"%.1f%%", lpPckIndex->cFileIndex.dwFileCipherTextSize * 100 / (double)lpPckIndex->cFileIndex.dwFileClearTextSize);
			SetDlgItemTextW(IDC_EDIT_ATTR_CMPR, szPrintf);
		}

		//����
		SetDlgItemTextA(IDC_EDIT_ATTR_FILECOUNT, "-");

		//ƫ�Ƶ�ַ
		//		#ifdef PCKV202
		swprintf_s(szPrintf, PRINTF_SIZE, L"%llu (0x%016llX)", lpPckIndex->cFileIndex.dwAddressOffset, lpPckIndex->cFileIndex.dwAddressOffset);
		SetDlgItemTextW(IDC_EDIT_ATTR_ADDR, szPrintf);

	} else {

		LPPCK_PATH_NODE	lpPckNode = (LPPCK_PATH_NODE)lpPckInfo;

		SetDlgItemTextW(IDC_EDIT_ATTR_NAME, lpszwFilename = lpPckNode->szName);
		SetDlgItemTextW(IDC_EDIT_ATTR_PATH, lpszPath);

		if(NULL == lpPckNode->child)//�ļ�
		{
			SetDlgItemTextW(IDC_EDIT_ATTR_TYPE, L"�ļ�");

			//ѹ����С
			swprintf_s(szPrintf, PRINTF_SIZE, lpszFileFormat,
				StrFormatByteSizeW(lpPckNode->lpPckIndexTable->cFileIndex.dwFileCipherTextSize, szPrintfBytesize, PRINTF_BYTESIZE),
				lpPckNode->lpPckIndexTable->cFileIndex.dwFileCipherTextSize);

			SetDlgItemTextW(IDC_EDIT_ATTR_CIPHER, szPrintf);


			//ʵ�ʴ�С
			swprintf_s(szPrintf, PRINTF_SIZE, lpszFileFormat,
				StrFormatByteSizeW(lpPckNode->lpPckIndexTable->cFileIndex.dwFileClearTextSize, szPrintfBytesize, PRINTF_BYTESIZE),
				lpPckNode->lpPckIndexTable->cFileIndex.dwFileClearTextSize);

			SetDlgItemTextW(IDC_EDIT_ATTR_SIZE, szPrintf);



			//ѹ����
			if(0 == lpPckNode->lpPckIndexTable->cFileIndex.dwFileClearTextSize)
				SetDlgItemTextA(IDC_EDIT_ATTR_CMPR, "-");
			else {
				swprintf_s(szPrintf, PRINTF_SIZE, L"%.1f%%", lpPckNode->lpPckIndexTable->cFileIndex.dwFileCipherTextSize / (double)lpPckNode->lpPckIndexTable->cFileIndex.dwFileClearTextSize * 100);
				SetDlgItemTextW(IDC_EDIT_ATTR_CMPR, szPrintf);
			}

			//����
			SetDlgItemTextA(IDC_EDIT_ATTR_FILECOUNT, "-");

			//ƫ�Ƶ�ַ
			swprintf_s(szPrintf, PRINTF_SIZE, L"%llu (0x%016llX)", lpPckNode->lpPckIndexTable->cFileIndex.dwAddressOffset, lpPckNode->lpPckIndexTable->cFileIndex.dwAddressOffset);
			SetDlgItemTextW(IDC_EDIT_ATTR_ADDR, szPrintf);
		} else {

			SetDlgItemTextW(IDC_EDIT_ATTR_TYPE, L"�ļ���");

			//ѹ����С
			swprintf_s(szPrintf, PRINTF_SIZE, lpszFormat,
				StrFormatByteSizeW(lpPckNode->child->qdwDirCipherTextSize, szPrintfBytesize, PRINTF_BYTESIZE),
				lpPckNode->child->qdwDirCipherTextSize);
			SetDlgItemTextW(IDC_EDIT_ATTR_CIPHER, szPrintf);


			//ʵ�ʴ�С
			swprintf_s(szPrintf, PRINTF_SIZE, lpszFormat,
				StrFormatByteSizeW(lpPckNode->child->qdwDirClearTextSize, szPrintfBytesize, PRINTF_BYTESIZE),
				lpPckNode->child->qdwDirClearTextSize);

			SetDlgItemTextW(IDC_EDIT_ATTR_SIZE, szPrintf);


			//ѹ����
			swprintf_s(szPrintf, PRINTF_SIZE, L"%.1f%%", lpPckNode->child->qdwDirCipherTextSize / (float)lpPckNode->child->qdwDirClearTextSize * 100.0);
			SetDlgItemTextW(IDC_EDIT_ATTR_CMPR, szPrintf);

			//����
			swprintf_s(szPrintf, PRINTF_SIZE, L"%u ���ļ���%u ���ļ���", lpPckNode->child->dwFilesCount, lpPckNode->child->dwDirsCount);
			SetDlgItemTextW(IDC_EDIT_ATTR_FILECOUNT, szPrintf);

			SetDlgItemTextA(IDC_EDIT_ATTR_ADDR, "-");

		}


	}

	//�ļ�����Ϣ
	LPPCK_PATH_NODE	lpRootNode = (LPPCK_PATH_NODE)lpRootInfo;
	//�ļ��ܴ�С
	swprintf_s(szPrintf, PRINTF_SIZE, lpszFormat,
		StrFormatByteSizeW(lpRootNode->child->qdwDirClearTextSize, szPrintfBytesize, PRINTF_BYTESIZE),
		lpRootNode->child->qdwDirClearTextSize);

	SetDlgItemTextW(IDC_EDIT_ATTR_ALLSIZE, szPrintf);

	//ѹ������С
	swprintf_s(szPrintf, PRINTF_SIZE, lpszFormat,
		StrFormatByteSizeW(lpRootNode->child->qdwDirCipherTextSize, szPrintfBytesize, PRINTF_BYTESIZE),
		lpRootNode->child->qdwDirCipherTextSize);
	SetDlgItemTextW(IDC_EDIT_ATTR_PCKSIZE, szPrintf);

	//ѹ����
	swprintf_s(szPrintf, PRINTF_SIZE, L"%.1f%%", lpRootNode->child->qdwDirCipherTextSize / (float)lpRootNode->child->qdwDirClearTextSize * 100.0);
	SetDlgItemTextW(IDC_EDIT_ATTR_PCKCMPR, szPrintf);

	//����������
	//int dwDirt = dwPckFileSize - lpRootNode->child->dwDirCipherTextSize;
	swprintf_s(szPrintf, PRINTF_SIZE, lpszFormat,
		StrFormatByteSizeW(qwPckFileSize, szPrintfBytesize, PRINTF_BYTESIZE),
		qwPckFileSize);
	SetDlgItemTextW(IDC_EDIT_ATTR_DIRT, szPrintf);


	//��������
	TCHAR	szTitle[MAX_PATH];
	_stprintf_s(szTitle, MAX_PATH, TEXT("%s ����"), lpszwFilename);
	SetWindowText(szTitle);

	Show();

	return	FALSE;

#undef PRINTF_SIZE
#undef PRINTF_BYTESIZE
}

BOOL TAttrDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch(wID) {
	case IDOK:
	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}
