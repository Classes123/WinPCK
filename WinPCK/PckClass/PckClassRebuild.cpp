#include "PckClassRebuild.h"
#include "PckClassFileDisk.h"
#include "PckClassRebuildFilter.h"

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4267 )
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4244 )

CPckClassRebuild::CPckClassRebuild()
{}

CPckClassRebuild::~CPckClassRebuild()
{}

/********************************
*
*��������
*
********************/

BOOL CPckClassRebuild::RebuildPckFile(LPCTSTR szRebuildPckFile, BOOL bUseRecompress)
{
	BOOL rtn = bUseRecompress ? RecompressPckFile(szRebuildPckFile) : RebuildPckFile(szRebuildPckFile);

	//�ؽ��������������
	NPckClassRebuildFilter::ResetRebuildFilterInIndexList(m_PckAllInfo);

	return rtn;
}

BOOL CPckClassRebuild::RebuildPckFile(LPCTSTR szRebuildPckFile)
{

	m_PckLog.PrintLogI(TEXT_LOG_REBUILD);

	LPPCKINDEXTABLE_COMPRESS	lpPckIndexTable, lpPckIndexTablePtr;
	QWORD	dwAddress = PCK_DATA_START_AT, dwAddressName;
	DWORD	dwFileCount = m_PckAllInfo.dwFileCount;
	DWORD	dwNoDupFileCount = ReCountFiles();
	QWORD	dwTotalFileSizeAfterRebuild = GetPckFilesizeRebuild(szRebuildPckFile, m_PckAllInfo.qwPckSize);

	PCK_ALL_INFOS		pckAllInfo;

	LPBYTE				lpBufferToWrite, lpBufferToRead;
	CMapViewFileRead	cFileRead;

	//����ͷ��βʱ��Ҫ�Ĳ���
	memset(&pckAllInfo, 0, sizeof(PCK_ALL_INFOS));
	pckAllInfo.lpSaveAsPckVerFunc = m_PckAllInfo.lpSaveAsPckVerFunc;

	//���ý����������ֵ
	m_lpPckParams->cVarParams.dwUIProgressUpper = dwNoDupFileCount;

	//��Դ�ļ� 
	if(!cFileRead.OpenPckAndMappingRead(m_PckAllInfo.szFilename)) {
		return FALSE;
	}

	//��Ŀ���ļ� 
	//�����Ǵ���һ���ļ������������ؽ�����ļ�
	CMapViewFileWrite	cFileWrite(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	if(!cFileWrite.OpenPckAndMappingWrite(szRebuildPckFile, CREATE_ALWAYS, dwTotalFileSizeAfterRebuild)) {
		return FALSE;
	}

	//����ռ�,�ļ���ѹ������ ����
	if(NULL == (lpPckIndexTable = new PCKINDEXTABLE_COMPRESS[dwNoDupFileCount])) {
		m_PckLog.PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	//��ʹ��Enum���б�����������_PCK_INDEX_TABLE
	lpPckIndexTablePtr = lpPckIndexTable;

	LPPCKINDEXTABLE lpPckIndexTableSource = m_PckAllInfo.lpPckIndexTable;
	DWORD		IndexCompressedFilenameDataLengthCryptKey1 = m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->IndexCompressedFilenameDataLengthCryptKey1;
	DWORD		IndexCompressedFilenameDataLengthCryptKey2 = m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->IndexCompressedFilenameDataLengthCryptKey2;
	int			level = m_lpPckParams->dwCompressLevel;

	for(DWORD i = 0; i < dwFileCount; ++i) {

		if(lpPckIndexTableSource->isInvalid) {
			++lpPckIndexTableSource;
			continue;
		}

		DWORD dwNumberOfBytesToMap = lpPckIndexTableSource->cFileIndex.dwFileCipherTextSize;

		if(NULL == (lpBufferToWrite = cFileWrite.View(dwAddress, dwNumberOfBytesToMap))) {
			m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
			delete[] lpPckIndexTable;
			return FALSE;
		}

		DWORD dwSrcAddress = lpPckIndexTableSource->cFileIndex.dwAddressOffset;	//����ԭ���ĵ�ַ

		if(NULL == (lpBufferToRead = cFileRead.View(dwSrcAddress, dwNumberOfBytesToMap))) {
			m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
			delete[] lpPckIndexTable;
			return FALSE;
		}

		memcpy(lpBufferToWrite, lpBufferToRead, dwNumberOfBytesToMap);

		cFileRead.UnmapView();

		//д����ļ���PckFileIndex�ļ�ѹ����Ϣ��ѹ��
		lpPckIndexTableSource->cFileIndex.dwAddressOffset = dwAddress;	//���ļ���ѹ��������ʼ��ַ

		dwAddress += lpPckIndexTableSource->cFileIndex.dwFileCipherTextSize;	//��һ���ļ���ѹ��������ʼ��ַ

		FillAndCompressIndexData(lpPckIndexTablePtr, &lpPckIndexTableSource->cFileIndex);

		lpPckIndexTableSource->cFileIndex.dwAddressOffset = dwSrcAddress;	//��ԭԭ���ĵ�ַ

		cFileWrite.UnmapView();

		++lpPckIndexTablePtr;
		++lpPckIndexTableSource;
		++(m_lpPckParams->cVarParams.dwUIProgress);

	}

	if(!m_lpPckParams->cVarParams.bThreadRunning) {
		m_PckLog.PrintLogW(TEXT_USERCANCLE);
		dwFileCount = m_lpPckParams->cVarParams.dwUIProgress;
	}

	//�رն��ļ�
	//д�ļ�����
	dwAddressName = dwAddress;
	WritePckIndexTable(&cFileWrite, lpPckIndexTable, dwNoDupFileCount, dwAddress);

	pckAllInfo.dwAddressOfFilenameIndex = dwAddressName;
	pckAllInfo.dwFileCount = dwNoDupFileCount;
	AfterProcess(&cFileWrite, pckAllInfo, dwAddress);

	delete[] lpPckIndexTable;

	m_PckLog.PrintLogI(TEXT_LOG_WORKING_DONE);

	return TRUE;

}


//��ѹ���ļ�
BOOL CPckClassRebuild::RecompressPckFile(LPCTSTR szRecompressPckFile)
{

	m_PckLog.PrintLogI(TEXT_LOG_RECOMPRESS);

	LPPCKINDEXTABLE_COMPRESS	lpPckIndexTable, lpPckIndexTablePtr;
	QWORD	dwAddress = PCK_DATA_START_AT, dwAddressName;
	DWORD	dwFileCount = m_PckAllInfo.dwFileCount;
	DWORD	dwNoDupFileCount = ReCountFiles();
	QWORD	dwTotalFileSizeAfterRebuild = GetPckFilesizeRebuild(szRecompressPckFile, m_PckAllInfo.qwPckSize);

	//CMapViewFileWrite	*lpFileWrite;
	CMapViewFileRead	cFileRead;
	int					threadnum = m_lpPckParams->dwMTThread;

	//����ͷ��βʱ��Ҫ�Ĳ���
	PCK_ALL_INFOS		pckAllInfo_backup;
	memcpy(&pckAllInfo_backup, &m_PckAllInfo, sizeof(PCK_ALL_INFOS));
	_tcscpy_s(m_PckAllInfo.szNewFilename, szRecompressPckFile);

	//���ý����������ֵ
	SetParams_ProgressUpper(dwNoDupFileCount);

	//��Դ�ļ� 
	if(!cFileRead.OpenPckAndMappingRead(m_PckAllInfo.szFilename)) {
		assert(FALSE);
		return FALSE;
	}

#pragma region ����Ŀ���ļ�
	CMapViewFileWrite cFileWriter(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	//OPEN_ALWAYS���½��µ�pck(CREATE_ALWAYS)����´��ڵ�pck(OPEN_EXISTING)
	if(!cFileWriter.OpenPckAndMappingWrite(m_PckAllInfo.szNewFilename, CREATE_ALWAYS, dwTotalFileSizeAfterRebuild)) {

		return FALSE;
	}

#pragma endregion

	if(!BeforeMultiThreadProcess(&cFileWriter)) {
		assert(FALSE);
		return FALSE;
	}

	if(!initCompressedDataQueue(threadnum, dwFileCount, PCK_DATA_START_AT)) {
		assert(FALSE);
		return FALSE;
	}

	mt_lpFileRead = &cFileRead;
	mt_dwFileCount = dwFileCount;
	mt_dwFileCountOfWriteTarget = dwNoDupFileCount;

	MultiThreadInitialize(CompressThreadRecompress, WriteThread, threadnum);

	//д�ļ�����
	lpPckIndexTablePtr = lpPckIndexTable = mt_lpPckIndexTable;
	dwAddressName = dwAddress = mt_dwAddressQueue;

	//��������߳�ʹ�õ���ѹ�����λ
	LPPCKINDEXTABLE lpPckIndexTablePtrSrc = m_PckAllInfo.lpPckIndexTable;

	for(DWORD i = 0; i < m_PckAllInfo.dwFileCount; ++i) {

		lpPckIndexTablePtrSrc->isRecompressed = FALSE;
		lpPckIndexTablePtrSrc++;
	}

	//ȡ�����ļ�����
	dwNoDupFileCount = mt_dwFileCountOfWriteTarget;

	WritePckIndexTable(&cFileWriter, mt_lpPckIndexTable, dwNoDupFileCount, dwAddress);

	m_PckAllInfo.dwAddressOfFilenameIndex = dwAddressName;
	m_PckAllInfo.dwFileCount = dwNoDupFileCount;
	AfterProcess(&cFileWriter, m_PckAllInfo, dwAddress);

	uninitCompressedDataQueue(threadnum);

	m_PckLog.PrintLogN(TEXT_LOG_WORKING_DONE);

	//��ԭ
	memcpy(&m_PckAllInfo, &pckAllInfo_backup, sizeof(PCK_ALL_INFOS));

	return TRUE;
}