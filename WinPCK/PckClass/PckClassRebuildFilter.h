#pragma once

class CPckClassRebuildScriptFilter
{
public:
	CPckClassRebuildScriptFilter(PCK_ALL_INFOS _DstPckAllInfo, LPPCKINDEXTABLE _lpDstPckIndexTable, PCK_PATH_NODE _DstRootNode);
	~CPckClassRebuildScriptFilter();


	BOOL	ParseScript(LPCTSTR lpszScriptFile);
	static void	ResetRebuildFilterInIndexList(LPPCKINDEXTABLE lpDstPckIndexTable, DWORD dwFileCount);

private:
	BOOL	ApplyScript2IndexList(VOID *pfirstFileOp);	//���ű�����Ӧ�õ��ļ��б���

	PCK_ALL_INFOS			m_DstPckAllInfo;
	LPPCKINDEXTABLE			m_lpDstPckIndexTable;	//����
	PCK_PATH_NODE			m_DstRootNode;			//���ڵ�

	CPckClassLog	m_PckLog;						//��ӡ��־

};

