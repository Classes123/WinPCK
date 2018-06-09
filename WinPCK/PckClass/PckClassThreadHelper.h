#pragma once
#include "PckClassBodyWriter.h"
#include "PckClassIndex.h"
#include <process.h>
#include "PckClassIndexDataAppend.h"


#define USE_OLD_QUEUE 1

#if !(USE_OLD_QUEUE)
#include <deque>
using namespace std;
#endif


/////////���±������ڶ��߳�pckѹ���Ĳ���
#define PCK_MODE_COMPRESS_CREATE	1
#define PCK_MODE_COMPRESS_ADD		2

//�û�ȡ����������ʱ�����浱ǰ�����ѹ�����ļ���
#define SET_PCK_FILE_COUNT_GLOBAL	mt_dwFileCountOfWriteTarget = lpPckParams->cVarParams.dwUIProgress;
#define SET_PCK_FILE_COUNT_AT_FAIL	lpPckParams->cVarParams.bThreadRunning = FALSE;SET_PCK_FILE_COUNT_GLOBAL


_inline char * __fastcall mystrcpy(char * dest, const char *src)
{
	while((*dest = *src))
		dest++, src++;
	return dest;
}

class CPckClassThreadHelper :
	public virtual CPckClassBodyWriter,
	public virtual CPckClassIndex
{
public:
	CPckClassThreadHelper();
	~CPckClassThreadHelper();

	//��ѹ��ģʽ
	static	VOID CompressThreadRecompress(VOID* pParam);

	//PckClassThread.cpp
	static	VOID CompressThreadCreate(VOID* pParam);
	static	VOID WriteThread(VOID* pParam);

	//���ģʽ
	static	VOID CompressThreadAdd(VOID* pParam);

protected:


//private:
//����
	BOOL BeforeMultiThreadProcess(CMapViewFileWrite* lpFileWriter);
	void AfterWriteThreadFailProcess(BOOL *lpbThreadRunning, HANDLE hevtAllCompressFinish, DWORD &dwFileCount, DWORD dwFileHasBeenWritten, QWORD &dwAddressFinal, QWORD dwAddress, BYTE **bufferPtrToWrite);

	void	MultiThreadInitialize(VOID CompressThread(VOID*), VOID WriteThread(VOID*), int threadnum);

	//ѹ��ʱ�ڴ�ʹ��
	void	detectMaxAndAddMemory(DWORD dwMallocSize);
	void	freeMaxAndSubtractMemory(DWORD dwMallocSize);

	//ѹ�����ݶ���
	BOOL	initCompressedDataQueue(int threadnum, DWORD dwFileCount, QWORD dwAddressStartAt);
	BOOL	putCompressedDataQueue(LPBYTE lpBuffer, LPPCKINDEXTABLE_COMPRESS lpPckIndexTable, LPPCKFILEINDEX lpPckFileIndexToCompress);
	BOOL	getCompressedDataQueue(LPBYTE &lpBuffer, LPPCKINDEXTABLE_COMPRESS &lpPckIndexTable);
	void	uninitCompressedDataQueue(int threadnum);

	char			m_szEventAllWriteFinish[16];
	char			m_szEventAllCompressFinish[16];
	char			m_szEventMaxMemory[16];

	//static vars
	static	CRITICAL_SECTION	g_cs;
	static	CRITICAL_SECTION	g_mt_threadID;
	static	CRITICAL_SECTION	g_mt_nMallocBlocked;
	static	CRITICAL_SECTION	g_mt_lpMaxMemory;
	static	CRITICAL_SECTION	g_dwCompressedflag;
	static	CRITICAL_SECTION	g_mt_pckCompressedDataPtrArrayPtr;

	static	int					mt_threadID;		//�߳�ID


	static	HANDLE				mt_evtAllWriteFinish;											//�߳��¼�
	static	HANDLE				mt_evtAllCompressFinish;										//�߳��¼�
	static	HANDLE				mt_evtMaxMemory;												//�߳��¼�
	static	CMapViewFileRead	*mt_lpFileRead;													//ȫ�ֶ��ļ��ľ��,Ŀǰ��������ѹ��
	static	CMapViewFileWrite	*mt_lpFileWrite;												//ȫ��д�ļ��ľ��
	static	QWORD				mt_CompressTotalFileSize;										//Ԥ�Ƶ�ѹ���ļ���С
	static	LPDWORD				mt_lpdwCount;													//�ӽ����̴߳��������ļ�����ָ��
	static	DWORD				mt_dwFileCount;													//�ӽ����̴߳����������ļ�����ָ��
	static	DWORD				mt_dwFileCountOfWriteTarget;									//д�����ݵ�Ŀ������,һ��=mt_dwFileCount�����ʱ=��ѹ��ʱ����Ч�ļ�����

#if USE_OLD_QUEUE
	static	BYTE			**	mt_pckCompressedDataPtrArray;									//�ڴ�����ָ�������
	static	LPPCKINDEXTABLE_COMPRESS	mt_lpPckIndexTable;										//ѹ������ļ�������
	static	DWORD				mt_dwMaxQueueLength;											//������󳤶�
#ifdef _DEBUG
	static	DWORD				mt_dwCurrentQueuePosPut, mt_dwCurrentQueuePosGet;				//��ǰ����λ��
#endif
	static	DWORD				mt_dwCurrentQueueLength;										//��ǰ���п�ʹ�����ݵĸ���
	static	BYTE			**	mt_pckCompressedDataPtrArrayGet, **mt_pckCompressedDataPtrArrayPut;
	static	LPPCKINDEXTABLE_COMPRESS	mt_lpPckIndexTablePut, mt_lpPckIndexTableGet;

#else
	static	deque<LPPCKINDEXTABLE_COMPRESS>	mt_CompressedIndexs;
	static	CPckClassIndexDataAppend *		mt_lpIndexData;
#endif
	static	QWORD				mt_dwAddressQueue;												//ȫ��ѹ�����̵�д�ļ���λ�ã�ֻ��Queue����
	static	QWORD				mt_dwAddressNameQueue;											//������pck�ļ���ѹ���ļ���������ʼλ��

	static	BOOL				*mt_lpbThreadRunning;											//�߳��Ƿ�������״̬��ֵ����Ϊfalseʱ�߳��˳�
	static	DWORD				*mt_lpMaxMemory;												//ѹ����������ʹ�õ�����ڴ棬����һ�����������õģ����ôӽ����̴߳�������ֵ������
	static	DWORD				mt_MaxMemory;													//ѹ����������ʹ�õ�����ڴ�
	static	int					mt_nMallocBlocked;												//�򻺴����걻��ͣ���߳���


																						//���ʱʹ�ñ���

	static	char				mt_szCurrentNodeString[MAX_PATH_PCK_260];						//�������߳��е�ǰ��ʾ�ģ��ڵ��Ӧ��pck�е��ļ�·��
	static	int					mt_nCurrentNodeStringLen;										//�䳤��

};

