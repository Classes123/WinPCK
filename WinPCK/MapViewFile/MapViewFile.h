//////////////////////////////////////////////////////////////////////
// MapViewFile.h: ����ӳ���ļ���ͼ
// 
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <assert.h>

#define TEST_T 1
/*
#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5

#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
*/

#if _MSC_VER >= 1400
#pragma warning ( disable : 4996 )
#endif

#define ENABLE_PCK_PKX_FILE 1

#if !defined(_MAPVIEWFILE_H_)
#define _MAPVIEWFILE_H_

typedef unsigned __int64	QWORD;

typedef union _QWORD{
	QWORD qwValue;
	LONGLONG llwValue;
	struct {
		DWORD dwValue;
		DWORD dwValueHigh;
	};
	struct {
		LONG lValue;
		LONG lValueHigh;
	};
}UNQWORD, *LPUNQWORD;

#define PATH_LOCAL_PREFIX			"\\\\?\\"
#define PATH_UNC_PREFIX				"\\\\?\\UNC"

#define PATHW_LOCAL_PREFIX			L"\\\\?\\"
#define PATHW_UNC_PREFIX			L"\\\\?\\UNC"

#define PATH_LOCAL_PREFIX_LEN		(strlen(PATH_LOCAL_PREFIX))
#define PATH_UNC_PREFIX_LEN			(strlen(PATH_UNC_PREFIX))

class CMapViewFile
{
public:
	CMapViewFile();
	virtual ~CMapViewFile();

	BOOL FileExists(LPCSTR szName);
	BOOL FileExists(LPCWSTR szName);

	BOOL	Open(HANDLE &hFile, LPCSTR lpszFilename, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes);
	BOOL	Open(HANDLE &hFile, LPCWSTR lpszFilename, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes);

	LPBYTE	View(QWORD dwAddress, DWORD dwSize = 0);
	virtual LPBYTE	ReView(QWORD dwAddress, DWORD dwSize = 0);

	virtual void	SetFilePointer(QWORD lDistanceToMove, DWORD dwMoveMethod = FILE_BEGIN);

	virtual DWORD	Read(LPVOID buffer, DWORD dwBytesToRead);

	QWORD	GetFileSize();

	void	UnmapView();
	void	UnMaping();
	void	clear();

	//��ȡ��ǰ�ļ��Ĵ�����
	const char*	GetFileDiskName();

#if ENABLE_PCK_PKX_FILE

	BOOL	SetPckPackSize(QWORD qwPckSize);
#endif

protected:
	////�Զ�����CreateFileMappingAʱ����Ҫ��name
	LPCSTR	GenerateMapName();
	
	BOOL isWinNt();
	void MakeUnlimitedPath(LPWSTR _dst, LPCWSTR	_src, size_t size);
	void MakeUnlimitedPath(LPSTR _dst, LPCSTR _src, size_t size);

#if ENABLE_PCK_PKX_FILE
	void GetPkxName(LPSTR dst, LPCSTR src);
	void GetPkxName(LPWSTR dst, LPCWSTR src);
#endif

	template <typename T>
	void GetDiskNameFromFilename(T* lpszFilename);

	LPBYTE	ViewReal(LPVOID & lpMapAddress, HANDLE hFileMapping, QWORD qwAddress, DWORD dwSize);

public:

	LPBYTE	lpTargetBuffer;

protected:

	HANDLE	hFile;
	HANDLE	hFileMapping;
	LPVOID	lpMapAddress;

#if ENABLE_PCK_PKX_FILE

	HANDLE	hFile2;
	HANDLE	hFileMapping2;
	LPVOID	lpMapAddress2;

	UNQWORD	dwPckSize, dwPkxSize;
	UNQWORD	uqdwMaxPckSize;				//pck�ļ������ֵ

	UNQWORD	uqwFullSize;				//pck+pkx�ļ��Ĵ�С����PCKHEAD�е�dwPckSizeΪ׼

	BOOL	IsPckFile, hasPkx;

	BOOL	isCrossView;	//�Ƿ��ǿ��ļ���VIEW
	LPBYTE	lpCrossBuffer, lpCrossAddressPck;

	DWORD	dwViewSizePck, dwViewSizePkx;

	//char	m_szPckFileName[MAX_PATH];
	//wchar_t	m_tszPckFileName[MAX_PATH];

	char	m_szPkxFileName[MAX_PATH];
	wchar_t	m_tszPkxFileName[MAX_PATH];
#endif
	//�ļ���Ӧ�Ĵ���
	char	m_szDisk[8];
	//char	m_szFullFilename[MAX_PATH];
	//wchar_t	m_wszFullFilename[MAX_PATH];

	UNQWORD	uqwCurrentPos;		//��ǰ�ļ�ָ��λ��

	BOOL	isWriteMode;

	//���ڴ��MapName
	char szFileMappingName[32];
};



class CMapViewFileRead : public CMapViewFile
{
public:
	CMapViewFileRead();
	virtual ~CMapViewFileRead();

#if ENABLE_PCK_PKX_FILE

	BOOL	OpenPck(LPCSTR lpszFilename);
	BOOL	OpenPck(LPCWSTR lpszFilename);

	BOOL	OpenPckAndMappingRead(LPCSTR lpFileName);
	BOOL	OpenPckAndMappingRead(LPCWSTR lpFileName);

	LPBYTE OpenMappingAndViewAllRead(LPCSTR lpFileName);
	LPBYTE OpenMappingAndViewAllRead(LPCWSTR lpFileName);
#endif

	BOOL	Open(LPCSTR lpszFilename);
	BOOL	Open(LPCWSTR lpszFilename);

	BOOL	Mapping();


protected:


};


class CMapViewFileWrite : public CMapViewFile
{
public:
	CMapViewFileWrite(DWORD);
	virtual ~CMapViewFileWrite();
	
#if ENABLE_PCK_PKX_FILE
	//void	SetMaxSinglePckSize(QWORD qwMaxPckSize);
	BOOL	OpenPck(LPCSTR lpszFilename, DWORD dwCreationDisposition);
	BOOL	OpenPck(LPCWSTR lpszFilename, DWORD dwCreationDisposition);

	BOOL	OpenPckAndMappingWrite(LPCSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap);
	BOOL	OpenPckAndMappingWrite(LPCWSTR lpFileName, DWORD dwCreationDisposition, QWORD qdwSizeToMap);
#endif

	BOOL	Open(LPCSTR lpszFilename, DWORD dwCreationDisposition);
	BOOL	Open(LPCWSTR lpszFilename, DWORD dwCreationDisposition);

	BOOL	Mapping(QWORD dwMaxSize);
	void	SetEndOfFile();

	DWORD	Write(LPVOID buffer, DWORD dwBytesToWrite);

	//BOOL	FlushViewOfFile();

protected:

#if ENABLE_PCK_PKX_FILE
	void	OpenPkx(LPCSTR lpszFilename, DWORD dwCreationDisposition);
	void	OpenPkx(LPCWSTR lpszFilename, DWORD dwCreationDisposition);
#endif

public:

protected:
#if ENABLE_PCK_PKX_FILE
	//һ�����ļ���Ҫ�ֿ��2��С�ļ�ʱ���ֿ�Ĵ�С
	QWORD	m_Max_PckFile_Size;
#endif

};

#endif //_MAPVIEWFILE_H_