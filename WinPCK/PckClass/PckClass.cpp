//////////////////////////////////////////////////////////////////////
// PckClass.cpp: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// �й���ĳ�ʼ����
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.4.10
//////////////////////////////////////////////////////////////////////

#include "PckClass.h"

#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4146 )
#pragma warning ( disable : 4267 )

void CPckClass::CPckClassInit()
{

	m_PckAllInfo.lpRootNode = m_classNode.GetRootNode();
	DWORD	dwCurrentPID = GetCurrentProcessId();

	sprintf_s(m_szEventAllWriteFinish, 16, TEXT_EVENT_WRITE_PCK_DATA_FINISH, dwCurrentPID);
	sprintf_s(m_szEventAllCompressFinish, 16, TEXT_EVENT_COMPRESS_PCK_DATA_FINISH, dwCurrentPID);
	sprintf_s(m_szEventMaxMemory, 16, TEXT_EVENT_PCK_MAX_MEMORY, dwCurrentPID);

	init_compressor();

}

CPckClass::CPckClass(LPPCK_RUNTIME_PARAMS inout) :
	m_lpPckParams(inout),
	m_ReadCompleted(FALSE),
	m_firstFile(NULL),
	m_PckAllInfo({ 0 })
{
	CPckClassInit();
}

CPckClass::~CPckClass()
{}

BOOL CPckClass::Init(LPCTSTR	szFile)
{
	_tcscpy(m_PckAllInfo.szFilename, szFile);
	GetFileTitle(m_PckAllInfo.szFilename, m_PckAllInfo.szFileTitle, MAX_PATH);

	if(MountPckFile(m_PckAllInfo.szFilename)) {
		BuildDirTree();
		return m_ReadCompleted = TRUE;
	} else {
		return m_ReadCompleted = FALSE;
	}
}

CONST	LPPCKINDEXTABLE CPckClass::GetPckIndexTable()
{
	return m_PckAllInfo.lpPckIndexTable;
}

CONST	LPPCK_PATH_NODE CPckClass::GetPckPathNode()
{
	return m_PckAllInfo.lpRootNode;
}

QWORD CPckClass::GetPckSize()
{
	return m_PckAllInfo.qwPckSize;
}

DWORD CPckClass::GetPckFileCount()
{
	return m_PckAllInfo.dwFileCount;
}

QWORD CPckClass::GetPckDataAreaSize()
{
	return m_PckAllInfo.dwAddressOfFilenameIndex - PCK_DATA_START_AT;
}

QWORD CPckClass::GetPckRedundancyDataSize()
{
	return m_PckAllInfo.dwAddressOfFilenameIndex - PCK_DATA_START_AT - m_PckAllInfo.lpRootNode->child->qdwDirCipherTextSize;
}

char * CPckClass::GetAdditionalInfo()
{
	return m_PckAllInfo.szAdditionalInfo;
}

BOOL CPckClass::SetAdditionalInfo()
{
	if(0 == *m_PckAllInfo.szFilename)
		return FALSE;

	CMapViewFileWrite	cWritefile(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	if(NULL == strstr(m_PckAllInfo.szAdditionalInfo, PCK_ADDITIONAL_INFO)) {
		strcpy(m_PckAllInfo.szAdditionalInfo, PCK_ADDITIONAL_INFO
			PCK_ADDITIONAL_INFO_STSM);
	}

	if(!cWritefile.OpenPck(m_PckAllInfo.szFilename, OPEN_EXISTING)) {
		m_PckLog.PrintLogEL(TEXT_OPENWRITENAME_FAIL, m_PckAllInfo.szFilename, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	cWritefile.SetFilePointer(-((QWORD)(m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize)), FILE_END);

	if(!cWritefile.Write(m_PckAllInfo.lpSaveAsPckVerFunc->FillTailData(&m_PckAllInfo), \
		m_PckAllInfo.lpSaveAsPckVerFunc->dwTailSize)) {
		m_PckLog.PrintLogEL(TEXT_WRITEFILE_FAIL, __FILE__, __FUNCTION__, __LINE__);

		return FALSE;
	}

	return TRUE;
}
