//////////////////////////////////////////////////////////////////////
// ZupClassThread.cpp: ���ڽ����������繫˾��zup�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.5.23
//////////////////////////////////////////////////////////////////////

#include "ZupClass.h"

BOOL CZupClass::CreatePckFile(LPTSTR szPckFile, LPTSTR szPath)
{
	m_PckLog.PrintLogE(TEXT_NOTSUPPORT);
	return FALSE;
}

//�ؽ�pck�ļ�
BOOL CZupClass::RebuildPckFile(LPTSTR szRebuildPckFile, BOOL bUseRecompress)
{
	m_PckLog.PrintLogE(TEXT_NOTSUPPORT);
	return FALSE;
}

//����pck�ļ�//��bug
BOOL CZupClass::UpdatePckFile(LPTSTR szPckFile, TCHAR(*lpszFilePath)[MAX_PATH], int nFileCount, LPPCK_PATH_NODE lpNodeToInsert)
{
	m_PckLog.PrintLogE(TEXT_NOTSUPPORT);
	return FALSE;
}

//�������ļ�
BOOL CZupClass::RenameFilename()
{
	m_PckLog.PrintLogE(TEXT_NOTSUPPORT);
	return FALSE;
}
