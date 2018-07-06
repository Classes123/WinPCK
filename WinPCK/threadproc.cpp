//////////////////////////////////////////////////////////////////////
// threadproc.cpp: WinPCK �����̲߳���
// ѹ������ѹ�����µȴ���ʱ���̣�����̵߳��� 
//
// �˳����� �����/stsm/liqf ��д
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.4.10
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#pragma warning ( disable : 4244 )
#pragma warning ( disable : 4311 )
//#pragma warning ( disable : 4018 )
#pragma warning ( disable : 4005 )

#include "tlib.h"
#include "resource.h"
#include "globals.h"
#include "winmain.h"
#include <process.h>
#include <tchar.h>
#include "OpenSaveDlg.h"
#include "StopWatch.h"

VOID GetPckFileNameBySource(LPTSTR dst, LPCTSTR src, BOOL isDirectory);

VOID TInstDlg::UpdatePckFile(VOID *pParam)
{

	TInstDlg	*pThis = (TInstDlg*)pParam;
	BOOL		bHasPckOpened = !pThis->m_cPckCenter.IsValidPck();
	TCHAR		szFilenameToSave[MAX_PATH];
	LPPCK_RUNTIME_PARAMS	lpParams = pThis->lpPckParams;
	BOOL		*lpbThreadRunning = &lpParams->cVarParams.bThreadRunning;
	TCHAR		szPrintf[320];
	CStopWatch	timer;

	if(bHasPckOpened)
		pThis->m_cPckCenter.New();

	*szFilenameToSave = 0;

	if(bHasPckOpened) {
		//��ʱû�д��ļ�����ʱ�Ĳ����൱���½��ĵ�
		if(1 == pThis->m_lpszFilePath.size()) {
			GetPckFileNameBySource(szFilenameToSave, pThis->m_lpszFilePath[0].c_str(), FALSE);
		}

		//ѡ�񱣴���ļ���
		int nSelectFilter = SaveFile(pThis->hWnd, szFilenameToSave, TEXT("pck"), pThis->m_cPckCenter.GetSaveDlgFilterString());
		if(!nSelectFilter) {
			pThis->m_cPckCenter.Close();

			return;
		}

		//�趨Ŀ��pck�İ汾
		pThis->m_cPckCenter.SetPckVersion(nSelectFilter);

		_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWING), _tcsrchr(szFilenameToSave, TEXT('\\')) + 1);
	} else {
		_tcscpy_s(szFilenameToSave, pThis->m_Filename);
		_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWING), _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);
	}

	//��ʼ��ʱ
	timer.start();

	pThis->EnbaleButtons(ID_MENU_ADD, FALSE);


	pThis->SetStatusBarText(4, szPrintf);

	pThis->m_cPckCenter.Reset(pThis->m_lpszFilePath.size());
	*lpbThreadRunning = TRUE;

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	if(pThis->m_cPckCenter.UpdatePckFile(szFilenameToSave, pThis->m_lpszFilePath, pThis->m_currentNodeOnShow)) {
		if(*lpbThreadRunning) {
			//��ʱ����
			timer.stop();
			pThis->m_PckLog.PrintLogN(GetLoadStr(IDS_STRING_RENEWOK), timer.getElapsedTime());
			//pThis->SetStatusBarText(4, szPrintf);
		} else {

			pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}

		if(bHasPckOpened) {

			pThis->OpenPckFile(szFilenameToSave, TRUE);
		} else {
			pThis->OpenPckFile(pThis->m_Filename, TRUE);
		}

	} else {

		pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));

		if(bHasPckOpened) {
			pThis->m_cPckCenter.Close();
		}
	}

	pThis->EnbaleButtons(ID_MENU_ADD, TRUE);

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if((!(*lpbThreadRunning)) && pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	*lpbThreadRunning = FALSE;

	//��ԭDrop״̬
	pThis->m_lpszFilePath.clear();
	DragAcceptFiles(pThis->hWnd, TRUE);

	//���ϱ���
	// ��ӡ����
	// pck����ԭ���ļ� %d ��\r\n
	// �¼����ļ� %d ���������ظ��ļ��� %d ��\r\n
	// ʹ��ԭ���ݵ�ַ %d ���������ݵ�ַ %d ��\r\n
	// ͨ���ɵ���pck�ļ��� %d ���ļ�\r\n

	if(0 != lpParams->cVarParams.dwPrepareToAddFileCount) {

		_stprintf_s(szPrintf,
			TEXT("�˸��¹����������£�\r\n")
			TEXT("PCK ����ԭ���ļ����� %d\r\n")
			TEXT("�ƻ������ļ����� %d\r\n")
			TEXT("ʵ�ʸ����ļ����� %d\r\n")
			TEXT("�����ļ����� %d\r\n")
			TEXT("δ�����ļ����� %d\r\n")
			TEXT("���º� PCK �����ļ����� %d"),
			lpParams->cVarParams.dwOldFileCount,
			lpParams->cVarParams.dwPrepareToAddFileCount,
			lpParams->cVarParams.dwChangedFileCount,
			lpParams->cVarParams.dwDuplicateFileCount,
			lpParams->cVarParams.dwPrepareToAddFileCount - lpParams->cVarParams.dwChangedFileCount,
			lpParams->cVarParams.dwFinalFileCount);

		pThis->MessageBox(szPrintf, TEXT("���±���"), MB_OK | MB_ICONINFORMATION);

		pThis->m_PckLog.PrintLogI(szPrintf);
	}

	return;

}

