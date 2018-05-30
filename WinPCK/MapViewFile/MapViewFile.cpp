//////////////////////////////////////////////////////////////////////
// MapViewFile.cpp: ����ӳ���ļ���ͼ
// 
//
// �˳����� �����/stsm/liqf ��д�����ִ���ȡ��FastCopy
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#include "MapViewFile.h"
#include <stdio.h>

BOOL CMapViewFile::isWinNt()
{
	DWORD dwWinVersion = ::GetVersion();
	return (LOBYTE(LOWORD(dwWinVersion)) >= 4 && dwWinVersion < 0x80000000);
}

void CMapViewFile::MakeUnlimitedPath(LPWSTR _dst, LPCWSTR	_src, size_t size)
{
	WCHAR	*prefix;
	// (isUNC ? 1 : 0) ... PATH_UNC_PREFIX �Έ��ϡ�\\server -> \\?\UNC\server 
	//  �ˤ��뤿�ᡢ\\server ���^�� \ ��һ�ĝ�����
	BOOL	isUNC = ('\\' == *_src) ? _src++, TRUE : FALSE;

	prefix = isUNC ? PATHW_UNC_PREFIX : PATHW_LOCAL_PREFIX;

	wcscpy_s(_dst, size, prefix);
	wcscat_s(_dst, size, _src);

}

void CMapViewFile::MakeUnlimitedPath(LPSTR _dst, LPCSTR _src, size_t size)
{
	char	*prefix;
	BOOL	isUNC = ('\\' == *_src) ? _src++, TRUE : FALSE;
	prefix = isUNC ? PATH_UNC_PREFIX : PATH_LOCAL_PREFIX;

	strcpy_s(_dst, size, prefix);
	strcat_s(_dst, size, _src);

}

/*****************************************************************************
BOOL FileExists(CONST TCHAR szName[MAX_PATH_EX])
szName	: (IN) string

Return Value:
returns TRUE if szName exists and is not a directory; FALSE otherwise
*****************************************************************************/
BOOL CMapViewFile::FileExists(LPCSTR szName)
{
	DWORD dwResult = GetFileAttributesA(szName);
	return (dwResult != INVALID_FILE_ATTRIBUTES && !(dwResult & FILE_ATTRIBUTE_DIRECTORY));
}

BOOL CMapViewFile::FileExists(LPCWSTR szName)
{
	DWORD dwResult = GetFileAttributesW(szName);
	return (dwResult != INVALID_FILE_ATTRIBUTES && !(dwResult & FILE_ATTRIBUTE_DIRECTORY));
}

BOOL CMapViewFile::Open(HANDLE &hFile, LPCSTR lpszFilename, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes)
{
	if(FileExists(lpszFilename) ||
		((OPEN_EXISTING != dwCreationDisposition) && (TRUNCATE_EXISTING != dwCreationDisposition))) {
		char szFilename[MAX_PATH];
		if(INVALID_HANDLE_VALUE == (hFile = CreateFileA(lpszFilename, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL))) {
			if(isWinNt()) {
				MakeUnlimitedPath(szFilename, lpszFilename, MAX_PATH);
				if(INVALID_HANDLE_VALUE == (hFile = CreateFileA(szFilename, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL))) {
					assert(FALSE);
					return FALSE;
				}
			} else {
				assert(FALSE);
				return FALSE;
			}
		}
	} else {
		return FALSE;
	}

	return TRUE;
}

BOOL CMapViewFile::Open(HANDLE &hFile, LPCWSTR lpszFilename, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes)
{
	if(FileExists(lpszFilename) ||
		((OPEN_EXISTING != dwCreationDisposition) && (TRUNCATE_EXISTING != dwCreationDisposition))) {
		WCHAR szFilename[MAX_PATH];

		if(INVALID_HANDLE_VALUE == (hFile = CreateFileW(lpszFilename, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL))) {
			if(isWinNt()) {
				MakeUnlimitedPath(szFilename, lpszFilename, MAX_PATH);
				if(INVALID_HANDLE_VALUE == (hFile = CreateFileW(szFilename, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, dwFlagsAndAttributes, NULL))) {
					assert(FALSE);
					return FALSE;
				}
			} else {
				assert(FALSE);
				return FALSE;
			}
		}
	} else {
		return FALSE;
	}

	return TRUE;
}

#if ENABLE_PCK_PKX_FILE

