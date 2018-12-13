#include "PckClassWriteOperator.h"
#include "PckClassFileDisk.h"

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4267 )
#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4244 )

CPckClassWriteOperator::CPckClassWriteOperator()
{}

CPckClassWriteOperator::~CPckClassWriteOperator()
{}

/********************************
*
*��������
*
********************/

BOOL CPckClassWriteOperator::RebuildPckFile(LPCTSTR szRebuildPckFile, BOOL bUseRecompress)
{
	BOOL rtn = bUseRecompress ? RecompressPckFile(szRebuildPckFile) : RebuildPckFile(szRebuildPckFile);

	//�ؽ��������������
	ResetRebuildFilterInIndexList(m_PckAllInfo);

	return rtn;
}

BOOL CPckClassWriteOperator::RebuildPckFile(LPCTSTR szRebuildPckFile)
{

	m_PckLog.PrintLogI(TEXT_LOG_REBUILD);

	
	QWORD	dwAddress = PCK_DATA_START_AT;
	DWORD	dwFileCount = m_PckAllInfo.dwFileCount;
	DWORD	dwValidFileCount = ReCountFiles();
	QWORD	dwTotalFileSizeAfterRebuild = GetPckFilesizeRebuild(szRebuildPckFile, m_PckAllInfo.qwPckSize);

	//����ͷ��βʱ��Ҫ�Ĳ���
	PCK_ALL_INFOS		pckAllInfo;
	//����ͷ��βʱ��Ҫ�Ĳ���
	memcpy(&pckAllInfo, &m_PckAllInfo, sizeof(PCK_ALL_INFOS));
	_tcscpy_s(pckAllInfo.szNewFilename, szRebuildPckFile);

	//�̱߳��
	SetThreadFlag(TRUE);

	//���ý����������ֵ
	SetParams_ProgressUpper(dwValidFileCount);

	//��Դ�ļ� 
	CMapViewFileMultiPckRead	cFileRead;
	if(!cFileRead.OpenPckAndMappingRead(pckAllInfo.szFilename)) 
		return FALSE;

	//��Ŀ���ļ� 
	//�����Ǵ���һ���ļ������������ؽ�����ļ�
	CMapViewFileMultiPckWrite	cFileWrite(pckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize);

	if(!cFileWrite.OpenPckAndMappingWrite(szRebuildPckFile, CREATE_ALWAYS, dwTotalFileSizeAfterRebuild))
		return FALSE;

	vector<PCKINDEXTABLE_COMPRESS> cPckIndexTable(dwValidFileCount);

	//��ʹ��Enum���б�����������_PCK_INDEX_TABLE

	LPPCKINDEXTABLE lpPckIndexTableSource = pckAllInfo.lpPckIndexTable;
	pckAllInfo.dwFileCountToAdd = 0;

	for(DWORD i = 0; i < dwFileCount; ++i) {

		if(CheckIfNeedForcedStopWorking()) {
			m_PckLog.PrintLogW(TEXT_USERCANCLE);
			break;
		}

		if(lpPckIndexTableSource->isInvalid) {
			++lpPckIndexTableSource;
			continue;
		}

		LPBYTE lpBufferToWrite, lpBufferToRead;

		DWORD dwNumberOfBytesToMap = lpPckIndexTableSource->cFileIndex.dwFileCipherTextSize;
		DWORD dwSrcAddress = lpPckIndexTableSource->cFileIndex.dwAddressOffset;	//����ԭ���ĵ�ַ

		if (0 != dwNumberOfBytesToMap) {

			if (NULL == (lpBufferToWrite = cFileWrite.View(dwAddress, dwNumberOfBytesToMap))) {
				m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
				return FALSE;
			}

			if (NULL == (lpBufferToRead = cFileRead.View(dwSrcAddress, dwNumberOfBytesToMap))) {
				m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
				return FALSE;
			}

			memcpy(lpBufferToWrite, lpBufferToRead, dwNumberOfBytesToMap);

			cFileRead.UnmapViewAll();
			cFileWrite.UnmapViewAll();
		}

		//д����ļ���PckFileIndex�ļ�ѹ����Ϣ��ѹ��
		lpPckIndexTableSource->cFileIndex.dwAddressOffset = dwAddress;	//���ļ���ѹ��������ʼ��ַ

		dwAddress += dwNumberOfBytesToMap;	//��һ���ļ���ѹ��������ʼ��ַ

		FillAndCompressIndexData(&cPckIndexTable[pckAllInfo.dwFileCountToAdd], &lpPckIndexTableSource->cFileIndex);

		lpPckIndexTableSource->cFileIndex.dwAddressOffset = dwSrcAddress;	//��ԭԭ���ĵ�ַ

		++lpPckIndexTableSource;
		++(pckAllInfo.dwFileCountToAdd);
		SetParams_ProgressInc();

	}

	pckAllInfo.dwFileCountOld = pckAllInfo.dwFileCount = 0;
	pckAllInfo.lpPckIndexTableToAdd = &cPckIndexTable;

	//�رն��ļ�
	//д�ļ�����
	pckAllInfo.dwAddressOfFilenameIndex = dwAddress;

	WriteAllIndex(&cFileWrite, &pckAllInfo, dwAddress);

	AfterProcess(&cFileWrite, pckAllInfo, dwAddress);

	//�̱߳��
	SetThreadFlag(FALSE);

	m_PckLog.PrintLogI(TEXT_LOG_WORKING_DONE);

	return TRUE;
}


