
#include "PckClassHeadTailWriter.h"
CPckClassHeadTailWriter::CPckClassHeadTailWriter()
{}

CPckClassHeadTailWriter::~CPckClassHeadTailWriter()
{}

BOOL CPckClassHeadTailWriter::AfterProcess(CMapViewFileMultiPckWrite *lpWrite, PCK_ALL_INFOS &PckAllInfo, QWORD &dwAddress, BOOL isRenewAddtional)
{

	assert(NULL != dwAddress);
	assert(0 != (PckAllInfo.dwFileCount + PckAllInfo.dwFileCountToAdd));

	if(isRenewAddtional)
		strcpy(PckAllInfo.szAdditionalInfo, PCK_ADDITIONAL_INFO);

	//дpckTail
	if (!lpWrite->Write2(dwAddress, m_PckAllInfo.lpSaveAsPckVerFunc->FillTailData(&PckAllInfo), m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize)) {
		
		m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	dwAddress += m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize;

	//дpckHead
	PckAllInfo.qwPckSize = dwAddress;

	assert(0 != PckAllInfo.qwPckSize);

	if (!lpWrite->Write2(0, m_PckAllInfo.lpSaveAsPckVerFunc->FillHeadData(&PckAllInfo), m_PckAllInfo.lpSaveAsPckVerFunc->dwHeadSize)) {
		m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	lpWrite->UnMaping();

	//���ｫ�ļ���С��������һ��
	lpWrite->SetFilePointer(dwAddress, FILE_BEGIN);
	lpWrite->SetEndOfFile();
	return TRUE;

}