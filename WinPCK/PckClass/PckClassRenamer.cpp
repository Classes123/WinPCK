#include "PckClassWriteOperator.h"

//�������ļ�
BOOL CPckClassWriteOperator::RenameFilename()
{
	init_compressor();
	m_PckLog.PrintLogI(TEXT_LOG_RENAME);

	//�����Ǵ���һ���ļ�����������ѹ������ļ�
	CMapViewFileWrite cFileWrite(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	if(!cFileWrite.OpenPck(m_PckAllInfo.szFilename, OPEN_EXISTING)) {

		m_PckLog.PrintLogEL(TEXT_OPENWRITENAME_FAIL, m_PckAllInfo.szFilename, __FILE__, __FUNCTION__, __LINE__);
		assert(FALSE);
		return FALSE;
	}

	QWORD dwFileSize = GetPckFilesizeRename(m_PckAllInfo.szFilename, cFileWrite.GetFileSize());

	if(!cFileWrite.Mapping(dwFileSize)) {

		m_PckLog.PrintLogEL(TEXT_CREATEMAPNAME_FAIL, m_PckAllInfo.szFilename, __FILE__, __FUNCTION__, __LINE__);
		assert(FALSE);
		return FALSE;
	}

	//д�ļ�����
	QWORD dwAddress = m_PckAllInfo.dwAddressOfFilenameIndex;

	WriteAllIndex(&cFileWrite, &m_PckAllInfo, dwAddress);
	
	AfterProcess(&cFileWrite, m_PckAllInfo, dwAddress, FALSE);

	m_PckLog.PrintLogI(TEXT_LOG_WORKING_DONE);

	return TRUE;
}