//��ѹ���ļ�
BOOL CPckClassWriteOperator::RecompressPckFile(LPCTSTR szRecompressPckFile)
{

	m_PckLog.PrintLogI(TEXT_LOG_RECOMPRESS);

	QWORD	dwAddress = PCK_DATA_START_AT;
	DWORD	dwFileCount = m_PckAllInfo.dwFileCount;
	DWORD	dwNoDupFileCount = ReCountFiles();
	QWORD	dwTotalFileSizeAfterRebuild = GetPckFilesizeRebuild(szRecompressPckFile, m_PckAllInfo.qwPckSize);

	THREAD_PARAMS		cThreadParams;
	CMapViewFileMultiPckRead	cFileRead;
	int					threadnum = m_lpPckParams->dwMTThread;

#pragma region ����ѹ������
	m_zlib.init_compressor(m_lpPckParams->dwCompressLevel);
#pragma endregion

	//����ͷ��βʱ��Ҫ�Ĳ���
	PCK_ALL_INFOS		pckAllInfo;
	memcpy(&pckAllInfo, &m_PckAllInfo, sizeof(PCK_ALL_INFOS));
	_tcscpy_s(pckAllInfo.szNewFilename, szRecompressPckFile);

	//���ý����������ֵ
	SetParams_ProgressUpper(dwNoDupFileCount);

	//��Դ�ļ� 
	if(!cFileRead.OpenPckAndMappingRead(pckAllInfo.szFilename))
		return FALSE;

#pragma region ����Ŀ���ļ�
	CMapViewFileMultiPckWrite cFileWriter(pckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize);

	//OPEN_ALWAYS���½��µ�pck(CREATE_ALWAYS)����´��ڵ�pck(OPEN_EXISTING)
	if(!cFileWriter.OpenPckAndMappingWrite(pckAllInfo.szNewFilename, CREATE_ALWAYS, dwTotalFileSizeAfterRebuild))
		return FALSE;

#pragma endregion

	cThreadParams.pThis = (CPckClassThreadWorker*)this;
	cThreadParams.cDataFetchMethod.lpFileReadPCK = &cFileRead;
	cThreadParams.cDataFetchMethod.dwProcessIndex = 0;
	cThreadParams.cDataFetchMethod.dwTotalIndexCount = pckAllInfo.dwFileCount;
	cThreadParams.cDataFetchMethod.lpPckIndexTablePtrSrc = pckAllInfo.lpPckIndexTable;
	cThreadParams.GetUncompressedData = &GetUncompressedDataFromPCK;

	if(!initMultiThreadVars(&cFileWriter)) {
		assert(FALSE);
		return FALSE;
	}

	if(!initCompressedDataQueue(dwFileCount, PCK_DATA_START_AT)) {
		assert(FALSE);
		return FALSE;
	}

	mt_dwFileCountOfWriteTarget = dwNoDupFileCount;

	ExecuteMainThreadGroup(pckAllInfo, threadnum, &cThreadParams);

	//д�ļ�����

	pckAllInfo.lpPckIndexTable = NULL;
	pckAllInfo.dwFileCountOld = pckAllInfo.dwFileCount = 0;

	//ȡ�����ļ�����
	dwAddress = pckAllInfo.dwAddressOfFilenameIndex;

	WriteAllIndex(&cFileWriter, &pckAllInfo, dwAddress);
	AfterProcess(&cFileWriter, pckAllInfo, dwAddress);

	uninitCompressedDataQueue();

	m_PckLog.PrintLogN(TEXT_LOG_WORKING_DONE);

	return TRUE;
}