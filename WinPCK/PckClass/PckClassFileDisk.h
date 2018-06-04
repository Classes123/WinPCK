#pragma once

typedef unsigned __int64	QWORD;

class CPckClassFileDisk
{
public:
	CPckClassFileDisk();
	~CPckClassFileDisk();

	//ѹ���ؽ���ѹ��д��ռ䲻��ʱ��չ�ռ�
	static BOOL		IsNeedExpandWritingFile(
		CMapViewFileWrite *lpWrite,
		QWORD dwWritingAddressPointer,
		QWORD dwFileSizeToWrite,
		QWORD &dwExpectedTotalCompressedFileSize);

	//�ؽ�pck
	static QWORD	GetPckFilesizeRebuild(LPCTSTR lpszFilename, QWORD qwPckFilesize);

	//������ʱ��Ҫ���ļ��Ĵ�С
	static QWORD	GetPckFilesizeRename(LPCTSTR lpszFilename, QWORD qwCurrentPckFilesize);

	//��Ӻ��½��ļ�ʱ��д���pck�ļ���С
	static QWORD	GetPckFilesizeByCompressed(LPCSTR lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize);
	static QWORD	GetPckFilesizeByCompressed(LPCWSTR lpszFilename, QWORD qwToCompressFilesize, QWORD qwCurrentPckFilesize);
private:
	template <typename T>
	static void GetDriverNameFromFilename(const T* lpszFilename, T lpszDiskName[]);
	static QWORD	GetPckFilesizeByCompressed(QWORD qwDiskFreeSpace, QWORD qwToAddSpace, QWORD qwCurrentPckFilesize);
};