VOID TInstDlg::RenamePckFile(VOID *pParam)
{

	TInstDlg	*pThis = (TInstDlg*)pParam;
	TCHAR		szPrintf[64];
	CStopWatch	timer;

	_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWING), _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);

	//��ʼ��ʱ
	timer.start();

	pThis->EnbaleButtons(ID_MENU_RENAME, FALSE);

	pThis->SetStatusBarText(4, szPrintf);

	pThis->m_cPckCenter.Reset();
	pThis->lpPckParams->cVarParams.bThreadRunning = TRUE;

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	if(pThis->m_cPckCenter.RenameFilename()) {

		//��ʱ����
		timer.stop();
		_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWOK), timer.getElapsedTime());

		pThis->SetStatusBarText(4, szPrintf);

		pThis->OpenPckFile(pThis->m_Filename, TRUE);

	} else {
		pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		pThis->m_cPckCenter.Close();
	}

	pThis->EnbaleButtons(ID_MENU_RENAME, TRUE);

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if(pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	pThis->lpPckParams->cVarParams.bThreadRunning = FALSE;

	return;

}

VOID TInstDlg::RebuildPckFile(VOID	*pParam)
{

	TInstDlg	*pThis = (TInstDlg*)pParam;

	BOOL		bDeleteClass = !pThis->m_cPckCenter.IsValidPck();

	TCHAR		szFilenameToSave[MAX_PATH];
	TCHAR		szPrintf[288];

	BOOL		*lpbThreadRunning = &pThis->lpPckParams->cVarParams.bThreadRunning;

	CStopWatch	timer;

	if(bDeleteClass) {
		if(!pThis->OpenPckFile()) {
			return;
		}
	}

	_tcscpy_s(szFilenameToSave, pThis->m_Filename);

	TCHAR		*lpszFileTitle = _tcsrchr(szFilenameToSave, TEXT('\\')) + 1;
	_tcscpy(lpszFileTitle, TEXT("Rebuild_"));
	_tcscat_s(szFilenameToSave, _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);

	//����ѡ��Ի���
	//���öԻ���
	BOOL  bNeedRecompress;
	TRebuildOptDlg	dlg(pThis->lpPckParams, &bNeedRecompress, pThis);
	DWORD dwCompressLevel = pThis->lpPckParams->dwCompressLevel;
	if(IDCANCEL == dlg.Exec())
		return;

	//ѡ�񱣴���ļ���
	int nSelectFilter = SaveFile(pThis->hWnd, szFilenameToSave, TEXT("pck"), pThis->m_cPckCenter.GetSaveDlgFilterString(), pThis->m_cPckCenter.GetPckVersion());
	if(!nSelectFilter) {
		return;
	}

	pThis->m_cPckCenter.SetPckVersion(nSelectFilter);

	//��ʼ��ʱ
	timer.start();

	pThis->EnbaleButtons(ID_MENU_REBUILD, FALSE);

	_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_REBUILDING), _tcsrchr(szFilenameToSave, TEXT('\\')) + 1);
	pThis->SetStatusBarText(4, szPrintf);

	pThis->m_cPckCenter.Reset(pThis->m_cPckCenter.GetPckFileCount());
	*lpbThreadRunning = TRUE;

	pThis->SetTimer(WM_TIMER_PROGRESS_100, 100, NULL);

	if(pThis->m_cPckCenter.RebuildPckFile(szFilenameToSave, bNeedRecompress)) {
		if(*lpbThreadRunning) {
			//��ʱ����
			timer.stop();

			pThis->m_PckLog.PrintLogN(GetLoadStr(IDS_STRING_REBUILDOK), timer.getElapsedTime());
			//pThis->SetStatusBarText(4, szPrintf);
		} else {
			pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}

	} else {

		pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));

	}

	pThis->EnbaleButtons(ID_MENU_REBUILD, TRUE);

	if(bDeleteClass) {
		ListView_DeleteAllItems(pThis->GetDlgItem(IDC_LIST));
		pThis->m_cPckCenter.Close();
	}

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if((!(*lpbThreadRunning)) && pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	*lpbThreadRunning = FALSE;

	return;

}

