//////////////////////////////////////////////////////////////////////
// PckClassMount.cpp: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// �й���ĳ�ʼ����
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2015.5.27
//////////////////////////////////////////////////////////////////////
#pragma warning ( disable : 4996 )

#include "PckClass.h"
#include "CharsCodeConv.h"

BOOL CPckClass::MountPckFile(LPCTSTR	szFile)
{
	if(!cVerDetect.DetectPckVerion(szFile, &m_PckAllInfo))
		return FALSE;

	if(!ReadPckFileIndexes())
		return FALSE;

	return TRUE;
}

void CPckClass::BuildDirTree()
{

	LPPCKINDEXTABLE lpPckIndexTable = m_PckAllInfo.lpPckIndexTable;

	for(DWORD i = 0;i < m_PckAllInfo.dwFileCount;++i) {
		//����Ŀ¼
#ifdef UNICODE
		CAnsi2Ucs cA2U;
		cA2U.GetString(lpPckIndexTable->cFileIndex.szFilename, lpPckIndexTable->cFileIndex.sztFilename, sizeof(lpPckIndexTable->cFileIndex.sztFilename) / sizeof(TCHAR));
#else
		memcpy(lpPckIndexTable->cFileIndex.sztFilename, lpPckIndexTable->cFileIndex.szFilename, sizeof(lpPckIndexTable->cFileIndex.szFilename));
#endif
		AddFileToNode(&m_PckAllInfo.lpRootNode, lpPckIndexTable);
		++lpPckIndexTable;
	}

}
