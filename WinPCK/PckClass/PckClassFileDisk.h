#pragma once

typedef unsigned __int64	QWORD;
#include "PckDefines.h"
#include "PckClassLinkList.h"
#include "PckClassBaseFeatures.h"

class CPckClassFileDisk
{
public:
	CPckClassFileDisk();
	~CPckClassFileDisk();
	//

	BOOL		EnumAllFilesByPathList(vector<tstring> &lpszFilePath, DWORD &_out_FileCount, QWORD &_out_TotalFileSize, LinkList<FILES_TO_COMPRESS> *lpFileLinkList);


	//ѹ���ؽ���ѹ��д��ռ䲻��ʱ��չ�ռ�
	BOOL		IsNeedExpandWritingFile(
		CMapViewFileWrite *lpWrite,
		QWORD dwWritingAddressPointer,
		QWORD dwFileSizeToWrite,
		QWORD &dwExpectedTotalCompressedFileSize);

	//�ؽ�pckʱ��Ҫ�Ĵ��̿ռ䣨Ŀ���ļ��������ؽ���PCK�ļ���С��
	QWORD	GetPckFilesizeRebuild(LPCTSTR lpszFilename, QWORD qwPckFilesize);

	//������ʱ��Ҫ���ļ��Ĵ�С
	QWORD	GetPckFilesizeRename(LPCTSTR lpszFilename, QWORD qwCurrentPckFilesize);

	//��Ӻ��½��ļ�ʱ��д���pck�ļ���С
	QWORD	GetPckFilesizeByCompressed(LPCSTR lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize);
	QWORD	GetPckFilesizeByCompressed(LPCWSTR lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize);

private:

	//qwCurrentPckFilesizeΪ�Ѿ����ڵ��ļ���С��qwToAddSpace����Ҫ����Ĵ�С������ֵΪ��qwCurrentPckFilesize + ���������������С��
	QWORD	GetPckFilesizeByCompressed(QWORD qwDiskFreeSpace, QWORD qwToAddSpace, QWORD qwCurrentPckFilesize);
	//�����ļ���
	VOID	EnumFile(LPSTR szFilename, BOOL IsPatition, DWORD &dwFileCount, LinkList<FILES_TO_COMPRESS> *lpFileLinkList, QWORD &qwTotalFileSize, size_t nLen);

};