VOID TInstDlg::CreateNewPckFile(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;

	BOOL		isNotOpenedPck = !pThis->m_cPckCenter.IsValidPck();
	TCHAR		szFilenameToSave[MAX_PATH];

	TCHAR		szPrintf[64];
	BOOL		*lpbThreadRunning = &pThis->lpPckParams->cVarParams.bThreadRunning;

	CStopWatch	timer;

	//ѡ��Ŀ¼
	if(!OpenFilesVistaUp(pThis->hWnd, pThis->m_CurrentPath))
		return;

	pThis->m_lpszFilePath.push_back(pThis->m_CurrentPath);

	GetPckFileNameBySource(szFilenameToSave, pThis->m_CurrentPath, TRUE);

	if(isNotOpenedPck)
		pThis->m_cPckCenter.New();

	//ѡ�񱣴���ļ���
	int nSelectFilter = SaveFile(pThis->hWnd, szFilenameToSave, TEXT("pck"), pThis->m_cPckCenter.GetSaveDlgFilterString());
	if(!nSelectFilter)
		return;

	//�趨Ŀ��pck�İ汾
	pThis->m_cPckCenter.SetPckVersion(nSelectFilter);

	//��ʼ��ʱ
	timer.start();

	pThis->EnbaleButtons(ID_MENU_NEW, FALSE);

	_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_COMPING), _tcsrchr(szFilenameToSave, TEXT('\\')) + 1);
	pThis->SetStatusBarText(4, szPrintf);

	pThis->m_cPckCenter.Reset();
	*lpbThreadRunning = TRUE;

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	if(pThis->m_cPckCenter.UpdatePckFile(szFilenameToSave, pThis->m_lpszFilePath, pThis->m_currentNodeOnShow)) {

		if(*lpbThreadRunning) {
			//��ʱ����
			timer.stop();

			pThis->m_PckLog.PrintLogN(GetLoadStr(IDS_STRING_COMPOK), timer.getElapsedTime());
		} else {
			pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}

	} else {

		pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
	}

	pThis->EnbaleButtons(ID_MENU_NEW, TRUE);

	if(isNotOpenedPck)
		pThis->m_cPckCenter.Close();

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if((!(*lpbThreadRunning)) && pThis->bGoingToExit) {

		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	*lpbThreadRunning = FALSE;

	//��ԭDrop״̬
	pThis->m_lpszFilePath.clear();
	return;

}

VOID TInstDlg::ToExtractAllFiles(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;
	TCHAR		szPrintf[64];
	BOOL		*lpbThreadRunning = &pThis->lpPckParams->cVarParams.bThreadRunning;

	CStopWatch	timer;

	//��ʼ��ʱ
	timer.start();

	pThis->EnbaleButtons(ID_MENU_UNPACK_ALL, FALSE);

	_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_EXPING), _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);
	pThis->SetStatusBarText(4, szPrintf);

	pThis->m_cPckCenter.Reset(pThis->m_cPckCenter.m_lpPckRootNode->child->dwFilesCount);
	*lpbThreadRunning = TRUE;

	pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

	if(pThis->m_cPckCenter.ExtractFiles(&pThis->m_cPckCenter.m_lpPckRootNode, 1)) {
		//��ʱ����
		timer.stop();
		_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_EXPOK), timer.getElapsedTime());

		pThis->SetStatusBarText(4, szPrintf);

	} else {
		pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
	}

	pThis->EnbaleButtons(ID_MENU_UNPACK_ALL, TRUE);

	pThis->KillTimer(WM_TIMER_PROGRESS_100);
	pThis->RefreshProgress();

	if((!(*lpbThreadRunning)) && pThis->bGoingToExit) {
		pThis->bGoingToExit = FALSE;
		pThis->SendMessage(WM_CLOSE, 0, 0);
	}

	*lpbThreadRunning = FALSE;

	return;

}

