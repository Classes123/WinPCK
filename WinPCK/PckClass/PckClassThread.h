#pragma once
#include "PckClassFileDisk.h"
#include "PckClassIndex.h"
#include <process.h>
#include <deque>

//��ȡ�ļ�����ֵ
typedef enum { FD_OK, FD_END, FD_ERR ,FD_CANCEL} FETCHDATA_RET;

class CPckClassThreadWorker;

typedef struct _DataFetchMethod
{
	vector<FILES_TO_COMPRESS>::const_iterator ciFilesList;
	vector<FILES_TO_COMPRESS>::const_iterator ciFilesListEnd;

	CMapViewFileMultiPckRead		*lpFileReadPCK;
	LPPCKINDEXTABLE			lpPckIndexTablePtrSrc;
	DWORD					dwProcessIndex;
	DWORD					dwTotalIndexCount;

	char					szCurrentNodeString[MAX_PATH_PCK_260];						//�������߳��е�ǰ��ʾ�ģ��ڵ��Ӧ��pck�е��ļ�·��
	int						nCurrentNodeStringLen;										//�䳤��

}DATA_FETCH_METHOD, *LPDATA_FETCH_METHOD;

typedef struct _ThreadParams
{
	void					*pThis;
	FETCHDATA_RET(*GetUncompressedData)(CPckClassThreadWorker *pThis, LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex);
	DATA_FETCH_METHOD		cDataFetchMethod;
}THREAD_PARAMS, *LPTHREAD_PARAMS;

#define MALLOCED_EMPTY_DATA			(1)

_inline char * __fastcall mystrcpy(char * dest, const char *src)
{
	while((*dest = *src))
		dest++, src++;
	return dest;
}

class CPckClassThreadWorker :
	protected virtual CPckClassFileDisk,
	public virtual CPckClassIndex
{
public:
	CPckClassThreadWorker();
	~CPckClassThreadWorker();

private:
	static	VOID WriteThread(VOID* pParam);
	static	VOID CompressThread(VOID* pParam);

protected:

//����
	BOOL initMultiThreadVars(CMapViewFileMultiPckWrite* lpFileWriter);
protected:
	void	ExecuteMainThreadGroup(PCK_ALL_INFOS &pckAllInfo, int threadnum, void* threadparams);

private:
	//ѹ��ʱ�ڴ�ʹ��
	FETCHDATA_RET	detectMaxToAddMemory(DWORD dwMallocSize);
	FETCHDATA_RET	detectMaxAndAddMemory(LPBYTE &_out_buffer, DWORD dwMallocSize);
	void	freeMaxToSubtractMemory(DWORD dwMallocSize);
	void	freeMaxAndSubtractMemory(LPBYTE &_out_buffer, DWORD dwMallocSize);
	void	SetThreadWorkerCanceled();
	BOOL	isThreadWorkerStillWorking();

	//ѹ�����ݶ���
protected:
	BOOL	initCompressedDataQueue(DWORD dwFileCount, QWORD dwAddressStartAt);
private:
	BOOL	putCompressedDataQueue(PCKINDEXTABLE &lpPckFileIndexToCompress);
	BOOL	getCompressedDataQueue(LPBYTE &lpBuffer, PCKINDEXTABLE_COMPRESS &lpPckIndexTable);
protected:
	void	uninitCompressedDataQueue(int threadnum);

private:
	char			m_szEventAllWriteFinish[16];

	static	SRWLOCK				g_mt_LockCompressedflag;
	static	SRWLOCK				g_mt_LockThreadID;
	static	SRWLOCK				g_mt_LockReadFileMap;

	static	int					mt_threadID;		//�߳�ID
	static	HANDLE				mt_evtAllWriteFinish;											//�߳��¼�

protected:

	static	CMapViewFileMultiPckWrite	*mt_lpFileWrite;												//ȫ��д�ļ��ľ��
	static	QWORD				mt_CompressTotalFileSize;										//Ԥ�Ƶ�ѹ���ļ���С
	static	DWORD				mt_dwFileCountOfWriteTarget;									//д�����ݵ�Ŀ������,һ��=mt_dwFileCount�����ʱ=��ѹ��ʱ����Ч�ļ�����


private:
#ifdef _DEBUG
	static	DWORD				mt_dwCurrentQueuePosPut, mt_dwCurrentQueuePosGet;				//��ǰ����λ��
#endif

protected:
	static	QWORD				mt_dwAddressQueue;												//ȫ��ѹ�����̵�д�ļ���λ�ã�ֻ��Queue����
	static	QWORD				mt_dwAddressNameQueue;											//������pck�ļ���ѹ���ļ���������ʼλ��


//private:

	//�ڶ��߳������л�ȡѹ���õ�Դ����
	static FETCHDATA_RET		GetUncompressedDataFromFile(CPckClassThreadWorker *pThis, LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex/*, LPBYTE &lpCompressedBuffer, DWORD &dwMallocSize*/);
	static FETCHDATA_RET		GetUncompressedDataFromPCK(CPckClassThreadWorker *pThis, LPDATA_FETCH_METHOD lpDataFetchMethod, PCKINDEXTABLE &pckFileIndex/*, LPBYTE &lpCompressedBuffer, DWORD &dwMallocSize*/);

private:
	SRWLOCK						m_LockQueue, m_LockMaxMemory;

	CONDITION_VARIABLE			m_cvReadyToPut, m_cvReadThreadComplete, m_cvMemoryNotEnough;
	BOOL						m_memoryNotEnoughBlocked;

	deque<PCKINDEXTABLE>		m_QueueContent;
	vector<PCKINDEXTABLE_COMPRESS> m_Index_Compress;
protected:
	vector<FILES_TO_COMPRESS>	m_FilesToBeAdded;
};


