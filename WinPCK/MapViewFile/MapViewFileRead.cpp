//////////////////////////////////////////////////////////////////////
// CMapViewFileRead.cpp: ����ӳ���ļ���ͼ������
// 
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2014.4.24
//////////////////////////////////////////////////////////////////////

#include "MapViewFile.h"

CMapViewFileRead::CMapViewFileRead()
{
	isWriteMode = FALSE;
}

CMapViewFileRead::~CMapViewFileRead()
{
}

#if ENABLE_PCK_PKX_FILE

BOOL CMapViewFileRead::OpenPck(LPCSTR lpszFilename)
{

	IsPckFile = TRUE;
	if(Open(lpszFilename)){

		dwPkxSize.qwValue = 0;
		dwPckSize.dwValue = ::GetFileSize(hFile, &dwPckSize.dwValueHigh);

		GetPkxName(m_szPkxFileName, lpszFilename);

		HANDLE hFilePck = hFile;

		if(Open(m_szPkxFileName)){

			hFile2 = hFile;
			hasPkx = TRUE;
			uqdwMaxPckSize.qwValue = dwPckSize.qwValue;

			dwPkxSize.dwValue = ::GetFileSize(hFile2, &dwPkxSize.dwValueHigh);

		}

		hFile = hFilePck;

		uqwFullSize.qwValue = dwPckSize.qwValue + dwPkxSize.qwValue;

	}else{
		return FALSE;
	}

	return TRUE;
}

BOOL CMapViewFileRead::OpenPck(LPCWSTR lpszFilename)
{

	IsPckFile = TRUE;

	if(Open(lpszFilename)){

		dwPkxSize.qwValue = 0;
		dwPckSize.dwValue = ::GetFileSize(hFile, &dwPckSize.dwValueHigh);

		GetPkxName(m_tszPkxFileName, lpszFilename);

		HANDLE hFilePck = hFile;
			

		if(Open(m_tszPkxFileName)){

			hFile2 = hFile;
			hasPkx = TRUE;
			uqdwMaxPckSize.qwValue = dwPckSize.qwValue;

			dwPkxSize.dwValue = ::GetFileSize(hFile2, &dwPkxSize.dwValueHigh);

		}

		hFile = hFilePck;

		uqwFullSize.qwValue = dwPckSize.qwValue + dwPkxSize.qwValue;

	}else{
		assert(FALSE);
		return FALSE;
	}

	return TRUE;

}

#endif

BOOL CMapViewFileRead::Open(LPCSTR lpszFilename)
{
	return CMapViewFile::Open(hFile, lpszFilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN);
}

BOOL CMapViewFileRead::Open(LPCWSTR lpszFilename)
{
	return CMapViewFile::Open(hFile, lpszFilename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN);
}

BOOL CMapViewFileRead::Mapping()
{
	if(NULL == (hFileMapping = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, 0, GenerateMapName()))){
		assert(FALSE);
		return FALSE;
	}
#if ENABLE_PCK_PKX_FILE
	if(hasPkx){
		if(NULL == (hFileMapping2 = CreateFileMappingA(hFile2, NULL, PAGE_READONLY, 0, 0, GenerateMapName()))){
			assert(FALSE);
			return FALSE;
		}

	}
#endif
	return TRUE;

}

#if ENABLE_PCK_PKX_FILE

BOOL CMapViewFileRead::OpenPckAndMappingRead(LPCSTR lpFileName)
{
	if(!(OpenPck(lpFileName))) 
		return FALSE;

	if(!(Mapping())) 
		return FALSE;

	return TRUE;
}

BOOL CMapViewFileRead::OpenPckAndMappingRead(LPCWSTR lpFileName)
{
	if(!(OpenPck(lpFileName)))
		return FALSE;

	if(!(Mapping())) 
		return FALSE;

	return TRUE;
}

LPBYTE CMapViewFileRead::OpenMappingAndViewAllRead(LPCSTR lpFileName)
{
	if(OpenPckAndMappingRead(lpFileName))
		return View(0, 0);
	else
		return NULL;
}

LPBYTE CMapViewFileRead::OpenMappingAndViewAllRead(LPCWSTR lpFileName)
{
	if(OpenPckAndMappingRead(lpFileName))
		return View(0, 0);
	else
		return NULL;
}

#endif