VOID TInstDlg::ToExtractSelectedFiles(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;

	BOOL		*lpbThreadRunning = &pThis->lpPckParams->cVarParams.bThreadRunning;
	LPDWORD		lpdwUIProgressUpper = &pThis->lpPckParams->cVarParams.dwUIProgressUpper;

	pThis->m_cPckCenter.Reset(0);

	HWND	hList = pThis->GetDlgItem(IDC_LIST);

	CStopWatch	timer;

	LPPCK_PATH_NODE		*lpNodeToShow, *lpNodeToShowPtr;
	LPPCKINDEXTABLE		*lpIndexToShow, *lpIndexToShowPtr;

	UINT uiSelectCount = ListView_GetSelectedCount(hList);

	if(0 != uiSelectCount) {
		if(NULL != (lpNodeToShow = (LPPCK_PATH_NODE*)malloc(sizeof(LPPCK_PATH_NODE) * uiSelectCount))) {
			TCHAR		szPrintf[64];

			lpIndexToShow = (LPPCKINDEXTABLE*)lpNodeToShow;

			//ȡlpNodeToShow
			int	nCurrentItemCount = ListView_GetItemCount(hList);

			LVITEM item;

			item.mask = LVIF_PARAM | LVIF_STATE;
			item.iSubItem = 0;
			item.stateMask = LVIS_SELECTED;		// get all state flags

			lpNodeToShowPtr = lpNodeToShow;
			lpIndexToShowPtr = lpIndexToShow;

			uiSelectCount = 0;

			if(pThis->m_cPckCenter.GetListInSearchMode()) {
				for(item.iItem = 1;item.iItem < nCurrentItemCount;item.iItem++) {
					ListView_GetItem(hList, &item);

					if(item.state & LVIS_SELECTED) {
						*lpIndexToShowPtr = (LPPCKINDEXTABLE)item.lParam;
						(*lpdwUIProgressUpper)++;
						lpIndexToShowPtr++;

						uiSelectCount++;
					}
				}
			} else {
				for(item.iItem = 1;item.iItem < nCurrentItemCount;item.iItem++) {
					ListView_GetItem(hList, &item);

					if(item.state & LVIS_SELECTED) {
						*lpNodeToShowPtr = (LPPCK_PATH_NODE)item.lParam;
						if(NULL == (*lpNodeToShowPtr)->child) {
							(*lpdwUIProgressUpper)++;
						} else {
							(*lpdwUIProgressUpper) += (*lpNodeToShowPtr)->child->dwFilesCount;
						}
						lpNodeToShowPtr++;

						uiSelectCount++;
					}
				}
			}

			if(0 == uiSelectCount)return;

			//��ʼ��ʱ
			timer.start();

			*lpbThreadRunning = TRUE;

			pThis->EnbaleButtons(ID_MENU_UNPACK_SELECTED, FALSE);

			_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_EXPING), _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);
			pThis->SetStatusBarText(4, szPrintf);

			pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);

			lpNodeToShowPtr = lpNodeToShow;

			if(pThis->m_cPckCenter.GetListInSearchMode()) {
				if(pThis->m_cPckCenter.ExtractFiles(lpIndexToShow, uiSelectCount)) {
					//��ʱ����
					timer.stop();
					_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_EXPOK), timer.getElapsedTime());

					pThis->SetStatusBarText(4, szPrintf);
				} else {
					pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
				}
			} else {
				if(pThis->m_cPckCenter.ExtractFiles(lpNodeToShow, uiSelectCount)) {
					//��ʱ����
					timer.stop();
					_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_EXPOK), timer.getElapsedTime());

					pThis->SetStatusBarText(4, szPrintf);
				} else {
					pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
				}

			}

			free(lpNodeToShow);

			pThis->EnbaleButtons(ID_MENU_UNPACK_SELECTED, TRUE);

			pThis->KillTimer(WM_TIMER_PROGRESS_100);
			pThis->RefreshProgress();

			if((!(*lpbThreadRunning)) && pThis->bGoingToExit) {
				pThis->bGoingToExit = FALSE;
				pThis->SendMessage(WM_CLOSE, 0, 0);
			}
		}
	}

	*lpbThreadRunning = FALSE;

	return;

}


