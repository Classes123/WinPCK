
#include "PckClassHeadTailWriter.h"
CPckClassHeadTailWriter::CPckClassHeadTailWriter()
{}

CPckClassHeadTailWriter::~CPckClassHeadTailWriter()
{}

void CPckClassHeadTailWriter::AfterProcess(CMapViewFileWrite *lpWrite, PCK_ALL_INFOS &PckAllInfo, QWORD &dwAddress, BOOL isRenewAddtional)
{

	assert(NULL != dwAddress);
	assert(0 != (PckAllInfo.dwFileCount + PckAllInfo.dwFileCountToAdd));

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