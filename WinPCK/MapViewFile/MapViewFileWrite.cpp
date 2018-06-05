//////////////////////////////////////////////////////////////////////
// MapViewFileWrite.cpp: ����ӳ���ļ���ͼ��д��
// 
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2014.4.24
//////////////////////////////////////////////////////////////////////

#include "MapViewFile.h"
#if ENABLE_PCK_PKX_FILE
CMapViewFileWrite::CMapViewFileWrite(DWORD dwMaxPckSize)
{
#ifdef _DEBUG
	m_Max_PckFile_Size = 0x6400000;	//100MB
#else
	m_Max_PckFile_Size = dwMaxPckSize;
#endif
	dwViewSizePck = dwViewSizePkx = 0;

	//*m_szPckFileName = 0;
	//*m_tszPckFileName = 0;

	*m_szPkxFileName = 0;
	*m_tszPkxFileName = 0;
	isWriteMode = TRUE;
}
#else
CMapViewFileWrite::CMapViewFileWrite()
{
	isWriteMode = TRUE;
}
#endif

CMapViewFileWrite::~CMapViewFileWrite()
{
}

#if ENABLE_PCK_PKX_FILE

//void CMapViewFileWrite::SetMaxSinglePckSize(QWORD qwMaxPckSize)
//{
//	m_Max_PckFile_Size = qwMaxPckSize;
//}

BOOL CMapViewFileWrite::OpenPck(LPCSTR lpszFilename, DWORD dwCreationDisposition)
{

	IsPckFile = TRUE;

	if(Open(lpszFilename, dwCreationDisposition)){

		dwPkxSize.qwValue = 0;
		GetPkxName(m_szPkxFileName, lpszFilename);
		dwPckSize.dwValue = ::GetFileSize(hFile, &dwPckSize.dwValueHigh);

		OpenPkx(m_szPkxFileName, OPEN_EXISTING);

		uqwFullSize.qwValue = dwPckSize.qwValue + dwPkxSize.qwValue;

	}else{
		assert(FALSE);
		return FALSE;
	}

	return TRUE;

}

BOOL CMapViewFileWrite::OpenPck(LPCWSTR lpszFilename, DWORD dwCreationDisposition)
{

	IsPckFile = TRUE;

	if(Open(lpszFilename, dwCreationDisposition)){

		dwPkxSize.qwValue = 0;
		GetPkxName(m_tszPkxFileName, lpszFilename);
		dwPckSize.dwValue = ::GetFileSize(hFile, &dwPckSize.dwValueHigh);

		OpenPkx(m_tszPkxFileName, OPEN_EXISTING);

		uqwFullSize.qwValue = dwPckSize.qwValue + dwPkxSize.qwValue;

	}else{
		assert(FALSE);
		return FALSE;
	}

	return TRUE;

}

void CMapViewFileWrite::OpenPkx(LPCSTR lpszFilename, DWORD dwCreationDisposition)
{
	//char szFilename[MAX_PATH];

	//GetPkxName(szFilename, lpszFilename);
	HANDLE hFilePck = hFile;

	if(Open(lpszFilename, dwCreationDisposition)){

		hFile2 = hFile;
		hasPkx = TRUE;
		uqdwMaxPckSize.qwValue = dwPckSize.qwValue;

		dwPkxSize.dwValue = ::GetFileSize(hFile2, &dwPkxSize.dwValueHigh);
	}else{
		//assert(m_Max_PckFile_Size);
		uqdwMaxPckSize.qwValue = m_Max_PckFile_Size;
	}

	hFile = hFilePck;
}

void CMapViewFileWrite::OpenPkx(LPCWSTR lpszFilename, DWORD dwCreationDisposition)
{
	//TCHAR szFilename[MAX_PATH];
	//GetPkxName(szFilename, lpszFilename);
	HANDLE hFilePck = hFile;

	if(Open(lpszFilename, dwCreationDisposition)){

		hFile2 = hFile;
		hasPkx = TRUE;
		uqdwMaxPckSize.qwValue = dwPckSize.qwValue;

		dwPkxSize.dwValue = ::GetFileSize(hFile2, &dwPkxSize.dwValueHigh);

	}else{
		//assert(m_Max_PckFile_Size);
		uqdwMaxPckSize.qwValue = m_Max_PckFile_Size;
	}

	hFile = hFilePck;
}

#endif

BOOL CMapViewFileWrite::Open(LPCSTR lpszFilename, DWORD dwCreationDisposition)
{
	return CMapViewFile::Open(hFile, lpszFilename, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL);
}

BOOL CMapViewFileWrite::Open(LPCWSTR lpszFilename, DWORD dwCreationDisposition)
{
	return CMapViewFile::Open(hFile, lpszFilename, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL);
}

