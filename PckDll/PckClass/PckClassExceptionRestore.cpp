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
#include "PckClassBaseFeatures.h"
#include "MapViewFileMultiPck.h"

BOOL CPckClassBaseFeatures::GetPckBasicInfo(LPRESTORE_INFOS lpRestoreInfo)
{

	BYTE	*lpFileBuffer;

	lpRestoreInfo->isValid = FALSE;
	lpRestoreInfo->sizeOfIndexTailBuffer = m_PckAllInfo.qwPckSize - m_PckAllInfo.dwAddressOfFilenameIndex;
	lpRestoreInfo->dwAddressOfFilenameIndex = m_PckAllInfo.dwAddressOfFilenameIndex;
	lpRestoreInfo->dwMaxSinglePckSize = m_PckAllInfo.lpSaveAsPckVerFunc->cPckXorKeys.dwMaxSinglePckSize;

	_tcscpy_s(lpRestoreInfo->szFile, m_PckAllInfo.szFilename);
	
	if(NULL == (lpRestoreInfo->lpIndexTailBuffer = (LPBYTE) malloc (lpRestoreInfo->sizeOfIndexTailBuffer))){

		m_PckLog.PrintLogEL(TEXT_MALLOC_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	CMapViewFileMultiPckRead	cReadfile;

	if(!cReadfile.OpenPckAndMappingRead(m_PckAllInfo.szFilename))
	{
		free(lpRestoreInfo->lpIndexTailBuffer);
		return FALSE;
	}

	if(NULL == (lpFileBuffer = cReadfile.View(m_PckAllInfo.dwAddressOfFilenameIndex, lpRestoreInfo->sizeOfIndexTailBuffer)))
	{
		m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		free(lpRestoreInfo->lpIndexTailBuffer);
		return FALSE;
	}

	memcpy(lpRestoreInfo->lpIndexTailBuffer, lpFileBuffer, lpRestoreInfo->sizeOfIndexTailBuffer);
	memcpy(&lpRestoreInfo->Head, m_PckAllInfo.lpSaveAsPckVerFunc->FillHeadData(&m_PckAllInfo), sizeof(PCKHEAD_V2020));

	lpRestoreInfo->isValid = TRUE;
	return TRUE;

}

//���޸��ļ����Ȳ������pck�ļ����޷���ȡʱ���ָ�pck����������
BOOL CPckClassBaseFeatures::SetPckBasicInfo(LPRESTORE_INFOS lpRestoreInfo)
{
	BYTE	*lpFileBuffer;

	CMapViewFileMultiPckWrite	cWritefile(lpRestoreInfo->dwMaxSinglePckSize);

	if(!cWritefile.OpenPckAndMappingWrite(lpRestoreInfo->szFile, OPEN_EXISTING, lpRestoreInfo->Head.dwPckSize)){
		return FALSE;
	}

	cWritefile.SetFilePointer(0, FILE_BEGIN);
	cWritefile.Write(&lpRestoreInfo->Head, sizeof(PCKHEAD_V2020));

	if(NULL == (lpFileBuffer = cWritefile.View(lpRestoreInfo->Head.dwPckSize - lpRestoreInfo->sizeOfIndexTailBuffer, lpRestoreInfo->sizeOfIndexTailBuffer)))
	{
		m_PckLog.PrintLogEL(TEXT_VIEWMAP_FAIL, __FILE__, __FUNCTION__, __LINE__);
		return FALSE;
	}

	memcpy(lpFileBuffer, lpRestoreInfo->lpIndexTailBuffer, lpRestoreInfo->sizeOfIndexTailBuffer);

	return TRUE;
}
