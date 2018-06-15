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

#include <windows.h>
#include "PckClassFileDisk.h"
#include "CharsCodeConv.h"


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

BOOL CPckClassFileDisk::IsNeedExpandWritingFile(
	CMapViewFileWrite *lpWrite,
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
		CPckClassLog			m_PckLogFD;

		QWORD qwSizeToExpand = ((dwFileSizeToWrite + PCK_SPACE_DETECT_SIZE) > PCK_STEP_ADD_SIZE ? (dwFileSizeToWrite + PCK_SPACE_DETECT_SIZE) : PCK_STEP_ADD_SIZE);
		ULARGE_INTEGER lpfree;

		if(GetDiskFreeSpaceExA(lpWrite->GetFileDiskName(), NULL, NULL, &lpfree)) {

			qwSizeToExpand = GetPckFilesizeByCompressed(lpfree.QuadPart, qwSizeToExpand, 0);
		} else {
			qwSizeToExpand = GetPckFilesizeByCompressed(-1, qwSizeToExpand, 0);
		}

		if(dwFileSizeToWrite > qwSizeToExpand) {
			m_PckLogFD.PrintLogW("���̿ռ䲻�㣬����ռ䣺%d��ʣ��ռ䣺%d", dwFileSizeToWrite, qwSizeToExpand);
			return FALSE;
		}

		QWORD qwNewExpectedTotalCompressedFileSize = dwExpectedTotalCompressedFileSize + qwSizeToExpand;

		lpWrite->UnMaping();

		if(!lpWrite->Mapping(qwNewExpectedTotalCompressedFileSize)) {

			m_PckLogFD.PrintLogW(TEXT_VIEWMAP_FAIL);
			lpWrite->Mapping(dwExpectedTotalCompressedFileSize);
			return FALSE;
		}
		dwExpectedTotalCompressedFileSize = qwNewExpectedTotalCompressedFileSize;
	}
	return TRUE;


}

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
VOID CPckClassFileDisk::EnumFile(LPSTR szFilename, BOOL IsPatition, DWORD &dwFileCount, vector<FILES_TO_COMPRESS> *lpFileLinkList, QWORD &qwTotalFileSize, size_t nLen)
{

	char		szPath[MAX_PATH], szFile[MAX_PATH];

	size_t nLenBytePath = mystrcpy(szPath, szFilename) - szPath + 1;
	strcat(szFilename, "\\*.*");

	HANDLE					hFile;
	WIN32_FIND_DATAA		WFD;

	if((hFile = FindFirstFileA(szFilename, &WFD)) != INVALID_HANDLE_VALUE) {
		if(!IsPatition) {
			FindNextFileA(hFile, &WFD);
			if(!FindNextFileA(hFile, &WFD)) {
				return;
			}
		}

		do {
			if((WFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {

				if((MAX_PATH_PCK_260 - 13) >= mystrcpy(mystrcpy(mystrcpy(szFile, szPath), "\\"), WFD.cFileName) - szFile) {
					EnumFile(szFile, FALSE, dwFileCount, lpFileLinkList, qwTotalFileSize, nLen);
				}

			} else {

				//if(NULL == pFileinfo)return;
				if(0 != WFD.nFileSizeHigh)continue;

				++dwFileCount;

				lpFileLinkList->push_back(FILES_TO_COMPRESS{ 0 });
				LPFILES_TO_COMPRESS	pFileinfo = &lpFileLinkList->back();

				if(MAX_PATH_PCK_260 < nLenBytePath + strlen(WFD.cFileName)) {
					mystrcpy(mystrcpy(mystrcpy(pFileinfo->szFilename, szPath), "\\"), WFD.cAlternateFileName);
				} else {
					mystrcpy(mystrcpy(mystrcpy(pFileinfo->szFilename, szPath), "\\"), WFD.cFileName);
				}

#if 0
				logOutput(__FUNCTION__, formatString("pFileinfo add:(%d)\t%s\r\n", (int)pFileinfo, pFileinfo->szFilename));
#endif

				pFileinfo->lpszFileTitle = pFileinfo->szFilename + nLen;
				pFileinfo->nBytesToCopy = MAX_PATH - nLen;

				qwTotalFileSize += (pFileinfo->dwFileSize = WFD.nFileSizeLow);

				//lpFileLinkList->insertNext();
				//pFileinfo->next = (LPFILES_TO_COMPRESS)AllocMemory(sizeof(FILES_TO_COMPRESS));
				//pFileinfo = pFileinfo->next;

			}

		} while(FindNextFileA(hFile, &WFD));

		FindClose(hFile);
	}

}

BOOL CPckClassFileDisk::EnumAllFilesByPathList(const vector<tstring> &lpszFilePath, DWORD &_out_FileCount, QWORD &_out_TotalFileSize, vector<FILES_TO_COMPRESS> *lpFileLinkList)
{
	char		szPathMbsc[MAX_PATH];
	DWORD		dwAppendCount = lpszFilePath.size();

	for(DWORD i = 0; i < dwAppendCount; i++) {

#ifdef UNICODE
		CUcs2Ansi cU2A;
		cU2A.GetString(lpszFilePath[i].c_str(), szPathMbsc, MAX_PATH);
#else
		strcpy_s(szPathMbsc, lpszFilePath[i].c_str());
#endif
		size_t nLen = (size_t)(strrchr(szPathMbsc, '\\') - szPathMbsc) + 1;

		if(FILE_ATTRIBUTE_DIRECTORY == (FILE_ATTRIBUTE_DIRECTORY & GetFileAttributesA(szPathMbsc))) {
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

			strcpy(lpfirstFile->szFilename, szPathMbsc);

			lpfirstFile->lpszFileTitle = lpfirstFile->szFilename + nLen;
			lpfirstFile->nBytesToCopy = MAX_PATH - nLen;

			_out_TotalFileSize += (lpfirstFile->dwFileSize = cFileRead.GetFileSize());

			//lpFileLinkList->insertNext();
			_out_FileCount++;
		}
	}

	assert(0 != _out_FileCount);
	return TRUE;

}

#pragma endregion