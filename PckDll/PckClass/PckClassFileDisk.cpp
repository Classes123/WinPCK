//////////////////////////////////////////////////////////////////////
// PckClassFileDisk.cpp: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// ��Ҫ����д���ļ��ĳ�ʼ��С�����̿ռ��
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2018.5.31
//////////////////////////////////////////////////////////////////////

//#include "imagehlp.h"
//#pragma comment(lib,"imagehlp.lib")

#include <windows.h>
#include "PckClassFileDisk.h"


//����ʱ��ʣ���ļ��Ŀռ�������ʱ�������
#define	PCK_STEP_ADD_SIZE				(64*1024*1024)
//����ʱ��ʣ���ļ��Ŀռ���С�ڴ�ֵʱ����չ����
#define	PCK_SPACE_DETECT_SIZE			(4*1024*1024)
//����ʱ��ʣ���ļ��Ŀռ���С�ڴ�ֵ(PCK_SPACE_DETECT_SIZE)ʱ����չ���ݵ�ֵ
//#define PCK_RENAME_EXPAND_ADD			(16*1024*1024)

#define ZLIB_AVG_RATIO					0.6

CPckClassFileDisk::CPckClassFileDisk()
{}

CPckClassFileDisk::~CPckClassFileDisk()
{}

#pragma region ���̿ռ��С

template <typename T>
_inline T * __fastcall mystrcpy(T * dest, const T *src)
{
	while((*dest = *src))
		++dest, ++src;
	return dest;
}


template <typename T>
void GetDriverNameFromFilename(const T* lpszFilename, T lpszDiskName[])
{
	lpszDiskName[0] = lpszFilename[0];
	lpszDiskName[1] = lpszFilename[1];
	lpszDiskName[2] = lpszFilename[2];
	lpszDiskName[3] = '\0';
}

//qwCurrentPckFilesizeΪ�Ѿ����ڵ��ļ���С��qwToAddSpace����Ҫ����Ĵ�С������ֵΪ��qwCurrentPckFilesize + ���������������С��
QWORD CPckClassFileDisk::GetPckFilesizeByCompressed(QWORD qwDiskFreeSpace, QWORD qwToAddSpace, QWORD qwCurrentPckFilesize)
{
	//��������Ҫ���ռ�qwTotalFileSize
	QWORD	qwTotalFileSizeTemp = qwToAddSpace;

	if(-1 != qwDiskFreeSpace) {

		//�������Ŀռ�С�ڴ���ʣ��ռ䣬�������ļ��ռ��С����ʣ����̿ռ�
		if(qwDiskFreeSpace < qwTotalFileSizeTemp)
			qwTotalFileSizeTemp = qwDiskFreeSpace;
	}

	return (qwTotalFileSizeTemp + qwCurrentPckFilesize);

}

#if 0
BOOL CPckClassFileDisk::IsNeedExpandWritingFile(
	CMapViewFileMultiPckWrite *lpWrite,
	QWORD dwWritingAddressPointer,
	QWORD dwFileSizeToWrite,
	QWORD &dwExpectedTotalCompressedFileSize)
{
	//�ж�һ��dwAddress��ֵ�᲻�ᳬ��dwTotalFileSizeAfterCompress
	//���������˵���ļ��ռ�����Ĺ�С����������һ��ReCreateFileMapping
	//���ļ���С��ԭ���Ļ���������(lpfirstFile->dwFileSize + 1mb) >= 64mb ? (lpfirstFile->dwFileSize + 1mb) :64mb
	//1mb=0x100000
	//64mb=0x4000000
	if((dwWritingAddressPointer + dwFileSizeToWrite + PCK_SPACE_DETECT_SIZE) > dwExpectedTotalCompressedFileSize) {
		
		//��ӡ��־
		//CPckClassLog			m_PckLogFD;

		QWORD qwSizeToExpand = ((dwFileSizeToWrite + PCK_SPACE_DETECT_SIZE) > PCK_STEP_ADD_SIZE ? (dwFileSizeToWrite + PCK_SPACE_DETECT_SIZE) : PCK_STEP_ADD_SIZE);
		ULARGE_INTEGER lpfree;

		if(GetDiskFreeSpaceExA(lpWrite->GetFileDiskName(), NULL, NULL, &lpfree)) {

			qwSizeToExpand = GetPckFilesizeByCompressed(lpfree.QuadPart, qwSizeToExpand, 0);
		} else {
			qwSizeToExpand = GetPckFilesizeByCompressed(-1, qwSizeToExpand, 0);
		}

		if(dwFileSizeToWrite > qwSizeToExpand) {
			//m_PckLogFD.PrintLogW("���̿ռ䲻�㣬����ռ䣺%d��ʣ��ռ䣺%d", dwFileSizeToWrite, qwSizeToExpand);
			SetErrMsgFlag(PCK_ERR_DISKFULL);
			return FALSE;
		}

		QWORD qwNewExpectedTotalCompressedFileSize = dwExpectedTotalCompressedFileSize + qwSizeToExpand;

		lpWrite->UnMaping();

		if(!lpWrite->Mapping(qwNewExpectedTotalCompressedFileSize)) {

			//m_PckLogFD.PrintLogW(TEXT_VIEWMAP_FAIL);
			SetErrMsgFlag(PCK_ERR_VIEWMAP_FAIL);
			lpWrite->Mapping(dwExpectedTotalCompressedFileSize);
			return FALSE;
		}
		dwExpectedTotalCompressedFileSize = qwNewExpectedTotalCompressedFileSize;
	}
	return TRUE;
}
#endif

