//////////////////////////////////////////////////////////////////////
// tRebuildOptDlg.cpp: WinPCK �����̲߳���
// �Ի������
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2018.05.11
//////////////////////////////////////////////////////////////////////

#include "globals.h"
#include "miscdlg.h"
#include "PckControlCenter.h"

/*
ѹ��ѡ��
*/
BOOL TRebuildOptDlg::EvCreate(LPARAM lParam)
{
	char	szStr[8];

	SendDlgItemMessage(IDC_EDIT_SCRIPT, EM_LIMITTEXT, MAX_PATH, 0);

	SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_SETRANGE, FALSE, MAKELONG(1, MAX_COMPRESS_LEVEL));
	SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_SETPOS, TRUE, (LPARAM)lpParams->dwCompressLevel);

	SetDlgItemTextA(IDC_STATIC_LEVEL, ultoa(lpParams->dwCompressLevel, szStr, 10));
	SetDlgItemTextA(IDC_EDIT_SCRIPT, "C:\\");

	return	TRUE;
}

BOOL TRebuildOptDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{

	switch(wID) {
	case IDOK:
	{
		DWORD dwCompressLevel = lpParams->dwCompressLevel;
		lpParams->dwCompressLevel = SendDlgItemMessage(IDC_SLIDER_LEVEL, TBM_GETPOS, 0, 0);

		if(dwCompressLevel != lpParams->dwCompressLevel) {
			if(lpParams->lpPckControlCenter->IsValidPck())
				lpParams->lpPckControlCenter->ResetCompressor();
		}

		*lpNeedRecompress = IsDlgButtonChecked(IDC_CHECK_RECPMPRESS);
		GetDlgItemText(IDC_EDIT_SCRIPT, lpszScriptFile, MAX_PATH);

		EndDialog(wID);
		return	TRUE;
	}
	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}

BOOL TRebuildOptDlg::EventScroll(UINT uMsg, int nCode, int nPos, HWND scrollBar)
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

		break;
	}
	return 0;
}

