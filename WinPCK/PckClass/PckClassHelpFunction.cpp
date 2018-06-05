//////////////////////////////////////////////////////////////////////
// PckClassHelpFunction.cpp: PCK�ļ����ܹ����е��ӹ��ܼ���
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2015.5.19
//////////////////////////////////////////////////////////////////////

#include "PckClass.h"

#pragma warning ( disable : 4267 )
#pragma warning ( disable : 4996 )

void CPckClass::AfterProcess(CMapViewFileWrite *lpWrite, PCK_ALL_INFOS &PckAllInfo, QWORD &dwAddress, BOOL isRenewAddtional)
{

	assert(NULL != dwAddress);
	assert(0 != PckAllInfo.dwFileCount);

	LPBYTE lpBufferToWrite;

	if(isRenewAddtional)
		strcpy(PckAllInfo.szAdditionalInfo, PCK_ADDITIONAL_INFO
			PCK_ADDITIONAL_INFO_STSM);

	//дpckTail
	if(NULL == (lpBufferToWrite = lpWrite->View(dwAddress, m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize))) {

		m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		lpWrite->SetFilePointer(dwAddress, FILE_BEGIN);
		dwAddress += lpWrite->Write(m_PckAllInfo.lpSaveAsPckVerFunc->FillTailData(&PckAllInfo), \
			m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize);
	} else {

		memcpy(lpBufferToWrite, m_PckAllInfo.lpSaveAsPckVerFunc->FillTailData(&PckAllInfo), \
			m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize);
		dwAddress += m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize;
		lpWrite->UnmapView();
	}

	//дpckHead
	PckAllInfo.qwPckSize = dwAddress;

	assert(0 != PckAllInfo.qwPckSize);

	if(NULL == (lpBufferToWrite = lpWrite->View(0, m_PckAllInfo.lpSaveAsPckVerFunc->dwHeadSize))) {

		m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		lpWrite->SetFilePointer(0, FILE_BEGIN);
		lpWrite->Write(m_PckAllInfo.lpSaveAsPckVerFunc->FillHeadData(&PckAllInfo), \
			m_PckAllInfo.lpSaveAsPckVerFunc->dwHeadSize);
	} else {
		memcpy(lpBufferToWrite, m_PckAllInfo.lpSaveAsPckVerFunc->FillHeadData(&PckAllInfo), \
			m_PckAllInfo.lpSaveAsPckVerFunc->dwHeadSize);
		lpWrite->UnmapView();
	}

	lpWrite->UnMaping();

	//���ｫ�ļ���С��������һ��
	lpWrite->SetFilePointer(dwAddress, FILE_BEGIN);
	lpWrite->SetEndOfFile();

}

//�ؽ�ʱ������Ч�ļ��������ų��ظ����ļ�
DWORD CPckClass::ReCountFiles()
{
	DWORD deNewFileCount = 0;
	LPPCKINDEXTABLE lpPckIndexTableSource = m_PckAllInfo.lpPckIndexTable;

	for(DWORD i = 0; i < m_PckAllInfo.dwFileCount; ++i) {

		if(!lpPckIndexTableSource->isInvalid) {
			++deNewFileCount;
		}
		++lpPckIndexTableSource;
	}
	return deNewFileCount;
}

BOOL CPckClass::WritePckIndex(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, QWORD &dwAddress)
{

	LPBYTE	lpBufferToWrite;
	DWORD	dwNumberOfBytesToMap = lpPckIndexTablePtr->dwIndexDataLength + 8;

	assert(lpPckIndexTablePtr->dwIndexValueHead != 0);

	if(NULL == (lpBufferToWrite = lpWrite->View(dwAddress, dwNumberOfBytesToMap))) {
		m_PckLog.PrintLogEL(TEXT_WRITE_PCK_INDEX, __FILE__, __FUNCTION__, __LINE__);
		assert(FALSE);
		return FALSE;
	}

	memcpy(lpBufferToWrite, lpPckIndexTablePtr, dwNumberOfBytesToMap);
	lpWrite->UnmapView();

	dwAddress += dwNumberOfBytesToMap;

	//����������ʾ���ļ�����
	m_lpPckParams->cVarParams.dwUIProgress++;

	return TRUE;
}


BOOL CPckClass::WritePckIndexTable(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, DWORD &dwFileCount, QWORD &dwAddress)
{

	//����������ʾ���ļ����ȣ���ʼ������ʾд��������
	m_lpPckParams->cVarParams.dwUIProgress = 0;

	for(DWORD i = 0; i < dwFileCount; i++) {
		if(!WritePckIndex(lpWrite, lpPckIndexTablePtr, dwAddress)) {
			//���д��ʧ�ܣ�����ǰ��д��ɹ����ļ�
			assert(FALSE);
			dwFileCount = i;
			return FALSE;
		}
		lpPckIndexTablePtr++;

	}

	return TRUE;
}

//д�߳�ʧ�ܺ�Ĵ���
void CPckClass::AfterWriteThreadFailProcess(BOOL *lpbThreadRunning, HANDLE hevtAllCompressFinish, DWORD &dwFileCount, DWORD dwFileHasBeenWritten, QWORD &dwAddressFinal, QWORD dwAddress, BYTE **bufferPtrToWrite)
{

	*(lpbThreadRunning) = FALSE;

	WaitForSingleObject(hevtAllCompressFinish, INFINITE);
	dwFileCount = dwFileHasBeenWritten;
	dwAddressFinal = dwAddress;

	//�ͷ�
	while(0 != *bufferPtrToWrite) {
		if(1 != (int)(*bufferPtrToWrite))
			free(*bufferPtrToWrite);
		bufferPtrToWrite++;
	}

}

LPPCKINDEXTABLE_COMPRESS CPckClass::FillAndCompressIndexData(LPPCKINDEXTABLE_COMPRESS lpPckIndexTableComped, LPPCKFILEINDEX lpPckFileIndexToCompress)
{
	BYTE pckFileIndexBuf[MAX_INDEXTABLE_CLEARTEXT_LENGTH];
	lpPckIndexTableComped->dwIndexDataLength = MAX_INDEXTABLE_CLEARTEXT_LENGTH;
	compress(lpPckIndexTableComped->buffer, &lpPckIndexTableComped->dwIndexDataLength,
		m_PckAllInfo.lpSaveAsPckVerFunc->FillIndexData(lpPckFileIndexToCompress, pckFileIndexBuf), m_PckAllInfo.lpSaveAsPckVerFunc->dwFileIndexSize, m_lpPckParams->dwCompressLevel);
	//����ȡ��
	lpPckIndexTableComped->dwIndexValueHead = lpPckIndexTableComped->dwIndexDataLength ^ m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->IndexCompressedFilenameDataLengthCryptKey1;
	lpPckIndexTableComped->dwIndexValueTail = lpPckIndexTableComped->dwIndexDataLength ^ m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->IndexCompressedFilenameDataLengthCryptKey2;

	return lpPckIndexTableComped;
}

DWORD CPckClass::GetCompressBoundSizeByFileSize(LPPCKFILEINDEX	lpPckFileIndex, DWORD dwFileSize)
{
	if(PCK_BEGINCOMPRESS_SIZE < dwFileSize) {
		lpPckFileIndex->dwFileClearTextSize = dwFileSize;
		lpPckFileIndex->dwFileCipherTextSize = this->compressBound(dwFileSize);
	} else {
		lpPckFileIndex->dwFileCipherTextSize = lpPckFileIndex->dwFileClearTextSize = dwFileSize;
	}

	return lpPckFileIndex->dwFileCipherTextSize;
}