//������ʱ��Ҫ���ļ��Ĵ�С
QWORD CPckClassFileDisk::GetPckFilesizeRename(LPCTSTR lpszFilename, QWORD qwCurrentPckFilesize)
{
	//�鿴���̿ռ�
	TCHAR szDiskName[4];
	ULARGE_INTEGER lpfree;
	GetDriverNameFromFilename(lpszFilename, szDiskName);


	if(GetDiskFreeSpaceEx(szDiskName, NULL, NULL, &lpfree)) {

		return GetPckFilesizeByCompressed(lpfree.QuadPart, PCK_SPACE_DETECT_SIZE, qwCurrentPckFilesize);
	} else {
		return GetPckFilesizeByCompressed(-1, PCK_SPACE_DETECT_SIZE, qwCurrentPckFilesize);
	}

}

QWORD CPckClassFileDisk::GetPckFilesizeRebuild(LPCTSTR lpszFilename, QWORD qwPckFilesize)
{
	//�鿴���̿ռ�
	TCHAR szDiskName[4];
	ULARGE_INTEGER lpfree;
	GetDriverNameFromFilename(lpszFilename, szDiskName);

	QWORD qwToAddSpace = qwPckFilesize + PCK_SPACE_DETECT_SIZE;

	if(GetDiskFreeSpaceEx(szDiskName, NULL, NULL, &lpfree)) {

		return GetPckFilesizeByCompressed(lpfree.QuadPart, qwToAddSpace, 0);
	} else {
		return GetPckFilesizeByCompressed(-1, qwToAddSpace, 0);
	}
}

QWORD CPckClassFileDisk::GetPckFilesizeByCompressed(LPCSTR lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize)
{
	//�鿴���̿ռ�
	char szDiskName[4];
	ULARGE_INTEGER lpfree;
	GetDriverNameFromFilename<char>(lpszFilename, szDiskName);

	QWORD qwToAddSpace = qwToCompressFilesize * ZLIB_AVG_RATIO + PCK_SPACE_DETECT_SIZE;

	if(GetDiskFreeSpaceExA(szDiskName, NULL, NULL, &lpfree)) {

		return GetPckFilesizeByCompressed(lpfree.QuadPart, qwToAddSpace, qwCurrentPckFilesize);
	} else {
		return GetPckFilesizeByCompressed(-1, qwToAddSpace, qwCurrentPckFilesize);
	}
}
#pragma endregion
#pragma region Open file,enum
//�ļ��򿪡�����������
QWORD CPckClassFileDisk::GetPckFilesizeByCompressed(LPCWSTR lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize)
{
	//�鿴���̿ռ�
	wchar_t szDiskName[4];
	ULARGE_INTEGER lpfree;
	GetDriverNameFromFilename<wchar_t>(lpszFilename, szDiskName);

	QWORD qwToAddSpace = qwToCompressFilesize * ZLIB_AVG_RATIO + PCK_SPACE_DETECT_SIZE;

	if(GetDiskFreeSpaceExW(szDiskName, NULL, NULL, &lpfree)) {

		return GetPckFilesizeByCompressed(lpfree.QuadPart, qwToAddSpace, qwCurrentPckFilesize);
	} else {
		return GetPckFilesizeByCompressed(-1, qwToAddSpace, qwCurrentPckFilesize);
	}

}

