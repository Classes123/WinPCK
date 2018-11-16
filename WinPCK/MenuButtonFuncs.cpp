//////////////////////////////////////////////////////////////////////
// MenuButtonFuncs.cpp: WinPCK �����̲߳���
// ���水ť����
//
// �˳����� �����/stsm/liqf ��д
//
// �˴��뿪Դ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2018.9.4
//////////////////////////////////////////////////////////////////////

#include "tlib.h"
#include "resource.h"
#include "globals.h"
#include "winmain.h"
#include <process.h>

void TInstDlg::ListViewEnter()
{
	if (!m_isListviewRenaming)
		DbClickListView(m_iListHotItem);
	else
		keybd_event(VK_TAB, 0, 0, 0);
}

void TInstDlg::MenuClose()
{
	if (pck_isThreadWorking(m_PckHandle))
		pck_forceBreakThreadWorking(m_PckHandle);

	ListView_DeleteAllItems(GetDlgItem(IDC_LIST));
	pck_close(m_PckHandle);
}

void TInstDlg::MenuInfo()
{
	if (pck_IsValidPck(m_PckHandle)) {
		TInfoDlg	dlg(m_PckHandle, this);
		dlg.Exec();
	}
}

void TInstDlg::MenuSearch()
{
	if (pck_IsValidPck(m_PckHandle)) {
		TSearchDlg	dlg(m_szStrToSearch, this);
		if (dlg.Exec() == TRUE) {
			SearchPckFiles();
		}
	}
}

void TInstDlg::MenuNew(WORD wID)
{
		
	if (pck_isThreadWorking(m_PckHandle)) {
		pck_forceBreakThreadWorking(m_PckHandle);
		EnableButton(wID, FALSE);
	}
	else {
		_beginthread(CreateNewPckFile, 0, this);
	}
}

void TInstDlg::MenuAdd(WORD wID)
{
	if (pck_isThreadWorking(m_PckHandle)) {
		pck_forceBreakThreadWorking(m_PckHandle);
		EnableButton(wID, FALSE);
	}
	else {
		AddFiles();
	}
}

void TInstDlg::MenuRebuild(WORD wID)
{
	if (pck_isThreadWorking(m_PckHandle)) {
		pck_forceBreakThreadWorking(m_PckHandle);
		EnableButton(wID, FALSE);
	}
	else {

		//pck_setMTMemoryUsed(m_PckHandle, 0);
		_beginthread(RebuildPckFile, 0, this);
	}
}

void TInstDlg::MenuCompressOpt()
{
	TCompressOptDlg	dlg(m_PckHandle, this);
	dlg.Exec();
}

void TInstDlg::MenuRename()
{
	if(pck_isThreadWorking(m_PckHandle))
		return;

	HWND	hList = GetDlgItem(IDC_LIST);
	::SetWindowLong(hList, GWL_STYLE, ::GetWindowLong(hList, GWL_STYLE) | LVS_EDITLABELS);
	ListView_EditLabel(hList, m_iListHotItem);
}

void TInstDlg::MenuDelete()
{
	if (pck_isThreadWorking(m_PckHandle))
		return;
	if (IDNO == MessageBox(GetLoadStr(IDS_STRING_ISDELETE), GetLoadStr(IDS_STRING_ISDELETETITLE), MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON2))
		return;

	//pck_setMTMemoryUsed(m_PckHandle, 0);
	_beginthread(DeleteFileFromPckFile, 0, this);
}

void TInstDlg::MenuSelectAll()
{
	LVITEM item = { 0 };

	item.mask = LVIF_STATE;
	item.stateMask = item.state = LVIS_SELECTED;

	HWND hList = GetDlgItem(IDC_LIST);

	int	nItemCount = ListView_GetItemCount(hList);

	for (item.iItem = 1; item.iItem < nItemCount; item.iItem++) {
		ListView_SetItem(hList, &item);
	}
}

void TInstDlg::MenuSelectReverse()
{
	LVITEM item = { 0 };

	item.mask = LVIF_STATE;
	item.stateMask = LVIS_SELECTED;

	HWND hList = GetDlgItem(IDC_LIST);

	int	nItemCount = ListView_GetItemCount(hList);

	for (item.iItem = 1; item.iItem < nItemCount; item.iItem++) {
		ListView_GetItem(hList, &item);
		item.state = LVIS_SELECTED == item.state ? 0 : LVIS_SELECTED;
		ListView_SetItem(hList, &item);
	}
}

void TInstDlg::MenuAttribute()
{
	if (pck_isThreadWorking(m_PckHandle))
		return;

	ViewFileAttribute();
}

void TInstDlg::MenuView()
{
	if (pck_isThreadWorking(m_PckHandle))
		return;

	HWND	hWndList = GetDlgItem(IDC_LIST);

	LVITEM	item = { 0 };

	item.mask = LVIF_PARAM;
	item.iItem = m_iListHotItem;
	ListView_GetItem(hWndList, &item);

	int entry_type = ((LPPCK_UNIFIED_FILE_ENTRY)item.lParam)->entryType;

	if (PCK_ENTRY_TYPE_FOLDER == entry_type)
		return;

	ViewFile((LPPCK_UNIFIED_FILE_ENTRY)item.lParam);
}

void TInstDlg::MenuAbout()
{
#ifdef _DEBUG
	//BOOL a = do_AddFileToPckFile(L"z:\\2222", L"z:\\Rebuild_1111zxxx.pck", L"1111\\");
	BOOL b = do_CreatePckFile(L"z:\\2222", L"z:\\Rebuild_1111zxxx.pck");
#endif
	MessageBoxW(THIS_MAIN_CAPTION
		"\r\n"
		THIS_DESC
		"\r\n\r\n"
		THIS_AUTHOR
		"\r\n\r\n"
		THIS_UESDLIB,
		L"���� "
		THIS_NAME,
		MB_OK | MB_ICONASTERISK);
}