void CMapViewFile::GetPkxName(LPSTR dst, LPCSTR src)
{

	strcpy_s(dst, MAX_PATH, src);
	int slen = strlen(dst);
	char *lpszDst = dst + slen - 2;

	//.pck -> .pkx
	*(__int16*)lpszDst = *(__int16*)"kx";


}

void CMapViewFile::GetPkxName(LPWSTR dst, LPCWSTR src)
{
	wcscpy_s(dst, MAX_PATH, src);
	int slen = wcslen(dst);
	wchar_t *lpszDst = dst + slen - 2;

	//.pck -> .pkx
	*(__int32*)lpszDst = *(__int32*)L"kx";

}

#endif

CMapViewFile::CMapViewFile()
{
	hFile = hFileMapping = NULL;
	lpMapAddress = NULL;

#if ENABLE_PCK_PKX_FILE
	hFile2 = hFileMapping2 = NULL;
	IsPckFile = hasPkx = isCrossView = FALSE;
	lpMapAddress2 = NULL;
	lpCrossBuffer = NULL;
#endif
	uqwCurrentPos.qwValue = 0;

}

CMapViewFile::~CMapViewFile()
{
	clear();
}

void CMapViewFile::clear()
{


	UnmapView();
	//UnMaping();

	if(NULL != hFileMapping) {
		CloseHandle(hFileMapping);
		hFileMapping = NULL;
	}

	if(NULL != hFile && INVALID_HANDLE_VALUE != hFile) {
		CloseHandle(hFile);
		hFile = NULL;
	}
#if ENABLE_PCK_PKX_FILE
	if(hasPkx) {

		if(NULL != hFileMapping2) {
			CloseHandle(hFileMapping2);
			hFileMapping2 = NULL;
		}

		if(NULL != hFile2 && INVALID_HANDLE_VALUE != hFile2) {
			CloseHandle(hFile2);
			hFile2 = NULL;
		}

		//pkx�ļ�Ϊ0ʱ��ɾ��
		if(0 == dwPkxSize.qwValue) {
			if(0 != *m_szPkxFileName)
				DeleteFileA(m_szPkxFileName);
			else if(0 != *m_tszPkxFileName)
				DeleteFileW(m_tszPkxFileName);
		}
	}
#endif
}



LPBYTE CMapViewFile::ViewReal(LPVOID & lpMapAddress, HANDLE hFileMapping, QWORD qwAddress, DWORD dwSize)
{
	//�ļ�ӳ���ַ������64k(0x10000)������

	UNQWORD	qwViewAddress;
	qwViewAddress.qwValue = qwAddress;

	DWORD	dwMapViewBlock64KAlignd, dwMapViewBlockLow, dwNumberOfBytesToMap;

	dwMapViewBlock64KAlignd = qwViewAddress.dwValue & 0xffff0000;
	dwMapViewBlockLow = qwViewAddress.dwValue & 0xffff;
	dwNumberOfBytesToMap = dwMapViewBlockLow + dwSize;

	DWORD dwDesiredAccess = isWriteMode ? FILE_MAP_WRITE : FILE_MAP_READ;

	lpMapAddress = MapViewOfFile(hFileMapping, // Handle to mapping object.
		dwDesiredAccess, // Read/write permission
		qwViewAddress.dwValueHigh, // Max. object size.
		dwMapViewBlock64KAlignd, // Size of hFile.
		dwNumberOfBytesToMap); // Map entire file.
	if(NULL == lpMapAddress)
		return (lpTargetBuffer = NULL);
	else
		return (lpTargetBuffer = ((LPBYTE)lpMapAddress + dwMapViewBlockLow));
}