VOID TInstDlg::DeleteFileFromPckFile(VOID	*pParam)
{
	TInstDlg	*pThis = (TInstDlg*)pParam;
	BOOL		*lpbThreadRunning = &pThis->lpPckParams->cVarParams.bThreadRunning;

	pThis->m_cPckCenter.Reset();

	HWND	hList = pThis->GetDlgItem(IDC_LIST);

	CStopWatch	timer;

	LPPCK_PATH_NODE		lpNodeToShow;//, *lpNodeToShowPtr;
	LPPCKINDEXTABLE		lpIndexToShow;//, *lpIndexToShowPtr;

	UINT uiSelectCount = ListView_GetSelectedCount(hList);

	if(0 != uiSelectCount) {

		TCHAR		szPrintf[64];

		int	nCurrentItemCount = ListView_GetItemCount(hList);

		LVITEM item;
		item.mask = LVIF_PARAM | LVIF_STATE;
		item.iSubItem = 0;
		item.stateMask = LVIS_SELECTED;		// get all state flags

		uiSelectCount = 0;


		if(pThis->m_cPckCenter.GetListInSearchMode()) {
			for(item.iItem = 1;item.iItem < nCurrentItemCount;item.iItem++) {
				ListView_GetItem(hList, &item);

				if(item.state & LVIS_SELECTED) {

					lpIndexToShow = (LPPCKINDEXTABLE)item.lParam;
					pThis->m_cPckCenter.DeleteNode(lpIndexToShow);
					uiSelectCount++;
				}
			}
		} else {
			for(item.iItem = 1;item.iItem < nCurrentItemCount;item.iItem++) {
				ListView_GetItem(hList, &item);

				if(item.state & LVIS_SELECTED) {

					lpNodeToShow = (LPPCK_PATH_NODE)item.lParam;

					if(NULL == lpNodeToShow->child) {
						pThis->m_cPckCenter.DeleteNode(lpNodeToShow->lpPckIndexTable);
					} else {
						pThis->m_cPckCenter.DeleteNode(lpNodeToShow);
					}

					uiSelectCount++;
				}
			}
		}

		if(0 == uiSelectCount)return;

		//��ʼ��ʱ
		timer.start();

		*lpbThreadRunning = TRUE;

		pThis->EnbaleButtons(ID_MENU_DELETE, FALSE);

		_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWING), _tcsrchr(pThis->m_Filename, TEXT('\\')) + 1);
		pThis->SetStatusBarText(4, szPrintf);

		pThis->SetTimer(WM_TIMER_PROGRESS_100, TIMER_PROGRESS, NULL);


		if(pThis->m_cPckCenter.RenameFilename()) {
			//��ʱ����
			timer.stop();
			_stprintf_s(szPrintf, GetLoadStr(IDS_STRING_RENEWOK), timer.getElapsedTime());

			pThis->SetStatusBarText(4, szPrintf);

			pThis->OpenPckFile(pThis->m_Filename, TRUE);

		} else {
			pThis->SetStatusBarText(4, GetLoadStr(IDS_STRING_PROCESS_ERROR));
		}

		pThis->EnbaleButtons(ID_MENU_DELETE, TRUE);

		pThis->KillTimer(WM_TIMER_PROGRESS_100);
		pThis->RefreshProgress();

		if(pThis->bGoingToExit) {
			pThis->bGoingToExit = FALSE;
			pThis->SendMessage(WM_CLOSE, 0, 0);
		}

	}

	*lpbThreadRunning = FALSE;

	return;

}

//�������Դ�ļ����Ƴ�Ԥ�����pck�ļ���
VOID GetPckFileNameBySource(LPTSTR dst, LPCTSTR src, BOOL isDirectory)
{
	int szPathToCompressLen;
	_tcscpy(dst, src);

	if(isDirectory) {
		if((szPathToCompressLen = lstrlen(dst)) > 13 && 0 == lstrcmp(dst + szPathToCompressLen - 10, TEXT(".pck.files"))) {
			*(dst + szPathToCompressLen - 10) = 0;
		}
	}

	_tcscat(dst, TEXT(".pck"));
}