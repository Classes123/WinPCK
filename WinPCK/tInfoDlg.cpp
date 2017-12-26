//////////////////////////////////////////////////////////////////////
// tInfoDlg.cpp: WinPCK �����̲߳���
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
#include "PckHeader.h"

/*
������Ϣ�Ի���
*/
BOOL TInfoDlg::EvCreate(LPARAM lParam)
{
	SendDlgItemMessage(IDC_EDIT_INFO, EM_LIMITTEXT, PCK_ADDITIONAL_INFO_SIZE - 1, 0);
	SetDlgItemTextA(IDC_EDIT_INFO, dirBuf);
	return	TRUE;
}

BOOL TInfoDlg::EvCommand(WORD wNotifyCode, WORD wID, LPARAM hwndCtl)
{
	switch(wID) {
	case IDOK:
		memset(dirBuf, 0, PCK_ADDITIONAL_INFO_SIZE);
		GetDlgItemTextA(IDC_EDIT_INFO, dirBuf, PCK_ADDITIONAL_INFO_SIZE);
		EndDialog(wID);
		return	TRUE;

	case IDCANCEL:
		EndDialog(wID);
		return	TRUE;
	}
	return	FALSE;
}

