//////////////////////////////////////////////////////////////////////
// tCompressOptDlg.cpp: WinPCK �����̲߳���
// �Ի������
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2017.12.26
//////////////////////////////////////////////////////////////////////

#include "globals.h"
#include "miscdlg.h"
#include "pck_handle.h"

/*
ѹ��ѡ��
*/
BOOL TCompressOptDlg::EvCreate(LPARAM lParam)
{
	char	szStr[8];

	SendDlgItemMessage(IDC_EDIT_MEM, EM_LIMITTEXT, 4, 0);

	SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_SETRANGE, FALSE, MAKELONG(1, pck_getMaxCompressLevel()));
	SendDlgItemMessage(IDC_SLIDER_THREAD, TBM_SETRANGE, FALSE, MAKELONG(1, pck_getMaxThreadUpperLimit()));
	SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_SETPOS, TRUE, (LPARAM)pck_getCompressLevel(m_PckHandle));
	SendDlgItemMessage(IDC_SLIDER_THREAD, TBM_SETPOS, TRUE, (LPARAM)pck_getMaxThread(m_PckHandle));

	SendDlgItemMessageA(IDC_CBO_CODEPAGE, CB_ADDSTRING, 0, (LPARAM)"CP936");
	SendDlgItemMessageA(IDC_CBO_CODEPAGE, CB_SETCURSEL, 0, 0);

	SetDlgItemTextA(IDC_STATIC_LEVEL, ultoa(pck_getCompressLevel(m_PckHandle), szStr, 10));
	SetDlgItemTextA(IDC_STATIC_THREAD, ultoa(pck_getMaxThread(m_PckHandle), szStr, 10));
	SetDlgItemTextA(IDC_EDIT_MEM, ultoa((pck_getMaxMemoryAllowed()) >> 20, szStr, 10));

	return	TRUE;
}

BOOL TCompressOptDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	char	szStr[8];

	switch(wID) {
	case IDOK:
	{
		DWORD dwCompressLevel = pck_getCompressLevel(m_PckHandle);
		pck_setCompressLevel(m_PckHandle, SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_GETPOS, 0, 0));

		//pck_setCompressLevel ����������
		//if(dwCompressLevel != pck_getCompressLevel(m_PckHandle)) {
		//	if(lpParams->lpPckControlCenter->IsValidPck())
		//		lpParams->lpPckControlCenter->ResetCompressor();
		//}

		pck_setMaxThread(m_PckHandle, SendDlgItemMessage(IDC_SLIDER_THREAD, TBM_GETPOS, 0, 0));

		GetDlgItemTextA(IDC_EDIT_MEM, szStr, 8);
		pck_setMTMaxMemory(m_PckHandle, (atoi(szStr) << 20));

		//if(0 >= lpParams->dwMTMaxMemory)
		//	lpParams->dwMTMaxMemory = MT_MAX_MEMORY;
		//else
		//	(lpParams->dwMTMaxMemory) <<= 20;

		EndDialog(wID);
		return	TRUE;
	}
	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}

BOOL TCompressOptDlg::EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar)
{
	int		iPos;
	char	szStr[4];

	switch(nCode) {
	case TB_THUMBTRACK:
	case TB_PAGEDOWN:
	case TB_PAGEUP:

		iPos = ::SendMessage(scrollBar, TBM_GETPOS, 0, 0);

		if(scrollBar == GetDlgItem(IDC_SLIDER_LEVEL)) {
			SetDlgItemTextA(IDC_STATIC_LEVEL, ultoa(iPos, szStr, 10));
			break;
		}
		if(scrollBar == GetDlgItem(IDC_SLIDER_THREAD)) {
			SetDlgItemTextA(IDC_STATIC_THREAD, ultoa(iPos, szStr, 10));
			break;
		}

		break;
	}
	return 0;
}
