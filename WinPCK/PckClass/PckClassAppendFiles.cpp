
#include "PckClassAppendFiles.h"
#include "PckClassFileDisk.h"
#include "CharsCodeConv.h"

CPckClassAppendFiles::CPckClassAppendFiles()
{}

CPckClassAppendFiles::~CPckClassAppendFiles()
{}


//�½�������pck��
BOOL CPckClassAppendFiles::UpdatePckFile(LPCTSTR szPckFile, vector<tstring> &lpszFilePath, const LPPCK_PATH_NODE lpNodeToInsert)
{
	DWORD		dwNewFileCount = 0, dwOldPckFileCount;			//�ļ�����, ԭpck�ļ��е��ļ���
	QWORD		qwTotalNewFileSize = 0;						//δѹ��ʱ�����ļ���С

	int			level = m_lpPckParams->dwCompressLevel;
	int			threadnum = m_lpPckParams->dwMTThread;

	QWORD		dwAddress;

	//��ʼ�����ļ�
	LinkList<FILES_TO_COMPRESS> cFileLinkList;
	LPPCK_PATH_NODE		lpNodeToInsertPtr;

#pragma region ���ò���

	if(m_PckAllInfo.isPckFileLoaded) {

		dwOldPckFileCount = m_PckAllInfo.dwFileCount;

		lpNodeToInsertPtr = lpNodeToInsert;

		//ȡ�õ�ǰ�ڵ�����·��
		if(!GetCurrentNodeString(mt_szCurrentNodeString, lpNodeToInsert)) {
			assert(FALSE);
			return FALSE;
		}

		mt_nCurrentNodeStringLen = strlen(mt_szCurrentNodeString);

		m_PckLog.PrintLogI(TEXT_LOG_UPDATE_ADD
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);

	} else {

		m_PckAllInfo.dwAddressOfFilenameIndex = PCK_DATA_START_AT;
		dwOldPckFileCount = 0;
		lpNodeToInsertPtr = m_PckAllInfo.cRootNode.child;
		*mt_szCurrentNodeString = 0;
		mt_nCurrentNodeStringLen = 0;

		m_PckLog.PrintLogI(TEXT_LOG_UPDATE_NEW
			"-"
			TEXT_LOG_LEVEL_THREAD, level, threadnum);

	}

	_tcscpy_s(m_PckAllInfo.szNewFilename, szPckFile);
	m_firstFile = cFileLinkList.first();

#pragma endregion

#pragma region ��������ӵ��ļ� 
	if(!EnumAllFilesByPathList(lpszFilePath, dwNewFileCount, qwTotalNewFileSize, &cFileLinkList))
		return FALSE;

	if(0 == dwNewFileCount)return TRUE;

	m_PckAllInfo.dwFileCountToAdd = dwNewFileCount;
#pragma endregion
	//����˵����
	// mt_dwFileCount	��ӵ��ļ����������ظ�
	// dwFileCount		�������ʹ�ò�����������ļ�������н�ʹ�ô˲�����ʾ��ӵ��ļ������������ظ�

	//�ļ���д�봰�����б�������ʾ����
	SetParams_ProgressUpper(mt_dwFileCount = dwNewFileCount);

	//��������Ҫ���ռ�qwTotalFileSize
	mt_CompressTotalFileSize = GetPckFilesizeByCompressed(szPckFile, qwTotalNewFileSize, m_PckAllInfo.qwPckSize);

	//��ԭ��Ŀ¼�е��ļ��Աȣ��Ƿ�������
	//���ԣ����������ǰ�				��������Ҷ�Ϊ�ļ����ļ��У��򸲸�
	//
	//����FindFileNode����-1�˳�������0����ʾֱ����ӣ���0�������ظ���
	//дר�õ�writethread��compressthread,�Ե���
	//��PCKINDEXTABLE_COMPRESS�����addר�����ԣ����ж��Ƿ����ô˽ڵ㣨����ʱ��0ʹ�ã�1��ʹ��
	//���� ʱʹ��2��ѭ��д��ѹ������ 

	//dwFileCount�����ڴ˴�ָ������ӵ��ļ���ȥ���������� 
	if(m_PckAllInfo.isPckFileLoaded) {
		if(!FindDuplicateNodeFromFileList(lpNodeToInsertPtr, dwNewFileCount))
			return FALSE;
	}

	//��־
	m_PckLog.PrintLogI(TEXT_UPDATE_FILE_INFO, m_PckAllInfo.dwFileCountToAdd, mt_CompressTotalFileSize);

	//PCK_ALL_INFOS	pckAllInfo;
	//OPEN_ALWAYS���½��µ�pck(CREATE_ALWAYS)����´��ڵ�pck(OPEN_EXISTING)

#pragma region ����Ŀ���ļ�
	CMapViewFileWrite cFileWriter(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	//OPEN_ALWAYS���½��µ�pck(CREATE_ALWAYS)����´��ڵ�pck(OPEN_EXISTING)
	if(!cFileWriter.OpenPckAndMappingWrite(m_PckAllInfo.szNewFilename, OPEN_ALWAYS, mt_CompressTotalFileSize)) {
		return FALSE;
	}

#pragma endregion

	if(!BeforeMultiThreadProcess(&cFileWriter)) {
		assert(FALSE);
		return FALSE;
	}
	if(!initCompressedDataQueue(threadnum, mt_dwFileCountOfWriteTarget = mt_dwFileCount, dwAddress = m_PckAllInfo.dwAddressOfFilenameIndex)) {
		delete mt_lpFileWrite;
		assert(FALSE);
		return FALSE;
	}

	MultiThreadInitialize(CompressThreadAdd, WriteThread, threadnum);
	dwAddress = mt_dwAddressQueue;

	//��ӡ�����ò���
	DWORD	dwUseNewDataAreaInDuplicateFile = 0;

	//д�ļ�����
	QWORD	dwAddressName = dwAddress;

	//����������ʾ���ļ����ȣ���ʼ������ʾд��������mt_hFileToWrite
	SetParams_ProgressUpper(dwNewFileCount + dwOldPckFileCount);

	//дԭ�����ļ�
	LPPCKINDEXTABLE	lpPckIndexTableSrc = m_PckAllInfo.lpPckIndexTable;

	for(DWORD i = 0; i < dwOldPckFileCount; i++) {

		PCKINDEXTABLE_COMPRESS	pckIndexTableTemp;
		WritePckIndex(mt_lpFileWrite, FillAndCompressIndexData(&pckIndexTableTemp, &lpPckIndexTableSrc->cFileIndex), dwAddress);
		lpPckIndexTableSrc++;

	}

	//д��ӵ��ļ�
	LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr = mt_lpPckIndexTable;

	mt_dwFileCount = dwNewFileCount = mt_dwFileCountOfWriteTarget;
	for(DWORD i = 0; i < mt_dwFileCountOfWriteTarget; i++) {
		////����lpPckFileIndex->dwAddressOffset

		if(!lpPckIndexTablePtr->bInvalid) {

			WritePckIndex(mt_lpFileWrite, lpPckIndexTablePtr, dwAddress);

		} else {

			assert(0 != dwNewFileCount);
			dwNewFileCount--;
		}

		if(0 == lpPckIndexTablePtr->dwAddressOfDuplicateOldDataArea)
			dwUseNewDataAreaInDuplicateFile++;

		lpPckIndexTablePtr++;

	}

	m_PckAllInfo.dwAddressOfFilenameIndex = dwAddressName;
	//pckAllInfo.dwFileCount = dwFileCount + dwOldPckFileCount;
	AfterProcess(mt_lpFileWrite, m_PckAllInfo, dwAddress);

	uninitCompressedDataQueue(threadnum);

	//���������´�һ�Σ�����ֱ�Ӵ򿪣��ɽ����߳����
	m_lpPckParams->cVarParams.dwOldFileCount = dwOldPckFileCount;
	m_lpPckParams->cVarParams.dwPrepareToAddFileCount = m_PckAllInfo.dwFileCountToAdd;
	m_lpPckParams->cVarParams.dwChangedFileCount = mt_dwFileCount;
	m_lpPckParams->cVarParams.dwDuplicateFileCount = mt_dwFileCount - dwNewFileCount;
	m_lpPckParams->cVarParams.dwUseNewDataAreaInDuplicateFileSize = dwUseNewDataAreaInDuplicateFile;
	m_lpPckParams->cVarParams.dwFinalFileCount = m_PckAllInfo.dwFileCount + m_PckAllInfo.dwFileCountToAdd;

	m_PckLog.PrintLogI(TEXT_LOG_WORKING_DONE);

	return TRUE;

}
