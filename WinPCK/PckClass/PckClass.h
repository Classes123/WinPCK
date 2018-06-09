//////////////////////////////////////////////////////////////////////
// PckClass.h: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "CharsCodeConv.h"

#include "PckClassVersionDetect.h"
//#include "PckClassNode.h"
//#include "PckClassHeadTail.h"

#include "PckClassAppendFiles.h"
#include "PckClassCreateNew.h"
#include "PckClassRebuild.h"
#include "PckClassRenamer.h"

#if !defined(_PCKCLASS_H_)
#define _PCKCLASS_H_

#ifdef _DEBUG
int logOutput(const char *file, const char *text);
char *formatString(const char *format, ...);
#endif

class CPckClass : 
	public CPckClassRenamer,
	public CPckClassRebuild,
	public CPckClassAppendFiles,
	public CPckClassVersionDetect
{
//����
public:
	CPckClass(LPPCK_RUNTIME_PARAMS inout);
	virtual ~CPckClass();

	virtual BOOL	Init(LPCTSTR szFile);
#if 0
	virtual CONST	LPPCKINDEXTABLE		GetPckIndexTable();
	virtual CONST	LPPCK_PATH_NODE		GetPckPathNode();

	//�ļ���С
	QWORD	GetPckSize();

	//��ȡ�ļ���
	DWORD	GetPckFileCount();

	//��������С
	QWORD	GetPckDataAreaSize();

	//�������������ݴ�С
	QWORD	GetPckRedundancyDataSize();
#endif
	//��ѹ�ļ�
	BOOL	ExtractFiles(LPPCKINDEXTABLE *lpIndexToExtract, int nFileCount);
	BOOL	ExtractFiles(LPPCK_PATH_NODE *lpNodeToExtract, int nFileCount);
#if 0
	//�½�pck�ļ�
	virtual BOOL	CreatePckFile(LPTSTR szPckFile, LPTSTR szPath);
#endif
#if 0
	//�ؽ�pck�ļ�
	virtual BOOL	RebuildPckFile(LPCTSTR szRebuildPckFile, BOOL bUseRecompress);
protected:
	BOOL	RebuildPckFile(LPCTSTR szRebuildPckFile);
	BOOL	RecompressPckFile(LPCTSTR szRecompressPckFile);
#endif
public:
#if 0
	//����pck�ļ�
	virtual BOOL	UpdatePckFile(LPTSTR szPckFile, TCHAR (*lpszFilePath)[MAX_PATH], int nFileCount, LPPCK_PATH_NODE lpNodeToInsert);
#endif
#if 0
	//�������ļ�
	virtual BOOL	RenameFilename();
#endif
	//Ԥ���ļ�
	virtual BOOL	GetSingleFileData(LPVOID lpvoidFileRead, LPPCKINDEXTABLE lpPckFileIndexTable, char *buffer, size_t sizeOfBuffer = 0);

	//��ȡ������Ϣ������д���ļ�����ɵ��ļ��𻵵Ļָ�
	#include "PckClassExceptionRestore.h"

protected:

	//PckClass.cpp
	BOOL	MountPckFile(LPCTSTR	szFile);
	virtual void	BuildDirTree();

	//PckClassFunction.cpp

#if 0
	//�ؽ�ʱ���¼����ļ���������ȥ��Ч�ĺ��ļ����ظ���
	DWORD	ReCountFiles();

#endif
#if 0
	DWORD GetCompressBoundSizeByFileSize(LPPCKFILEINDEX	lpPckFileIndex, DWORD dwFileSize);
#endif
#if 0
	//����
	BOOL BeforeSingleOrMultiThreadProcess(LPPCK_ALL_INFOS lpPckAllInfo, CMapViewFileWrite* &lpWrite, LPCTSTR szPckFile, DWORD dwCreationDisposition, QWORD qdwSizeToMap, int threadnum);
#endif
#if 0
	void MultiThreadInitialize(VOID CompressThread(VOID*), VOID WriteThread(VOID*), int threadnum);
#endif
#if 0
	BOOL WritePckIndex(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, QWORD &dwAddress);
	BOOL WritePckIndexTable(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, DWORD &dwFileCount, QWORD &dwAddress);
#endif
#if 0
	void AfterProcess(CMapViewFileWrite *lpWrite, PCK_ALL_INFOS &PckAllInfo, QWORD &dwAddress, BOOL isRenewAddtional = TRUE);

	void AfterWriteThreadFailProcess(BOOL *lpbThreadRunning, HANDLE hevtAllCompressFinish, DWORD &dwFileCount, DWORD dwFileHasBeenWritten, QWORD &dwAddressFinal, QWORD dwAddress, BYTE **bufferPtrToWrite);
#endif
#if 0
	LPPCKINDEXTABLE_COMPRESS FillAndCompressIndexData(LPPCKINDEXTABLE_COMPRESS lpPckIndexTableComped, LPPCKFILEINDEX lpPckFileIndexToCompress);
#endif

	//PckClassExtract.cpp
	BOOL	StartExtract(LPPCK_PATH_NODE lpNodeToExtract, LPVOID lpMapAddress);
	BOOL	DecompressFile(LPCWSTR lpszFilename, LPPCKINDEXTABLE lpPckFileIndexTable, LPVOID lpvoidFileRead);

#if 0
	//PckClassThread.cpp
	static	VOID CompressThreadCreate(VOID* pParam);
	static	VOID WriteThread(VOID* pParam);

	//���ģʽ
	static	VOID CompressThreadAdd(VOID* pParam);

#endif
#if 0
	//��ѹ��ģʽ
	static	VOID CompressThreadRecompress(VOID* pParam);
#endif
#if 0
	//ѹ��ʱ�ڴ�ʹ��
	void	detectMaxAndAddMemory(DWORD dwMallocSize);
	void	freeMaxAndSubtractMemory(DWORD dwMallocSize);
#endif
#if 0
	//ѹ�����ݶ���
	BOOL	initCompressedDataQueue(int threadnum, DWORD dwFileCount, QWORD dwAddressStartAt);
	BOOL	putCompressedDataQueue(LPBYTE lpBuffer, LPPCKINDEXTABLE_COMPRESS lpPckIndexTable, LPPCKFILEINDEX lpPckFileIndexToCompress);
	BOOL	getCompressedDataQueue(LPBYTE &lpBuffer, LPPCKINDEXTABLE_COMPRESS &lpPckIndexTable);
	void	uninitCompressedDataQueue(int threadnum);
#endif

	///�������
	//virtual void test();
#if 0
public:
	void	init_compressor();
#endif
	//PckClassRebuildFilter.cpp
public:
	BOOL	ParseScript(LPCTSTR lpszScriptFile);

protected:

	
	
#if 0
	char			m_szEventAllWriteFinish[16];
	char			m_szEventAllCompressFinish[16];
	char			m_szEventMaxMemory[16];
#endif
};

#endif