LPBYTE CMapViewFile::View(QWORD qdwAddress, DWORD dwSize)
{

	//��dwSize=0ʱ��VIEW�����ļ���
	//��hasPkx=1ʱ��dwSize=0���ᷢ��
#if ENABLE_PCK_PKX_FILE
	if(hasPkx) {

		//����ʼ��ַ����pck�ļ�ʱ
		if(qdwAddress >= uqdwMaxPckSize.qwValue) {
			QWORD	dwAddressPkx = qdwAddress - uqdwMaxPckSize.qwValue;
			return ViewReal(lpMapAddress2, hFileMapping2, dwAddressPkx, dwSize);
		} else {

			QWORD	dwViewEndAT = qdwAddress + dwSize;

			//��View�Ŀ�ȫ���ļ�pck��ʱ
			if(dwViewEndAT < uqdwMaxPckSize.qwValue) {

				return ViewReal(lpMapAddress, hFileMapping, qdwAddress, dwSize);
			} else {

				//��View�Ŀ����ļ�pck�ں�pkx��

				if(NULL == (lpCrossBuffer = (LPBYTE)malloc(dwSize))) {

					return NULL;
				}

				dwViewSizePck = (DWORD)(uqdwMaxPckSize.qwValue - qdwAddress);
				dwViewSizePkx = dwSize - dwViewSizePck;
				//dwCrossAddress = dwAddress;

				LPBYTE buf;

				if(NULL == (buf = ViewReal(lpMapAddress, hFileMapping, qdwAddress, dwViewSizePck))) {

					free(lpCrossBuffer);
					lpCrossBuffer = NULL;

					return NULL;
				}

				memcpy(lpCrossBuffer, buf, dwViewSizePck);
				//����pck��buf����unmapʱ���ڻ�д����
				lpCrossAddressPck = buf;

				if(NULL == (buf = ViewReal(lpMapAddress2, hFileMapping2, 0, dwViewSizePkx))) {

					free(lpCrossBuffer);
					lpCrossBuffer = NULL;

					UnmapView();

					return NULL;
				}

				isCrossView = TRUE;

				memcpy(lpCrossBuffer + dwViewSizePck, buf, dwViewSizePkx);

				return lpCrossBuffer;
			}
		}
	} else
#endif
	{
		return ViewReal(lpMapAddress, hFileMapping, qdwAddress, dwSize);
	}
}


LPBYTE CMapViewFile::ReView(QWORD dwAddress, DWORD dwSize)
{
	UnmapView();
	return View(dwAddress, dwSize);
}

void CMapViewFile::SetFilePointer(QWORD lDistanceToMove, DWORD dwMoveMethod)
{
	DWORD rtn;
#if ENABLE_PCK_PKX_FILE
	if(hasPkx) {

		UNQWORD	uqdwSetRealPointerAt;

		switch(dwMoveMethod) {
		case FILE_BEGIN:
			uqwCurrentPos.qwValue = lDistanceToMove;
			break;

		case FILE_CURRENT:
			uqwCurrentPos.qwValue += (LONGLONG)lDistanceToMove;
			break;

		case FILE_END:
			uqwCurrentPos.qwValue = uqwFullSize.qwValue + (LONGLONG)lDistanceToMove;
			break;
		}

		if(uqwCurrentPos.qwValue >= uqdwMaxPckSize.qwValue) {
			uqdwSetRealPointerAt.qwValue = uqwCurrentPos.qwValue - uqdwMaxPckSize.qwValue;


			rtn = ::SetFilePointer(hFile, uqdwMaxPckSize.dwValue, &uqdwMaxPckSize.lValueHigh, FILE_BEGIN);
			assert(INVALID_SET_FILE_POINTER != rtn);
			rtn = ::SetFilePointer(hFile2, uqdwSetRealPointerAt.dwValue, &uqdwSetRealPointerAt.lValueHigh, FILE_BEGIN);
			assert(INVALID_SET_FILE_POINTER != rtn);
		} else {
			rtn = ::SetFilePointer(hFile, uqwCurrentPos.dwValue, &uqwCurrentPos.lValueHigh, FILE_BEGIN);
			assert(INVALID_SET_FILE_POINTER != rtn);
			rtn = ::SetFilePointer(hFile2, 0, 0, FILE_BEGIN);
			assert(INVALID_SET_FILE_POINTER != rtn);
		}

	} else
#endif
	{
		//dwPosHigh = HIWORD(lDistanceToMove);
		uqwCurrentPos.qwValue = lDistanceToMove;
		rtn = ::SetFilePointer(hFile, uqwCurrentPos.dwValue, &uqwCurrentPos.lValueHigh, dwMoveMethod);
		assert(INVALID_SET_FILE_POINTER != rtn);
	}
}

void CMapViewFile::UnmapView()
{
#if ENABLE_PCK_PKX_FILE
	if(hasPkx) {

		if(isCrossView) {

			if(NULL != lpCrossBuffer) {

				//�����ļ�Viewʱ��д���������Ҫ�ֶ���д
				if(isWriteMode) {
					memcpy(lpCrossAddressPck, lpCrossBuffer, dwViewSizePck);
					memcpy(lpMapAddress2, lpCrossBuffer + dwViewSizePck, dwViewSizePkx);
				}
				free(lpCrossBuffer);
			}
			isCrossView = FALSE;
		}

		if(NULL != lpMapAddress2)
			UnmapViewOfFile(lpMapAddress2);
		lpMapAddress2 = NULL;
	}
#endif
	if(NULL != lpMapAddress)
		UnmapViewOfFile(lpMapAddress);

	lpMapAddress = NULL;
}

