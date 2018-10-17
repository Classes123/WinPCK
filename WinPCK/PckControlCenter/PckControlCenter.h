//////////////////////////////////////////////////////////////////////
// CPckControlCenter.h: ���ڽ����������繫˾��pck�ļ��е����ݣ�����ʾ��List��
// ͷ�ļ�,������PCK������ݽ�������������
//
// �˳����� �����/stsm/liqf ��д��pck�ṹ�ο���ˮ��pck�ṹ.txt����
// �ο����������Դ����в��ڶ�pck�ļ��б�Ĳ���
//
// �˴���Ԥ�ƽ��ῪԴ���κλ��ڴ˴�����޸ķ����뱣��ԭ������Ϣ
// 
// 2012.10.10
//////////////////////////////////////////////////////////////////////

#pragma once

#include "globals.h"
#include "PckHeader.h"
#include "PckClassLog.h"


class CPckClass;

class CPckControlCenter
{
	//����
public:
	CPckControlCenter();
	virtual ~CPckControlCenter();

	LPPCK_RUNTIME_PARAMS	GetParams();

	void	setMainWnd(HWND _hWnd);
	void	init();
	void	Reset(DWORD dwUIProgressUpper = 1);

	void	New();
private:
	BOOL	Open(LPCTSTR lpszFile, BOOL isOpenAfterRestore);
public:
	BOOL	Open(LPCTSTR lpszFile);
	void	Close();

	int		GetPckVersion();
	void	SetPckVersion(int verID);

	//��ȡ��ǰ��������
	LPCTSTR	GetCurrentVersionName();

	LPCTSTR	GetSaveDlgFilterString();


	//���潻��
	//��ǰ�������б��index
	CONST INT GetListCurrentHotItem();
	void	SetListCurrentHotItem(int _val);
	//�б���ʾģʽ ���ļ�����ʾor ������ʾģʽ
	CONST BOOL GetListInSearchMode();
	void	SetListInSearchMode(BOOL _val);


	///pck ��Ľ���
	CONST	LPPCKINDEXTABLE		GetPckIndexTable();


	BOOL		IsValidPck();

	//��ȡnode·��
	BOOL		GetCurrentNodeString(wchar_t* szCurrentNodePathString, LPPCK_PATH_NODE lpNode);

	//��ȡ�ļ���
	DWORD		GetPckFileCount();

	QWORD		GetPckSize();
	QWORD		GetPckDataAreaSize();
	QWORD		GetPckRedundancyDataSize();

	//Ԥ���ļ�
	BOOL		GetSingleFileData(LPVOID lpvoidFileRead, LPPCKINDEXTABLE lpPckFileIndexTable, char *buffer, size_t sizeOfBuffer = 0);

	//���ø�����Ϣ
	char*		GetAdditionalInfo();
	BOOL		SetAdditionalInfo();

	BOOL		RenameIndex(LPPCK_PATH_NODE lpNode, wchar_t* lpszReplaceString);
	BOOL		RenameIndex(LPPCKINDEXTABLE lpIndex, wchar_t* lpszReplaceString);

	//������һ���ڵ�
	BOOL		RenameNode(LPPCK_PATH_NODE lpNode, wchar_t* lpszReplaceString);

	//��ѹ�ļ�
	BOOL		ExtractFiles(LPPCKINDEXTABLE *lpIndexToExtract, int nFileCount);
	BOOL		ExtractFiles(LPPCK_PATH_NODE *lpNodeToExtract, int nFileCount);

	//����PCK��ѹ������
	void	ResetCompressor();

	//�ؽ�pck�ļ�
	BOOL	ParseScript(LPCTSTR lpszScriptFile);
	BOOL	RebuildPckFile(LPTSTR szRebuildPckFile, BOOL bUseRecompress);

	//�½�������pck�ļ�
	BOOL	UpdatePckFile(LPCTSTR szPckFile, const vector<tstring> &lpszFilePath, const LPPCK_PATH_NODE lpNodeToInsert);

	//�Ƿ���֧�ָ��µ��ļ�
	BOOL	isSupportAddFileToPck();

	//�������ļ�
	BOOL	RenameFilename();

	//ɾ��һ���ڵ�
	VOID	DeleteNode(LPPCKINDEXTABLE lpNode);
	VOID	DeleteNode(LPPCK_PATH_NODE lpNode);

private:

	FMTPCK	GetPckTypeFromFilename(LPCTSTR lpszFile);

	void	CreateRestoreData();
	BOOL	RestoreData(LPCTSTR lpszFile, FMTPCK emunFileFormat);
	void	DeleteRestoreData();

public:


	LPPCK_PATH_NODE				m_lpPckRootNode;

private:

	BOOL						m_isSearchMode;

	PCK_RUNTIME_PARAMS			cParams;
	CPckClass					*m_lpClassPck;
	CPckClassLog				m_PckLog;

	RESTORE_INFOS				m_RestoreInfomation;
	//LOG
	HWND						m_hWndMain;

	//��ʽ
	FMTPCK						m_emunFileFormat;

};