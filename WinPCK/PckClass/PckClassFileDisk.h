#pragma once

typedef unsigned __int64	QWORD;
#include "PckDefines.h"
#include "PckStructs.h"
#include "PckClassLinkList.h"


namespace NPckClassFileDisk
{
	//ѹ���ؽ���ѹ��д��ռ䲻��ʱ��չ�ռ�
	BOOL		IsNeedExpandWritingFile(
		CMapViewFileWrite *lpWrite,
		QWORD dwWritingAddressPointer,
		QWORD dwFileSizeToWrite,
		QWORD &dwExpectedTotalCompressedFileSize);

	//�ؽ�pck
	QWORD	GetPckFilesizeRebuild(LPCTSTR lpszFilename, QWORD qwPckFilesize);

	//������ʱ��Ҫ���ļ��Ĵ�С
	QWORD	GetPckFilesizeRename(LPCTSTR lpszFilename, QWORD qwCurrentPckFilesize);

	//��Ӻ��½��ļ�ʱ��д���pck�ļ���С
	QWORD	GetPckFilesizeByCompressed(LPCSTR lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize);
	QWORD	GetPckFilesizeByCompressed(LPCWSTR lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize);

	//�����ļ���
	VOID	EnumFile(LPSTR szFilename, BOOL IsPatition, DWORD &dwFileCount, LinkList<FILES_TO_COMPRESS> *lpFileLinkList, QWORD &qwTotalFileSize, size_t nLen);

}