//////////////////////////�����ǳ����������Ҫ���õĹ���///////////////////////////////
VOID CPckClassFileDisk::EnumFile(LPWSTR szFilename, BOOL IsPatition, DWORD &dwFileCount, vector<FILES_TO_COMPRESS> *lpFileLinkList, QWORD &qwTotalFileSize, size_t nLen)
{

	wchar_t		szPath[MAX_PATH], szFile[MAX_PATH];

	size_t nLenBytePath = mystrcpy(szPath, szFilename) - szPath + 1;
	wcscat(szFilename, L"\\*.*");

	HANDLE					hFile;
	WIN32_FIND_DATAW		WFD;

	if((hFile = FindFirstFileW(szFilename, &WFD)) != INVALID_HANDLE_VALUE) {
		if(!IsPatition) {
			FindNextFileW(hFile, &WFD);
			if(!FindNextFileW(hFile, &WFD)) {
				return;
			}
		}

		do {
			if((WFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {

				if((MAX_PATH_PCK_260 - 13) >= mystrcpy(mystrcpy(mystrcpy(szFile, szPath), L"\\"), WFD.cFileName) - szFile) {
					EnumFile(szFile, FALSE, dwFileCount, lpFileLinkList, qwTotalFileSize, nLen);
				}

			} else {

				//if(NULL == pFileinfo)return;
				if(0 != WFD.nFileSizeHigh)continue;

				++dwFileCount;

				lpFileLinkList->push_back(FILES_TO_COMPRESS{ 0 });
				LPFILES_TO_COMPRESS	pFileinfo = &lpFileLinkList->back();

				if(MAX_PATH_PCK_260 < nLenBytePath + wcslen(WFD.cFileName)) {
					mystrcpy(mystrcpy(mystrcpy(pFileinfo->szwFilename, szPath), L"\\"), WFD.cAlternateFileName);
				} else {
					mystrcpy(mystrcpy(mystrcpy(pFileinfo->szwFilename, szPath), L"\\"), WFD.cFileName);
				}

#ifdef _DEBUG
				pFileinfo->id = lpFileLinkList->size();
#endif

				pFileinfo->nFileTitleLen = nLen;
				pFileinfo->nBytesToCopy = MAX_PATH - nLen;

				qwTotalFileSize += (pFileinfo->dwFileSize = WFD.nFileSizeLow);

			}

		} while(FindNextFileW(hFile, &WFD));

		FindClose(hFile);
	}

}

BOOL CPckClassFileDisk::EnumAllFilesByPathList(const vector<tstring> &lpszFilePath, DWORD &_out_FileCount, QWORD &_out_TotalFileSize, vector<FILES_TO_COMPRESS> *lpFileLinkList)
{
	wchar_t		szPathMbsc[MAX_PATH];
	DWORD		dwAppendCount = lpszFilePath.size();

	for(DWORD i = 0; i < dwAppendCount; i++) {

#ifdef UNICODE
		
		//cU2A.GetString(lpszFilePath[i].c_str(), szPathMbsc, MAX_PATH);
		wcscpy_s(szPathMbsc, lpszFilePath[i].c_str());
#else
		CAnsi2Ucs cA2U(CP_ACP);
		cA2U.GetString(lpszFilePath[i].c_str(), szPathMbsc, MAX_PATH);
		//strcpy_s(szPathMbsc, lpszFilePath[i].c_str());
#endif
		size_t nLen = (size_t)(wcsrchr(szPathMbsc, L'\\') - szPathMbsc) + 1;

		if(FILE_ATTRIBUTE_DIRECTORY == (FILE_ATTRIBUTE_DIRECTORY & GetFileAttributesW(szPathMbsc))) {
			//�ļ���
			EnumFile(szPathMbsc, FALSE, _out_FileCount, lpFileLinkList, _out_TotalFileSize, nLen);
		} else {

			CMapViewFileRead cFileRead;

			if(!cFileRead.Open(szPathMbsc)) {
				//m_PckLog.PrintLogEL(TEXT_OPENNAME_FAIL, szPathMbsc, __FILE__, __FUNCTION__, __LINE__);

				assert(FALSE);
				return FALSE;
			}

			lpFileLinkList->push_back(FILES_TO_COMPRESS{ 0 });
			LPFILES_TO_COMPRESS	lpfirstFile = &lpFileLinkList->back();

			wcscpy(lpfirstFile->szwFilename, szPathMbsc);

			lpfirstFile->nFileTitleLen = nLen;
			lpfirstFile->nBytesToCopy = MAX_PATH - nLen;

#ifdef _DEBUG
			lpfirstFile->id = lpFileLinkList->size();
#endif
			_out_TotalFileSize += (lpfirstFile->dwFileSize = cFileRead.GetFileSize());
			_out_FileCount++;
		}
	}

	assert(0 != _out_FileCount);
	return TRUE;

}


#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

BOOL CPckClassFileDisk::MakeFolderExistInternal(const wchar_t* lpszDirectory)
{
	wchar_t szUpwardPath[MAX_PATH];
	wcscpy_s(szUpwardPath, lpszDirectory);


	wchar_t *lpLastDir = wcsrchr(szUpwardPath, '\\');

	if (PathFileExistsW(szUpwardPath)) {

		if (PathIsDirectoryW(szUpwardPath)) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}

	*lpLastDir = 0;

	if (MakeFolderExistInternal(szUpwardPath)) {
		*lpLastDir = '\\';
		if (CreateDirectoryW(szUpwardPath, NULL))
			return TRUE;
	}
	return FALSE;
}

BOOL CPckClassFileDisk::MakeFolderExist(const wchar_t* lpszDirectory)
{

	if ((NULL == lpszDirectory) || (0 == *lpszDirectory))
		return FALSE;

	wchar_t szUpwardPath[MAX_PATH];

	GetFullPathNameW(lpszDirectory, MAX_PATH, szUpwardPath, NULL);
	//wcscpy_s(szUpwardPath, lpszDirectory);
	size_t len = wcslen(szUpwardPath);

	if ('\\' == szUpwardPath[len - 1])
		szUpwardPath[len - 1] = 0;

	return MakeFolderExistInternal(szUpwardPath);
}

#pragma endregion
