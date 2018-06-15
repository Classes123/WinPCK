
#include "PckClassIndexWriter.h"

CPckClassIndexWriter::CPckClassIndexWriter()
{}

CPckClassIndexWriter::~CPckClassIndexWriter()
{}

//д�뵥������
BOOL CPckClassIndexWriter::WritePckIndex(CMapViewFileWrite *lpWrite, const PCKINDEXTABLE_COMPRESS *lpPckIndexTablePtr, QWORD &dwAddress)
{

	LPBYTE	lpBufferToWrite;
	DWORD	dwNumberOfBytesToMap = lpPckIndexTablePtr->dwIndexDataLength + 8;

	assert(lpPckIndexTablePtr->dwIndexValueHead != 0);

	if(NULL == (lpBufferToWrite = lpWrite->View(dwAddress, dwNumberOfBytesToMap))) {
		m_PckLog.PrintLogEL(TEXT_WRITE_PCK_INDEX, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	memcpy(lpBufferToWrite, lpPckIndexTablePtr->compressed_index_data, dwNumberOfBytesToMap);
	lpWrite->UnmapView();

	dwAddress += dwNumberOfBytesToMap;

	//����������ʾ���ļ�����
	SetParams_ProgressInc();


	return TRUE;
}

//д��ȫ������
BOOL CPckClassIndexWriter::WriteAllIndex(CMapViewFileWrite *lpWrite, LPPCK_ALL_INFOS lpPckAllInfo,  QWORD &dwAddress)
{

	//����������ʾ���ļ����ȣ���ʼ������ʾд��������
	SetParams_ProgressUpper(lpPckAllInfo->dwFileCount + lpPckAllInfo->dwFileCountToAdd);

	//дԭ�����ļ�
	LPPCKINDEXTABLE	lpPckIndexTableOld = lpPckAllInfo->lpPckIndexTable;
	DWORD dwOldPckFileCount = lpPckAllInfo->dwFileCountOld;
	DWORD dwFinalFileCount = 0;

	for(DWORD i = 0; i < dwOldPckFileCount; i++) {

		if(!lpPckIndexTableOld->isInvalid) {
			PCKINDEXTABLE_COMPRESS	pckIndexTableTemp;
			WritePckIndex(lpWrite, FillAndCompressIndexData(&pckIndexTableTemp, &lpPckIndexTableOld->cFileIndex), dwAddress);
			++dwFinalFileCount;
		}
		lpPckIndexTableOld++;

	}

	lpPckAllInfo->dwFileCount = dwFinalFileCount;

	const vector<PCKINDEXTABLE_COMPRESS> *lpCompedPckIndexTableNew = lpPckAllInfo->lpPckIndexTableToAdd;

	DWORD dwNewPckFileCount = lpPckAllInfo->dwFileCountToAdd;
#ifdef _DEBUG
	DWORD dwVectorSize = 0;
	if(NULL != lpCompedPckIndexTableNew) dwVectorSize = lpCompedPckIndexTableNew->size();
	assert(dwNewPckFileCount <= dwVectorSize);
#endif
	for(DWORD i = 0; i < dwNewPckFileCount; i++) {
		if(!WritePckIndex(lpWrite, &(*lpCompedPckIndexTableNew)[i], dwAddress)) {
			//���д��ʧ�ܣ�����ǰ��д��ɹ����ļ�
			lpPckAllInfo->dwFinalFileCount = dwFinalFileCount;
			assert(FALSE);
			return FALSE;
		}
		++dwFinalFileCount;

	}

	lpPckAllInfo->dwFinalFileCount = dwFinalFileCount;

	return TRUE;
}