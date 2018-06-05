//////////////////////////////////////////////////////////////////////
// PckClassExceptionRestore.cpp: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// ����д���ļ�����ɵ��ļ��𻵵Ļָ�
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2017.6.27
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4244 )
//#pragma warning ( disable : 4311 )

#include "PckClass.h"

BOOL CPckClass::GetPckBasicInfo(LPTSTR lpszFile, PCKHEAD_V2020 *lpHead, LPBYTE &lpFileIndexData, DWORD &dwPckFileIndexDataSize)
{

	BYTE	*lpFileBuffer;

	dwPckFileIndexDataSize = m_PckAllInfo.qwPckSize - m_PckAllInfo.dwAddressOfFilenameIndex;

	//lstrcpy(lpszFile, m_Filename);
	memcpy(lpszFile, m_PckAllInfo.szFilename, MAX_PATH * sizeof(TCHAR));
	
	if(NULL == (lpFileIndexData = (LPBYTE) malloc (dwPckFileIndexDataSize))){

		m_PckLog.PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	CMapViewFileRead	cReadfile;

	if(!cReadfile.OpenPckAndMappingRead(m_PckAllInfo.szFilename))
	{
		free(lpFileIndexData);
		return FALSE;
	}

	if(NULL == (lpFileBuffer = cReadfile.View(m_PckAllInfo.dwAddressOfFilenameIndex, dwPckFileIndexDataSize)))
	{
		m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		free(lpFileIndexData);
		return FALSE;
	}

	memcpy(lpFileIndexData, lpFileBuffer, dwPckFileIndexDataSize);
	memcpy(lpHead, m_PckAllInfo.lpSaveAsPckVerFunc->FillHeadData(&m_PckAllInfo), sizeof(PCKHEAD_V2020));

	return TRUE;

}

//���޸��ļ����Ȳ������pck�ļ����޷���ȡʱ���ָ�pck����������
BOOL CPckClass::SetPckBasicInfo(PCKHEAD_V2020 *lpHead, LPBYTE lpFileIndexData, DWORD &dwPckFileIndexDataSize)
{
	BYTE	*lpFileBuffer;

	CMapViewFileWrite	cWritefile(m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys->dwMaxSinglePckSize);

	if(!cWritefile.OpenPckAndMappingWrite(m_PckAllInfo.szFilename, OPEN_EXISTING, lpHead->dwPckSize)){
		return FALSE;
	}

	cWritefile.SetFilePointer(0, FILE_BEGIN);
	cWritefile.Write(lpHead, sizeof(PCKHEAD_V2020));

	if(NULL == (lpFileBuffer = cWritefile.View(lpHead->dwPckSize - dwPckFileIndexDataSize, dwPckFileIndexDataSize)))
	{
		m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	memcpy(lpFileBuffer, lpFileIndexData, dwPckFileIndexDataSize);

	return TRUE;
}
