
//////////////////////////////////////////////////////////////////////
// PckControlCenterOperation.cpp: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�,������PCK������ݽ�������������
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#include "PckControlCenter.h"
#include "ZupClass.h"
#include "PckClassZlib.h"
//#include <strsafe.h>


void CPckControlCenter::New()
{
	Close();
	m_lpClassPck = new CPckClass(&cParams);
}

FMTPCK	CPckControlCenter::GetPckTypeFromFilename(LPCTSTR lpszFile)
{
	size_t nFileLength = _tcsnlen(lpszFile, MAX_PATH);

	if(0 == _tcsicmp(lpszFile + nFileLength - 4, TEXT(".pck"))) {
		return FMTPCK_PCK;
	} else if(0 == _tcsicmp(lpszFile + nFileLength - 4, TEXT(".zup"))) {
		return FMTPCK_ZUP;
	}
	return FMTPCK_UNKNOWN;
}

BOOL CPckControlCenter::Open(LPCTSTR lpszFile, BOOL isOpenAfterRestore)
{
	//�ж��ļ���ʽ
	FMTPCK emunFileFormat = GetPckTypeFromFilename(lpszFile);

	while(1) {

		Close();
		switch(emunFileFormat) {
		case FMTPCK_ZUP:
			m_lpClassPck = new CZupClass(&cParams);
			break;
		case FMTPCK_PCK:
		default:
			m_lpClassPck = new CPckClass(&cParams);
			break;
		}

		m_PckLog.PrintLogI(TEXT(TEXT_LOG_OPENFILE), lpszFile);

		if(m_lpClassPck->Init(lpszFile)) {

			m_emunFileFormat = emunFileFormat;

			DeleteRestoreData();

			//����һ�����ݣ�����ʧ�ܺ�ָ�
			CreateRestoreData();

			m_lpPckRootNode = m_lpClassPck->GetPckPathNode();

			//�򿪳ɹ���ˢ�±���
			SendMessage(m_hWndMain, WM_FRESH_MAIN_CAPTION, 1, 0);
			return TRUE;

		} else {
			if((FALSE == isOpenAfterRestore) && RestoreData(lpszFile, emunFileFormat)) {

				return Open(lpszFile, TRUE);
			} else {
				return FALSE;
			}
		}
	}
}

BOOL CPckControlCenter::Open(LPCTSTR lpszFile)
{
	return Open(lpszFile, FALSE);
}

void CPckControlCenter::SetPckVersion(int verID)
{
	//GetSaveFileName���ص�nFilterIndex����1Ϊ�����ģ���ʱӦ��-1
	m_lpClassPck->SetSavePckVersion(verID - 1);
}

int	CPckControlCenter::GetPckVersion()
{
	//GetSaveFileName���ص�nFilterIndex����1Ϊ�����ģ���ʱӦ��+1
	return (m_lpClassPck->GetPckVersion()->id + 1);
}

void CPckControlCenter::Close()
{
	if(NULL != m_lpClassPck) {
		m_PckLog.PrintLogI(TEXT_LOG_CLOSEFILE);

		delete m_lpClassPck;
		m_lpClassPck = NULL;

		m_lpPckRootNode = NULL;
	}

	m_isSearchMode = FALSE;
	m_emunFileFormat = FMTPCK_UNKNOWN;

	//�ر��ļ���ˢ�±���
	SendMessage(m_hWndMain, WM_FRESH_MAIN_CAPTION, 0, 0);
}

void CPckControlCenter::CreateRestoreData()
{
	if(FMTPCK_PCK == m_emunFileFormat) {
		if(!m_lpClassPck->GetPckBasicInfo(&m_RestoreInfomation)) {
			m_PckLog.PrintLogE(TEXT_ERROR_GET_RESTORE_DATA);
		}
	}
}

