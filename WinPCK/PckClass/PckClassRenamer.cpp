#include "PckClassRenamer.h"
#include "PckClassFileDisk.h"

CPckClassRenamer::CPckClassRenamer()
{}

CPckClassRenamer::~CPckClassRenamer()
{}

//�������ļ�
BOOL CPckClassRenamer::RenameFilename()
{

	m_PckLog.PrintLogI(TEXT_LOG_RENAME);

	int			level = m_lpPckParams->dwCompressLevel;
	DWORD		IndexCompressedFilenameDataLengthCryptKey1 = m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->IndexCompressedFilenameDataLengthCryptKey1;
	DWORD		IndexCompressedFilenameDataLengthCryptKey2 = m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->IndexCompressedFilenameDataLengthCryptKey2;

	//�����Ǵ���һ���ļ�����������ѹ������ļ�
	CMapViewFileWrite cFileWrite(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	if(!cFileWrite.OpenPck(m_PckAllInfo.szFilename, OPEN_EXISTING)) {

		m_PckLog.PrintLogEL(TEXT_OPENWRITENAME_FAIL, m_PckAllInfo.szFilename, __FILE__, __FUNCTION__, __LINE__);
		assert(FALSE);
		return FALSE;
	}

	QWORD dwFileSize = GetPckFilesizeRename(m_PckAllInfo.szFilename, cFileWrite.GetFileSize());//cFileWrite.GetFileSize() + PCK_RENAME_EXPAND_ADD;

	if(!cFileWrite.Mapping(dwFileSize)) {

		m_PckLog.PrintLogEL(TEXT_CREATEMAPNAME_FAIL, m_PckAllInfo.szFilename, __FILE__, __FUNCTION__, __LINE__);
		assert(FALSE);
		return FALSE;
	}

	//д�ļ�����
	QWORD dwAddress = m_PckAllInfo.dwAddressOfFilenameIndex;

	//����������ʾ���ļ����ȣ���ʼ������ʾд��������mt_hFileToWrite
	m_lpPckParams->cVarParams.dwUIProgress = 0;
	m_lpPckParams->cVarParams.dwUIProgressUpper = m_PckAllInfo.dwFileCount;

	//дԭ�����ļ�
	LPPCKINDEXTABLE	lpPckIndexTableSrc = m_PckAllInfo.lpPckIndexTable;

	//д������
	for(DWORD i = 0; i < m_lpPckParams->cVarParams.dwUIProgressUpper; ++i) {

		PCKINDEXTABLE_COMPRESS	pckIndexTableTemp;

		if(lpPckIndexTableSrc->bSelected) {
			--m_PckAllInfo.dwFileCount;
			++lpPckIndexTableSrc;

			//����������ʾ���ļ�����
			++m_lpPckParams->cVarParams.dwUIProgress;
			continue;
		}

		FillAndCompressIndexData(&pckIndexTableTemp, &lpPckIndexTableSrc->cFileIndex);

		WritePckIndex(&cFileWrite, &pckIndexTableTemp, dwAddress);

		++lpPckIndexTableSrc;

	}

	AfterProcess(&cFileWrite, m_PckAllInfo, dwAddress, FALSE);

	m_PckLog.PrintLogI(TEXT_LOG_WORKING_DONE);

	return TRUE;
}