void CMapViewFile::UnMaping()
{
#if ENABLE_PCK_PKX_FILE
	if(hasPkx) {

		if(NULL != hFileMapping2)
			CloseHandle(hFileMapping2);
		hFileMapping2 = NULL;

	}
#endif
	if(NULL != hFileMapping)
		CloseHandle(hFileMapping);
	hFileMapping = NULL;
}

DWORD CMapViewFile::Read(LPVOID buffer, DWORD dwBytesToRead)
{
	DWORD	dwFileBytesRead = 0;
#if ENABLE_PCK_PKX_FILE
	if(hasPkx) {

		//����ʼ��ַ����pck�ļ�ʱ
		if(uqwCurrentPos.qwValue >= uqdwMaxPckSize.qwValue) {

			if(!ReadFile(hFile2, buffer, dwBytesToRead, &dwFileBytesRead, NULL)) {
				return 0;
			}

		} else {
			QWORD qwReadEndAT = uqwCurrentPos.qwValue + dwBytesToRead;

			//��Read�Ŀ�ȫ���ļ�pck��ʱ
			if(qwReadEndAT < uqdwMaxPckSize.qwValue) {

				if(!ReadFile(hFile, buffer, dwBytesToRead, &dwFileBytesRead, NULL)) {
					return 0;
				}
			} else {

				//��Read�Ŀ����ļ�pck�ں�pkx��
				//��pck
				DWORD dwReadInPck = (DWORD)(uqdwMaxPckSize.qwValue - uqwCurrentPos.qwValue);
				DWORD dwReadInPkx = dwBytesToRead - dwReadInPck;

				if(!ReadFile(hFile, buffer, dwReadInPck, &dwFileBytesRead, NULL)) {
					return 0;
				}

				dwReadInPck = dwFileBytesRead;
				//��pkx
				if(!ReadFile(hFile2, ((LPBYTE)buffer) + dwReadInPck, dwReadInPkx, &dwFileBytesRead, NULL)) {
					return 0;
				}

				dwFileBytesRead += dwReadInPck;
			}
		}

		uqwCurrentPos.qwValue += dwFileBytesRead;
	} else
#endif
	{
		if(!ReadFile(hFile, buffer, dwBytesToRead, &dwFileBytesRead, NULL)) {
			return 0;
		}
	}

	return dwFileBytesRead;
}


#if ENABLE_PCK_PKX_FILE

QWORD CMapViewFile::GetFileSize()
{
	UNQWORD cqwSize;

	if(!IsPckFile) {
		cqwSize.dwValue = ::GetFileSize(hFile, &cqwSize.dwValueHigh);
		return cqwSize.qwValue;
	} else
		return uqwFullSize.qwValue;
}

#else

QWORD CMapViewFile::GetFileSize()
{
	UNQWORD cqwSize;
	cqwSize.dwValue = ::GetFileSize(hFile, &cqwSize.dwValueHigh);
	return cqwSize.qwValue;
}

#endif

#if ENABLE_PCK_PKX_FILE
BOOL CMapViewFile::SetPckPackSize(QWORD qwPckSize)
{
	if(uqwFullSize.qwValue < qwPckSize) {
		//pck��ע���ļ���С��ʵ���ļ�Ҫ���ļ���ȫ
		return FALSE;
	} else if(uqwFullSize.qwValue > qwPckSize) {
		//pck��ע���ļ���С��ʵ���ļ�ҪС������ָ���ļ���С
		uqwFullSize.qwValue = qwPckSize;
		if(hasPkx) {
			dwPkxSize.qwValue = qwPckSize - dwPckSize.qwValue;
		}
	}

	return TRUE;
}
#endif

//�Զ�����CreateFileMappingAʱ����Ҫ��name��Ҫ��ͬʱ����ʹ�õ�FileMapping��name�����ظ�
LPCSTR CMapViewFile::GenerateMapName()
{
	//
	static unsigned int dwDupRemover = 0;
	
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);

	sprintf_s(szFileMappingName, "mapv%x%x%x", GetCurrentThreadId(), counter.LowPart, dwDupRemover++);
#ifdef _DEBUG
	OutputDebugStringA(__FUNCTION__);
	OutputDebugStringA(": ");
	OutputDebugStringA(szFileMappingName);
	OutputDebugStringA("\r\n");	
#endif
	return szFileMappingName;
}