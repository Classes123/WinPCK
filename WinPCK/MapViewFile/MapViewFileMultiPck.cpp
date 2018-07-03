#include "MapViewFileMultiPck.h"


CMapViewFileMultiPck::CMapViewFileMultiPck()
{}

CMapViewFileMultiPck::~CMapViewFileMultiPck()
{}

void CMapViewFileMultiPck::GetPkXName(LPSTR dst, LPCSTR src, int _pckid)
{

	strcpy_s(dst, MAX_PATH, src);
	int slen = strlen(dst);
	char *lpszDst = strrchr(dst, '.');

	//.pck -> .pkx
	switch(_pckid) {

	case ID_PKX:
		strcpy(lpszDst, ".pkx");
		break;
	case ID_PKG:
		strcpy(lpszDst, ".pkg");
		break;
	default:
		break;
	}

}

void CMapViewFileMultiPck::GetPkXName(LPWSTR dst, LPCWSTR src, int _pckid)
{
	wcscpy_s(dst, MAX_PATH, src);
	int slen = wcslen(dst);
	wchar_t *lpszDst = wcsrchr(dst, L'.');

	//.pck -> .pkx
	switch(_pckid) {

	case ID_PKX:
		wcscpy(lpszDst, L".pkx");
		break;
	case ID_PKG:
		wcscpy(lpszDst, L".pkg");
		break;
	default:
		break;
	}

}

BOOL CMapViewFileMultiPck::SetPckPackSize(QWORD qwPckSize)
{

	if(m_uqwPckStructSize.qwValue < qwPckSize) {
		//pck��ע���ļ���С��ʵ���ļ�Ҫ���ļ���ȫ
		return FALSE;
	} else if(m_uqwPckStructSize.qwValue > qwPckSize) {
		//pck��ע���ļ���С��ʵ���ļ�ҪС������ָ���ļ���С
		m_uqwPckStructSize.qwValue = qwPckSize;
	}

	return TRUE;
}

