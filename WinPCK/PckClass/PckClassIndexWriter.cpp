
#include "PckClassIndexWriter.h"

CPckClassIndexWriter::CPckClassIndexWriter()
{}

CPckClassIndexWriter::~CPckClassIndexWriter()
{}

//д�뵥������
BOOL CPckClassIndexWriter::WritePckIndex(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, QWORD &dwAddress)
{

	LPBYTE	lpBufferToWrite;
	DWORD	dwNumberOfBytesToMap = lpPckIndexTablePtr->dwIndexDataLength + 8;

	assert(lpPckIndexTablePtr->dwIndexValueHead != 0);

	if(NULL == (lpBufferToWrite = lpWrite->View(dwAddress, dwNumberOfBytesToMap))) {
		m_PckLog.PrintLogEL(TEXT_WRITE_PCK_INDEX, __FILE__, __FUNCTION__, __LINE__);
		assert(FALSE);
		return FALSE;
	}

	memcpy(lpBufferToWrite, lpPckIndexTablePtr->compressed_index_data, dwNumberOfBytesToMap);
	lpWrite->UnmapView();

	dwAddress += dwNumberOfBytesToMap;

	//����������ʾ���ļ�����
	m_lpPckParams->cVarParams.dwUIProgress++;

	return TRUE;
}

//д��ȫ������
BOOL CPckClassIndexWriter::WritePckIndexTable(CMapViewFileWrite *lpWrite, LPPCKINDEXTABLE_COMPRESS lpPckIndexTablePtr, DWORD &dwFileCount, QWORD &dwAddress)
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