BOOL CMapViewFileWrite::Mapping(QWORD qwMaxSize)
{
#if ENABLE_PCK_PKX_FILE
	if(IsPckFile && (uqdwMaxPckSize.qwValue < qwMaxSize)){

		if(NULL == (hFileMapping = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, uqdwMaxPckSize.dwValueHigh, uqdwMaxPckSize.dwValue, GenerateMapName()))){
			assert(FALSE);
			return FALSE;
		}

		dwPckSize.qwValue = uqdwMaxPckSize.qwValue;

		if(!hasPkx){
			if(0 != *m_szPkxFileName)
				OpenPkx(m_szPkxFileName, OPEN_ALWAYS);
			else if(0 != *m_tszPkxFileName)
				OpenPkx(m_tszPkxFileName, OPEN_ALWAYS);
			else{
			
				UnMaping();
				assert(FALSE);
				return FALSE;
			}
		}

		dwPkxSize.qwValue = qwMaxSize - uqdwMaxPckSize.qwValue;
		//dwPckSize = dwMaxPckSize;

		//if(dwMaxPckSize < dwPkxSize){
		//	int a= 1;
		//	if(dwMaxPckSize < dwMaxSize){
		//		a=2;
		//	}
		//}

		uqwFullSize.qwValue = qwMaxSize;

		//char szNamespace_2[16];
		//memcpy(szNamespace_2, lpszNamespace, 16);
		//strcat_s(szNamespace_2, 16, "_2");

		if(NULL == (hFileMapping2 = CreateFileMappingA(hFile2, NULL, PAGE_READWRITE, dwPkxSize.dwValueHigh, dwPkxSize.dwValue, GenerateMapName()))){
			assert(FALSE);
			return FALSE;
		}


	}else
#endif	
	{
		UNQWORD uqwMaxSize;
		uqwMaxSize.qwValue = qwMaxSize;

		if(NULL == (hFileMapping = CreateFileMappingA(hFile, NULL, PAGE_READWRITE, uqwMaxSize.dwValueHigh, uqwMaxSize.dwValue, GenerateMapName()))){
			assert(FALSE);
			return FALSE;
		}	
	}
	return TRUE;
}

void CMapViewFileWrite::SetEndOfFile()
{
#if ENABLE_PCK_PKX_FILE
	if(hasPkx){
		if(::SetEndOfFile(hFile2))
		dwPkxSize.qwValue = ::SetFilePointer(hFile2, 0, 0, FILE_CURRENT);
	}
#endif
	if(::SetEndOfFile(hFile))
		dwPckSize.qwValue = ::SetFilePointer(hFile, 0, 0, FILE_CURRENT);

}

DWORD CMapViewFileWrite::Write(LPVOID buffer, DWORD dwBytesToWrite)
{
	DWORD	dwFileBytesWrote = 0;
#if ENABLE_PCK_PKX_FILE
	if(hasPkx){

		//DWORD	dwAddressPck, dwAddressPkx;

		//����ʼ��ַ����pck�ļ�ʱ
		if(uqwCurrentPos.qwValue >= uqdwMaxPckSize.qwValue){
			//dwAddressPkx = dwCurrentPos - dwPckSize;

			if(!WriteFile(hFile2, buffer, dwBytesToWrite, &dwFileBytesWrote, NULL))
			{
				return 0;
			}

		}else{
			//UNQWORD	uqwReadEndAT;
			QWORD qwReadEndAT = uqwCurrentPos.qwValue + dwFileBytesWrote;

			//��Read�Ŀ�ȫ���ļ�pck��ʱ
			if(qwReadEndAT < uqdwMaxPckSize.qwValue){

				if(!WriteFile(hFile, buffer, dwBytesToWrite, &dwFileBytesWrote, NULL))
				{
					return 0;
				}

			}else{

				//��Read�Ŀ����ļ�pck�ں�pkx��
				//дpck
				DWORD dwWriteInPck = (DWORD)(uqdwMaxPckSize.qwValue - uqwCurrentPos.qwValue);
				DWORD dwWriteInPkx = dwBytesToWrite - dwWriteInPck;

				if(!WriteFile(hFile, buffer, dwWriteInPck, &dwFileBytesWrote, NULL))
				{
					return 0;
				}

				dwWriteInPck = dwFileBytesWrote;
				//дpkx
				if(!WriteFile(hFile2, ((LPBYTE)buffer) + dwWriteInPck, dwWriteInPkx, &dwFileBytesWrote, NULL))
				{
					return 0;
				}

				dwFileBytesWrote += dwWriteInPck;
			}
		}

		uqwCurrentPos.qwValue += dwFileBytesWrote;
	}else
#endif
	{

		if(!WriteFile(hFile, buffer, dwBytesToWrite, &dwFileBytesWrote, NULL))
		{
			return 0;
		}
	}
	return dwFileBytesWrote;
}

#if ENABLE_PCK_PKX_FILE

BOOL CMapViewFileWrite::OpenPckAndMappingWrite(LPCSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap)
{
	if(!OpenPck(lpFileName, dwCreationDisposition))
		return FALSE;

	if(!Mapping(qdwSizeToMap))
		return FALSE;

	return TRUE;
}

BOOL CMapViewFileWrite::OpenPckAndMappingWrite(LPCWSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap)
{
	if(!OpenPck(lpFileName, dwCreationDisposition))
		return FALSE;

	if(!Mapping(qdwSizeToMap))
		return FALSE;

	return TRUE;
}

#endif