BOOL CPckControlCenter::RestoreData(LPCTSTR lpszFile, FMTPCK emunFileFormat)
{
	BOOL rtn = FALSE;
	if(FMTPCK_PCK == emunFileFormat) {
		if(m_RestoreInfomation.isValid) {

			if(IDYES == MessageBoxA(m_hWndMain, TEXT_ERROR_OPEN_AFTER_UPDATE, TEXT_ERROR, MB_YESNO | MB_ICONHAND)) {
				if(0 == _tcsicmp(m_RestoreInfomation.szFile, lpszFile)) {

					if(!m_lpClassPck->SetPckBasicInfo(&m_RestoreInfomation))
						m_PckLog.PrintLogE(TEXT_ERROR_RESTORING);
					else {
						m_PckLog.PrintLogI(TEXT_LOG_RESTOR_OK);
						rtn = TRUE;
					}
				}
			}
			DeleteRestoreData();
		}
	}
	return rtn;
}

void CPckControlCenter::DeleteRestoreData()
{
	if(m_RestoreInfomation.isValid) {
		if(NULL != m_RestoreInfomation.lpIndexTailBuffer) {

			free(m_RestoreInfomation.lpIndexTailBuffer);
			m_RestoreInfomation.lpIndexTailBuffer = NULL;
		}
		m_RestoreInfomation.isValid = FALSE;
	}
}

BOOL	CPckControlCenter::RenameIndex(LPPCK_PATH_NODE lpNode, char* lpszReplaceString)
{
	return m_lpClassPck->RenameIndex(lpNode, lpszReplaceString);
}

BOOL	CPckControlCenter::RenameIndex(LPPCKINDEXTABLE lpIndex, char* lpszReplaceString)
{
	return m_lpClassPck->RenameIndex(lpIndex, lpszReplaceString);
}

BOOL	CPckControlCenter::RenameNode(LPPCK_PATH_NODE lpNode, char* lpszReplaceString)
{
	return m_lpClassPck->RenameNode(lpNode, lpszReplaceString);
}

//�½�������pck�ļ�
BOOL	CPckControlCenter::UpdatePckFile(LPCTSTR szPckFile, const vector<tstring> &lpszFilePath, const LPPCK_PATH_NODE lpNodeToInsert)
{
	return m_lpClassPck->UpdatePckFile(szPckFile, lpszFilePath, lpNodeToInsert);
}

//�������ļ�
BOOL	CPckControlCenter::RenameFilename()
{
	return m_lpClassPck->RenameFilename();
}

//�ؽ�pck�ļ�
BOOL	CPckControlCenter::RebuildPckFile(LPTSTR szRebuildPckFile, BOOL bUseRecompress)
{
	return m_lpClassPck->RebuildPckFile(szRebuildPckFile, bUseRecompress);
}

//��ѹ�ļ�
BOOL	CPckControlCenter::ExtractFiles(LPPCKINDEXTABLE *lpIndexToExtract, int nFileCount)
{
	return m_lpClassPck->ExtractFiles(lpIndexToExtract, nFileCount);
}

BOOL	CPckControlCenter::ExtractFiles(LPPCK_PATH_NODE *lpNodeToExtract, int nFileCount)
{
	return m_lpClassPck->ExtractFiles(lpNodeToExtract, nFileCount);
}

//ɾ��һ���ڵ�
VOID	CPckControlCenter::DeleteNode(LPPCKINDEXTABLE lpNode)
{
	return m_lpClassPck->DeleteNode(lpNode);
}

//ɾ��һ���ڵ�
VOID	CPckControlCenter::DeleteNode(LPPCK_PATH_NODE lpNode)
{
	return m_lpClassPck->DeleteNode(lpNode);
}

//����PCK��ѹ������
void	CPckControlCenter::ResetCompressor()
{
	CPckClassZlib::set_level(cParams.dwCompressLevel);
}

//�ؽ�ʱ���ļ�����
BOOL	CPckControlCenter::ParseScript(LPCTSTR lpszScriptFile)
{
	return m_lpClassPck->ParseScript(lpszScriptFile);
}
