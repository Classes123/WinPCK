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
	m_classNode.GenerateUnicodeStringToIndex();
	m_classNode.ParseIndexTableToNode();
}
